#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QThread>
#include <QShortcut>
#include <atomic>
#include <memory>
#include <filesystem>

#include "../models/ScanResult.h"
#include "../models/MoveOperation.h"
#include "../core/FileScanner.h"
#include "../core/FileClassifier.h"
#include "../core/DuplicateDetector.h"
#include "../core/OrganizationPlanner.h"
#include "../operations/OperationJournal.h"
#include "../operations/FileOperationService.h"
#include "Theme.h"

#include "OverviewPage.h"
#include "FilesPage.h"
#include "CategoriesPage.h"
#include "DuplicatesPage.h"
#include "OrganizePage.h"
#include "HistoryPage.h"

namespace Ui {

class ScanWorker : public QObject {
    Q_OBJECT

public:
    ScanWorker(fs::path rootPath, bool includeHidden);

public slots:
    void run();
    void cancel();

signals:
    void progress(int filesCount, const QString& currentFolder);
    void hashProgress(int processed, int total, const QString& currentFile);
    void finished(const ScanResult& result);
    void error(const QString& errorMessage);

private:
    fs::path m_rootPath;
    bool m_includeHidden;
    std::atomic<bool> m_cancelRequested{false};
};

class ExecutionWorker : public QObject {
    Q_OBJECT

public:
    ExecutionWorker(Operations::FileOperationService* opService, std::vector<MoveOperation> moves);

public slots:
    void run();
    void cancel();

signals:
    void moveProgress(int completed, int total, const QString& currentFileName);
    void finished(const Operations::ExecutionSummary& summary, const std::vector<MoveOperation>& finalMoves);

private:
    Operations::FileOperationService* m_opService;
    std::vector<MoveOperation> m_moves;
    std::atomic<bool> m_cancelRequested{false};
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void onSelectFolderClicked();
    void onStartScanClicked();
    void onCancelClicked();
    void onNavigationChanged(int index);

    // Sidebar & Theme toggles
    void toggleSidebar();
    void toggleTheme();
    void focusSearch();

    // Scan worker slots
    void onScanProgress(int filesCount, const QString& currentFolder);
    void onHashProgress(int processed, int total, const QString& currentFile);
    void onScanFinished(const ScanResult& result);
    void onScanError(const QString& errorMessage);

    // Execution slots
    void onApplyApprovedMoves(const std::vector<MoveOperation>& approvedMoves);
    void onExecutionProgress(int completed, int total, const QString& currentFileName);
    void onExecutionFinished(const Operations::ExecutionSummary& summary, const std::vector<MoveOperation>& finalMoves);

    // Page navigation helper slots
    void onCategoryFilterRequested(const QString& category);
    void onReviewPlanRequested();

private:
    void setupUi();
    void applyTheme();
    void updateRootPathDisplay();

    fs::path m_selectedRoot;
    ScanResult m_lastScanResult;
    std::vector<MoveOperation> m_currentPlan;
    bool m_sidebarCollapsed = false;

    // Core services
    Core::FileClassifier m_classifier;
    Core::OrganizationPlanner m_planner;
    Operations::OperationJournal m_journal;
    Operations::FileOperationService m_opService;

    // Worker threads
    QThread* m_scanThread = nullptr;
    ScanWorker* m_scanWorker = nullptr;
    QThread* m_execThread = nullptr;
    ExecutionWorker* m_execWorker = nullptr;

    // UI Widgets
    QWidget* m_sidebarWidget;
    QLabel* m_lblLogo;
    QLabel* m_lblDot;
    QLabel* m_lblTagline;
    QPushButton* m_btnCollapse;
    QListWidget* m_navList;
    QWidget* m_pathBox;
    QLabel* m_lblCurrentPath;
    QPushButton* m_btnSelectFolder;
    QPushButton* m_btnTheme;

    QLabel* m_lblHeaderTitle;
    QLabel* m_lblHeaderSub;
    QPushButton* m_btnScan;
    QPushButton* m_btnCancel;
    QProgressBar* m_progressBar;
    QLabel* m_lblStatus;

    QShortcut* m_shortcutK = nullptr;

    QStackedWidget* m_stackWidget;
    OverviewPage* m_pageOverview;
    FilesPage* m_pageFiles;
    CategoriesPage* m_pageCategories;
    DuplicatesPage* m_pageDuplicates;
    OrganizePage* m_pageOrganize;
    HistoryPage* m_pageHistory;
};

} // namespace Ui
