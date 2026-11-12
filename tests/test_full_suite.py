import subprocess
import sys
import os
from pathlib import Path

print("=================================================")
print("  SORTED — Full Verification & Acceptance Suite  ")
print("=================================================")

# 1. C++17 Core Verification
print("[1/3] Running Compiled C++17 Core Verification Suite...")
env = os.environ.copy()
env["PATH"] = r"C:\QtTools\Tools\mingw1310_64\bin;" + env.get("PATH", "")
res1 = subprocess.run([".\\test_core.exe"], capture_output=True, text=True, env=env)
print(res1.stdout.strip())
assert res1.returncode == 0, f"C++ test failed: {res1.stderr}"
print("=> C++17 Core Suite: PASSED!\n")

# 2. Python CLI Engine on real dataset
print("[2/3] Running Python CLI Engine on SORTED_TEST...")
res2 = subprocess.run([sys.executable, "scripts/run_sorted_cli.py", "SORTED_TEST"], capture_output=True, text=True)
print(res2.stdout.strip())
assert res2.returncode == 0, f"CLI test failed: {res2.stderr}"
print("=> Python CLI Engine: PASSED!\n")

# 3. Headless Qt 6 Desktop Application Pipeline
print("[3/3] Running Headless PySide6 Desktop Application Pipeline...")
os.environ["QT_QPA_PLATFORM"] = "offscreen"
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))
import desktop_app
from PySide6.QtWidgets import QApplication

app = QApplication.instance() or QApplication([])
win = desktop_app.MainWindow()

# Test Navigation
assert win.stack.count() == 6, "Stack should have 6 pages"
for i in range(6):
    win.nav_list.setCurrentRow(i)
    assert win.stack.currentIndex() == i

# Test Light & Dark Theme Toggle
win.toggle_theme()
assert win.is_dark_mode == True
win.toggle_theme()
assert win.is_dark_mode == False

# Test Sidebar Collapse / Expand
win.toggle_sidebar()
assert win.is_sidebar_collapsed == True
win.toggle_sidebar()
assert win.is_sidebar_collapsed == False

# Test Scan Worker
worker = desktop_app.ScanWorker(Path("SORTED_TEST").resolve())
data_res = {}
worker.finished.connect(lambda d: data_res.update(d))
worker.run()
win.on_scan_finished(data_res)

assert len(data_res["records"]) == 8, f"Expected 8 records, got {len(data_res['records'])}"
assert len(data_res["duplicates"]) == 1, f"Expected 1 dup group, got {len(data_res['duplicates'])}"
assert len(data_res["plan"]) == 8, f"Expected 8 moves, got {len(data_res['plan'])}"

# Test DAA Merge Sort
win.combo_sort.setCurrentIndex(1)
win.sort_files_table()

# Test Search Filter
win.txt_search.setText("photo")
win.filter_files_table()
assert win.table_files.rowCount() == 2, f"Expected 2 rows for 'photo', got {win.table_files.rowCount()}"
win.txt_search.setText("")
win.filter_files_table()
assert win.table_files.rowCount() == 8

# Test Plan Collision Filter
win.combo_plan_status.setCurrentIndex(2) # Collision
win.filter_plan_table()
assert win.table_plan.rowCount() == 1, "Expected 1 collision move"

win.combo_plan_status.setCurrentIndex(0) # All
win.filter_plan_table()
assert win.table_plan.rowCount() == 8

print("=> Headless PySide6 Desktop Pipeline: PASSED!\n")

print("=================================================")
print("  ALL VERIFICATION TESTS COMPLETED WITH 0 ERRORS ")
print("=================================================")
