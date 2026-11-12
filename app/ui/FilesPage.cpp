#include "FilesPage.h"
#include <QHeaderView>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>
#include <QGuiApplication>
#include <QClipboard>
#include <chrono>

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

FilesPage::FilesPage(QWidget* parent)
    : QWidget(parent) {
    setupUi();
    applyTheme();
}

void FilesPage::setupUi() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(36, 24, 36, 24);
    mainLayout->setSpacing(14);

    // Control Bar (Search, Category, Sort)
    auto ctrlBar = new QHBoxLayout();
    ctrlBar->setSpacing(12);

    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Search file name... (Ctrl+K)");
    connect(m_searchEdit, &QLineEdit::textChanged, this, &FilesPage::onSearchChanged);
    ctrlBar->addWidget(m_searchEdit, 2);

    m_categoryCombo = new QComboBox(this);
    m_categoryCombo->addItem("All Categories");
    connect(m_categoryCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FilesPage::onCategoryChanged);
    ctrlBar->addWidget(m_categoryCombo, 1);

    m_sortCombo = new QComboBox(this);
    m_sortCombo->addItem("Sort: Name (A-Z)");
    m_sortCombo->addItem("Sort: Size (Smallest)");
    m_sortCombo->addItem("Sort: Size (Largest)");
    connect(m_sortCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &FilesPage::onSortChanged);
    ctrlBar->addWidget(m_sortCombo, 1);

    mainLayout->addLayout(ctrlBar);

    m_lblCount = new QLabel("DAA Merge Sort ready: O(n log n)", this);
    m_lblCount->setStyleSheet("font-size: 12px; margin-bottom: 2px;");
    mainLayout->addWidget(m_lblCount);

    // 44px Row Height Data Table
    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setColumnCount(5);
    m_tableWidget->setHorizontalHeaderLabels({"Name", "Category", "Ext", "Size", "Current Location"});
    m_tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableWidget->verticalHeader()->setDefaultSectionSize(44);
    m_tableWidget->verticalHeader()->setVisible(false);
    m_tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_tableWidget, &QTableWidget::customContextMenuRequested,
            this, &FilesPage::onCustomContextMenuRequested);

    mainLayout->addWidget(m_tableWidget, 1);
}

void FilesPage::applyTheme() {
    ColorTheme theme = Theme::currentTheme;
    m_tableWidget->setStyleSheet(Theme::getTableStyleSheet(theme));
    m_lblCount->setStyleSheet(QString("font-size: 12px; color: %1; margin-bottom: 2px;").arg(Theme::getTextSecondary()));

    // Refresh row colors
    QString textPri = Theme::getTextPrimary();
    QString textSec = Theme::getTextSecondary();
    for (int r = 0; r < m_tableWidget->rowCount(); ++r) {
        if (auto it = m_tableWidget->item(r, 0)) it->setForeground(QColor(textPri));
        if (auto it = m_tableWidget->item(r, 1)) it->setForeground(QColor(textPri));
        if (auto it = m_tableWidget->item(r, 2)) it->setForeground(QColor(textSec));
        if (auto it = m_tableWidget->item(r, 3)) it->setForeground(QColor(textPri));
        if (auto it = m_tableWidget->item(r, 4)) it->setForeground(QColor(textSec));
    }
}

void FilesPage::focusSearch() {
    m_searchEdit->setFocus();
    m_searchEdit->selectAll();
}

void FilesPage::setFiles(const std::vector<FileRecord>& files) {
    m_allFiles = files;

    // Repopulate categories
    m_categoryCombo->blockSignals(true);
    m_categoryCombo->clear();
    m_categoryCombo->addItem("All Categories");

    std::unordered_set<std::string> cats;
    for (const auto& f : m_allFiles) {
        if (cats.insert(f.category).second) {
            m_categoryCombo->addItem(QString::fromStdString(f.category));
        }
    }
    m_categoryCombo->blockSignals(false);

    applySortAndFilter();
}

void FilesPage::filterByCategory(const QString& category) {
    int idx = m_categoryCombo->findText(category);
    if (idx >= 0) {
        m_categoryCombo->setCurrentIndex(idx);
    }
}

void FilesPage::onSearchChanged(const QString& /*query*/) {
    applySortAndFilter();
}

void FilesPage::onCategoryChanged(int /*index*/) {
    applySortAndFilter();
}

void FilesPage::onSortChanged(int /*index*/) {
    applySortAndFilter();
}

void FilesPage::applySortAndFilter() {
    QString query = m_searchEdit->text().trimmed().toLower();
    QString selectedCategory = m_categoryCombo->currentText();

    m_displayedFiles.clear();
    for (const auto& f : m_allFiles) {
        if (!query.isEmpty()) {
            QString name = QString::fromStdString(f.name).toLower();
            if (!name.contains(query)) continue;
        }
        if (selectedCategory != "All Categories") {
            if (QString::fromStdString(f.category) != selectedCategory) continue;
        }
        m_displayedFiles.push_back(f);
    }

    auto startSort = std::chrono::high_resolution_clock::now();
    int sortIdx = m_sortCombo->currentIndex();
    if (sortIdx == 0) {
        Core::MergeSort::sortByName(m_displayedFiles, true);
    } else if (sortIdx == 1) {
        Core::MergeSort::sortBySize(m_displayedFiles, true);
    } else if (sortIdx == 2) {
        Core::MergeSort::sortBySize(m_displayedFiles, false);
    }
    auto endSort = std::chrono::high_resolution_clock::now();
    auto sortDurationMs = std::chrono::duration<double, std::milli>(endSort - startSort).count();

    m_lblCount->setText(QString("DAA Merge Sort: %1 records sorted in %2 ms (O(n log n))")
        .arg(m_displayedFiles.size())
        .arg(sortDurationMs, 0, 'f', 3));

    renderTable();
}

void FilesPage::renderTable() {
    m_tableWidget->setRowCount(0);
    QString textPri = Theme::getTextPrimary();
    QString textSec = Theme::getTextSecondary();

    for (std::size_t i = 0; i < m_displayedFiles.size(); ++i) {
        const auto& f = m_displayedFiles[i];
        int row = static_cast<int>(i);
        m_tableWidget->insertRow(row);

        auto itemName = new QTableWidgetItem(QString::fromStdString(f.name));
        itemName->setForeground(QColor(textPri));

        auto itemCat = new QTableWidgetItem(QString::fromStdString(f.category));
        itemCat->setForeground(QColor(textPri));

        auto itemExt = new QTableWidgetItem(QString::fromStdString(f.extension));
        itemExt->setForeground(QColor(textSec));

        auto itemSize = new QTableWidgetItem(formatBytes(f.size));
        itemSize->setForeground(QColor(textPri));

        auto itemLoc = new QTableWidgetItem(QString::fromStdWString(f.path.parent_path().wstring()));
        itemLoc->setForeground(QColor(textSec));

        m_tableWidget->setItem(row, 0, itemName);
        m_tableWidget->setItem(row, 1, itemCat);
        m_tableWidget->setItem(row, 2, itemExt);
        m_tableWidget->setItem(row, 3, itemSize);
        m_tableWidget->setItem(row, 4, itemLoc);
    }
}

void FilesPage::onCustomContextMenuRequested(const QPoint& pos) {
    auto item = m_tableWidget->itemAt(pos);
    if (!item) return;

    QMenu menu(this);
    auto actOpen = menu.addAction("📂  Open Containing Folder");
    connect(actOpen, &QAction::triggered, this, &FilesPage::openSelectedFileLocation);

    auto actCopy = menu.addAction("📋  Copy Full Path");
    connect(actCopy, &QAction::triggered, this, &FilesPage::copySelectedFilePath);

    auto actFilter = menu.addAction("🔍  Filter by this Category");
    connect(actFilter, &QAction::triggered, this, &FilesPage::filterBySelectedCategory);

    menu.exec(m_tableWidget->viewport()->mapToGlobal(pos));
}

void FilesPage::openSelectedFileLocation() {
    int row = m_tableWidget->currentRow();
    if (row < 0 || row >= static_cast<int>(m_displayedFiles.size())) return;
    const auto& f = m_displayedFiles[row];
    QDesktopServices::openUrl(QUrl::fromLocalFile(QString::fromStdWString(f.path.parent_path().wstring())));
}

void FilesPage::copySelectedFilePath() {
    int row = m_tableWidget->currentRow();
    if (row < 0 || row >= static_cast<int>(m_displayedFiles.size())) return;
    const auto& f = m_displayedFiles[row];
    QGuiApplication::clipboard()->setText(QString::fromStdWString(f.path.wstring()));
}

void FilesPage::filterBySelectedCategory() {
    int row = m_tableWidget->currentRow();
    if (row < 0 || row >= static_cast<int>(m_displayedFiles.size())) return;
    const auto& f = m_displayedFiles[row];
    filterByCategory(QString::fromStdString(f.category));
}

} // namespace Ui
