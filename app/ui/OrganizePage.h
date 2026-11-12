#pragma once

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <vector>
#include "../models/MoveOperation.h"
#include "Theme.h"

namespace Ui {

class OrganizePage : public QWidget {
    Q_OBJECT

public:
    explicit OrganizePage(QWidget* parent = nullptr);

    void setPlan(const fs::path& rootPath, const std::vector<MoveOperation>& plan);
    const std::vector<MoveOperation>& getPlan() const { return m_plan; }
    void updateMoveStatus(int opId, MoveStatus status, const QString& errorMsg);
    void applyTheme();

signals:
    void applyApprovedMovesRequested(const std::vector<MoveOperation>& approvedMoves);

private slots:
    void onSearchChanged(const QString& query);
    void onStatusFilterChanged(int index);
    void onSelectAllClicked();
    void onDeselectAllClicked();
    void onApplyClicked();
    void onItemChanged(QTableWidgetItem* item);

private:
    void setupUi();
    void applyFilters();
    void renderTable();
    void updateSummary();

    fs::path m_rootPath;
    std::vector<MoveOperation> m_plan;
    std::vector<std::size_t> m_filteredIndices;

    QLineEdit* m_searchEdit = nullptr;
    QComboBox* m_statusCombo = nullptr;
    QLabel* m_lblPlanSummary = nullptr;
    QPushButton* m_btnSelectAll = nullptr;
    QPushButton* m_btnDeselectAll = nullptr;

    QTableWidget* m_tableWidget = nullptr;

    QFrame* m_bottomActionBar = nullptr;
    QLabel* m_lblApprovedCount = nullptr;
    QPushButton* m_btnApply = nullptr;
};

} // namespace Ui
