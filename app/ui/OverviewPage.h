#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QStackedWidget>
#include "../models/ScanResult.h"
#include "Theme.h"

namespace Ui {

class OverviewPage : public QWidget {
    Q_OBJECT

public:
    explicit OverviewPage(QWidget* parent = nullptr);

    void updateStats(const ScanResult& result);
    void showEmptyState();
    void applyTheme();

signals:
    void chooseFolderRequested();
    void scanRequested();
    void reviewPlanRequested();

private slots:
    void onLearnHowItWorksClicked();

private:
    void setupUi();
    QFrame* createMetricTile(const QString& title, QLabel*& valueLabel, const QString& subtitle);

    QStackedWidget* m_pageStack = nullptr;

    // --- Onboarding / Empty State View ---
    QWidget* m_emptyWidget = nullptr;
    QLabel* m_lblEmptyIcon = nullptr;
    QLabel* m_lblEmptyHeading = nullptr;
    QLabel* m_lblEmptySub = nullptr;
    QPushButton* m_btnEmptyChoose = nullptr;
    QPushButton* m_btnEmptyLearn = nullptr;

    // --- Populated Results View ---
    QWidget* m_resultsWidget = nullptr;
    QLabel* m_lblFilesCount = nullptr;
    QLabel* m_lblTotalSize = nullptr;
    QLabel* m_lblCategoriesCount = nullptr;
    QLabel* m_lblDuplicatesCount = nullptr;
    QLabel* m_lblScanMeta = nullptr;
    QLabel* m_lblSectionTitle = nullptr;

    QFrame* m_cardScanMeta = nullptr;
    QPushButton* m_btnQuickPlan = nullptr;
    QTableWidget* m_tableCategories = nullptr;

    std::vector<QLabel*> m_metricTitleLabels;
    std::vector<QLabel*> m_metricValueLabels;
    std::vector<QLabel*> m_metricSubLabels;

    ScanResult m_lastResult;
    bool m_hasScannedData = false;
};

} // namespace Ui
