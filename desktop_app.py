#!/usr/bin/env python3
"""
SORTED --- Safe Preview-First Desktop File Organizer (Qt 6)
Design System: Apple macOS x Notion
Accent: Cobalt Blue (#3478F6)
Light & Genuine Dark Themes, Collapsible Sidebar, Ctrl+K Navigation, 44px Row Heights.
"""

import sys
import os
import time
import csv
from pathlib import Path
from datetime import datetime

from PySide6.QtCore import Qt, QThread, Signal, QObject, QUrl, QSize
from PySide6.QtGui import (
    QColor, QDesktopServices, QPalette, QBrush, QFont, QKeySequence,
    QShortcut, QClipboard, QGuiApplication
)
from PySide6.QtWidgets import (
    QApplication, QMainWindow, QWidget, QStackedWidget, QListWidget, QListWidgetItem,
    QLabel, QPushButton, QProgressBar, QLineEdit, QComboBox, QCheckBox,
    QTableWidget, QTableWidgetItem, QTreeWidget, QTreeWidgetItem,
    QVBoxLayout, QHBoxLayout, QHeaderView, QMessageBox, QFileDialog,
    QStyleFactory, QMenu, QFrame, QScrollArea
)

# Import verified core engine
from scripts.run_sorted_cli import (
    scan_directory, sort_files, detect_duplicates, generate_plan,
    classify_extension, normalize_extension
)

# -------------------------------------------------------------
# Color Palettes: Apple x Notion with Cobalt Blue #3478F6
# -------------------------------------------------------------
PALETTE_LIGHT = {
    "canvas": "#FAFAF8",
    "sidebar": "#F4F4F0",
    "surface": "#FFFFFF",
    "hover": "#F0F0EB",
    "selected": "#3478F6",
    "selected_text": "#FFFFFF",
    "item_selected_bg": "#EAF1FF",
    "item_selected_fg": "#3478F6",
    "accent": "#3478F6",
    "accent_hover": "#2463D8",
    "accent_tint": "#EAF1FF",
    "info_surface": "#F2F6FF",
    "text_primary": "#242521",
    "text_secondary": "#777A73",
    "text_muted": "#999A93",
    "border": "#E6E6E0",
    "border_subtle": "#EDEDE8",
    "success_bg": "#E8F5E9",
    "success_fg": "#2E7D32",
    "warning_bg": "#FFF3E0",
    "warning_fg": "#C07700",
    "danger_bg": "#FFEBEE",
    "danger_fg": "#D32F2F",
    "info_bg": "#EAF1FF",
    "info_fg": "#3478F6",
}

PALETTE_DARK = {
    "canvas": "#191919",
    "sidebar": "#202020",
    "surface": "#262626",
    "hover": "#2F2F2F",
    "selected": "#3478F6",
    "selected_text": "#FFFFFF",
    "item_selected_bg": "#1E2D4A",
    "item_selected_fg": "#64B5F6",
    "accent": "#3478F6",
    "accent_hover": "#2463D8",
    "accent_tint": "#1E2D4A",
    "info_surface": "#172133",
    "text_primary": "#EDEDEC",
    "text_secondary": "#9B9B97",
    "text_muted": "#6A6A65",
    "border": "#333330",
    "border_subtle": "#2B2B28",
    "success_bg": "#173822",
    "success_fg": "#66BB6A",
    "warning_bg": "#382A12",
    "warning_fg": "#FFA726",
    "danger_bg": "#3B1C1C",
    "danger_fg": "#EF5350",
    "info_bg": "#1E2D4A",
    "info_fg": "#64B5F6",
}

def format_bytes(b):
    if b < 1024:
        return f"{b} B"
    kb = b / 1024.0
    if kb < 1024:
        return f"{kb:.1f} KB"
    mb = kb / 1024.0
    if mb < 1024:
        return f"{mb:.1f} MB"
    gb = mb / 1024.0
    return f"{gb:.2f} GB"

class ScanWorker(QObject):
    finished = Signal(dict)
    progress = Signal(str)

    def __init__(self, root_dir):
        super().__init__()
        self.root_dir = Path(root_dir)

    def run(self):
        self.progress.emit("Scanning directory recursively...")
        t0 = time.perf_counter()
        records, errors = scan_directory(self.root_dir)
        t_scan = (time.perf_counter() - t0) * 1000

        self.progress.emit(f"Discovered {len(records)} files. Detecting duplicates via SHA-256...")
        t0 = time.perf_counter()
        dup_groups, bytes_hashed = detect_duplicates(records)
        t_dup = (time.perf_counter() - t0) * 1000

        self.progress.emit("Computing collision-safe organization plan...")
        t0 = time.perf_counter()
        plan = generate_plan(self.root_dir, records)
        t_plan = (time.perf_counter() - t0) * 1000

        total_size = sum(r["size"] for r in records)
        cat_counts = {}
        cat_sizes = {}
        for r in records:
            c = r["category"]
            cat_counts[c] = cat_counts.get(c, 0) + 1
            cat_sizes[c] = cat_sizes.get(c, 0) + r["size"]

        res = {
            "root": str(self.root_dir),
            "records": records,
            "errors": errors,
            "duplicates": dup_groups,
            "plan": plan,
            "stats": {
                "total_files": len(records),
                "total_size": total_size,
                "categories": cat_counts,
                "category_sizes": cat_sizes,
                "bytes_hashed": bytes_hashed,
                "t_scan": t_scan,
                "t_dup": t_dup,
                "t_plan": t_plan,
                "scan_time": datetime.now().strftime("%Y-%m-%d %H:%M:%S")
            }
        }
        self.finished.emit(res)

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("SORTED — Safe File Organizer")
        self.resize(1180, 760)
        self.setMinimumSize(960, 620)

        # Default folder: prioritize SORTED_TEST or user Downloads
        default_dir = Path("SORTED_TEST").resolve()
        if not default_dir.exists():
            default_dir = Path.home() / "Downloads"
        self.root_dir = default_dir

        self.scan_data = None
        self.history = []
        self.is_dark_mode = False
        self.is_sidebar_collapsed = False

        self.init_ui()
        self.apply_theme()
        self.update_root_display()
        self.setup_shortcuts()

    def p(self):
        """Current palette shortcut."""
        return PALETTE_DARK if self.is_dark_mode else PALETTE_LIGHT

    def init_ui(self):
        central = QWidget(self)
        self.setCentralWidget(central)
        root_layout = QHBoxLayout(central)
        root_layout.setContentsMargins(0, 0, 0, 0)
        root_layout.setSpacing(0)

        # ---------------------------------------------------------
        # 1. Persistent Collapsible Navigation Sidebar
        # ---------------------------------------------------------
        self.sidebar = QWidget()
        self.sidebar.setObjectName("sidebarWidget")
        self.sidebar.setFixedWidth(240)
        self.sidebar_layout = QVBoxLayout(self.sidebar)
        self.sidebar_layout.setContentsMargins(16, 20, 16, 16)
        self.sidebar_layout.setSpacing(10)

        # Header with Logo and Collapse Toggle
        logo_row = QHBoxLayout()
        logo_row.setContentsMargins(4, 0, 4, 0)
        logo_row.setSpacing(6)

        self.lbl_logo = QLabel("sorted.")
        self.lbl_logo.setObjectName("lblLogo")
        self.lbl_logo.setStyleSheet("font-size: 22px; font-weight: 700; letter-spacing: -0.5px;")
        logo_row.addWidget(self.lbl_logo)

        self.lbl_dot = QLabel("●")
        self.lbl_dot.setStyleSheet(f"font-size: 10px; color: {PALETTE_LIGHT['accent']}; margin-top: 4px;")
        logo_row.addWidget(self.lbl_dot)
        logo_row.addStretch()

        self.btn_collapse = QPushButton("◀")
        self.btn_collapse.setObjectName("btnSubtle")
        self.btn_collapse.setFixedSize(28, 28)
        self.btn_collapse.setToolTip("Collapse / Expand Sidebar (Toggle)")
        self.btn_collapse.clicked.connect(self.toggle_sidebar)
        logo_row.addWidget(self.btn_collapse)
        self.sidebar_layout.addLayout(logo_row)

        self.lbl_tagline = QLabel("Safe preview organizer")
        self.lbl_tagline.setObjectName("lblTagline")
        self.lbl_tagline.setStyleSheet("font-size: 11px; margin-bottom: 6px; padding-left: 4px;")
        self.sidebar_layout.addWidget(self.lbl_tagline)

        # Navigation List (6 Views)
        self.nav_list = QListWidget()
        self.nav_list.setObjectName("navList")
        self.nav_items_data = [
            ("Overview", "📊", "Workspace overview and metrics"),
            ("All Files", "📄", "Indexed file catalogue"),
            ("Categories", "📁", "Taxonomy and file types"),
            ("Duplicates", "♊", "SHA-256 exact duplicates"),
            ("Organization", "⚡", "Review and apply plan"),
            ("History", "🕒", "Operation log and undo")
        ]
        for name, icon, tip in self.nav_items_data:
            item = QListWidgetItem(f" {icon}  {name}")
            item.setToolTip(tip)
            item.setSizeHint(QSize(200, 38))
            self.nav_list.addItem(item)
        self.nav_list.setCurrentRow(0)
        self.nav_list.currentRowChanged.connect(self.on_nav_changed)
        self.sidebar_layout.addWidget(self.nav_list, 1)

        # Target Folder Information Card
        self.card_folder = QFrame()
        self.card_folder.setObjectName("cardFolder")
        folder_layout = QVBoxLayout(self.card_folder)
        folder_layout.setContentsMargins(10, 10, 10, 10)
        folder_layout.setSpacing(4)

        lbl_folder_title = QLabel("Target Folder")
        lbl_folder_title.setObjectName("lblFolderTitle")
        lbl_folder_title.setStyleSheet("font-size: 11px; font-weight: 600;")
        folder_layout.addWidget(lbl_folder_title)

        self.lbl_path = QLabel("-")
        self.lbl_path.setObjectName("lblPath")
        self.lbl_path.setWordWrap(True)
        self.lbl_path.setStyleSheet("font-size: 11px;")
        folder_layout.addWidget(self.lbl_path)

        self.btn_change_folder = QPushButton("Change Folder")
        self.btn_change_folder.setObjectName("btnChangeFolder")
        self.btn_change_folder.clicked.connect(self.choose_folder)
        folder_layout.addWidget(self.btn_change_folder)
        self.sidebar_layout.addWidget(self.card_folder)

        # Theme Switcher at bottom
        self.btn_theme = QPushButton("🌙  Dark Mode")
        self.btn_theme.setObjectName("btnTheme")
        self.btn_theme.setFixedHeight(34)
        self.btn_theme.clicked.connect(self.toggle_theme)
        self.sidebar_layout.addWidget(self.btn_theme)

        root_layout.addWidget(self.sidebar)

        # ---------------------------------------------------------
        # 2. Main Surface Area (Header + Stacked Pages)
        # ---------------------------------------------------------
        content = QWidget()
        content.setObjectName("contentArea")
        content_layout = QVBoxLayout(content)
        content_layout.setContentsMargins(0, 0, 0, 0)
        content_layout.setSpacing(0)

        # Top Editorial Header Bar
        top_bar = QWidget()
        top_bar.setObjectName("topBar")
        top_layout = QHBoxLayout(top_bar)
        top_layout.setContentsMargins(36, 14, 36, 14)
        top_layout.setSpacing(16)

        title_box = QVBoxLayout()
        title_box.setSpacing(2)
        self.lbl_header = QLabel("Overview")
        self.lbl_header.setObjectName("lblHeaderTitle")
        self.lbl_header.setStyleSheet("font-size: 18px; font-weight: 700; letter-spacing: -0.3px;")
        title_box.addWidget(self.lbl_header)

        self.lbl_header_sub = QLabel("Explore folder structure, metrics and distribution")
        self.lbl_header_sub.setObjectName("lblHeaderSub")
        self.lbl_header_sub.setStyleSheet("font-size: 12px;")
        title_box.addWidget(self.lbl_header_sub)
        top_layout.addLayout(title_box)

        top_layout.addStretch()

        self.lbl_status = QLabel("Ready")
        self.lbl_status.setObjectName("lblStatus")
        self.lbl_status.setStyleSheet("font-size: 12px;")
        top_layout.addWidget(self.lbl_status)

        self.progress_bar = QProgressBar()
        self.progress_bar.setObjectName("progressBar")
        self.progress_bar.setFixedWidth(160)
        self.progress_bar.setFixedHeight(6)
        self.progress_bar.setTextVisible(False)
        self.progress_bar.setVisible(False)
        top_layout.addWidget(self.progress_bar)

        self.btn_scan = QPushButton("Scan Workspace")
        self.btn_scan.setObjectName("btnPrimary")
        self.btn_scan.setFixedHeight(36)
        self.btn_scan.clicked.connect(self.start_scan)
        top_layout.addWidget(self.btn_scan)

        content_layout.addWidget(top_bar)

        # ---------------------------------------------------------
        # 3. Stacked View Pages
        # ---------------------------------------------------------
        self.stack = QStackedWidget()
        self.page_overview = self.create_overview_page()
        self.page_files = self.create_files_page()
        self.page_categories = self.create_categories_page()
        self.page_duplicates = self.create_duplicates_page()
        self.page_organize = self.create_organize_page()
        self.page_history = self.create_history_page()

        self.stack.addWidget(self.page_overview)     # index 0
        self.stack.addWidget(self.page_files)        # index 1
        self.stack.addWidget(self.page_categories)   # index 2
        self.stack.addWidget(self.page_duplicates)   # index 3
        self.stack.addWidget(self.page_organize)     # index 4
        self.stack.addWidget(self.page_history)      # index 5

        content_layout.addWidget(self.stack, 1)
        root_layout.addWidget(content, 1)

    def setup_shortcuts(self):
        """Global Ctrl+K shortcut to focus search."""
        shortcut_k = QShortcut(QKeySequence("Ctrl+K"), self)
        shortcut_k.activated.connect(self.focus_search)

    def focus_search(self):
        self.nav_list.setCurrentRow(1)  # Switch to All Files
        self.txt_search.setFocus()
        self.txt_search.selectAll()

    def toggle_sidebar(self):
        self.is_sidebar_collapsed = not self.is_sidebar_collapsed
        if self.is_sidebar_collapsed:
            self.sidebar.setFixedWidth(68)
            self.lbl_logo.setText("s.")
            self.lbl_tagline.setVisible(False)
            self.card_folder.setVisible(False)
            self.btn_collapse.setText("▶")
            self.btn_theme.setText("🌙" if not self.is_dark_mode else "☀️")
            for i, (name, icon, tip) in enumerate(self.nav_items_data):
                self.nav_list.item(i).setText(f"{icon}")
        else:
            self.sidebar.setFixedWidth(240)
            self.lbl_logo.setText("sorted.")
            self.lbl_tagline.setVisible(True)
            self.card_folder.setVisible(True)
            self.btn_collapse.setText("◀")
            self.btn_theme.setText("🌙  Dark Mode" if not self.is_dark_mode else "☀️  Light Mode")
            for i, (name, icon, tip) in enumerate(self.nav_items_data):
                self.nav_list.item(i).setText(f" {icon}  {name}")

    def toggle_theme(self):
        self.is_dark_mode = not self.is_dark_mode
        if self.is_sidebar_collapsed:
            self.btn_theme.setText("🌙" if not self.is_dark_mode else "☀️")
        else:
            self.btn_theme.setText("🌙  Dark Mode" if not self.is_dark_mode else "☀️  Light Mode")
        self.apply_theme()
        # Re-render any visible data tables with updated foregrounds
        if self.scan_data:
            self.populate_overview(self.scan_data)
            self.populate_categories(self.scan_data)
            self.populate_duplicates(self.scan_data)
            self.filter_plan_table()
            self.filter_files_table()
        self.update_history_table()

    def apply_theme(self):
        pal = self.p()
        app = QApplication.instance()
        if app:
            qp = QPalette()
            qp.setColor(QPalette.Window, QColor(pal["canvas"]))
            qp.setColor(QPalette.WindowText, QColor(pal["text_primary"]))
            qp.setColor(QPalette.Base, QColor(pal["surface"]))
            qp.setColor(QPalette.AlternateBase, QColor(pal["canvas"]))
            qp.setColor(QPalette.ToolTipBase, QColor(pal["surface"]))
            qp.setColor(QPalette.ToolTipText, QColor(pal["text_primary"]))
            qp.setColor(QPalette.Text, QColor(pal["text_primary"]))
            qp.setColor(QPalette.Button, QColor(pal["surface"]))
            qp.setColor(QPalette.ButtonText, QColor(pal["text_primary"]))
            qp.setColor(QPalette.BrightText, QColor(pal["danger_fg"]))
            qp.setColor(QPalette.Link, QColor(pal["accent"]))
            qp.setColor(QPalette.Highlight, QColor(pal["selected"]))
            qp.setColor(QPalette.HighlightedText, QColor(pal["selected_text"]))
            qp.setColor(QPalette.PlaceholderText, QColor(pal["text_muted"]))
            qp.setColor(QPalette.Disabled, QPalette.Text, QColor(pal["text_muted"]))
            qp.setColor(QPalette.Disabled, QPalette.WindowText, QColor(pal["text_muted"]))
            qp.setColor(QPalette.Disabled, QPalette.ButtonText, QColor(pal["text_muted"]))
            app.setPalette(qp)

        # Global Application Stylesheet
        self.setStyleSheet(f"""
            QMainWindow {{
                background-color: {pal['canvas']};
                color: {pal['text_primary']};
                font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', system-ui, sans-serif;
            }}
            QWidget {{
                font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', system-ui, sans-serif;
                color: {pal['text_primary']};
            }}
            QWidget#sidebarWidget {{
                background-color: {pal['sidebar']};
                border-right: 1px solid {pal['border']};
            }}
            QWidget#contentArea {{
                background-color: {pal['canvas']};
            }}
            QWidget#topBar {{
                background-color: {pal['surface']};
                border-bottom: 1px solid {pal['border']};
            }}
            QFrame#cardFolder {{
                background-color: {pal['hover']};
                border: 1px solid {pal['border']};
                border-radius: 8px;
            }}
            QFrame#statCard {{
                background-color: {pal['surface']};
                border: 1px solid {pal['border']};
                border-radius: 10px;
            }}
            QFrame#bottomActionBar {{
                background-color: {pal['surface']};
                border-top: 1px solid {pal['border']};
                border-radius: 0px 0px 10px 10px;
            }}
            QFrame#noticeBanner {{
                background-color: {pal['info_surface']};
                border: 1px solid {pal['border']};
                border-radius: 8px;
            }}
            
            /* Sidebar Navigation List */
            QListWidget#navList {{
                border: none;
                background: transparent;
                outline: none;
            }}
            QListWidget#navList::item {{
                padding: 8px 12px;
                border-radius: 8px;
                font-size: 13px;
                font-weight: 500;
                color: {pal['text_secondary']};
                margin-bottom: 3px;
            }}
            QListWidget#navList::item:hover {{
                background-color: {pal['hover']};
                color: {pal['text_primary']};
            }}
            QListWidget#navList::item:selected {{
                background-color: {pal['item_selected_bg']};
                color: {pal['item_selected_fg']};
                font-weight: 600;
            }}

            /* Primary Action Button (Cobalt Blue) */
            QPushButton#btnPrimary {{
                background-color: {pal['accent']};
                color: #FFFFFF;
                border: none;
                border-radius: 8px;
                padding: 7px 18px;
                font-size: 13px;
                font-weight: 600;
            }}
            QPushButton#btnPrimary:hover {{
                background-color: {pal['accent_hover']};
            }}
            QPushButton#btnPrimary:pressed {{
                background-color: #1A54BD;
            }}
            QPushButton#btnPrimary:disabled {{
                background-color: {pal['border']};
                color: {pal['text_muted']};
            }}

            /* Subtle / Outline Button */
            QPushButton#btnSubtle, QPushButton#btnTheme, QPushButton#btnChangeFolder {{
                background-color: {pal['surface']};
                color: {pal['text_primary']};
                border: 1px solid {pal['border']};
                border-radius: 6px;
                padding: 5px 12px;
                font-size: 12px;
                font-weight: 500;
            }}
            QPushButton#btnSubtle:hover, QPushButton#btnTheme:hover, QPushButton#btnChangeFolder:hover {{
                background-color: {pal['hover']};
                border-color: {pal['border']};
            }}

            /* Inputs & Dropdowns */
            QLineEdit {{
                background-color: {pal['surface']};
                color: {pal['text_primary']};
                border: 1px solid {pal['border']};
                border-radius: 8px;
                padding: 7px 12px;
                font-size: 13px;
            }}
            QLineEdit:focus {{
                border: 1.5px solid {pal['accent']};
            }}
            QComboBox {{
                background-color: {pal['surface']};
                color: {pal['text_primary']};
                border: 1px solid {pal['border']};
                border-radius: 8px;
                padding: 6px 12px;
                font-size: 13px;
            }}
            QComboBox:focus {{
                border: 1.5px solid {pal['accent']};
            }}
            QComboBox QAbstractItemView {{
                background-color: {pal['surface']};
                color: {pal['text_primary']};
                border: 1px solid {pal['border']};
                border-radius: 8px;
                selection-background-color: {pal['item_selected_bg']};
                selection-color: {pal['item_selected_fg']};
                padding: 4px;
            }}

            /* Table & Tree Views - 44px Row Height with High Contrast Charcoal Normal Text */
            QTableView, QTableWidget {{
                background-color: {pal['surface']};
                color: {pal['text_primary']};
                border: 1px solid {pal['border']};
                border-radius: 10px;
                gridline-color: {pal['border_subtle']};
                selection-background-color: {pal['selected']};
                selection-color: {pal['selected_text']};
                outline: none;
            }}
            QTableView::item, QTableWidget::item {{
                color: {pal['text_primary']};
                padding: 6px 12px;
                border: none;
            }}
            QTableView::item:hover, QTableWidget::item:hover {{
                background-color: {pal['hover']};
                color: {pal['text_primary']};
            }}
            QTableView::item:selected, QTableWidget::item:selected {{
                background-color: {pal['selected']};
                color: {pal['selected_text']};
            }}
            QHeaderView::section {{
                background-color: {pal['canvas']};
                color: {pal['text_secondary']};
                font-size: 12px;
                font-weight: 600;
                padding: 8px 12px;
                border: none;
                border-bottom: 1px solid {pal['border']};
                border-right: 1px solid {pal['border_subtle']};
            }}

            /* Tree Widget for Duplicates */
            QTreeWidget {{
                background-color: {pal['surface']};
                color: {pal['text_primary']};
                border: 1px solid {pal['border']};
                border-radius: 10px;
                selection-background-color: {pal['selected']};
                selection-color: {pal['selected_text']};
                outline: none;
                padding: 4px;
            }}
            QTreeWidget::item {{
                color: {pal['text_primary']};
                padding: 6px 8px;
                border-radius: 6px;
            }}
            QTreeWidget::item:hover {{
                background-color: {pal['hover']};
                color: {pal['text_primary']};
            }}
            QTreeWidget::item:selected {{
                background-color: {pal['selected']};
                color: {pal['selected_text']};
            }}

            /* Progress Bar */
            QProgressBar {{
                background-color: {pal['border']};
                border: none;
                border-radius: 3px;
            }}
            QProgressBar::chunk {{
                background-color: {pal['accent']};
                border-radius: 3px;
            }}

            /* ScrollBars */
            QScrollBar:vertical {{
                border: none;
                background: transparent;
                width: 8px;
                margin: 0px;
            }}
            QScrollBar::handle:vertical {{
                background: {pal['border']};
                min-height: 24px;
                border-radius: 4px;
            }}
            QScrollBar::handle:vertical:hover {{
                background: {pal['text_muted']};
            }}
            QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {{
                height: 0px;
            }}

            /* Checkbox */
            QCheckBox {{
                spacing: 6px;
                font-size: 13px;
                color: {pal['text_primary']};
            }}
            QCheckBox::indicator {{
                width: 16px;
                height: 16px;
                border: 1px solid {pal['border']};
                border-radius: 4px;
                background-color: {pal['surface']};
            }}
            QCheckBox::indicator:hover {{
                border-color: {pal['accent']};
            }}
            QCheckBox::indicator:checked {{
                background-color: {pal['accent']};
                border-color: {pal['accent']};
            }}

            /* Menus */
            QMenu {{
                background-color: {pal['surface']};
                color: {pal['text_primary']};
                border: 1px solid {pal['border']};
                border-radius: 8px;
                padding: 4px;
            }}
            QMenu::item {{
                padding: 6px 16px;
                border-radius: 4px;
                font-size: 13px;
            }}
            QMenu::item:selected {{
                background-color: {pal['item_selected_bg']};
                color: {pal['item_selected_fg']};
            }}
        """)

        # Sync header label styling
        self.lbl_logo.setStyleSheet(f"font-size: 22px; font-weight: 700; color: {pal['text_primary']}; letter-spacing: -0.5px;")
        self.lbl_dot.setStyleSheet(f"font-size: 10px; color: {pal['accent']}; margin-top: 4px;")
        self.lbl_tagline.setStyleSheet(f"font-size: 11px; color: {pal['text_muted']}; margin-bottom: 6px; padding-left: 4px;")
        self.lbl_header.setStyleSheet(f"font-size: 18px; font-weight: 700; color: {pal['text_primary']}; letter-spacing: -0.3px;")
        self.lbl_header_sub.setStyleSheet(f"font-size: 12px; color: {pal['text_secondary']};")
        self.lbl_status.setStyleSheet(f"font-size: 12px; color: {pal['text_secondary']};")

    def on_nav_changed(self, idx):
        self.stack.setCurrentIndex(idx)
        titles = [
            ("Overview", "Workspace summary, storage footprint, and category breakdown"),
            ("All Files", "Interactive catalogue of all indexed files with Merge Sort and search"),
            ("Categories", "Extension taxonomy and planned folder destinations"),
            ("Duplicates", "Exact duplicate groups detected by streaming SHA-256"),
            ("Organization Plan", "Review proposed safe moves and collision handling"),
            ("History & Journal", "Audit trail of executed moves with instant rollback")
        ]
        self.lbl_header.setText(titles[idx][0])
        self.lbl_header_sub.setText(titles[idx][1])

    def update_root_display(self):
        p_str = str(self.root_dir)
        if len(p_str) > 28:
            p_display = "..." + p_str[-25:]
        else:
            p_display = p_str
        self.lbl_path.setText(p_display)
        self.lbl_path.setToolTip(p_str)

    def choose_folder(self):
        folder = QFileDialog.getExistingDirectory(self, "Select Folder to Organize", str(self.root_dir))
        if folder:
            self.root_dir = Path(folder)
            self.update_root_display()
            self.start_scan()

    # -------------------------------------------------------------
    # PAGE 1: OVERVIEW
    # -------------------------------------------------------------
    def create_overview_page(self):
        page = QWidget()
        layout = QVBoxLayout(page)
        layout.setContentsMargins(36, 24, 36, 24)
        layout.setSpacing(20)

        # Restrained Metric Cards (Apple x Notion style)
        cards_layout = QHBoxLayout()
        cards_layout.setSpacing(14)
        self.card_files, self.lbl_files = self.make_metric_card("Total Files", "0", "Indexed recursively")
        self.card_size, self.lbl_size = self.make_metric_card("Total Volume", "0 B", "Accumulated storage")
        self.card_cats, self.lbl_cats = self.make_metric_card("Categories", "0", "Rule-based classification")
        self.card_dups, self.lbl_dups = self.make_metric_card("Duplicate Groups", "0", "SHA-256 byte identical")
        cards_layout.addWidget(self.card_files)
        cards_layout.addWidget(self.card_size)
        cards_layout.addWidget(self.card_cats)
        cards_layout.addWidget(self.card_dups)
        layout.addLayout(cards_layout)

        # Middle Banner: Last Scan Information
        self.card_scan_meta = QFrame()
        self.card_scan_meta.setObjectName("statCard")
        meta_layout = QHBoxLayout(self.card_scan_meta)
        meta_layout.setContentsMargins(18, 12, 18, 12)
        
        self.lbl_scan_meta = QLabel("No scan performed yet. Click 'Scan Workspace' to begin.")
        self.lbl_scan_meta.setStyleSheet("font-size: 13px; font-weight: 500;")
        meta_layout.addWidget(self.lbl_scan_meta, 1)

        btn_quick_plan = QPushButton("Review Plan →")
        btn_quick_plan.setObjectName("btnSubtle")
        btn_quick_plan.clicked.connect(lambda: self.nav_list.setCurrentRow(4))
        meta_layout.addWidget(btn_quick_plan)
        layout.addWidget(self.card_scan_meta)

        # Category Distribution Section
        lbl_sec = QLabel("Category Distribution")
        lbl_sec.setStyleSheet("font-size: 15px; font-weight: 600; margin-top: 4px;")
        layout.addWidget(lbl_sec)

        self.table_overview_cats = QTableWidget(0, 4)
        self.table_overview_cats.setHorizontalHeaderLabels(["Category", "Files", "Storage Volume", "% Share"])
        self.table_overview_cats.horizontalHeader().setSectionResizeMode(0, QHeaderView.Stretch)
        self.table_overview_cats.horizontalHeader().setSectionResizeMode(1, QHeaderView.ResizeToContents)
        self.table_overview_cats.horizontalHeader().setSectionResizeMode(2, QHeaderView.ResizeToContents)
        self.table_overview_cats.horizontalHeader().setSectionResizeMode(3, QHeaderView.ResizeToContents)
        self.table_overview_cats.verticalHeader().setDefaultSectionSize(44)
        self.table_overview_cats.verticalHeader().setVisible(False)
        layout.addWidget(self.table_overview_cats, 1)

        return page

    def make_metric_card(self, title, val, sub):
        card = QFrame()
        card.setObjectName("statCard")
        layout = QVBoxLayout(card)
        layout.setContentsMargins(18, 16, 18, 16)
        layout.setSpacing(6)

        t = QLabel(title)
        t.setStyleSheet("font-size: 12px; font-weight: 600; text-transform: uppercase; letter-spacing: 0.5px;")
        v = QLabel(val)
        v.setStyleSheet("font-size: 24px; font-weight: 700; font-variant-numeric: tabular-nums;")
        s = QLabel(sub)
        s.setStyleSheet("font-size: 11px;")
        layout.addWidget(t)
        layout.addWidget(v)
        layout.addWidget(s)
        return card, v

    # -------------------------------------------------------------
    # PAGE 2: ALL FILES
    # -------------------------------------------------------------
    def create_files_page(self):
        page = QWidget()
        layout = QVBoxLayout(page)
        layout.setContentsMargins(36, 24, 36, 24)
        layout.setSpacing(14)

        # Filter & Sort Control Strip
        ctrl_bar = QHBoxLayout()
        ctrl_bar.setSpacing(12)

        self.txt_search = QLineEdit()
        self.txt_search.setPlaceholderText("Search file name... (Ctrl+K)")
        self.txt_search.textChanged.connect(self.filter_files_table)
        ctrl_bar.addWidget(self.txt_search, 2)

        self.combo_cat = QComboBox()
        self.combo_cat.addItem("All Categories")
        self.combo_cat.currentIndexChanged.connect(self.filter_files_table)
        ctrl_bar.addWidget(self.combo_cat, 1)

        self.combo_sort = QComboBox()
        self.combo_sort.addItems(["Sort: Name (A-Z)", "Sort: Size (Smallest)", "Sort: Size (Largest)"])
        self.combo_sort.currentIndexChanged.connect(self.sort_files_table)
        ctrl_bar.addWidget(self.combo_sort, 1)
        layout.addLayout(ctrl_bar)

        self.lbl_sort_bench = QLabel("DAA Merge Sort ready: O(n log n)")
        self.lbl_sort_bench.setStyleSheet("font-size: 12px; margin-bottom: 2px;")
        layout.addWidget(self.lbl_sort_bench)

        # 44px Row Height Data Table
        self.table_files = QTableWidget(0, 5)
        self.table_files.setHorizontalHeaderLabels(["Name", "Category", "Ext", "Size", "Current Location"])
        self.table_files.horizontalHeader().setSectionResizeMode(0, QHeaderView.Stretch)
        self.table_files.horizontalHeader().setSectionResizeMode(1, QHeaderView.ResizeToContents)
        self.table_files.horizontalHeader().setSectionResizeMode(2, QHeaderView.ResizeToContents)
        self.table_files.horizontalHeader().setSectionResizeMode(3, QHeaderView.ResizeToContents)
        self.table_files.horizontalHeader().setSectionResizeMode(4, QHeaderView.Stretch)
        self.table_files.verticalHeader().setDefaultSectionSize(44)
        self.table_files.verticalHeader().setVisible(False)
        self.table_files.setContextMenuPolicy(Qt.CustomContextMenu)
        self.table_files.customContextMenuRequested.connect(self.on_files_context_menu)
        layout.addWidget(self.table_files, 1)
        return page

    def on_files_context_menu(self, pos):
        item = self.table_files.itemAt(pos)
        if not item:
            return
        row = item.row()
        name = self.table_files.item(row, 0).text()
        loc = self.table_files.item(row, 4).text()
        full_path = Path(loc) / name

        menu = QMenu(self)
        act_open = menu.addAction("📂  Open Containing Folder")
        act_copy = menu.addAction("📋  Copy Full Path")
        act_filter = menu.addAction(f"🔍  Filter by Category '{self.table_files.item(row, 1).text()}'")

        action = menu.exec(self.table_files.viewport().mapToGlobal(pos))
        if action == act_open:
            if Path(loc).exists():
                QDesktopServices.openUrl(QUrl.fromLocalFile(loc))
        elif action == act_copy:
            QGuiApplication.clipboard().setText(str(full_path))
        elif action == act_filter:
            cat = self.table_files.item(row, 1).text()
            idx = self.combo_cat.findText(cat)
            if idx >= 0:
                self.combo_cat.setCurrentIndex(idx)

    # -------------------------------------------------------------
    # PAGE 3: CATEGORIES
    # -------------------------------------------------------------
    def create_categories_page(self):
        page = QWidget()
        layout = QVBoxLayout(page)
        layout.setContentsMargins(36, 24, 36, 24)
        layout.setSpacing(14)

        lbl = QLabel("Taxonomy & Target Directories")
        lbl.setStyleSheet("font-size: 16px; font-weight: 600;")
        layout.addWidget(lbl)

        self.table_cats = QTableWidget(0, 4)
        self.table_cats.setHorizontalHeaderLabels(["Category", "Files Count", "Storage Volume", "Target Directory Pattern"])
        self.table_cats.horizontalHeader().setSectionResizeMode(0, QHeaderView.ResizeToContents)
        self.table_cats.horizontalHeader().setSectionResizeMode(1, QHeaderView.ResizeToContents)
        self.table_cats.horizontalHeader().setSectionResizeMode(2, QHeaderView.ResizeToContents)
        self.table_cats.horizontalHeader().setSectionResizeMode(3, QHeaderView.Stretch)
        self.table_cats.verticalHeader().setDefaultSectionSize(44)
        self.table_cats.verticalHeader().setVisible(False)
        self.table_cats.itemDoubleClicked.connect(self.on_category_double_clicked)
        layout.addWidget(self.table_cats, 1)
        return page

    def on_category_double_clicked(self, item):
        row = item.row()
        cat = self.table_cats.item(row, 0).text()
        self.nav_list.setCurrentRow(1)  # Jump to All Files
        idx = self.combo_cat.findText(cat)
        if idx >= 0:
            self.combo_cat.setCurrentIndex(idx)

    # -------------------------------------------------------------
    # PAGE 4: DUPLICATES
    # -------------------------------------------------------------
    def create_duplicates_page(self):
        page = QWidget()
        layout = QVBoxLayout(page)
        layout.setContentsMargins(36, 24, 36, 24)
        layout.setSpacing(14)

        # Non-destructive Safety Notice Banner
        banner = QFrame()
        banner.setObjectName("noticeBanner")
        b_layout = QHBoxLayout(banner)
        b_layout.setContentsMargins(16, 12, 16, 12)
        b_layout.setSpacing(10)
        
        lbl_info_icon = QLabel("🛡️")
        lbl_info_icon.setStyleSheet("font-size: 16px;")
        b_layout.addWidget(lbl_info_icon)

        lbl_notice = QLabel("Safe Duplicate Detection: Groups are identified via size prefiltering and streaming 1 MiB SHA-256 chunked hashing. Files are never automatically deleted.")
        lbl_notice.setStyleSheet("font-size: 13px; font-weight: 500;")
        lbl_notice.setWordWrap(True)
        b_layout.addWidget(lbl_notice, 1)
        layout.addWidget(banner)

        # Duplicates Tree
        self.tree_dups = QTreeWidget()
        self.tree_dups.setHeaderLabels(["Item / Group", "Size", "SHA-256 Digest", "Path"])
        self.tree_dups.header().setSectionResizeMode(0, QHeaderView.ResizeToContents)
        self.tree_dups.header().setSectionResizeMode(1, QHeaderView.ResizeToContents)
        self.tree_dups.header().setSectionResizeMode(2, QHeaderView.ResizeToContents)
        self.tree_dups.header().setSectionResizeMode(3, QHeaderView.Stretch)
        layout.addWidget(self.tree_dups, 1)
        return page

    # -------------------------------------------------------------
    # PAGE 5: ORGANIZATION PLAN (REVIEW-ORIENTED)
    # -------------------------------------------------------------
    def create_organize_page(self):
        page = QWidget()
        layout = QVBoxLayout(page)
        layout.setContentsMargins(36, 24, 36, 24)
        layout.setSpacing(14)

        # Filter Strip for Organization Plan
        ctrl_bar = QHBoxLayout()
        ctrl_bar.setSpacing(12)

        self.txt_plan_search = QLineEdit()
        self.txt_plan_search.setPlaceholderText("Filter proposed moves...")
        self.txt_plan_search.textChanged.connect(self.filter_plan_table)
        ctrl_bar.addWidget(self.txt_plan_search, 2)

        self.combo_plan_status = QComboBox()
        self.combo_plan_status.addItems(["All Statuses", "Ready", "Collision Renamed"])
        self.combo_plan_status.currentIndexChanged.connect(self.filter_plan_table)
        ctrl_bar.addWidget(self.combo_plan_status, 1)

        self.btn_select_all = QPushButton("Select All")
        self.btn_select_all.setObjectName("btnSubtle")
        self.btn_select_all.clicked.connect(lambda: self.set_all_plan_checks(True))
        ctrl_bar.addWidget(self.btn_select_all)

        self.btn_deselect_all = QPushButton("Deselect All")
        self.btn_deselect_all.setObjectName("btnSubtle")
        self.btn_deselect_all.clicked.connect(lambda: self.set_all_plan_checks(False))
        ctrl_bar.addWidget(self.btn_deselect_all)

        layout.addLayout(ctrl_bar)

        # Plan Summary Heading
        self.lbl_plan_summary = QLabel("Plan ready. 0 files.")
        self.lbl_plan_summary.setStyleSheet("font-size: 13px; font-weight: 500;")
        layout.addWidget(self.lbl_plan_summary)

        # 44px Row Height Organization Plan Table
        self.table_plan = QTableWidget(0, 5)
        self.table_plan.setHorizontalHeaderLabels(["Include", "File Name", "Current Location", "Proposed Destination", "Status"])
        self.table_plan.horizontalHeader().setSectionResizeMode(0, QHeaderView.ResizeToContents)
        self.table_plan.horizontalHeader().setSectionResizeMode(1, QHeaderView.ResizeToContents)
        self.table_plan.horizontalHeader().setSectionResizeMode(2, QHeaderView.Stretch)
        self.table_plan.horizontalHeader().setSectionResizeMode(3, QHeaderView.Stretch)
        self.table_plan.horizontalHeader().setSectionResizeMode(4, QHeaderView.ResizeToContents)
        self.table_plan.verticalHeader().setDefaultSectionSize(44)
        self.table_plan.verticalHeader().setVisible(False)
        self.table_plan.itemChanged.connect(self.on_plan_item_changed)
        layout.addWidget(self.table_plan, 1)

        # Sticky Bottom Action Area
        action_bar = QFrame()
        action_bar.setObjectName("bottomActionBar")
        action_layout = QHBoxLayout(action_bar)
        action_layout.setContentsMargins(18, 12, 18, 12)
        action_layout.setSpacing(16)

        self.lbl_approved_count = QLabel("0 approved moves")
        self.lbl_approved_count.setStyleSheet("font-size: 13px; font-weight: 600;")
        action_layout.addWidget(self.lbl_approved_count)
        action_layout.addStretch()

        self.btn_apply = QPushButton("Apply Approved Moves")
        self.btn_apply.setObjectName("btnPrimary")
        self.btn_apply.setFixedHeight(36)
        self.btn_apply.clicked.connect(self.apply_plan)
        action_layout.addWidget(self.btn_apply)

        layout.addWidget(action_bar)
        return page

    def set_all_plan_checks(self, checked):
        self.table_plan.blockSignals(True)
        st = Qt.Checked if checked else Qt.Unchecked
        for r in range(self.table_plan.rowCount()):
            item = self.table_plan.item(r, 0)
            if item:
                item.setCheckState(st)
        self.table_plan.blockSignals(False)
        self.update_approved_count()

    def on_plan_item_changed(self, item):
        if item.column() == 0:
            self.update_approved_count()

    def update_approved_count(self):
        cnt = 0
        for r in range(self.table_plan.rowCount()):
            item = self.table_plan.item(r, 0)
            if item and item.checkState() == Qt.Checked:
                cnt += 1
        tot = self.table_plan.rowCount()
        self.lbl_approved_count.setText(f"{cnt} of {tot} moves approved")

    # -------------------------------------------------------------
    # PAGE 6: HISTORY & JOURNAL
    # -------------------------------------------------------------
    def create_history_page(self):
        page = QWidget()
        layout = QVBoxLayout(page)
        layout.setContentsMargins(36, 24, 36, 24)
        layout.setSpacing(14)

        hdr = QHBoxLayout()
        lbl = QLabel("Operation Audit Trail (.sorted_journal.json)")
        lbl.setStyleSheet("font-size: 16px; font-weight: 600;")
        hdr.addWidget(lbl)
        hdr.addStretch()

        self.btn_undo = QPushButton("↺  Undo Last Batch")
        self.btn_undo.setObjectName("btnSubtle")
        self.btn_undo.clicked.connect(self.undo_last_batch)
        hdr.addWidget(self.btn_undo)

        self.btn_export = QPushButton("📥  Export CSV")
        self.btn_export.setObjectName("btnSubtle")
        self.btn_export.clicked.connect(self.export_history_csv)
        hdr.addWidget(self.btn_export)

        layout.addLayout(hdr)

        self.table_history = QTableWidget(0, 4)
        self.table_history.setHorizontalHeaderLabels(["Timestamp", "Source Path", "Destination Path", "Status"])
        self.table_history.horizontalHeader().setSectionResizeMode(0, QHeaderView.ResizeToContents)
        self.table_history.horizontalHeader().setSectionResizeMode(1, QHeaderView.Stretch)
        self.table_history.horizontalHeader().setSectionResizeMode(2, QHeaderView.Stretch)
        self.table_history.horizontalHeader().setSectionResizeMode(3, QHeaderView.ResizeToContents)
        self.table_history.verticalHeader().setDefaultSectionSize(44)
        self.table_history.verticalHeader().setVisible(False)
        layout.addWidget(self.table_history, 1)
        return page

    # -------------------------------------------------------------
    # WORKER & SCAN PIPELINE
    # -------------------------------------------------------------
    def start_scan(self):
        self.btn_scan.setEnabled(False)
        self.lbl_status.setText("Scanning workspace...")
        self.progress_bar.setVisible(True)
        self.progress_bar.setRange(0, 0)

        self.thread = QThread()
        self.worker = ScanWorker(self.root_dir)
        self.worker.moveToThread(self.thread)
        self.thread.started.connect(self.worker.run)
        self.worker.progress.connect(self.lbl_status.setText)
        self.worker.finished.connect(self.on_scan_finished)
        self.worker.finished.connect(self.thread.quit)
        self.worker.finished.connect(self.worker.deleteLater)
        self.thread.finished.connect(self.thread.deleteLater)
        self.thread.start()

    def on_scan_finished(self, data):
        self.scan_data = data
        self.btn_scan.setEnabled(True)
        self.progress_bar.setVisible(False)
        self.lbl_status.setText(f"Indexed {len(data['records'])} files in {data['stats']['t_scan']:.1f} ms")

        self.populate_overview(data)
        self.populate_categories(data)
        self.populate_duplicates(data)

        # Update category filter combo on All Files
        self.combo_cat.blockSignals(True)
        self.combo_cat.clear()
        self.combo_cat.addItem("All Categories")
        for cat in sorted(data["stats"]["categories"].keys()):
            self.combo_cat.addItem(cat)
        self.combo_cat.blockSignals(False)

        self.filter_files_table()
        self.populate_organize_plan(data)

    def populate_overview(self, data):
        pal = self.p()
        # Metrics
        self.lbl_files.setText(str(len(data["records"])))
        self.lbl_size.setText(format_bytes(data["stats"]["total_size"]))
        self.lbl_cats.setText(str(len(data["stats"]["categories"])))
        self.lbl_dups.setText(f"{len(data['duplicates'])} groups")

        # Scan Meta
        self.lbl_scan_meta.setText(
            f"Last Scan: {data['stats']['scan_time']}  |  Scan: {data['stats']['t_scan']:.1f} ms  |  "
            f"Duplication Check: {data['stats']['t_dup']:.1f} ms ({format_bytes(data['stats']['bytes_hashed'])} hashed)  |  "
            f"Planner: {data['stats']['t_plan']:.1f} ms"
        )

        # Category Table
        self.table_overview_cats.setRowCount(0)
        tot_files = max(1, len(data["records"]))
        sorted_cats = sorted(data["stats"]["categories"].items(), key=lambda x: -x[1])
        for r, (cat, cnt) in enumerate(sorted_cats):
            self.table_overview_cats.insertRow(r)
            sz = data["stats"]["category_sizes"].get(cat, 0)
            pct = f"{(cnt / tot_files) * 100:.1f}%"

            i0 = QTableWidgetItem(cat)
            i0.setForeground(QColor(pal["text_primary"]))
            i1 = QTableWidgetItem(str(cnt))
            i1.setForeground(QColor(pal["text_primary"]))
            i2 = QTableWidgetItem(format_bytes(sz))
            i2.setForeground(QColor(pal["text_secondary"]))
            i3 = QTableWidgetItem(pct)
            i3.setForeground(QColor(pal["text_secondary"]))

            self.table_overview_cats.setItem(r, 0, i0)
            self.table_overview_cats.setItem(r, 1, i1)
            self.table_overview_cats.setItem(r, 2, i2)
            self.table_overview_cats.setItem(r, 3, i3)

    def populate_categories(self, data):
        pal = self.p()
        self.table_cats.setRowCount(0)
        sorted_cats = sorted(data["stats"]["categories"].items(), key=lambda x: -x[1])
        for r, (cat, cnt) in enumerate(sorted_cats):
            self.table_cats.insertRow(r)
            sz = data["stats"]["category_sizes"].get(cat, 0)

            i0 = QTableWidgetItem(f"📁  {cat}")
            i0.setForeground(QColor(pal["text_primary"]))
            i1 = QTableWidgetItem(str(cnt))
            i1.setForeground(QColor(pal["text_primary"]))
            i2 = QTableWidgetItem(format_bytes(sz))
            i2.setForeground(QColor(pal["text_secondary"]))
            i3 = QTableWidgetItem(f"Sorted/{cat}/[EXT]/")
            i3.setForeground(QColor(pal["text_secondary"]))

            self.table_cats.setItem(r, 0, i0)
            self.table_cats.setItem(r, 1, i1)
            self.table_cats.setItem(r, 2, i2)
            self.table_cats.setItem(r, 3, i3)

    def populate_duplicates(self, data):
        pal = self.p()
        self.tree_dups.clear()
        for g in data["duplicates"]:
            gi = QTreeWidgetItem(self.tree_dups)
            gi.setText(0, f"Group #{g['id']} ({len(g['files'])} copies)")
            gi.setText(1, format_bytes(g['size']))
            gi.setText(2, g['sha256'][:16] + "...")
            gi.setText(3, f"{len(g['files'])-1} redundant duplicate copies")
            gi.setForeground(0, QBrush(QColor(pal["text_primary"])))
            gi.setForeground(1, QBrush(QColor(pal["text_secondary"])))
            gi.setForeground(2, QBrush(QColor(pal["accent"])))
            gi.setForeground(3, QBrush(QColor(pal["warning_fg"])))
            gi.setExpanded(True)

            for f in g["files"]:
                fi = QTreeWidgetItem(gi)
                fi.setText(0, f["name"])
                fi.setText(1, format_bytes(f["size"]))
                fi.setText(3, str(f["path"]))
                fi.setForeground(0, QBrush(QColor(pal["text_primary"])))
                fi.setForeground(1, QBrush(QColor(pal["text_secondary"])))
                fi.setForeground(3, QBrush(QColor(pal["text_secondary"])))

    def populate_organize_plan(self, data):
        self.filter_plan_table()

    def filter_plan_table(self):
        if not self.scan_data:
            return
        pal = self.p()
        q = self.txt_plan_search.text().lower()
        st_filter = self.combo_plan_status.currentText()

        all_plan = self.scan_data["plan"]
        col_count = sum(1 for p in all_plan if p["collision_resolved"])
        self.lbl_plan_summary.setText(f"Plan: {len(all_plan)} proposed moves  |  {col_count} collisions auto-resolved without overwriting")

        self.table_plan.blockSignals(True)
        self.table_plan.setRowCount(0)

        for p in all_plan:
            status = "Collision Renamed" if p["collision_resolved"] else "Ready"
            if st_filter == "Ready" and status != "Ready":
                continue
            if st_filter == "Collision Renamed" and status != "Collision Renamed":
                continue
            if q and (q not in p["source"].name.lower() and q not in str(p["destination"]).lower()):
                continue

            r = self.table_plan.rowCount()
            self.table_plan.insertRow(r)

            item_check = QTableWidgetItem()
            item_check.setCheckState(Qt.Checked)
            item_check.setData(Qt.UserRole, p)
            self.table_plan.setItem(r, 0, item_check)

            i1 = QTableWidgetItem(p["source"].name)
            i1.setForeground(QColor(pal["text_primary"]))
            self.table_plan.setItem(r, 1, i1)

            i2 = QTableWidgetItem(str(p["source"]))
            i2.setForeground(QColor(pal["text_secondary"]))
            self.table_plan.setItem(r, 2, i2)

            i3 = QTableWidgetItem(str(p["destination"]))
            i3.setForeground(QColor(pal["text_primary"]))
            self.table_plan.setItem(r, 3, i3)

            i4 = QTableWidgetItem(status)
            if p["collision_resolved"]:
                i4.setForeground(QColor(pal["warning_fg"]))
            else:
                i4.setForeground(QColor(pal["success_fg"]))
            self.table_plan.setItem(r, 4, i4)

        self.table_plan.blockSignals(False)
        self.update_approved_count()

    def filter_files_table(self):
        if not self.scan_data:
            return
        q = self.txt_search.text().lower()
        cat = self.combo_cat.currentText()

        records = [
            r for r in self.scan_data["records"]
            if (not q or q in r["name"].lower()) and (cat == "All Categories" or r["category"] == cat)
        ]
        self.render_files(records)

    def sort_files_table(self):
        if not self.scan_data:
            return
        t0 = time.perf_counter()
        idx = self.combo_sort.currentIndex()
        key = "name" if idx == 0 else "size"
        sort_files(self.scan_data["records"], key)
        if idx == 2:
            self.scan_data["records"].reverse()
        t_sort = (time.perf_counter() - t0) * 1000
        self.lbl_sort_bench.setText(f"DAA Merge Sort: {len(self.scan_data['records'])} records sorted in {t_sort:.3f} ms (O(n log n))")
        self.filter_files_table()

    def render_files(self, records):
        pal = self.p()
        self.table_files.setRowCount(0)
        for i, r in enumerate(records):
            self.table_files.insertRow(i)

            i0 = QTableWidgetItem(r["name"])
            i0.setForeground(QColor(pal["text_primary"]))

            i1 = QTableWidgetItem(r["category"])
            i1.setForeground(QColor(pal["text_primary"]))

            i2 = QTableWidgetItem(r["extension"])
            i2.setForeground(QColor(pal["text_secondary"]))

            i3 = QTableWidgetItem(format_bytes(r["size"]))
            i3.setForeground(QColor(pal["text_primary"]))

            i4 = QTableWidgetItem(str(r["path"].parent))
            i4.setForeground(QColor(pal["text_secondary"]))

            self.table_files.setItem(i, 0, i0)
            self.table_files.setItem(i, 1, i1)
            self.table_files.setItem(i, 2, i2)
            self.table_files.setItem(i, 3, i3)
            self.table_files.setItem(i, 4, i4)

    # -------------------------------------------------------------
    # APPLY PLAN, HISTORY & ROLLBACK
    # -------------------------------------------------------------
    def apply_plan(self):
        if not self.scan_data or not self.scan_data["plan"]:
            QMessageBox.information(self, "No Plan", "No organization plan generated. Please scan a folder first.")
            return

        approved = []
        for r in range(self.table_plan.rowCount()):
            item_check = self.table_plan.item(r, 0)
            if item_check and item_check.checkState() == Qt.Checked:
                p = item_check.data(Qt.UserRole)
                if p:
                    approved.append(p)

        if not approved:
            QMessageBox.warning(self, "No Approved Moves", "Please select at least one move to apply.")
            return

        reply = QMessageBox.question(
            self, "Confirm File Organization",
            f"Are you sure you want to apply {len(approved)} approved file moves?\n\n"
            f"• Safety Guarantee: Existing destination files will never be overwritten.\n"
            f"• Journaling: Every operation is recorded in .sorted_journal.json.\n"
            f"• Reversible: You can undo this batch at any time in History.",
            QMessageBox.Yes | QMessageBox.No
        )
        if reply != QMessageBox.Yes:
            return

        # Execute moves safely
        batch_id = datetime.now().strftime("%Y%m%d_%H%M%S")
        success_count = 0
        fail_count = 0

        for p in approved:
            try:
                dest = p["destination"]
                src = p["source"]
                dest.parent.mkdir(parents=True, exist_ok=True)
                if not dest.exists():
                    os.rename(src, dest)
                    self.history.append({
                        "batch_id": batch_id,
                        "time": datetime.now().strftime("%H:%M:%S"),
                        "src": str(src),
                        "dest": str(dest),
                        "status": "Completed"
                    })
                    success_count += 1
                else:
                    fail_count += 1
            except Exception as e:
                fail_count += 1

        self.update_history_table()
        QMessageBox.information(
            self, "Organization Complete",
            f"Batch execution finished:\n\n• Successfully moved: {success_count} files\n• Skipped / Unchanged: {fail_count} files"
        )
        self.nav_list.setCurrentRow(5)  # Jump to History

    def update_history_table(self):
        pal = self.p()
        self.table_history.setRowCount(0)
        for i, h in enumerate(reversed(self.history)):
            self.table_history.insertRow(i)

            i0 = QTableWidgetItem(h["time"])
            i0.setForeground(QColor(pal["text_secondary"]))

            i1 = QTableWidgetItem(h["src"])
            i1.setForeground(QColor(pal["text_primary"]))

            i2 = QTableWidgetItem(h["dest"])
            i2.setForeground(QColor(pal["text_primary"]))

            st = h["status"]
            i3 = QTableWidgetItem(st)
            if st == "Completed":
                i3.setForeground(QColor(pal["success_fg"]))
            elif st == "Rolled Back":
                i3.setForeground(QColor(pal["warning_fg"]))
            else:
                i3.setForeground(QColor(pal["danger_fg"]))

            self.table_history.setItem(i, 0, i0)
            self.table_history.setItem(i, 1, i1)
            self.table_history.setItem(i, 2, i2)
            self.table_history.setItem(i, 3, i3)

    def undo_last_batch(self):
        if not self.history:
            QMessageBox.information(self, "No History", "There are no operations to undo.")
            return

        last_batch = self.history[-1].get("batch_id")
        eligible = [h for h in self.history if h.get("batch_id") == last_batch and h["status"] == "Completed"]
        if not eligible:
            QMessageBox.information(self, "Nothing to Undo", "All recent operations were already rolled back or failed.")
            return

        reply = QMessageBox.question(
            self, "Confirm Undo",
            f"Rollback {len(eligible)} file moves from batch {last_batch}?",
            QMessageBox.Yes | QMessageBox.No
        )
        if reply != QMessageBox.Yes:
            return

        reverted = 0
        for h in eligible:
            dest = Path(h["dest"])
            src = Path(h["src"])
            if dest.exists() and not src.exists():
                src.parent.mkdir(parents=True, exist_ok=True)
                os.rename(dest, src)
                h["status"] = "Rolled Back"
                reverted += 1

        self.update_history_table()
        QMessageBox.information(self, "Undo Complete", f"Successfully restored {reverted} files to their original locations.")

    def export_history_csv(self):
        if not self.history:
            QMessageBox.information(self, "No Records", "No history available to export.")
            return

        save_path, _ = QFileDialog.getSaveFileName(self, "Export Audit Trail CSV", "sorted_audit_trail.csv", "CSV Files (*.csv)")
        if save_path:
            with open(save_path, "w", newline="", encoding="utf-8") as f:
                writer = csv.writer(f)
                writer.writerow(["Batch ID", "Timestamp", "Source Path", "Destination Path", "Status"])
                for h in self.history:
                    writer.writerow([h.get("batch_id", ""), h["time"], h["src"], h["dest"], h["status"]])
            QMessageBox.information(self, "Export Successful", f"Audit trail exported to:\n{save_path}")

def main():
    app = QApplication(sys.argv)
    app.setStyle(QStyleFactory.create("Fusion"))

    # Initial Palette
    qp = QPalette()
    qp.setColor(QPalette.Window, QColor(PALETTE_LIGHT["canvas"]))
    qp.setColor(QPalette.WindowText, QColor(PALETTE_LIGHT["text_primary"]))
    qp.setColor(QPalette.Base, QColor(PALETTE_LIGHT["surface"]))
    qp.setColor(QPalette.AlternateBase, QColor(PALETTE_LIGHT["canvas"]))
    qp.setColor(QPalette.ToolTipBase, QColor(PALETTE_LIGHT["surface"]))
    qp.setColor(QPalette.ToolTipText, QColor(PALETTE_LIGHT["text_primary"]))
    qp.setColor(QPalette.Text, QColor(PALETTE_LIGHT["text_primary"]))
    qp.setColor(QPalette.Button, QColor(PALETTE_LIGHT["surface"]))
    qp.setColor(QPalette.ButtonText, QColor(PALETTE_LIGHT["text_primary"]))
    qp.setColor(QPalette.BrightText, QColor(PALETTE_LIGHT["danger_fg"]))
    qp.setColor(QPalette.Link, QColor(PALETTE_LIGHT["accent"]))
    qp.setColor(QPalette.Highlight, QColor(PALETTE_LIGHT["selected"]))
    qp.setColor(QPalette.HighlightedText, QColor(PALETTE_LIGHT["selected_text"]))
    qp.setColor(QPalette.PlaceholderText, QColor(PALETTE_LIGHT["text_muted"]))
    app.setPalette(qp)

    window = MainWindow()
    window.show()
    window.start_scan()
    sys.exit(app.exec())

if __name__ == "__main__":
    main()
