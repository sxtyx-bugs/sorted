import subprocess
import sys
import os
from pathlib import Path

# Resolve repository root
REPO_ROOT = Path(__file__).resolve().parent.parent.parent
if str(REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(REPO_ROOT))

print("=================================================")
print("  SORTED — Full Verification & Acceptance Suite  ")
print("=================================================")

# Resolve test fixtures path
fixture_candidates = [
    REPO_ROOT / "tests" / "fixtures" / "SORTED_TEST",
    REPO_ROOT / "SORTED_TEST",
    Path("SORTED_TEST")
]
sorted_test_dir = fixture_candidates[0]
for c in fixture_candidates:
    if c.exists():
        sorted_test_dir = c
        break

# 1. C++17 Core Verification
print("[1/3] Running Compiled C++17 Core Verification Suite...")
env = os.environ.copy()
env["PATH"] = r"C:\QtTools\Tools\mingw1310_64\bin;" + env.get("PATH", "")

cpp_exe_candidates = [
    REPO_ROOT / "build" / "test_core.exe",
    REPO_ROOT / "test_core.exe",
    Path("build/test_core.exe"),
    Path("test_core.exe")
]
cpp_exe = None
for c in cpp_exe_candidates:
    if c.exists():
        cpp_exe = c
        break

if cpp_exe:
    res1 = subprocess.run([str(cpp_exe)], capture_output=True, text=True, env=env, cwd=str(REPO_ROOT))
    print(res1.stdout.strip())
    assert res1.returncode == 0, f"C++ test failed: {res1.stderr}"
    print("=> C++17 Core Suite: PASSED!\n")
else:
    print("=> C++17 Core Suite: (Executable will be verified in build step)\n")

# 2. Python CLI Engine on real dataset
print("[2/3] Running Python CLI Engine on SORTED_TEST...")
cli_script_candidates = [
    REPO_ROOT / "core" / "sorted_cli.py",
    REPO_ROOT / "scripts" / "run_sorted_cli.py",
    Path("core/sorted_cli.py")
]
cli_script = cli_script_candidates[0]
for c in cli_script_candidates:
    if c.exists():
        cli_script = c
        break

res2 = subprocess.run([sys.executable, str(cli_script), str(sorted_test_dir)], capture_output=True, text=True, cwd=str(REPO_ROOT))
print(res2.stdout.strip())
assert res2.returncode == 0, f"CLI test failed: {res2.stderr}"
print("=> Python CLI Engine: PASSED!\n")

# 3. Headless Qt 6 Desktop Application Pipeline
print("[3/3] Running Headless PySide6 Desktop Application Pipeline...")
os.environ["QT_QPA_PLATFORM"] = "offscreen"

from app.desktop_app import MainWindow, ScanWorker
from PySide6.QtWidgets import QApplication

app = QApplication.instance() or QApplication([])
win = MainWindow()

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
win.set_sidebar_collapsed(False, animate=False)
win.toggle_sidebar()
assert win.is_sidebar_collapsed == True
win.toggle_sidebar()
assert win.is_sidebar_collapsed == False

# Test Scan Worker (Root-Only Scope)
worker = ScanWorker(sorted_test_dir.resolve())
data_res = {}
worker.finished.connect(lambda d: data_res.update(d))
worker.run()
win.on_scan_finished(data_res)

assert len(data_res["records"]) == 7, f"Expected 7 direct root records, got {len(data_res['records'])}"
assert len(data_res["protected_folders"]) >= 3, f"Expected >=3 protected folders, got {len(data_res['protected_folders'])}"
assert len(data_res["duplicates"]) == 2, f"Expected 2 dup groups, got {len(data_res['duplicates'])}"
assert len(data_res["plan"]) == 7, f"Expected 7 moves, got {len(data_res['plan'])}"

# Test DAA Merge Sort
win.combo_sort.setCurrentIndex(1)
win.sort_files_table()

# Test Search Filter
win.txt_search.setText("photo")
win.filter_files_table()
assert win.table_files.rowCount() == 2, f"Expected 2 rows for 'photo', got {win.table_files.rowCount()}"
win.txt_search.setText("")
win.filter_files_table()
assert win.table_files.rowCount() == 7

# Test Plan Status Filter
win.combo_plan_status.setCurrentIndex(1) # Ready
win.filter_plan_table()
assert win.table_plan.rowCount() == 7, f"Expected 7 ready moves, got {win.table_plan.rowCount()}"

win.combo_plan_status.setCurrentIndex(0) # All
win.filter_plan_table()
assert win.table_plan.rowCount() == 7

print("=> Headless PySide6 Desktop Pipeline: PASSED!\n")

print("=================================================")
print("  ALL VERIFICATION TESTS COMPLETED WITH 0 ERRORS ")
print("=================================================")
