import sys
import os
import shutil
from pathlib import Path

os.environ["QT_QPA_PLATFORM"] = "offscreen"

REPO_ROOT = Path(__file__).resolve().parent.parent.parent
if str(REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(REPO_ROOT))

from PySide6.QtWidgets import QApplication
from PySide6.QtCore import Qt, QSize
from PySide6.QtGui import QPixmap

from app.desktop_app import MainWindow, ScanWorker

def main():
    app = QApplication.instance() or QApplication(sys.argv)

    test_ws = REPO_ROOT / "tests" / "fixtures" / "SORTED_TEST"
    
    win = MainWindow()
    win.resize(1180, 760)
    win.show()

    # Load test workspace and scan
    win.root_dir = test_ws
    worker = ScanWorker(test_ws)
    def on_scan(data):
        win.on_scan_finished(data)
    worker.finished.connect(on_scan)
    worker.run()
    app.processEvents()

    build_dir = REPO_ROOT / "build"
    build_dir.mkdir(parents=True, exist_ok=True)
    brain_dir = Path(r"C:\Users\satya\.gemini\antigravity\brain\6146e3ed-a344-4a5c-aa29-a0c8ad8ef9f6")

    # 1. Capture Light Mode (Overview)
    win.is_dark_mode = False
    win.apply_theme()
    win.nav_list.setCurrentRow(0) # Overview
    app.processEvents()

    pix_light = win.grab()
    light_path = build_dir / "app_glass_light.png"
    pix_light.save(str(light_path), "PNG")
    print(f"Saved Light Mode Overview render to: {light_path}")
    if brain_dir.exists():
        shutil.copy(light_path, brain_dir / "app_glass_light.png")

    # 2. Capture Light Mode (Organization Plan - showing inspectability, clear buttons, regular glass panels)
    win.nav_list.setCurrentRow(4) # Organization
    app.processEvents()
    pix_light_plan = win.grab()
    light_plan_path = build_dir / "app_glass_light_plan.png"
    pix_light_plan.save(str(light_plan_path), "PNG")
    print(f"Saved Light Mode Plan render to: {light_plan_path}")
    if brain_dir.exists():
        shutil.copy(light_plan_path, brain_dir / "app_glass_light_plan.png")

    # 3. Capture Dark Mode (Overview)
    win.is_dark_mode = True
    win.apply_theme()
    win.nav_list.setCurrentRow(0) # Overview
    app.processEvents()

    pix_dark = win.grab()
    dark_path = build_dir / "app_glass_dark.png"
    pix_dark.save(str(dark_path), "PNG")
    print(f"Saved Dark Mode Overview render to: {dark_path}")
    if brain_dir.exists():
        shutil.copy(dark_path, brain_dir / "app_glass_dark.png")

    # 4. Capture Dark Mode (Organization Plan)
    win.nav_list.setCurrentRow(4) # Organization
    app.processEvents()
    pix_dark_plan = win.grab()
    dark_plan_path = build_dir / "app_glass_dark_plan.png"
    pix_dark_plan.save(str(dark_plan_path), "PNG")
    print(f"Saved Dark Mode Plan render to: {dark_plan_path}")
    if brain_dir.exists():
        shutil.copy(dark_plan_path, brain_dir / "app_glass_dark_plan.png")

    print("All full-application liquid glass renders captured successfully!")

if __name__ == "__main__":
    main()
