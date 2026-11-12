#!/usr/bin/env python3
"""
SORTED — Desktop File Organizer Application Entry Point
"""
import sys
import os
from pathlib import Path

# Ensure repository root is on sys.path
REPO_ROOT = Path(__file__).resolve().parent.parent
if str(REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(REPO_ROOT))

from app.desktop_app import MainWindow, QApplication

def run_app():
    app = QApplication.instance() or QApplication(sys.argv)
    window = MainWindow()
    window.show()
    return app.exec()

if __name__ == "__main__":
    sys.exit(run_app())
