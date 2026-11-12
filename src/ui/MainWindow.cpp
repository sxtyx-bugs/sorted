#include "MainWindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>
#include <QStandardPaths>
#include <QSettings>
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

    // Check previously selected workspace via QSettings
    loadSettings();
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
    setWindowTitle("SORTED — Safe Desktop File Organizer");
    resize(1180, 760);
    setMinimumSize(960, 620);

    auto centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    auto rootLayout = new QHBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // =========================================================
    // 1. Left Minimal Navigation Sidebar (Liquid Glass Material)
    // =========================================================
    m_sidebarWidget = new Glass::GlassSidebar(this);
    m_sidebarWidget->setObjectName("sidebarWidget");

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
    m_lblDot->setStyleSheet("font-size: 10px; color: #1D1D1F; margin-top: 4px; border: none;");
    logoRow->addWidget(m_lblDot);
    logoRow->addStretch();

    m_btnCollapse = new Glass::GlassButton("◀", Glass::ButtonVariant::IconOnly, m_sidebarWidget);
    m_btnCollapse->setObjectName("btnIconButton");
    m_btnCollapse->setToolTip("Collapse sidebar");
    connect(m_btnCollapse, &QPushButton::clicked, this, &MainWindow::toggleSidebar);
    logoRow->addWidget(m_btnCollapse);
    sidebarLayout->addLayout(logoRow);

    m_lblTagline = new QLabel("Safe preview organizer", m_sidebarWidget);
    m_lblTagline->setStyleSheet("font-size: 11px; margin-bottom: 6px; padding-left: 4px; border: none;");
    sidebarLayout->addWidget(m_lblTagline);

    // Navigation List with Custom Glass Item Delegate
    m_navList = new QListWidget(m_sidebarWidget);
    m_navList->setObjectName("navList");
    m_navDelegate = new Glass::GlassNavigationItem(m_navList, [this]() { return m_sidebarCollapsed; });
    m_navList->setItemDelegate(m_navDelegate);
    m_navList->addItem(" ⊞  Overview");
    m_navList->addItem(" ▤  All Files");
    m_navList->addItem(" 📁  Categories");
    m_navList->addItem(" ⧉  Duplicates");
    m_navList->addItem(" ⚡  Organization Plan");
    m_navList->addItem(" ⏱  History & Journal");
    m_navList->setCurrentRow(0);
    connect(m_navList, &QListWidget::currentRowChanged, this, &MainWindow::onNavigationChanged);
    sidebarLayout->addWidget(m_navList, 1);

    // Theme Switcher Glass Button at bottom
    m_btnTheme = new Glass::GlassButton("🌙  Dark Mode", Glass::ButtonVariant::Subtle, m_sidebarWidget);
    m_btnTheme->setObjectName("btnTheme");
    m_btnTheme->setFixedHeight(34);
    connect(m_btnTheme, &QPushButton::clicked, this, &MainWindow::toggleTheme);
    sidebarLayout->addWidget(m_btnTheme);

    rootLayout->addWidget(m_sidebarWidget);

    // =========================================================
    // 2. Right Content Area (Persistent Top Toolbar + Stack)
    // =========================================================
    auto contentArea = new QWidget(this);
    contentArea->setObjectName("contentArea");
    auto contentLayout = new QVBoxLayout(contentArea);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(0);

    // Persistent Top Liquid Glass Toolbar with Folder Selector
    m_topBar = new Glass::GlassToolbar(contentArea);
    m_topBar->setObjectName("topBar");
    auto topBarLayout = new QHBoxLayout(m_topBar);
    topBarLayout->setContentsMargins(28, 12, 28, 12);
    topBarLayout->setSpacing(14);

    // Primary Folder Selector Group (Glass Panel Capsule)
    auto folderPill = new Glass::GlassPanel(m_topBar);
    folderPill->setObjectName("folderPill");
    folderPill->setCornerRadius(10);
    auto folderGroup = new QHBoxLayout(folderPill);
    folderGroup->setContentsMargins(10, 4, 8, 4);
    folderGroup->setSpacing(10);

    m_lblFolderIcon = new QLabel("📁", folderPill);
    m_lblFolderIcon->setStyleSheet("font-size: 16px; border: none;");
    folderGroup->addWidget(m_lblFolderIcon);

    m_lblFolderName = new QLabel("No folder selected", folderPill);
    m_lblFolderName->setStyleSheet("font-size: 13px; font-weight: 600; border: none;");
    folderGroup->addWidget(m_lblFolderName);

    m_lblFolderPath = new QLabel("", folderPill);
    m_lblFolderPath->setStyleSheet("font-size: 11px; color: #777A73; border: none;");
    folderGroup->addWidget(m_lblFolderPath);

    m_btnChangeFolder = new Glass::GlassButton("Choose Folder", Glass::ButtonVariant::Subtle, folderPill);
    m_btnChangeFolder->setObjectName("btnSubtle");
    m_btnChangeFolder->setToolTip("Choose a different folder to organize");
    connect(m_btnChangeFolder, &QPushButton::clicked, this, &MainWindow::onSelectFolderClicked);
    folderGroup->addWidget(m_btnChangeFolder);

    topBarLayout->addWidget(folderPill);
    topBarLayout->addStretch();

    // Status, Progress, and Scan Action
    m_lblStatus = new QLabel("Ready", m_topBar);
    m_lblStatus->setStyleSheet("font-size: 12px; border: none;");
    topBarLayout->addWidget(m_lblStatus);

    m_progressBar = new QProgressBar(m_topBar);
    m_progressBar->setFixedWidth(140);
    m_progressBar->setFixedHeight(6);
    m_progressBar->setTextVisible(false);
    m_progressBar->setVisible(false);
    topBarLayout->addWidget(m_progressBar);

    m_btnCancel = new Glass::GlassButton("Cancel", Glass::ButtonVariant::Subtle, m_topBar);
    m_btnCancel->setObjectName("btnSubtle");
    m_btnCancel->setVisible(false);
    connect(m_btnCancel, &QPushButton::clicked, this, &MainWindow::onCancelClicked);
    topBarLayout->addWidget(m_btnCancel);

    m_btnScan = new Glass::GlassButton("Scan Workspace", Glass::ButtonVariant::Primary, m_topBar);
    m_btnScan->setObjectName("btnPrimary");
    m_btnScan->setFixedHeight(34);
    m_btnScan->setEnabled(false); // Enabled when a valid folder is selected
    connect(m_btnScan, &QPushButton::clicked, this, &MainWindow::onStartScanClicked);
    topBarLayout->addWidget(m_btnScan);

    contentLayout->addWidget(m_topBar);

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

    // Wire Onboarding / Overview signals
    connect(m_pageOverview, &OverviewPage::chooseFolderRequested, this, &MainWindow::onSelectFolderClicked);
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

void MainWindow::loadSettings() {
    QSettings settings("SORTED", "SORTED");
    QString lastPath = settings.value("lastWorkspacePath").toString();

    if (!lastPath.isEmpty() && fs::exists(fs::path(lastPath.toStdWString()))) {
        m_selectedRoot = fs::path(lastPath.toStdWString());
        updateTopToolbarFolder();
        m_btnScan->setEnabled(true);
        m_lblStatus->setText("Workspace loaded. Ready to scan.");
        m_journal.setJournalFilePath(m_selectedRoot / ".sorted_journal.json");
        m_pageHistory->refreshHistory();
    } else {
        m_selectedRoot.clear();
        updateTopToolbarFolder();
        m_btnScan->setEnabled(false);
        m_lblStatus->setText("Please choose a workspace.");
        m_pageOverview->showEmptyState();
    }

    bool collapsed = settings.value("sidebarCollapsed", false).toBool();
    if (collapsed && !m_sidebarCollapsed) {
        toggleSidebar();
    }
}

void MainWindow::saveSettings() {
    QSettings settings("SORTED", "SORTED");
    if (!m_selectedRoot.empty()) {
        settings.setValue("lastWorkspacePath", QString::fromStdWString(m_selectedRoot.wstring()));
    }
    settings.setValue("sidebarCollapsed", m_sidebarCollapsed);
}

void MainWindow::updateTopToolbarFolder() {
    if (m_selectedRoot.empty() || !fs::exists(m_selectedRoot)) {
        m_lblFolderName->setText("No folder selected");
        m_lblFolderPath->setText("");
        m_lblFolderPath->setToolTip("");
        m_btnChangeFolder->setText("Choose Folder");
        m_btnScan->setEnabled(false);
        return;
    }

    std::string folderName = m_selectedRoot.filename().string();
    if (folderName.empty()) {
        folderName = m_selectedRoot.string();
    }
    m_lblFolderName->setText(QString::fromStdString(folderName));

    std::string fullPath = m_selectedRoot.string();
    std::string shortPath = fullPath;
    if (shortPath.length() > 32) {
        shortPath = "..." + shortPath.substr(shortPath.length() - 28);
    }
    m_lblFolderPath->setText(QString::fromStdString("(" + shortPath + ")"));
    m_lblFolderPath->setToolTip(QString::fromStdWString(m_selectedRoot.wstring()));
    m_btnChangeFolder->setText("Change");
    m_btnScan->setEnabled(true);
}

void MainWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);
    Glass::WindowBackdrop::apply(this, Glass::BackdropMaterial::Mica, Theme::isDark());
}

void MainWindow::toggleSidebar() {
    m_sidebarCollapsed = !m_sidebarCollapsed;
    m_sidebarWidget->setCollapsed(m_sidebarCollapsed, true);
    if (m_sidebarCollapsed) {
        m_lblLogo->setText("S•");
        m_lblTagline->setVisible(false);
        m_btnCollapse->setText("▶");
        m_btnCollapse->setToolTip("Expand sidebar");
        m_btnTheme->setText(Theme::isDark() ? "☀️" : "🌙");
        m_btnTheme->setToolTip(Theme::isDark() ? "Switch to Light Mode" : "Switch to Dark Mode");
        m_navList->item(0)->setText("⊞");
        m_navList->item(1)->setText("▤");
        m_navList->item(2)->setText("📁");
        m_navList->item(3)->setText("⧉");
        m_navList->item(4)->setText("⚡");
        m_navList->item(5)->setText("⏱");
    } else {
        m_lblLogo->setText("sorted.");
        m_lblTagline->setVisible(true);
        m_btnCollapse->setText("◀");
        m_btnCollapse->setToolTip("Collapse sidebar");
        m_btnTheme->setText(Theme::isDark() ? "☀️  Light Mode" : "🌙  Dark Mode");
        m_btnTheme->setToolTip("");
        m_navList->item(0)->setText(" ⊞  Overview");
        m_navList->item(1)->setText(" ▤  All Files");
        m_navList->item(2)->setText(" 📁  Categories");
        m_navList->item(3)->setText(" ⧉  Duplicates");
        m_navList->item(4)->setText(" ⚡  Organization Plan");
        m_navList->item(5)->setText(" ⏱  History & Journal");
    }
    m_navList->update();
    saveSettings();
}

void MainWindow::toggleTheme() {
    Theme::toggleTheme();
    applyTheme();
    Glass::WindowBackdrop::setDarkMode(this, Theme::isDark());
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
    QString textMuted = Theme::getTextMuted();

    m_lblLogo->setStyleSheet(QString("font-size: 22px; font-weight: 700; color: %1; letter-spacing: -0.5px; border: none;").arg(textPri));
    m_lblTagline->setStyleSheet(QString("font-size: 11px; color: %1; margin-bottom: 6px; padding-left: 4px; border: none;").arg(textMuted));
    m_lblFolderName->setStyleSheet(QString("font-size: 13px; font-weight: 600; color: %1; border: none;").arg(textPri));
    m_lblFolderPath->setStyleSheet(QString("font-size: 11px; color: %1; border: none;").arg(textSec));
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

void MainWindow::onSelectFolderClicked() {
    QString startDir = m_selectedRoot.empty() ?
        QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) :
        QString::fromStdWString(m_selectedRoot.wstring());

    QString selected = QFileDialog::getExistingDirectory(this, "Select Target Workspace", startDir);
    if (!selected.isEmpty()) {
        m_selectedRoot = fs::path(selected.toStdWString());
        saveSettings();
        updateTopToolbarFolder();

        m_journal.setJournalFilePath(m_selectedRoot / ".sorted_journal.json");
        m_pageHistory->refreshHistory();

        // Scan the selected folder
        onStartScanClicked();
    }
}

void MainWindow::onNavigationChanged(int index) {
    m_stackWidget->setCurrentIndex(index);
}

void MainWindow::onCategoryFilterRequested(const QString& category) {
    m_navList->setCurrentRow(1);
    m_pageFiles->filterByCategory(category);
}

void MainWindow::onReviewPlanRequested() {
    m_navList->setCurrentRow(4);
}

void MainWindow::onStartScanClicked() {
    if (m_selectedRoot.empty() || !fs::exists(m_selectedRoot)) {
        onSelectFolderClicked();
        return;
    }

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
