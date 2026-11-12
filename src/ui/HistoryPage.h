#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <vector>
#include "../models/MoveOperation.h"
#include "../operations/OperationJournal.h"
#include "../operations/FileOperationService.h"
#include "Theme.h"

namespace Ui {

class HistoryPage : public QWidget {
    Q_OBJECT

public:
    explicit HistoryPage(Operations::OperationJournal* journal,
                         Operations::FileOperationService* opService,
                         QWidget* parent = nullptr);

    void refreshHistory();
    void applyTheme();

signals:
    void historyChanged();

private slots:
    void onUndoSelectedClicked();
    void onExportCsvClicked();
    void onRefreshClicked();

private:
    void setupUi();
    void renderTable();

    Operations::OperationJournal* m_journal;
    Operations::FileOperationService* m_opService;
    std::vector<MoveOperation> m_history;

    QLabel* m_lblTitle = nullptr;
    QPushButton* m_btnUndo = nullptr;
    QPushButton* m_btnExportCsv = nullptr;
    QPushButton* m_btnRefresh = nullptr;
    QTableWidget* m_tableWidget = nullptr;
};

} // namespace Ui
