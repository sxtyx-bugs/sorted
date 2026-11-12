#include "MainWindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QStandardPaths>
#include <chrono>

namespace Ui {

// -------------------------------------------------------------
// ScanWorker Implementation
// -------------------------------------------------------------
ScanWorker::ScanWorker(fs::path rootPath, bool includeHidden)
    : m_rootPath(std::move(rootPath)), m_includeHidden(includeHidden) {}

void ScanWorker::cancel() {
    m_cancelRequested.store(true);
}

void ScanWorker::run() {
    ScanResult result;
    result.rootPath = m_rootPath;

    Core::FileScanner scanner;
    scanner.setIncludeHiddenFiles(m_includeHidden);

    auto startScan = std::chrono::high_resolution_clock::now();

    result.files = scanner.scan(
        m_rootPath,
        &m_cancelRequested,
        [this](std::size_t count, const fs::path& currentDir) {
            emit progress(static_cast<int>(count), QString::fromStdWString(currentDir.wstring()));
        }
    );

    auto endScan = std::chrono::high_resolution_clock::now();
    result.stats.scanDurationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endScan - startScan);
    result.stats.totalFilesScanned = result.files.size();
    result.scanErrors = scanner.getErrors();
    result.stats.errorCount = result.scanErrors.size();

    if (m_cancelRequested.load()) {
        emit finished(result);
        return;
    }

    // Step 2: Classification
    auto startClassify = std::chrono::high_resolution_clock::now();
    Core::FileClassifier classifier;
    classifier.classifyAll(result.files);

    for (const auto& f : result.files) {
        result.stats.totalSizeBytes += f.size;
        result.stats.categoryCounts[f.category]++;
        result.stats.categorySizes[f.category] += f.size;
    }
    auto endClassify = std::chrono::high_resolution_clock::now();
    result.stats.classifyDurationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endClassify - startClassify);

    if (m_cancelRequested.load()) {
        emit finished(result);
        return;
    }

    // Step 3: Duplicate Detection (Size Prefilter + SHA-256)
    auto startHash = std::chrono::high_resolution_clock::now();
    Core::DuplicateDetector detector;
    result.duplicateGroups = detector.detectDuplicates(
        result.files,
        result.stats.totalBytesHashed,
        &m_cancelRequested,
        [this](std::size_t processed, std::size_t total, const fs::path& currentFile) {
            emit hashProgress(static_cast<int>(processed), static_cast<int>(total),
                             QString::fromStdString(currentFile.filename().string()));
        }
    );
    auto endHash = std::chrono::high_resolution_clock::now();
    result.stats.hashDurationMs = std::chrono::duration_cast<std::chrono::milliseconds>(endHash - startHash);
    result.stats.duplicateGroupCount = result.duplicateGroups.size();

    std::size_t totalDupCandidates = 0;
    for (const auto& group : result.duplicateGroups) {
        totalDupCandidates += group.files.size();
    }
    result.stats.duplicateCandidateCount = totalDupCandidates;

    emit finished(result);
}

// -------------------------------------------------------------
// ExecutionWorker Implementation
// -------------------------------------------------------------
ExecutionWorker::ExecutionWorker(Operations::FileOperationService* opService, std::vector<MoveOperation> moves)
    : m_opService(opService), m_moves(std::move(moves)) {}

void ExecutionWorker::cancel() {
    m_cancelRequested.store(true);
}

void ExecutionWorker::run() {
    auto summary = m_opService->executeBatch(
        m_moves,
        &m_cancelRequested,
        [this](std::size_t completed, std::size_t total, const MoveOperation& op) {
            emit moveProgress(static_cast<int>(completed), static_cast<int>(total),
                             QString::fromStdString(op.source.filename().string()));
        }
    );

    emit finished(summary, m_moves);
}

// -------------------------------------------------------------
// MainWindow Implementation
// -------------------------------------------------------------
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_planner(m_classifier), m_opService(&m_journal) {
    setupUi();
    applyTheme();

    // Default to Downloads folder
    QString downloadsPath = QStandardPaths::writableLocation(QStandardPaths::DownloadLocation);
    if (!downloadsPath.isEmpty() && fs::exists(fs::path(downloadsPath.toStdWString()))) {
        m_selectedRoot = fs::path(downloadsPath.toStdWString());
    } else {
        m_selectedRoot = fs::current_path();
    }
    updateRootPathDisplay();

    // Set journal file location
    m_journal.setJournalFilePath(m_selectedRoot / ".sorted_journal.json");
    m_pageHistory->refreshHistory();
}

MainWindow::~MainWindow() {
    if (m_scanThread && m_scanThread->isRunning()) {
        if (m_scanWorker) m_scanWorker->cancel();
        m_scanThread->quit();
        m_scanThread->wait();
    }
    if (m_execThread && m_execThread->isRunning()) {
        if (m_execWorker) m_execWorker->cancel();
        m_execThread->quit();
        m_execThread->wait();
    }
}

void MainWindow::setupUi() {
    setWindowTitle("SORTED — Safe File Organizer");
    resize(1180, 760);
    setMinimumSize(960, 620);

    auto centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto rootLayout = new QHBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // ---------------------------------------------------------
    // 1. Left Collapsible Sidebar
    // ---------------------------------------------------------
    m_sidebarWidget = new QWidget(this);
    m_sidebarWidget->setObjectName("sidebarWidget");
    m_sidebarWidget->setFixedWidth(240);

    auto sidebarLayout = new QVBoxLayout(m_sidebarWidget);
    sidebarLayout->setContentsMargins(16, 20, 16, 16);
    sidebarLayout->setSpacing(10);

    // Header with Logo, Dot, and Collapse Button
    auto logoRow = new QHBoxLayout();
    logoRow->setContentsMargins(4, 0, 4, 0);
    logoRow->setSpacing(6);

    m_lblLogo = new QLabel("sorted.", m_sidebarWidget);
    m_lblLogo->setStyleSheet("font-size: 22px; font-weight: 700; letter-spacing: -0.5px; border: none;");
    logoRow->addWidget(m_lblLogo);

    m_lblDot = new QLabel("●", m_sidebarWidget);
    m_lblDot->setStyleSheet("font-size: 10px; color: #3478F6; margin-top: 4px; border: none;");
    logoRow->addWidget(m_lblDot);
    logoRow->addStretch();

    m_btnCollapse = new QPushButton("◀", m_sidebarWidget);
    m_btnCollapse->setObjectName("btnSubtle");
    m_btnCollapse->setFixedSize(28, 28);
    m_btnCollapse->setToolTip("Collapse / Expand Sidebar");
    connect(m_btnCollapse, &QPushButton::clicked, this, &MainWindow::toggleSidebar);
    logoRow->addWidget(m_btnCollapse);
    sidebarLayout->addLayout(logoRow);

    m_lblTagline = new QLabel("Safe preview organizer", m_sidebarWidget);
    m_lblTagline->setStyleSheet("font-size: 11px; margin-bottom: 6px; padding-left: 4px; border: none;");
    sidebarLayout->addWidget(m_lblTagline);

    // Navigation List (6 Items)
    m_navList = new QListWidget(m_sidebarWidget);
    m_navList->setObjectName("navList");
    m_navList->addItem(" 📊  Overview");
    m_navList->addItem(" 📄  All Files");
    m_navList->addItem(" 📁  Categories");
    m_navList->addItem(" ♊  Duplicates");
    m_navList->addItem(" ⚡  Organization Plan");
    m_navList->addItem(" 🕒  History & Journal");
    m_navList->setCurrentRow(0);
    connect(m_navList, &QListWidget::currentRowChanged, this, &MainWindow::onNavigationChanged);
    sidebarLayout->addWidget(m_navList, 1);

    // Target Folder Card
    m_pathBox = new QWidget(m_sidebarWidget);
    m_pathBox->setObjectName("cardFolder");
    auto pathBoxLayout = new QVBoxLayout(m_pathBox);
    pathBoxLayout->setContentsMargins(10, 10, 10, 10);
    pathBoxLayout->setSpacing(4);

    auto lblTarget = new QLabel("Target Folder", m_pathBox);
    lblTarget->setStyleSheet("font-size: 11px; font-weight: 600; border: none;");
    pathBoxLayout->addWidget(lblTarget);

    m_lblCurrentPath = new QLabel("-", m_pathBox);
    m_lblCurrentPath->setStyleSheet("font-size: 11px; border: none;");
    m_lblCurrentPath->setWordWrap(true);
    pathBoxLayout->addWidget(m_lblCurrentPath);

    m_btnSelectFolder = new QPushButton("Change Folder", m_pathBox);
    m_btnSelectFolder->setObjectName("btnChangeFolder");
    connect(m_btnSelectFolder, &QPushButton::clicked, this, &MainWindow::onSelectFolderClicked);
    pathBoxLayout->addWidget(m_btnSelectFolder);
    sidebarLayout->addWidget(m_pathBox);

    // Theme Switcher Button at bottom
    m_btnTheme = new QPushButton("🌙  Dark Mode", m_sidebarWidget);
    m_btnTheme->setObjectName("btnTheme");
    m_btnTheme->setFixedHeight(34);
    connect(m_btnTheme, &QPushButton::clicked, this, &MainWindow::toggleTheme);
    sidebarLayout->addWidget(m_btnTheme);

    rootLayout->addWidget(m_sidebarWidget);

    // ---------------------------------------------------------
    // 2. Right Content Area (Header + Stacked Pages)
    // ---------------------------------------------------------
    auto contentArea = new QWidget(this);
    contentArea->setObjectName("contentArea");
    auto contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    // Top Bar
    auto topBar = new QWidget(contentArea);
    topBar->setObjectName("topBar");
    auto topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(36, 14, 36, 14);
    topBarLayout->setSpacing(16);

    auto titleBox = new QVBoxLayout();
    titleBox->setSpacing(2);
    m_lblHeaderTitle = new QLabel("Overview", topBar);
    m_lblHeaderTitle->setStyleSheet("font-size: 18px; font-weight: 700; letter-spacing: -0.3px; border: none;");
    titleBox->addWidget(m_lblHeaderTitle);

    m_lblHeaderSub = new QLabel("Explore folder structure, metrics and distribution", topBar);
    m_lblHeaderSub->setStyleSheet("font-size: 12px; border: none;");
    titleBox->addWidget(m_lblHeaderSub);
    topBarLayout->addLayout(titleBox);

    topBarLayout->addStretch();

    m_lblStatus = new QLabel("Ready", topBar);
    m_lblStatus->setStyleSheet("font-size: 12px; border: none;");
    topBarLayout->addWidget(m_lblStatus);

    m_progressBar = new QProgressBar(topBar);
    m_progressBar->setFixedWidth(160);
    m_progressBar->setFixedHeight(6);
    m_progressBar->setTextVisible(false);
    m_progressBar->setVisible(false);
    topBarLayout->addWidget(m_progressBar);

    m_btnCancel = new QPushButton("Cancel", topBar);
    m_btnCancel->setObjectName("btnSubtle");
    m_btnCancel->setVisible(false);
    connect(m_btnCancel, &QPushButton::clicked, this, &MainWindow::onCancelClicked);
    topBarLayout->addWidget(m_btnCancel);

    m_btnScan = new QPushButton("Scan Workspace", topBar);
    m_btnScan->setObjectName("btnPrimary");
    m_btnScan->setFixedHeight(36);
    connect(m_btnScan, &QPushButton::clicked, this, &MainWindow::onStartScanClicked);
    topBarLayout->addWidget(m_btnScan);

    contentLayout->addWidget(topBar);

    // Stacked Pages
    m_stackWidget = new QStackedWidget(contentArea);
    m_pageOverview = new OverviewPage(this);
    m_pageFiles = new FilesPage(this);
    m_pageCategories = new CategoriesPage(this);
    m_pageDuplicates = new DuplicatesPage(this);
    m_pageOrganize = new OrganizePage(this);
    m_pageHistory = new HistoryPage(&m_journal, &m_opService, this);

    m_stackWidget->addWidget(m_pageOverview);   // index 0
    m_stackWidget->addWidget(m_pageFiles);      // index 1
    m_stackWidget->addWidget(m_pageCategories); // index 2
    m_stackWidget->addWidget(m_pageDuplicates); // index 3
    m_stackWidget->addWidget(m_pageOrganize);   // index 4
    m_stackWidget->addWidget(m_pageHistory);    // index 5

    contentLayout->addWidget(m_stackWidget, 1);
    rootLayout->addWidget(contentArea, 1);

    // Page signal wiring
    connect(m_pageOverview, &OverviewPage::scanRequested, this, &MainWindow::onStartScanClicked);
    connect(m_pageOverview, &OverviewPage::reviewPlanRequested, this, &MainWindow::onReviewPlanRequested);
    connect(m_pageCategories, &CategoriesPage::categorySelected, this, &MainWindow::onCategoryFilterRequested);
    connect(m_pageOrganize, &OrganizePage::applyApprovedMovesRequested, this, &MainWindow::onApplyApprovedMoves);
    connect(m_pageHistory, &HistoryPage::historyChanged, [this]() {
        m_pageHistory->refreshHistory();
    });

    // Global shortcut Ctrl+K
    m_shortcutK = new QShortcut(QKeySequence("Ctrl+K"), this);
    connect(m_shortcutK, &QShortcut::activated, this, &MainWindow::focusSearch);
}

void MainWindow::toggleSidebar() {
    m_sidebarCollapsed = !m_sidebarCollapsed;
    if (m_sidebarCollapsed) {
        m_sidebarWidget->setFixedWidth(68);
        m_lblLogo->setText("s.");
        m_lblTagline->setVisible(false);
        m_pathBox->setVisible(false);
        m_btnCollapse->setText("▶");
        m_btnTheme->setText(Theme::isDark() ? "☀️" : "🌙");
        m_navList->item(0)->setText("📊");
        m_navList->item(1)->setText("📄");
        m_navList->item(2)->setText("📁");
        m_navList->item(3)->setText("♊");
        m_navList->item(4)->setText("⚡");
        m_navList->item(5)->setText("🕒");
    } else {
        m_sidebarWidget->setFixedWidth(240);
        m_lblLogo->setText("sorted.");
        m_lblTagline->setVisible(true);
        m_pathBox->setVisible(true);
        m_btnCollapse->setText("◀");
        m_btnTheme->setText(Theme::isDark() ? "☀️  Light Mode" : "🌙  Dark Mode");
        m_navList->item(0)->setText(" 📊  Overview");
        m_navList->item(1)->setText(" 📄  All Files");
        m_navList->item(2)->setText(" 📁  Categories");
        m_navList->item(3)->setText(" ♊  Duplicates");
        m_navList->item(4)->setText(" ⚡  Organization Plan");
        m_navList->item(5)->setText(" 🕒  History & Journal");
    }
}

void MainWindow::toggleTheme() {
    Theme::toggleTheme();
    applyTheme();
}

void MainWindow::focusSearch() {
    m_navList->setCurrentRow(1);
    m_pageFiles->focusSearch();
}

void MainWindow::applyTheme() {
    ColorTheme theme = Theme::currentTheme;
    Theme::applyTheme(theme);

    m_sidebarWidget->setStyleSheet(Theme::getSidebarStyleSheet(theme));

    QString textPri = Theme::getTextPrimary();
    QString textSec = Theme::getTextSecondary();
    QString textMuted = Theme::isDark() ? Theme::DarkTextMuted : Theme::LightTextMuted;
    QString surface = Theme::getSurface();
    QString border = Theme::getBorder();

    m_lblLogo->setStyleSheet(QString("font-size: 22px; font-weight: 700; color: %1; letter-spacing: -0.5px; border: none;").arg(textPri));
    m_lblTagline->setStyleSheet(QString("font-size: 11px; color: %1; margin-bottom: 6px; padding-left: 4px; border: none;").arg(textMuted));
    m_lblHeaderTitle->setStyleSheet(QString("font-size: 18px; font-weight: 700; color: %1; letter-spacing: -0.3px; border: none;").arg(textPri));
    m_lblHeaderSub->setStyleSheet(QString("font-size: 12px; color: %1; border: none;").arg(textSec));
    m_lblStatus->setStyleSheet(QString("font-size: 12px; color: %1; border: none;").arg(textSec));

    if (m_sidebarCollapsed) {
        m_btnTheme->setText(Theme::isDark() ? "☀️" : "🌙");
    } else {
        m_btnTheme->setText(Theme::isDark() ? "☀️  Light Mode" : "🌙  Dark Mode");
    }

    // Update all sub-pages
    m_pageOverview->applyTheme();
    m_pageFiles->applyTheme();
    m_pageCategories->applyTheme();
    m_pageDuplicates->applyTheme();
    m_pageOrganize->applyTheme();
    m_pageHistory->applyTheme();
}

void MainWindow::updateRootPathDisplay() {
    std::string pathStr = m_selectedRoot.string();
    if (pathStr.length() > 28) {
        pathStr = "..." + pathStr.substr(pathStr.length() - 25);
    }
    m_lblCurrentPath->setText(QString::fromStdString(pathStr));
    m_lblCurrentPath->setToolTip(QString::fromStdWString(m_selectedRoot.wstring()));
}

void MainWindow::onSelectFolderClicked() {
    QString selected = QFileDialog::getExistingDirectory(
        this, "Select Target Folder", QString::fromStdWString(m_selectedRoot.wstring())
    );
    if (!selected.isEmpty()) {
        m_selectedRoot = fs::path(selected.toStdWString());
        updateRootPathDisplay();
        m_journal.setJournalFilePath(m_selectedRoot / ".sorted_journal.json");
        m_pageHistory->refreshHistory();
        onStartScanClicked();
    }
}

void MainWindow::onNavigationChanged(int index) {
    m_stackWidget->setCurrentIndex(index);
    const char* titles[][2] = {
        {"Overview", "Workspace summary, storage footprint, and category breakdown"},
        {"All Files", "Interactive catalogue of all indexed files with Merge Sort and search"},
        {"Categories", "Extension taxonomy and planned folder destinations"},
        {"Duplicates", "Exact duplicate groups detected by streaming SHA-256"},
        {"Organization Plan", "Review proposed safe moves and collision handling"},
        {"History & Journal", "Audit trail of executed moves with instant rollback"}
    };
    if (index >= 0 && index < 6) {
        m_lblHeaderTitle->setText(titles[index][0]);
        m_lblHeaderSub->setText(titles[index][1]);
    }
}

void MainWindow::onCategoryFilterRequested(const QString& category) {
    m_navList->setCurrentRow(1);
    m_pageFiles->filterByCategory(category);
}

void MainWindow::onReviewPlanRequested() {
    m_navList->setCurrentRow(4);
}

void MainWindow::onStartScanClicked() {
    if (m_scanThread && m_scanThread->isRunning()) return;

    m_btnScan->setEnabled(false);
    m_btnCancel->setVisible(true);
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0);
    m_lblStatus->setText("Scanning directory recursively...");

    m_scanThread = new QThread(this);
    m_scanWorker = new ScanWorker(m_selectedRoot, false);
    m_scanWorker->moveToThread(m_scanThread);

    connect(m_scanThread, &QThread::started, m_scanWorker, &ScanWorker::run);
    connect(m_scanWorker, &ScanWorker::progress, this, &MainWindow::onScanProgress);
    connect(m_scanWorker, &ScanWorker::hashProgress, this, &MainWindow::onHashProgress);
    connect(m_scanWorker, &ScanWorker::finished, this, &MainWindow::onScanFinished);
    connect(m_scanWorker, &ScanWorker::error, this, &MainWindow::onScanError);

    connect(m_scanWorker, &ScanWorker::finished, m_scanThread, &QThread::quit);
    connect(m_scanWorker, &ScanWorker::finished, m_scanWorker, &QObject::deleteLater);
    connect(m_scanThread, &QThread::finished, m_scanThread, &QObject::deleteLater);

    m_scanThread->start();
}

void MainWindow::onCancelClicked() {
    if (m_scanWorker) m_scanWorker->cancel();
    if (m_execWorker) m_execWorker->cancel();
    m_lblStatus->setText("Cancellation requested...");
}

void MainWindow::onScanProgress(int filesCount, const QString& /*currentFolder*/) {
    m_lblStatus->setText(QString("Scanning... Discovered %1 files").arg(filesCount));
}

void MainWindow::onHashProgress(int processed, int total, const QString& currentFile) {
    m_lblStatus->setText(QString("Hashing duplicates [%1/%2]: %3").arg(processed).arg(total).arg(currentFile));
}

void MainWindow::onScanFinished(const ScanResult& result) {
    m_btnScan->setEnabled(true);
    m_btnCancel->setVisible(false);
    m_progressBar->setVisible(false);
    m_lblStatus->setText(QString("Scan complete: %1 files in %2 ms")
        .arg(result.files.size())
        .arg(result.stats.scanDurationMs.count()));

    m_lastScanResult = result;

    m_pageOverview->updateStats(result);
    m_pageFiles->setFiles(result.files);
    m_pageCategories->setFiles(result.files);
    m_pageDuplicates->setDuplicateGroups(result.duplicateGroups);

    m_currentPlan = m_planner.generatePlan(m_selectedRoot, result.files);
    m_pageOrganize->setPlan(m_selectedRoot, m_currentPlan);
}

void MainWindow::onScanError(const QString& errorMessage) {
    m_btnScan->setEnabled(true);
    m_btnCancel->setVisible(false);
    m_progressBar->setVisible(false);
    m_lblStatus->setText("Scan encountered an error");
    QMessageBox::warning(this, "Scan Warning", errorMessage);
}

void MainWindow::onApplyApprovedMoves(const std::vector<MoveOperation>& approvedMoves) {
    if (approvedMoves.empty()) return;

    m_btnScan->setEnabled(false);
    m_btnCancel->setVisible(true);
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, static_cast<int>(approvedMoves.size()));
    m_progressBar->setValue(0);
    m_lblStatus->setText("Applying safe file reorganization...");

    m_execThread = new QThread(this);
    m_execWorker = new ExecutionWorker(&m_opService, approvedMoves);
    m_execWorker->moveToThread(m_execThread);

    connect(m_execThread, &QThread::started, m_execWorker, &ExecutionWorker::run);
    connect(m_execWorker, &ExecutionWorker::moveProgress, this, &MainWindow::onExecutionProgress);
    connect(m_execWorker, &ExecutionWorker::finished, this, &MainWindow::onExecutionFinished);

    connect(m_execWorker, &ExecutionWorker::finished, m_execThread, &QThread::quit);
    connect(m_execWorker, &ExecutionWorker::finished, m_execWorker, &QObject::deleteLater);
    connect(m_execThread, &QThread::finished, m_execThread, &QObject::deleteLater);

    m_execThread->start();
}

void MainWindow::onExecutionProgress(int completed, int total, const QString& currentFileName) {
    m_progressBar->setValue(completed);
    m_lblStatus->setText(QString("Moving [%1/%2]: %3").arg(completed).arg(total).arg(currentFileName));
}

void MainWindow::onExecutionFinished(const Operations::ExecutionSummary& summary, const std::vector<MoveOperation>& /*finalMoves*/) {
    m_btnScan->setEnabled(true);
    m_btnCancel->setVisible(false);
    m_progressBar->setVisible(false);
    m_lblStatus->setText(QString("Organization complete: %1 succeeded, %2 failed in %3 ms")
        .arg(summary.successCount)
        .arg(summary.failedCount)
        .arg(summary.durationMs.count()));

    m_pageHistory->refreshHistory();
    m_navList->setCurrentRow(5);

    QMessageBox::information(
        this, "Execution Complete",
        QString("File organization batch completed:\n\n"
                "• Succeeded: %1 files\n"
                "• Failed / Skipped: %2 files\n"
                "• Time elapsed: %3 ms\n\n"
                "All operations have been safely journaled.")
            .arg(summary.successCount)
            .arg(summary.failedCount)
            .arg(summary.durationMs.count())
    );
}

} // namespace Ui
