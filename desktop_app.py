#!/usr/bin/env python3
"""
SORTED Desktop Application Entry Wrapper (Root Convenience Launcher)
Delegates to app/main.py and app/desktop_app.py.
"""
import sys
import os
from pathlib import Path

# Add project root to sys.path
sys.path.insert(0, str(Path(__file__).resolve().parent))

from app.desktop_app import MainWindow, QApplication

if __name__ == "__main__":
    app = QApplication.instance() or QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())
