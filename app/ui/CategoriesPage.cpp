#include "CategoriesPage.h"
#include <QHeaderView>
#include <map>

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

CategoriesPage::CategoriesPage(QWidget* parent)
    : QWidget(parent) {
    setupUi();
    applyTheme();
}

void CategoriesPage::setupUi() {
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(36, 24, 36, 24);
    layout->setSpacing(14);

    m_lblTitle = new QLabel("Taxonomy & Target Directories", this);
    m_lblTitle->setStyleSheet("font-size: 16px; font-weight: 600;");
    layout->addWidget(m_lblTitle);

    m_tableCategories = new QTableWidget(this);
    m_tableCategories->setColumnCount(4);
    m_tableCategories->setHorizontalHeaderLabels({"Category", "Files Count", "Storage Volume", "Target Directory Pattern"});
    m_tableCategories->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_tableCategories->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_tableCategories->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_tableCategories->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_tableCategories->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableCategories->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableCategories->verticalHeader()->setDefaultSectionSize(44);
    m_tableCategories->verticalHeader()->setVisible(false);

    connect(m_tableCategories, &QTableWidget::cellDoubleClicked,
            this, &CategoriesPage::onCellDoubleClicked);

    layout->addWidget(m_tableCategories, 1);
}

void CategoriesPage::applyTheme() {
    ColorTheme theme = Theme::currentTheme;
    m_tableCategories->setStyleSheet(Theme::getTableStyleSheet(theme));
    m_lblTitle->setStyleSheet(QString("font-size: 16px; font-weight: 600; color: %1;").arg(Theme::getTextPrimary()));

    QString textPri = Theme::getTextPrimary();
    QString textSec = Theme::getTextSecondary();
    for (int r = 0; r < m_tableCategories->rowCount(); ++r) {
        if (auto it = m_tableCategories->item(r, 0)) it->setForeground(QColor(textPri));
        if (auto it = m_tableCategories->item(r, 1)) it->setForeground(QColor(textPri));
        if (auto it = m_tableCategories->item(r, 2)) it->setForeground(QColor(textSec));
        if (auto it = m_tableCategories->item(r, 3)) it->setForeground(QColor(textSec));
    }
}

void CategoriesPage::setFiles(const std::vector<FileRecord>& files) {
    m_files = files;
    m_tableCategories->setRowCount(0);

    struct CatStat {
        int count = 0;
        std::uintmax_t size = 0;
    };
    std::map<std::string, CatStat> stats;

    for (const auto& f : files) {
        auto& st = stats[f.category];
        st.count++;
        st.size += f.size;
    }

    QString textPri = Theme::getTextPrimary();
    QString textSec = Theme::getTextSecondary();

    int row = 0;
    for (const auto& pair : stats) {
        m_tableCategories->insertRow(row);

        auto itemCat = new QTableWidgetItem("📁  " + QString::fromStdString(pair.first));
        itemCat->setForeground(QColor(textPri));

        auto itemCount = new QTableWidgetItem(QString::number(pair.second.count));
        itemCount->setForeground(QColor(textPri));

        auto itemSize = new QTableWidgetItem(formatBytes(pair.second.size));
        itemSize->setForeground(QColor(textSec));

        auto itemDest = new QTableWidgetItem("Sorted/" + QString::fromStdString(pair.first) + "/[EXT]/");
        itemDest->setForeground(QColor(textSec));

        m_tableCategories->setItem(row, 0, itemCat);
        m_tableCategories->setItem(row, 1, itemCount);
        m_tableCategories->setItem(row, 2, itemSize);
        m_tableCategories->setItem(row, 3, itemDest);
        row++;
    }
}

void CategoriesPage::onCellDoubleClicked(int row, int /*column*/) {
    auto item = m_tableCategories->item(row, 0);
    if (!item) return;
    QString cat = item->text().replace("📁  ", "").trimmed();
    emit categorySelected(cat);
}

} // namespace Ui
