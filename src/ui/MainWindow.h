#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QThread>
#include <QShortcut>
#include <QSettings>
#include <QMetaType>
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
#include "glass/WindowBackdrop.h"
#include "glass/GlassPanel.h"
#include "glass/GlassSidebar.h"
#include "glass/GlassToolbar.h"
#include "glass/GlassButton.h"
#include "glass/GlassSearchField.h"
#include "glass/GlassNavigationItem.h"
#include "glass/GlassDialog.h"

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

protected:
    void showEvent(QShowEvent* event) override;

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
    void loadSettings();
    void saveSettings();
    void updateTopToolbarFolder();

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

    // Sidebar Widgets
    Glass::GlassSidebar* m_sidebarWidget = nullptr;
    QLabel* m_lblLogo = nullptr;
    QLabel* m_lblDot = nullptr;
    QLabel* m_lblTagline = nullptr;
    Glass::GlassButton* m_btnCollapse = nullptr;
    QListWidget* m_navList = nullptr;
    Glass::GlassNavigationItem* m_navDelegate = nullptr;
    Glass::GlassButton* m_btnTheme = nullptr;

    // Top Persistent Toolbar Widgets
    Glass::GlassToolbar* m_topBar = nullptr;
    QLabel* m_lblFolderIcon = nullptr;
    QLabel* m_lblFolderName = nullptr;
    QLabel* m_lblFolderPath = nullptr;
    Glass::GlassButton* m_btnChangeFolder = nullptr;

    QLabel* m_lblStatus = nullptr;
    QProgressBar* m_progressBar = nullptr;
    Glass::GlassButton* m_btnCancel = nullptr;
    Glass::GlassButton* m_btnScan = nullptr;

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

Q_DECLARE_METATYPE(ScanResult)
