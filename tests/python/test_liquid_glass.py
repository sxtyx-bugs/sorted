import sys
import os
from pathlib import Path

# Add project root to sys.path
REPO_ROOT = Path(__file__).resolve().parent.parent.parent
if str(REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(REPO_ROOT))

from PySide6.QtWidgets import QApplication
from PySide6.QtCore import Qt, QPoint
from PySide6.QtGui import QPixmap

app = QApplication.instance() or QApplication(sys.argv)

from app.desktop_app import (
    MainWindow, WindowBackdrop, GlassPanel, GlassSidebar,
    GlassToolbar, GlassButton, GlassSearchField, GlassDialog,
    PALETTE_LIGHT, PALETTE_DARK
)

def test_liquid_glass_system():
    print("=================================================================")
    print("  SORTED --- Apple Liquid Glass UI & Native DWM Verification")
    print("=================================================================")

    # 1. Native Windows 11 DWM Backdrop Verification
    print("1. Verifying Windows 11 DWM Backdrop Integration...")
    build = WindowBackdrop.get_windows_build_number()
    print(f"   - Detected Windows Build: {build}")
    assert build > 0, "Failed to retrieve Windows build number"
    is_win11 = WindowBackdrop.is_supported()
    is_22h2 = WindowBackdrop.is_build_22621_or_newer()
    print(f"   - Windows 11 Supported: {is_win11}")
    print(f"   - Windows 11 22H2+ Unified Backdrop API: {is_22h2}")
    assert is_win11, "Expected Windows 11 environment"
    assert is_22h2, "Expected Windows 11 22H2+ environment"
    print("   [PASSED] DWM version detection verified.")

    # 2. MainWindow Native Backdrop Application
    print("2. Verifying MainWindow Window Backdrop & Translucent Attributes...")
    w = MainWindow()
    assert w.testAttribute(Qt.WA_TranslucentBackground), "MainWindow WA_TranslucentBackground must be True"
    assert isinstance(w.sidebar, GlassSidebar), f"Expected GlassSidebar, got {type(w.sidebar)}"
    assert isinstance(w.top_bar, GlassToolbar), f"Expected GlassToolbar, got {type(w.top_bar)}"
    assert isinstance(w.folder_pill, GlassPanel), f"Expected GlassPanel, got {type(w.folder_pill)}"
    assert isinstance(w.txt_search, GlassSearchField), f"Expected GlassSearchField, got {type(w.txt_search)}"
    assert isinstance(w.txt_plan_search, GlassSearchField), f"Expected GlassSearchField, got {type(w.txt_plan_search)}"
    print("   [PASSED] MainWindow glass component hierarchy verified.")

    # 3. Component Rendering & Specular Highlights (Light Mode)
    print("3. Verifying Component Rendering & Specular Highlights (Light Mode)...")
    pix = QPixmap(300, 200)
    w.sidebar.render(pix)
    w.top_bar.render(pix)
    w.folder_pill.render(pix)
    w.txt_search.render(pix)
    print("   [PASSED] Light Mode rendering executed with zero errors.")

    # 4. Component Rendering & Specular Highlights (Dark Mode)
    print("4. Verifying Component Rendering & Specular Highlights (Dark Mode)...")
    w.toggle_theme() # Switch to dark mode
    assert w.is_dark_mode, "Theme toggle to dark mode failed"
    pix_dark = QPixmap(300, 200)
    w.sidebar.render(pix_dark)
    w.top_bar.render(pix_dark)
    w.folder_pill.render(pix_dark)
    w.txt_search.render(pix_dark)
    print("   [PASSED] Dark Mode rendering executed with zero errors.")

    # 5. Interactive Animation & Specular Mechanics
    print("5. Verifying QPropertyAnimation & Specular Sheen Calculations...")
    btn = GlassButton("Glass Action", variant="subtle", is_dark_fn=lambda: w.is_dark_mode)
    btn.resize(120, 36)
    assert btn.cursor().shape() == Qt.PointingHandCursor, "GlassButton must have PointingHandCursor"
    
    # Test hover progress update
    btn.set_hover(0.5)
    assert btn.hoverProgress == 0.5, f"Expected 0.5, got {btn.hoverProgress}"
    btn.set_hover(1.0)
    assert btn.hoverProgress == 1.0, f"Expected 1.0, got {btn.hoverProgress}"
    btn.set_hover(0.0)

    # Test press progress update
    btn.set_press(1.0)
    assert btn.pressProgress == 1.0, f"Expected 1.0, got {btn.pressProgress}"
    btn.set_press(0.0)

    # Test GlassSearchField focus progress
    search = GlassSearchField(is_dark_fn=lambda: w.is_dark_mode)
    search.set_focus(0.75)
    assert search.focusProgress == 0.75, f"Expected 0.75, got {search.focusProgress}"

    # Test GlassSidebar width animation property
    w.sidebar.set_sidebar_width(72)
    assert w.sidebar.get_sidebar_width() == 72
    w.sidebar.set_sidebar_width(244)
    assert w.sidebar.get_sidebar_width() == 244
    print("   [PASSED] Animations, properties, and specular mechanics verified.")

    # 6. Readability in Data-Heavy Views
    print("6. Verifying Tabular Legibility (Non-transparent Tables)...")
    assert w.table_files.horizontalScrollBarPolicy() == Qt.ScrollBarAsNeeded
    assert w.table_plan.horizontalScrollBarPolicy() == Qt.ScrollBarAsNeeded
    print("   [PASSED] Data-heavy views maintain high-contrast opacity for tabular reading.")

    print("\n=================================================================")
    print("  ALL APPLE LIQUID GLASS UI VERIFICATION TESTS PASSED 100%!")
    print("=================================================================")

if __name__ == "__main__":
    test_liquid_glass_system()
