#include "HistoryPage.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <chrono>
#include <ctime>

namespace Ui {

namespace {

QString formatTime(std::chrono::system_clock::time_point tp) {
    auto timeT = std::chrono::system_clock::to_time_t(tp);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &timeT);
#else
    localtime_r(&timeT, &tm);
#endif
    char buf[64];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tm);
    return QString(buf);
}

} // namespace

HistoryPage::HistoryPage(Operations::OperationJournal* journal,
                         Operations::FileOperationService* opService,
                         QWidget* parent)
    : QWidget(parent), m_journal(journal), m_opService(opService) {
    setupUi();
    applyTheme();
}

void HistoryPage::setupUi() {
    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(36, 24, 36, 24);
    mainLayout->setSpacing(14);

    auto headerLayout = new QHBoxLayout();
    m_lblTitle = new QLabel("Operation Audit Trail (.sorted_journal.json)", this);
    m_lblTitle->setStyleSheet("font-size: 16px; font-weight: 600;");
    headerLayout->addWidget(m_lblTitle);
    headerLayout->addStretch();

    m_btnUndo = new QPushButton("↺  Undo Last Batch", this);
    m_btnUndo->setObjectName("btnSubtle");
    connect(m_btnUndo, &QPushButton::clicked, this, &HistoryPage::onUndoSelectedClicked);
    headerLayout->addWidget(m_btnUndo);

    m_btnExportCsv = new QPushButton("📥  Export CSV", this);
    m_btnExportCsv->setObjectName("btnSubtle");
    connect(m_btnExportCsv, &QPushButton::clicked, this, &HistoryPage::onExportCsvClicked);
    headerLayout->addWidget(m_btnExportCsv);

    m_btnRefresh = new QPushButton("Refresh", this);
    m_btnRefresh->setObjectName("btnSubtle");
    connect(m_btnRefresh, &QPushButton::clicked, this, &HistoryPage::onRefreshClicked);
    headerLayout->addWidget(m_btnRefresh);

    mainLayout->addLayout(headerLayout);

    // 44px Row Height Audit Table
    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setColumnCount(4);
    m_tableWidget->setHorizontalHeaderLabels({"Timestamp", "Source Path", "Destination Path", "Status"});
    m_tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    m_tableWidget->verticalHeader()->setDefaultSectionSize(44);
    m_tableWidget->verticalHeader()->setVisible(false);
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    mainLayout->addWidget(m_tableWidget, 1);
}

void HistoryPage::applyTheme() {
    ColorTheme theme = Theme::currentTheme;
    m_tableWidget->setStyleSheet(Theme::getTableStyleSheet(theme));
    m_lblTitle->setStyleSheet(QString("font-size: 16px; font-weight: 600; color: %1;").arg(Theme::getTextPrimary()));

    // Refresh row colors
    QString textPri = Theme::getTextPrimary();
    QString textSec = Theme::getTextSecondary();

    for (int r = 0; r < m_tableWidget->rowCount(); ++r) {
        if (auto it = m_tableWidget->item(r, 0)) it->setForeground(QColor(textSec));
        if (auto it = m_tableWidget->item(r, 1)) it->setForeground(QColor(textPri));
        if (auto it = m_tableWidget->item(r, 2)) it->setForeground(QColor(textPri));
        if (auto it = m_tableWidget->item(r, 3)) {
            QString st = it->text();
            if (st == "Completed") it->setForeground(QColor(Theme::StatusSuccess));
            else if (st == "Undone") it->setForeground(QColor(Theme::StatusWarning));
            else it->setForeground(QColor(Theme::StatusDanger));
        }
    }
}

void HistoryPage::refreshHistory() {
    if (!m_journal) return;
    m_history = m_journal->loadHistory();
    renderTable();
}

void HistoryPage::renderTable() {
    m_tableWidget->setRowCount(0);
    QString textPri = Theme::getTextPrimary();
    QString textSec = Theme::getTextSecondary();

    for (int i = static_cast<int>(m_history.size()) - 1; i >= 0; --i) {
        const auto& op = m_history[i];
        int row = m_tableWidget->rowCount();
        m_tableWidget->insertRow(row);

        auto itemTime = new QTableWidgetItem(formatTime(op.timestamp));
        itemTime->setForeground(QColor(textSec));

        auto itemSrc = new QTableWidgetItem(QString::fromStdWString(op.source.wstring()));
        itemSrc->setForeground(QColor(textPri));

        auto itemDest = new QTableWidgetItem(QString::fromStdWString(op.destination.wstring()));
        itemDest->setForeground(QColor(textPri));

        QString stStr = QString::fromStdString(moveStatusToString(op.status));
        QColor stCol = (op.status == MoveStatus::Completed) ? QColor(Theme::StatusSuccess) :
                       (op.status == MoveStatus::Undone) ? QColor(Theme::StatusWarning) :
                       QColor(Theme::StatusDanger);

        auto itemStatus = new QTableWidgetItem(stStr);
        itemStatus->setForeground(stCol);

        m_tableWidget->setItem(row, 0, itemTime);
        m_tableWidget->setItem(row, 1, itemSrc);
        m_tableWidget->setItem(row, 2, itemDest);
        m_tableWidget->setItem(row, 3, itemStatus);
    }
}

void HistoryPage::onRefreshClicked() {
    refreshHistory();
}

void HistoryPage::onUndoSelectedClicked() {
    if (m_history.empty()) {
        QMessageBox::information(this, "No History", "No recorded operations to undo.");
        return;
    }

    std::vector<MoveOperation> eligible;
    for (auto it = m_history.rbegin(); it != m_history.rend(); ++it) {
        if (it->status == MoveStatus::Completed) {
            eligible.push_back(*it);
        }
    }

    if (eligible.empty()) {
        QMessageBox::information(this, "Nothing to Undo", "All recorded operations were already undone or failed.");
        return;
    }

    auto reply = QMessageBox::question(
        this, "Confirm Undo",
        QString("Rollback %1 completed file move(s)?")
            .arg(eligible.size()),
        QMessageBox::Yes | QMessageBox::No
    );

    if (reply == QMessageBox::Yes && m_opService) {
        std::size_t successCount = 0;
        std::size_t failedCount = 0;
        for (auto& op : eligible) {
            if (m_opService->undoMove(op)) {
                successCount++;
            } else {
                failedCount++;
            }
        }
        refreshHistory();
        emit historyChanged();

        QMessageBox::information(
            this, "Undo Complete",
            QString("Successfully rolled back %1 moves. Failed: %2.")
                .arg(successCount)
                .arg(failedCount)
        );
    }
}

void HistoryPage::onExportCsvClicked() {
    if (m_history.empty()) {
        QMessageBox::information(this, "No Records", "No history records available to export.");
        return;
    }

    QString savePath = QFileDialog::getSaveFileName(this, "Export Audit Log CSV", "sorted_audit_trail.csv", "CSV Files (*.csv)");
    if (savePath.isEmpty()) return;

    QFile file(savePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open file for writing.");
        return;
    }

    QTextStream out(&file);
    out << "ID,Timestamp,SourcePath,DestinationPath,Status\n";

    for (const auto& op : m_history) {
        out << op.id << ","
            << "\"" << formatTime(op.timestamp) << "\","
            << "\"" << QString::fromStdWString(op.source.wstring()) << "\","
            << "\"" << QString::fromStdWString(op.destination.wstring()) << "\","
            << "\"" << QString::fromStdString(moveStatusToString(op.status)) << "\"\n";
    }

    file.close();
    QMessageBox::information(this, "Export Complete", QString("Audit trail saved to:\n%1").arg(savePath));
}

} // namespace Ui
