import sys
import os
sys.path.insert(0, os.getcwd())

from PySide6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QVBoxLayout, QHBoxLayout,
    QStackedWidget, QLabel, QLineEdit, QComboBox, QTableWidget,
    QListWidget, QPushButton, QFrame, QHeaderView
)
from PySide6.QtCore import Qt, QRect, QPoint
from PySide6.QtGui import QColor, QFont

def run_containment_and_geometry_test():
    app = QApplication.instance() or QApplication(sys.argv)

    resolutions = [
        (1280, 720, "1280x720 (Standard HD)"),
        (1366, 768, "1366x768 (Standard Laptop)"),
        (1920, 1080, "1920x1080 (Full HD)")
    ]

    for w_res, h_res, label in resolutions:
        print(f"\n--- Testing layout containment at {label} ---")
        win = QMainWindow()
        win.resize(w_res, h_res)

        centralWidget = QWidget(win)
        win.setCentralWidget(centralWidget)
        rootLayout = QHBoxLayout(centralWidget)
        rootLayout.setContentsMargins(0, 0, 0, 0)
        rootLayout.setSpacing(0)

        # Sidebar
        sidebarWidget = QWidget(win)
        sidebarWidget.setObjectName("sidebarWidget")
        sidebarWidget.setFixedWidth(244)
        sidebarLayout = QVBoxLayout(sidebarWidget)
        navList = QListWidget(sidebarWidget)
        navList.addItem(" ⊞  Overview")
        navList.addItem(" ▤  All Files")
        navList.addItem(" 📁  Categories")
        navList.addItem(" ⧉  Duplicates")
        navList.addItem(" ⚡  Organization Plan")
        navList.addItem(" ⏱  History & Journal")
        sidebarLayout.addWidget(navList)
        rootLayout.addWidget(sidebarWidget)

        # Content Area
        contentArea = QWidget(win)
        contentArea.setObjectName("contentArea")
        contentArea.setAttribute(Qt.WA_StyledBackground, True)
        contentLayout = QVBoxLayout(contentArea)
        contentLayout.setContentsMargins(0, 0, 0, 0)
        contentLayout.setSpacing(0)

        topBar = QFrame(contentArea)
        topBar.setFixedHeight(56)
        contentLayout.addWidget(topBar)

        stackWidget = QStackedWidget(contentArea)
        stackWidget.setObjectName("stackWidget")
        stackWidget.setAttribute(Qt.WA_StyledBackground, True)

        # Page 0: Overview
        pageOverview = QWidget(stackWidget)
        pageOverview.setAttribute(Qt.WA_StyledBackground, True)
        ovLayout = QVBoxLayout(pageOverview)
        ovLayout.setContentsMargins(0, 0, 0, 0)
        ovStack = QStackedWidget(pageOverview)
        ovStack.setAttribute(Qt.WA_StyledBackground, True)
        emptyWidget = QWidget(ovStack)
        emptyWidget.setAttribute(Qt.WA_StyledBackground, True)
        emptyL = QVBoxLayout(emptyWidget)
        emptyL.setContentsMargins(48, 64, 48, 64)
        lblEmptyHead = QLabel("Choose a workspace", emptyWidget)
        emptyL.addWidget(lblEmptyHead)
        ovStack.addWidget(emptyWidget)
        ovLayout.addWidget(ovStack)

        # Page 1: Files
        pageFiles = QWidget(stackWidget)
        pageFiles.setAttribute(Qt.WA_StyledBackground, True)
        fLayout = QVBoxLayout(pageFiles)
        fLayout.setContentsMargins(36, 24, 36, 24)
        fLayout.setSpacing(14)
        toolbarWidget = QWidget(pageFiles)
        toolbarLayout = QHBoxLayout(toolbarWidget)
        toolbarLayout.setContentsMargins(0, 0, 0, 0)
        toolbarLayout.setSpacing(12)
        searchEdit = QLineEdit(toolbarWidget)
        searchEdit.setFixedHeight(36)
        toolbarLayout.addWidget(searchEdit, 2)
        comboCat = QComboBox(toolbarWidget)
        comboCat.setFixedHeight(36)
        toolbarLayout.addWidget(comboCat, 1)
        comboSort = QComboBox(toolbarWidget)
        comboSort.setFixedHeight(36)
        toolbarLayout.addWidget(comboSort, 1)
        fLayout.addWidget(toolbarWidget)
        lblBench = QLabel("DAA Merge Sort ready: O(n log n)", pageFiles)
        lblBench.setFixedHeight(22)
        fLayout.addWidget(lblBench)
        tableFiles = QTableWidget(pageFiles)
        tableFiles.setColumnCount(5)
        tableFiles.setHorizontalHeaderLabels(["Name", "Category", "Ext", "Size", "Current Location"])
        fLayout.addWidget(tableFiles, 1)

        # Page 2: Categories
        pageCategories = QWidget(stackWidget)
        pageCategories.setAttribute(Qt.WA_StyledBackground, True)
        catLayout = QVBoxLayout(pageCategories)
        catLayout.setContentsMargins(36, 24, 36, 24)
        catLayout.setSpacing(14)
        headerWidget = QWidget(pageCategories)
        headerLayout = QHBoxLayout(headerWidget)
        headerLayout.setContentsMargins(0, 0, 0, 4)
        lblCatTitle = QLabel("Taxonomy & Target Directories", headerWidget)
        headerLayout.addWidget(lblCatTitle)
        catLayout.addWidget(headerWidget)
        tableCats = QTableWidget(pageCategories)
        tableCats.setColumnCount(4)
        tableCats.setHorizontalHeaderLabels(["Category", "Files Count", "Storage Volume", "Target Directory Pattern"])
        catLayout.addWidget(tableCats, 1)

        stackWidget.addWidget(pageOverview)
        stackWidget.addWidget(pageFiles)
        stackWidget.addWidget(pageCategories)

        contentLayout.addWidget(stackWidget, 1)
        rootLayout.addWidget(contentArea, 1)

        navList.currentRowChanged.connect(stackWidget.setCurrentIndex)
        win.show()
        app.processEvents()

        # Check Page 0 initial state
        assert pageOverview.parent() == stackWidget, "pageOverview parent must be stackWidget"
        assert pageFiles.parent() == stackWidget, "pageFiles parent must be stackWidget"
        assert pageCategories.parent() == stackWidget, "pageCategories parent must be stackWidget"
        assert pageOverview.isVisible(), "Overview page must be visible initially"
        assert not pageFiles.isVisible(), "Files page must not be visible on page 0"
        assert not pageCategories.isVisible(), "Categories page must not be visible on page 0"
        assert not searchEdit.isVisible(), "searchEdit must not be visible on page 0"

        # Switch to Files Page (index 1)
        navList.setCurrentRow(1)
        app.processEvents()
        assert not pageOverview.isVisible(), "Overview page must not be visible when on Files page"
        assert pageFiles.isVisible(), "Files page must be visible"
        assert not pageCategories.isVisible(), "Categories page must not be visible"
        assert searchEdit.isVisible(), "searchEdit must be visible on Files page"

        # Verify Geometry: searchEdit must be strictly ABOVE tableFiles with NO overlap
        searchRect = searchEdit.geometry()
        tableRect = tableFiles.geometry()
        toolbarRect = toolbarWidget.geometry()
        lblBenchRect = lblBench.geometry()

        assert toolbarRect.bottom() < lblBenchRect.top(), f"Toolbar ({toolbarRect.bottom()}) overlaps lblBench ({lblBenchRect.top()})"
        assert lblBenchRect.bottom() < tableRect.top(), f"lblBench ({lblBenchRect.bottom()}) overlaps tableFiles ({tableRect.top()})"
        print(f"   [PASSED] FilesPage layout geometry verified: toolbar.bottom ({toolbarRect.bottom()}) < bench.top ({lblBenchRect.top()}) < table.top ({tableRect.top()})")

        # Switch to Categories Page (index 2)
        navList.setCurrentRow(2)
        app.processEvents()
        assert not pageOverview.isVisible(), "Overview page must not be visible"
        assert not pageFiles.isVisible(), "Files page must not be visible"
        assert pageCategories.isVisible(), "Categories page must be visible"
        assert not searchEdit.isVisible(), "searchEdit must be hidden when Categories is active"

        # Verify Geometry: headerWidget must be strictly ABOVE tableCats with NO overlap
        catHeaderRect = headerWidget.geometry()
        catTableRect = tableCats.geometry()
        assert catHeaderRect.bottom() < catTableRect.top(), f"Header ({catHeaderRect.bottom()}) overlaps tableCats ({catTableRect.top()})"
        print(f"   [PASSED] CategoriesPage layout geometry verified: header.bottom ({catHeaderRect.bottom()}) < table.top ({catTableRect.top()})")

        # Verify backgrounds and styling
        assert pageOverview.testAttribute(Qt.WA_StyledBackground), "pageOverview must have WA_StyledBackground"
        assert pageFiles.testAttribute(Qt.WA_StyledBackground), "pageFiles must have WA_StyledBackground"
        assert pageCategories.testAttribute(Qt.WA_StyledBackground), "pageCategories must have WA_StyledBackground"
        assert contentArea.testAttribute(Qt.WA_StyledBackground), "contentArea must have WA_StyledBackground"
        print("   [PASSED] WA_StyledBackground verified on all stacked widgets and contentArea.")

    print("\n=================================================================")
    print("  ALL LAYOUT CONTAINMENT & GEOMETRY VERIFICATIONS PASSED 100%!")
    print("=================================================================")

if __name__ == "__main__":
    run_containment_and_geometry_test()
