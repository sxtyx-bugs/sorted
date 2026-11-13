#include "OrganizePage.h"
#include <QHeaderView>
#include <QMessageBox>

namespace Ui {

OrganizePage::OrganizePage(QWidget* parent)
    : QWidget(parent) {
    setAttribute(Qt::WA_StyledBackground, true);
    setupUi();
    applyTheme();
}

void OrganizePage::setupUi() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(36, 24, 36, 24);
    mainLayout->setSpacing(14);

    // Filter strip
    auto ctrlBar = new QHBoxLayout();
    ctrlBar->setSpacing(12);

    m_searchEdit = new QLineEdit(this);
    m_searchEdit->setPlaceholderText("Filter proposed moves...");
    m_searchEdit->setFixedHeight(36);
    connect(m_searchEdit, &QLineEdit::textChanged, this, &OrganizePage::onSearchChanged);
    ctrlBar->addWidget(m_searchEdit, 2);

    m_statusCombo = new QComboBox(this);
    m_statusCombo->addItems({"All Statuses", "Ready", "Collision Renamed"});
    m_statusCombo->setFixedHeight(36);
    connect(m_statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &OrganizePage::onStatusFilterChanged);
    ctrlBar->addWidget(m_statusCombo, 1);

    m_btnSelectAll = new QPushButton("Select All", this);
    m_btnSelectAll->setObjectName("btnSubtle");
    m_btnSelectAll->setFixedHeight(36);
    connect(m_btnSelectAll, &QPushButton::clicked, this, &OrganizePage::onSelectAllClicked);
    ctrlBar->addWidget(m_btnSelectAll);

    m_btnDeselectAll = new QPushButton("Deselect All", this);
    m_btnDeselectAll->setObjectName("btnSubtle");
    m_btnDeselectAll->setFixedHeight(36);
    connect(m_btnDeselectAll, &QPushButton::clicked, this, &OrganizePage::onDeselectAllClicked);
    ctrlBar->addWidget(m_btnDeselectAll);

    mainLayout->addLayout(ctrlBar);

    // Plan summary heading
    m_lblPlanSummary = new QLabel("Plan ready. 0 files.", this);
    m_lblPlanSummary->setFixedHeight(22);
    m_lblPlanSummary->setStyleSheet("font-size: 13px; font-weight: 500;");
    mainLayout->addWidget(m_lblPlanSummary);

    // 44px Row Height Organization Plan Table
    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setFrameShape(QFrame::NoFrame);
    m_tableWidget->setColumnCount(5);
    m_tableWidget->setHorizontalHeaderLabels({"Include", "File Name", "Current Location", "Proposed Destination", "Status"});
    m_tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    m_tableWidget->verticalHeader()->setDefaultSectionSize(44);
    m_tableWidget->verticalHeader()->setVisible(false);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(m_tableWidget, &QTableWidget::itemChanged, this, &OrganizePage::onItemChanged);
    mainLayout->addWidget(m_tableWidget, 1);

    // Sticky Bottom Action Area
    m_bottomActionBar = new QFrame(this);
    m_bottomActionBar->setObjectName("bottomActionBar");
    auto actionLayout = new QHBoxLayout(m_bottomActionBar);
    actionLayout->setContentsMargins(18, 12, 18, 12);
    actionLayout->setSpacing(16);

    m_lblApprovedCount = new QLabel("0 approved moves", m_bottomActionBar);
    m_lblApprovedCount->setStyleSheet("font-size: 13px; font-weight: 600;");
    actionLayout->addWidget(m_lblApprovedCount);
    actionLayout->addStretch();

    m_btnApply = new QPushButton("Apply Approved Moves", m_bottomActionBar);
    m_btnApply->setObjectName("btnPrimary");
    m_btnApply->setFixedHeight(36);
    connect(m_btnApply, &QPushButton::clicked, this, &OrganizePage::onApplyClicked);
    actionLayout->addWidget(m_btnApply);

    mainLayout->addWidget(m_bottomActionBar);
}

void OrganizePage::applyTheme() {
    ColorTheme theme = Theme::currentTheme;
    setStyleSheet(QString("OrganizePage { background-color: %1; }").arg(Theme::getCanvas()));
    m_tableWidget->setStyleSheet(Theme::getTableStyleSheet(theme));
    m_bottomActionBar->setStyleSheet(Theme::getCardStyleSheet(theme));

    QString textPri = Theme::getTextPrimary();
    QString textSec = Theme::getTextSecondary();

    m_lblPlanSummary->setStyleSheet(QString("font-size: 13px; font-weight: 500; color: %1;").arg(textPri));
    m_lblApprovedCount->setStyleSheet(QString("font-size: 13px; font-weight: 600; color: %1;").arg(textPri));

    // Refresh row colors
    for (int r = 0; r < m_tableWidget->rowCount(); ++r) {
        if (auto it = m_tableWidget->item(r, 1)) it->setForeground(QColor(textPri));
        if (auto it = m_tableWidget->item(r, 2)) it->setForeground(QColor(textSec));
        if (auto it = m_tableWidget->item(r, 3)) it->setForeground(QColor(textPri));
        if (auto it = m_tableWidget->item(r, 4)) {
            if (it->text().contains("Renamed")) {
                it->setForeground(QColor(Theme::StatusWarning));
            } else {
                it->setForeground(QColor(Theme::StatusSuccess));
            }
        }
    }
}

void OrganizePage::setPlan(const fs::path& rootPath, const std::vector<MoveOperation>& plan) {
    m_rootPath = rootPath;
    m_plan = plan;

    std::size_t collisionCount = 0;
    for (const auto& op : m_plan) {
        if (op.collisionResolved) collisionCount++;
    }

    m_lblPlanSummary->setText(
        QString("Plan: %1 proposed moves  |  %2 collisions auto-resolved without overwriting")
            .arg(m_plan.size())
            .arg(collisionCount)
    );

    applyFilters();
}

void OrganizePage::onSearchChanged(const QString& /*query*/) {
    applyFilters();
}

void OrganizePage::onStatusFilterChanged(int /*index*/) {
    applyFilters();
}

void OrganizePage::applyFilters() {
    QString q = m_searchEdit->text().trimmed().toLower();
    int stIdx = m_statusCombo->currentIndex(); // 0: All, 1: Ready, 2: Collision Renamed

    m_filteredIndices.clear();
    for (std::size_t i = 0; i < m_plan.size(); ++i) {
        const auto& op = m_plan[i];

        if (stIdx == 1 && op.collisionResolved) continue;
        if (stIdx == 2 && !op.collisionResolved) continue;

        if (!q.isEmpty()) {
            QString name = QString::fromStdWString(op.source.filename().wstring()).toLower();
            QString dest = QString::fromStdWString(op.destination.wstring()).toLower();
            if (!name.contains(q) && !dest.contains(q)) continue;
        }

        m_filteredIndices.push_back(i);
    }

    renderTable();
}

void OrganizePage::renderTable() {
    m_tableWidget->blockSignals(true);
    m_tableWidget->setRowCount(0);

    QString textPri = Theme::getTextPrimary();
    QString textSec = Theme::getTextSecondary();

    for (std::size_t row = 0; row < m_filteredIndices.size(); ++row) {
        std::size_t planIdx = m_filteredIndices[row];
        const auto& op = m_plan[planIdx];

        int r = static_cast<int>(row);
        m_tableWidget->insertRow(r);

        auto checkItem = new QTableWidgetItem();
        checkItem->setCheckState(op.included ? Qt::Checked : Qt::Unchecked);
        checkItem->setData(Qt::UserRole, static_cast<qlonglong>(planIdx));
        m_tableWidget->setItem(r, 0, checkItem);

        auto itemName = new QTableWidgetItem(QString::fromStdWString(op.source.filename().wstring()));
        itemName->setForeground(QColor(textPri));
        m_tableWidget->setItem(r, 1, itemName);

        auto itemSrc = new QTableWidgetItem(QString::fromStdWString(op.source.wstring()));
        itemSrc->setForeground(QColor(textSec));
        m_tableWidget->setItem(r, 2, itemSrc);

        auto itemDest = new QTableWidgetItem(QString::fromStdWString(op.destination.wstring()));
        itemDest->setForeground(QColor(textPri));
        m_tableWidget->setItem(r, 3, itemDest);

        QString statusText = op.collisionResolved ? "Collision Renamed" : "Ready";
        auto itemStatus = new QTableWidgetItem(statusText);
        if (op.collisionResolved) {
            itemStatus->setForeground(QColor(Theme::StatusWarning));
        } else {
            itemStatus->setForeground(QColor(Theme::StatusSuccess));
        }
        m_tableWidget->setItem(r, 4, itemStatus);
    }

    m_tableWidget->blockSignals(false);
    updateSummary();
}

void OrganizePage::onSelectAllClicked() {
    m_tableWidget->blockSignals(true);
    for (std::size_t planIdx : m_filteredIndices) {
        m_plan[planIdx].included = true;
    }
    for (int r = 0; r < m_tableWidget->rowCount(); ++r) {
        if (auto it = m_tableWidget->item(r, 0)) {
            it->setCheckState(Qt::Checked);
        }
    }
    m_tableWidget->blockSignals(false);
    updateSummary();
}

void OrganizePage::onDeselectAllClicked() {
    m_tableWidget->blockSignals(true);
    for (std::size_t planIdx : m_filteredIndices) {
        m_plan[planIdx].included = false;
    }
    for (int r = 0; r < m_tableWidget->rowCount(); ++r) {
        if (auto it = m_tableWidget->item(r, 0)) {
            it->setCheckState(Qt::Unchecked);
        }
    }
    m_tableWidget->blockSignals(false);
    updateSummary();
}

void OrganizePage::onItemChanged(QTableWidgetItem* item) {
    if (item && item->column() == 0) {
        std::size_t planIdx = static_cast<std::size_t>(item->data(Qt::UserRole).toLongLong());
        if (planIdx < m_plan.size()) {
            m_plan[planIdx].included = (item->checkState() == Qt::Checked);
        }
        updateSummary();
    }
}

void OrganizePage::updateSummary() {
    std::size_t approved = 0;
    for (const auto& op : m_plan) {
        if (op.included) approved++;
    }
    m_lblApprovedCount->setText(QString("%1 of %2 moves approved").arg(approved).arg(m_plan.size()));
}

void OrganizePage::onApplyClicked() {
    std::vector<MoveOperation> approvedMoves;
    for (const auto& op : m_plan) {
        if (op.included) {
            approvedMoves.push_back(op);
        }
    }

    if (approvedMoves.empty()) {
        QMessageBox::warning(this, "No Approved Moves", "Please select at least one move to apply.");
        return;
    }

    auto reply = QMessageBox::question(
        this, "Confirm Organization",
        QString("Are you sure you want to apply %1 approved moves?\n\n"
                "• Safety Guarantee: Existing destination files will never be overwritten.\n"
                "• Journaling: Every operation is recorded in .sorted_journal.json.\n"
                "• Reversible: You can undo this batch at any time in History.")
            .arg(approvedMoves.size()),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes) {
        emit applyApprovedMovesRequested(approvedMoves);
    }
}

void OrganizePage::updateMoveStatus(int opId, MoveStatus status, const QString& /*errorMsg*/) {
    for (auto& op : m_plan) {
        if (op.id == opId) {
            op.status = status;
            break;
        }
    }
}

} // namespace Ui
