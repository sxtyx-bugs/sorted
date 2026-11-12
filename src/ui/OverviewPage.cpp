#include "OverviewPage.h"
#include <QHeaderView>
#include <iomanip>
#include <sstream>

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
    resetStats();
}

void OverviewPage::setupUi() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(36, 24, 36, 24);
    mainLayout->setSpacing(20);

    // Metric Cards Layout (Apple x Notion style)
    auto cardsLayout = new QHBoxLayout();
    cardsLayout->setSpacing(14);

    cardsLayout->addWidget(createStatCard("Total Files", m_lblFilesCount, "Indexed recursively"));
    cardsLayout->addWidget(createStatCard("Total Volume", m_lblTotalSize, "Accumulated storage"));
    cardsLayout->addWidget(createStatCard("Categories", m_lblCategoriesCount, "Rule-based classification"));
    cardsLayout->addWidget(createStatCard("Duplicate Groups", m_lblDuplicatesCount, "SHA-256 byte identical"));
    mainLayout->addLayout(cardsLayout);

    // Last Scan Meta Banner
    m_cardScanMeta = new QFrame(this);
    m_cardScanMeta->setObjectName("statCard");
    auto metaLayout = new QHBoxLayout(m_cardScanMeta);
    metaLayout->setContentsMargins(18, 12, 18, 12);
    metaLayout->setSpacing(12);

    m_lblScanMeta = new QLabel("Ready. Select a target folder and click 'Scan Workspace' to begin.", m_cardScanMeta);
    m_lblScanMeta->setStyleSheet("font-size: 13px; font-weight: 500;");
    metaLayout->addWidget(m_lblScanMeta, 1);

    m_btnQuickPlan = new QPushButton("Review Plan →", m_cardScanMeta);
    m_btnQuickPlan->setObjectName("btnSubtle");
    connect(m_btnQuickPlan, &QPushButton::clicked, this, &OverviewPage::reviewPlanRequested);
    metaLayout->addWidget(m_btnQuickPlan);
    mainLayout->addWidget(m_cardScanMeta);

    // Category Distribution Section
    m_lblSectionTitle = new QLabel("Category Distribution", this);
    m_lblSectionTitle->setStyleSheet("font-size: 15px; font-weight: 600; margin-top: 4px;");
    mainLayout->addWidget(m_lblSectionTitle);

    m_tableCategories = new QTableWidget(this);
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
    mainLayout->addWidget(m_tableCategories, 1);
}

QFrame* OverviewPage::createStatCard(const QString& title, QLabel*& valueLabel, const QString& subtitle) {
    auto card = new QFrame(this);
    card->setObjectName("statCard");
    auto layout = new QVBoxLayout(card);
    layout->setContentsMargins(18, 16, 18, 16);
    layout->setSpacing(6);

    auto lblTitle = new QLabel(title, card);
    lblTitle->setStyleSheet("font-size: 12px; font-weight: 600; text-transform: uppercase; letter-spacing: 0.5px;");

    valueLabel = new QLabel("0", card);
    valueLabel->setStyleSheet("font-size: 24px; font-weight: 700; font-variant-numeric: tabular-nums;");

    auto lblSub = new QLabel(subtitle, card);
    lblSub->setStyleSheet("font-size: 11px;");

    layout->addWidget(lblTitle);
    layout->addWidget(valueLabel);
    layout->addWidget(lblSub);
    return card;
}

void OverviewPage::applyTheme() {
    ColorTheme theme = Theme::currentTheme;
    m_tableCategories->setStyleSheet(Theme::getTableStyleSheet(theme));
    m_cardScanMeta->setStyleSheet(Theme::getCardStyleSheet(theme));

    QString textPri = Theme::getTextPrimary();
    QString textSec = Theme::getTextSecondary();

    m_lblSectionTitle->setStyleSheet(QString("font-size: 15px; font-weight: 600; color: %1; margin-top: 4px;").arg(textPri));
    m_lblScanMeta->setStyleSheet(QString("font-size: 13px; font-weight: 500; color: %1;").arg(textPri));

    // Refresh category table items colors
    for (int r = 0; r < m_tableCategories->rowCount(); ++r) {
        if (auto it = m_tableCategories->item(r, 0)) it->setForeground(QColor(textPri));
        if (auto it = m_tableCategories->item(r, 1)) it->setForeground(QColor(textPri));
        if (auto it = m_tableCategories->item(r, 2)) it->setForeground(QColor(textSec));
        if (auto it = m_tableCategories->item(r, 3)) it->setForeground(QColor(textSec));
    }
}

void OverviewPage::resetStats() {
    m_lblFilesCount->setText("0");
    m_lblTotalSize->setText("0 B");
    m_lblCategoriesCount->setText("0");
    m_lblDuplicatesCount->setText("0");
    m_lblScanMeta->setText("Ready. Select a target folder and click 'Scan Workspace' to begin.");
    m_tableCategories->setRowCount(0);
}

void OverviewPage::updateStats(const ScanResult& result) {
    m_lastResult = result;
    m_lblFilesCount->setText(QString::number(result.stats.totalFilesScanned));
    m_lblTotalSize->setText(formatBytes(result.stats.totalSizeBytes));
    m_lblCategoriesCount->setText(QString::number(result.stats.categoryCounts.size()));
    m_lblDuplicatesCount->setText(QString::number(result.stats.duplicateGroupCount) + " groups");

    m_lblScanMeta->setText(
        QString("Scan duration: %1 ms  |  Classification: %2 ms  |  SHA-256 Hashing: %3 ms (%4 hashed)")
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
}

} // namespace Ui
