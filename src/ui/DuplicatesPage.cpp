#include "DuplicatesPage.h"
#include <QHeaderView>

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

DuplicatesPage::DuplicatesPage(QWidget* parent)
    : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setupUi();
    applyTheme();
}

void DuplicatesPage::setupUi() {
    auto layout = new QVBoxLayout(this);
    layout->setContentsMargins(36, 24, 36, 24);
    layout->setSpacing(14);

    // Non-destructive Safety Notice Banner
    m_noticeBanner = new QFrame(this);
    m_noticeBanner->setObjectName("noticeBanner");
    auto bLayout = new QHBoxLayout(m_noticeBanner);
    bLayout->setContentsMargins(16, 12, 16, 12);
    bLayout->setSpacing(10);

    auto lblIcon = new QLabel("🛡️", m_noticeBanner);
    lblIcon->setStyleSheet("font-size: 16px;");
    bLayout->addWidget(lblIcon);

    m_lblNotice = new QLabel(
        "Safe Duplicate Detection: Groups are identified via size prefiltering and streaming 1 MiB SHA-256 chunked hashing. Files are never automatically deleted.",
        m_noticeBanner
    );
    m_lblNotice->setStyleSheet("font-size: 13px; font-weight: 500;");
    m_lblNotice->setWordWrap(true);
    bLayout->addWidget(m_lblNotice, 1);
    layout->addWidget(m_noticeBanner);

    // Tree Widget
    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setFrameShape(QFrame::NoFrame);
    m_treeWidget->setHeaderLabels({"Item / Group", "Size", "SHA-256 Digest", "Path"});
    m_treeWidget->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_treeWidget->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_treeWidget->header()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    m_treeWidget->header()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_treeWidget->setAlternatingRowColors(false);

    layout->addWidget(m_treeWidget, 1);
}

void DuplicatesPage::applyTheme() {
    ColorTheme theme = Theme::currentTheme;
    setStyleSheet(QString("DuplicatesPage { background-color: %1; }").arg(Theme::getCanvas()));
    m_treeWidget->setStyleSheet(Theme::getTreeStyleSheet(theme));
    m_noticeBanner->setStyleSheet(Theme::getCardStyleSheet(theme));
    m_lblNotice->setStyleSheet(QString("font-size: 13px; font-weight: 500; color: %1;").arg(Theme::getTextPrimary()));

    // Refresh tree item colors
    QString textPri = Theme::getTextPrimary();
    QString textSec = Theme::getTextSecondary();

    for (int i = 0; i < m_treeWidget->topLevelItemCount(); ++i) {
        auto top = m_treeWidget->topLevelItem(i);
        top->setForeground(0, QBrush(QColor(textPri)));
        top->setForeground(1, QBrush(QColor(textSec)));
        top->setForeground(2, QBrush(QColor(textSec)));
        top->setForeground(3, QBrush(QColor(Theme::StatusWarning)));

        for (int c = 0; c < top->childCount(); ++c) {
            auto child = top->child(c);
            child->setForeground(0, QBrush(QColor(textPri)));
            child->setForeground(1, QBrush(QColor(textSec)));
            child->setForeground(2, QBrush(QColor(textSec)));
            child->setForeground(3, QBrush(QColor(textSec)));
        }
    }
}

void DuplicatesPage::reset() {
    m_treeWidget->clear();
    m_groups.clear();
}

void DuplicatesPage::setDuplicateGroups(const std::vector<DuplicateGroup>& groups) {
    m_groups = groups;
    m_treeWidget->clear();

    QString textPri = Theme::getTextPrimary();
    QString textSec = Theme::getTextSecondary();

    for (const auto& group : groups) {
        auto groupItem = new QTreeWidgetItem(m_treeWidget);
        groupItem->setText(0, QString("Group #%1 (%2 copies)").arg(group.id).arg(group.files.size()));
        groupItem->setText(1, formatBytes(group.size));

        QString truncatedHash = QString::fromStdString(
            group.sha256.length() > 16 ? group.sha256.substr(0, 16) + "..." : group.sha256);
        groupItem->setText(2, truncatedHash);
        groupItem->setText(3, QString("%1 redundant duplicate copies").arg(group.files.size() > 1 ? group.files.size() - 1 : 0));

        groupItem->setForeground(0, QBrush(QColor(textPri)));
        groupItem->setForeground(1, QBrush(QColor(textSec)));
        groupItem->setForeground(2, QBrush(QColor(textSec)));
        groupItem->setForeground(3, QBrush(QColor(Theme::StatusWarning)));
        groupItem->setExpanded(true);

        for (const auto& file : group.files) {
            auto fileItem = new QTreeWidgetItem(groupItem);
            fileItem->setText(0, QString::fromStdString(file.name));
            fileItem->setText(1, formatBytes(file.size));
            fileItem->setText(2, "");
            fileItem->setText(3, QString::fromStdWString(file.path.wstring()));

            fileItem->setForeground(0, QBrush(QColor(textPri)));
            fileItem->setForeground(1, QBrush(QColor(textSec)));
            fileItem->setForeground(2, QBrush(QColor(textSec)));
            fileItem->setForeground(3, QBrush(QColor(textSec)));
        }
    }
}

} // namespace Ui
