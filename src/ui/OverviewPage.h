#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include "../models/ScanResult.h"
#include "Theme.h"

namespace Ui {

class OverviewPage : public QWidget {
    Q_OBJECT

public:
    explicit OverviewPage(QWidget* parent = nullptr);

    void updateStats(const ScanResult& result);
    void resetStats();
    void applyTheme();

signals:
    void scanRequested();
    void reviewPlanRequested();

private:
    void setupUi();
    QFrame* createStatCard(const QString& title, QLabel*& valueLabel, const QString& subtitle);

    QLabel* m_lblFilesCount = nullptr;
    QLabel* m_lblTotalSize = nullptr;
    QLabel* m_lblCategoriesCount = nullptr;
    QLabel* m_lblDuplicatesCount = nullptr;
    QLabel* m_lblScanMeta = nullptr;
    QLabel* m_lblSectionTitle = nullptr;

    QFrame* m_cardScanMeta = nullptr;
    QPushButton* m_btnQuickPlan = nullptr;
    QTableWidget* m_tableCategories = nullptr;

    ScanResult m_lastResult;
};

} // namespace Ui
