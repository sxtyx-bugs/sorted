#!/usr/bin/env python3
"""
SORTED — Desktop File Organizer Application Entry Point
"""
import sys
import os
import datetime
import platform
from pathlib import Path

# Ensure repository root is on sys.path
REPO_ROOT = Path(__file__).resolve().parent.parent
if str(REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(REPO_ROOT))

from PySide6.QtCore import QStandardPaths
from PySide6.QtGui import QGuiApplication
from app.desktop_app import MainWindow, QApplication

def log_diagnostic_header():
    try:
        app_data = QStandardPaths.writableLocation(QStandardPaths.AppDataLocation)
        if not app_data:
            app_data = str(Path.home() / ".sorted")
        elif not app_data.endswith("SORTED"):
            app_data = str(Path(app_data) / "SORTED")
        log_dir = Path(app_data) / "logs"
        log_dir.mkdir(parents=True, exist_ok=True)
        log_file = log_dir / "sorted_dev.log"

        with open(log_file, "a", encoding="utf-8") as f:
            f.write("============================================================\n")
            f.write(f"SORTED Python Dev Session: {datetime.datetime.now().isoformat()}\n")
            f.write(f"Python Executable: {sys.executable}\n")
            f.write(f"Python Version:    {sys.version.splitlines()[0]}\n")
            f.write(f"Working Dir:       {os.getcwd()}\n")
            f.write(f"OS:                {platform.platform()} ({platform.machine()})\n")
            screen = QGuiApplication.primaryScreen()
            if screen:
                geo = screen.geometry()
                f.write(f"Primary Screen:    {geo.width()}x{geo.height()} (DPI: {screen.logicalDotsPerInch():.1f}, DPR: {screen.devicePixelRatio():.2f})\n")
            f.write("============================================================\n")
    except Exception as e:
        print(f"Warning: Failed to write dev diagnostic log: {e}", file=sys.stderr)

def run_app():
    app = QApplication.instance() or QApplication(sys.argv)
    log_diagnostic_header()
    window = MainWindow()
    window.show()
    return app.exec()

if __name__ == "__main__":
    sys.exit(run_app())
