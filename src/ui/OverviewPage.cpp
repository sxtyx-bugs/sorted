#include "OverviewPage.h"
#include <QHeaderView>
#include <QMessageBox>

namespace Ui {

namespace {

QString formatBytes(std::uintmax_t bytes) {
    if (bytes < 1024) return QString::number(bytes) + " B";
    double kb = bytes / 1024.0;
    if (kb < 1024) return QString::number(kb, 'f', 1) + " KB";
    double mb = kb / 1024.0;
    if (mb < 1024) return QString::number(mb, 'f', 1) + " MB";
    double gb = mb / 1024.0;
    return QString::number(gb, 'f', 2) + " GB";
}

} // namespace

OverviewPage::OverviewPage(QWidget* parent)
    : QWidget(parent) {
    setupUi();
    applyTheme();
    showEmptyState();
}

void OverviewPage::setupUi() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    m_pageStack = new QStackedWidget(this);

    // =========================================================
    // 1. Onboarding / First-Launch Empty State
    // =========================================================
    m_emptyWidget = new QWidget(this);
    auto emptyLayout = new QVBoxLayout(m_emptyWidget);
    emptyLayout->setContentsMargins(48, 64, 48, 64);
    emptyLayout->setSpacing(16);
    emptyLayout->setAlignment(Qt::AlignCenter);

    m_lblEmptyIcon = new QLabel("📁", m_emptyWidget);
    m_lblEmptyIcon->setStyleSheet("font-size: 48px; margin-bottom: 8px; border: none;");
    m_lblEmptyIcon->setAlignment(Qt::AlignCenter);
    emptyLayout->addWidget(m_lblEmptyIcon);

    m_lblEmptyHeading = new QLabel("Choose a workspace", m_emptyWidget);
    m_lblEmptyHeading->setStyleSheet("font-size: 20px; font-weight: 600; letter-spacing: -0.3px; border: none;");
    m_lblEmptyHeading->setAlignment(Qt::AlignCenter);
    emptyLayout->addWidget(m_lblEmptyHeading);

    m_lblEmptySub = new QLabel("Select a folder to scan and preview its organized structure.", m_emptyWidget);
    m_lblEmptySub->setStyleSheet("font-size: 13px; max-width: 380px; border: none;");
    m_lblEmptySub->setAlignment(Qt::AlignCenter);
    m_lblEmptySub->setWordWrap(true);
    emptyLayout->addWidget(m_lblEmptySub);

    auto btnRow = new QHBoxLayout();
    btnRow->setSpacing(12);
    btnRow->setAlignment(Qt::AlignCenter);

    m_btnEmptyChoose = new QPushButton("Choose Folder", m_emptyWidget);
    m_btnEmptyChoose->setObjectName("btnPrimary");
    m_btnEmptyChoose->setFixedHeight(36);
    connect(m_btnEmptyChoose, &QPushButton::clicked, this, &OverviewPage::chooseFolderRequested);
    btnRow->addWidget(m_btnEmptyChoose);

    m_btnEmptyLearn = new QPushButton("Learn how it works", m_emptyWidget);
    m_btnEmptyLearn->setObjectName("btnSubtle");
    m_btnEmptyLearn->setFixedHeight(36);
    connect(m_btnEmptyLearn, &QPushButton::clicked, this, &OverviewPage::onLearnHowItWorksClicked);
    btnRow->addWidget(m_btnEmptyLearn);

    emptyLayout->addLayout(btnRow);
    m_pageStack->addWidget(m_emptyWidget); // Index 0

    // =========================================================
    // 2. Populated Results View
    // =========================================================
    m_resultsWidget = new QWidget(this);
    auto resultsLayout = new QVBoxLayout(m_resultsWidget);
    resultsLayout->setContentsMargins(36, 24, 36, 24);
    resultsLayout->setSpacing(18);

    // Clean Metric Tiles Row (Restrained Apple/Notion style)
    auto metricsRow = new QHBoxLayout();
    metricsRow->setSpacing(12);

    metricsRow->addWidget(createMetricTile("Total Files", m_lblFilesCount, "Indexed recursively"));
    metricsRow->addWidget(createMetricTile("Storage Volume", m_lblTotalSize, "Accumulated storage"));
    metricsRow->addWidget(createMetricTile("Categories", m_lblCategoriesCount, "Classified types"));
    metricsRow->addWidget(createMetricTile("Duplicates", m_lblDuplicatesCount, "SHA-256 byte identical"));
    resultsLayout->addLayout(metricsRow);

    // Telemetry strip
    m_cardScanMeta = new QFrame(m_resultsWidget);
    m_cardScanMeta->setObjectName("statCard");
    auto metaLayout = new QHBoxLayout(m_cardScanMeta);
    metaLayout->setContentsMargins(16, 10, 16, 10);
    metaLayout->setSpacing(12);

    m_lblScanMeta = new QLabel(m_cardScanMeta);
    m_lblScanMeta->setStyleSheet("font-size: 12px; border: none;");
    metaLayout->addWidget(m_lblScanMeta, 1);

    m_btnQuickPlan = new QPushButton("Review Plan →", m_cardScanMeta);
    m_btnQuickPlan->setObjectName("btnSubtle");
    connect(m_btnQuickPlan, &QPushButton::clicked, this, &OverviewPage::reviewPlanRequested);
    metaLayout->addWidget(m_btnQuickPlan);
    resultsLayout->addWidget(m_cardScanMeta);

    // Category Distribution Section
    m_lblSectionTitle = new QLabel("Category Distribution", m_resultsWidget);
    m_lblSectionTitle->setStyleSheet("font-size: 14px; font-weight: 600; margin-top: 4px; border: none;");
    resultsLayout->addWidget(m_lblSectionTitle);

    m_tableCategories = new QTableWidget(m_resultsWidget);
    m_tableCategories->setColumnCount(4);
    m_tableCategories->setHorizontalHeaderLabels({"Category", "Files", "Storage Volume", "% Share"});
    m_tableCategories->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_tableCategories->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_tableCategories->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_tableCategories->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_tableCategories->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableCategories->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableCategories->verticalHeader()->setDefaultSectionSize(44);
    m_tableCategories->verticalHeader()->setVisible(false);
    resultsLayout->addWidget(m_tableCategories, 1);

    m_pageStack->addWidget(m_resultsWidget); // Index 1

    mainLayout->addWidget(m_pageStack);
}

QFrame* OverviewPage::createMetricTile(const QString& title, QLabel*& valueLabel, const QString& subtitle) {
    auto card = new QFrame(m_resultsWidget);
    card->setObjectName("statCard");
    auto layout = new QVBoxLayout(card);
    layout->setContentsMargins(16, 14, 16, 14);
    layout->setSpacing(4);

    auto lblTitle = new QLabel(title, card);
    lblTitle->setStyleSheet("font-size: 11px; font-weight: 600; text-transform: uppercase; letter-spacing: 0.5px; border: none;");

    valueLabel = new QLabel("-", card);
    valueLabel->setStyleSheet("font-size: 22px; font-weight: 600; font-variant-numeric: tabular-nums; border: none;");

    auto lblSub = new QLabel(subtitle, card);
    lblSub->setStyleSheet("font-size: 11px; border: none;");

    layout->addWidget(lblTitle);
    layout->addWidget(valueLabel);
    layout->addWidget(lblSub);
    return card;
}

void OverviewPage::showEmptyState() {
    m_hasScannedData = false;
    m_pageStack->setCurrentIndex(0);
}

void OverviewPage::applyTheme() {
    ColorTheme theme = Theme::currentTheme;
    m_tableCategories->setStyleSheet(Theme::getTableStyleSheet(theme));
    m_cardScanMeta->setStyleSheet(Theme::getCardStyleSheet(theme));

    QString textPri = Theme::getTextPrimary();
    QString textSec = Theme::getTextSecondary();
    QString textMuted = Theme::getTextMuted();

    // Empty state labels
    m_lblEmptyHeading->setStyleSheet(QString("font-size: 20px; font-weight: 600; color: %1; letter-spacing: -0.3px; border: none;").arg(textPri));
    m_lblEmptySub->setStyleSheet(QString("font-size: 13px; color: %1; max-width: 380px; border: none;").arg(textSec));

    // Results state labels
    m_lblSectionTitle->setStyleSheet(QString("font-size: 14px; font-weight: 600; color: %1; margin-top: 4px; border: none;").arg(textPri));
    m_lblScanMeta->setStyleSheet(QString("font-size: 12px; color: %1; border: none;").arg(textSec));

    // Refresh category table items colors
    for (int r = 0; r < m_tableCategories->rowCount(); ++r) {
        if (auto it = m_tableCategories->item(r, 0)) it->setForeground(QColor(textPri));
        if (auto it = m_tableCategories->item(r, 1)) it->setForeground(QColor(textPri));
        if (auto it = m_tableCategories->item(r, 2)) it->setForeground(QColor(textSec));
        if (auto it = m_tableCategories->item(r, 3)) it->setForeground(QColor(textSec));
    }
}

void OverviewPage::updateStats(const ScanResult& result) {
    m_lastResult = result;
    m_hasScannedData = true;

    m_lblFilesCount->setText(QString::number(result.stats.totalFilesScanned));
    m_lblTotalSize->setText(formatBytes(result.stats.totalSizeBytes));
    m_lblCategoriesCount->setText(QString::number(result.stats.categoryCounts.size()));

    // Correct grammatical inconsistencies: "1 group" vs "X groups"
    std::size_t dupGroups = result.stats.duplicateGroupCount;
    if (dupGroups == 1) {
        m_lblDuplicatesCount->setText("1 group");
    } else {
        m_lblDuplicatesCount->setText(QString("%1 groups").arg(dupGroups));
    }

    m_lblScanMeta->setText(
        QString("Scan: %1 ms  |  Classification: %2 ms  |  SHA-256 Hashing: %3 ms (%4 hashed)")
            .arg(result.stats.scanDurationMs.count())
            .arg(result.stats.classifyDurationMs.count())
            .arg(result.stats.hashDurationMs.count())
            .arg(formatBytes(result.stats.totalBytesHashed))
    );

    // Populate category distribution table
    m_tableCategories->setRowCount(0);
    std::size_t totalFiles = result.stats.totalFilesScanned > 0 ? result.stats.totalFilesScanned : 1;

    QString textPri = Theme::getTextPrimary();
    QString textSec = Theme::getTextSecondary();

    int row = 0;
    for (const auto& pair : result.stats.categoryCounts) {
        m_tableCategories->insertRow(row);

        auto itemCat = new QTableWidgetItem(QString::fromStdString(pair.first));
        itemCat->setForeground(QColor(textPri));

        auto itemCount = new QTableWidgetItem(QString::number(pair.second));
        itemCount->setForeground(QColor(textPri));

        auto itSize = result.stats.categorySizes.find(pair.first);
        std::uintmax_t sizeBytes = (itSize != result.stats.categorySizes.end()) ? itSize->second : 0;
        auto itemSize = new QTableWidgetItem(formatBytes(sizeBytes));
        itemSize->setForeground(QColor(textSec));

        double pct = (static_cast<double>(pair.second) / totalFiles) * 100.0;
        auto itemPct = new QTableWidgetItem(QString::number(pct, 'f', 1) + "%");
        itemPct->setForeground(QColor(textSec));

        m_tableCategories->setItem(row, 0, itemCat);
        m_tableCategories->setItem(row, 1, itemCount);
        m_tableCategories->setItem(row, 2, itemSize);
        m_tableCategories->setItem(row, 3, itemPct);
        row++;
    }

    m_pageStack->setCurrentIndex(1); // Switch to results view
}

void OverviewPage::onLearnHowItWorksClicked() {
    QMessageBox::information(
        this, "How SORTED Works",
        "<h3>SORTED — Safe Preview-First Organization</h3>"
        "<p><b>1. Select a folder:</b> Point SORTED to any directory (such as Downloads).</p>"
        "<p><b>2. Non-destructive scan:</b> SORTED recursively classifies files and identifies duplicates using streaming SHA-256.</p>"
        "<p><b>3. Preview & Review:</b> Every move is presented in the Organization Plan. Name collisions are auto-resolved with suffixes (e.g. <code>report (1).pdf</code>). Nothing is ever overwritten.</p>"
        "<p><b>4. Full Undo:</b> Operations are logged to <code>.sorted_journal.json</code> and can be reversed with a single click.</p>"
    );
}

} // namespace Ui
