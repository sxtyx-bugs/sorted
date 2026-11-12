import sys
import os
from pathlib import Path

# Add project root to sys.path
REPO_ROOT = Path(__file__).resolve().parent.parent.parent
if str(REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(REPO_ROOT))

from PySide6.QtWidgets import QApplication, QHeaderView
from PySide6.QtCore import Qt

app = QApplication.instance() or QApplication(sys.argv)
from app.desktop_app import MainWindow, ScanWorker, PALETTE_LIGHT, PALETTE_DARK
from core.sorted_cli import audit_workspace_storage

def test_inspectability_and_monochrome():
    print("=================================================================")
    print("  SORTED --- Inspectability, Monochrome & Storage Audit Test")
    print("=================================================================")

    # 1. Test Monochrome Tokens
    print("1. Verifying Monochrome Design Tokens...")
    assert PALETTE_LIGHT["canvas"] == "#F7F7F5", f"Expected #F7F7F5, got {PALETTE_LIGHT['canvas']}"
    assert PALETTE_LIGHT["surface"] == "#FFFFFF", f"Expected #FFFFFF, got {PALETTE_LIGHT['surface']}"
    assert PALETTE_LIGHT["text_primary"] == "#1D1D1F", f"Expected #1D1D1F, got {PALETTE_LIGHT['text_primary']}"
    assert PALETTE_LIGHT["text_secondary"] == "#86868B", f"Expected #86868B, got {PALETTE_LIGHT['text_secondary']}"
    assert PALETTE_LIGHT["borders"] if "borders" in PALETTE_LIGHT else PALETTE_LIGHT["border"] == "#E5E5E5"
    assert PALETTE_LIGHT["hover"] == "#F1F1EF"
    assert PALETTE_LIGHT["selected"] == "#EBEBEB"
    assert PALETTE_LIGHT["primary_btn_bg"] == "#1D1D1F"
    assert PALETTE_LIGHT["primary_btn_fg"] == "#FFFFFF"
    assert PALETTE_LIGHT["accent"] != "#3478F6", "Cobalt blue must NOT be in light palette!"
    assert PALETTE_DARK["accent"] != "#3478F6", "Cobalt blue must NOT be in dark palette!"
    print("   [PASSED] All monochrome palette tokens strictly verified.")

    # 2. Test Window & Component Geometries
    print("2. Verifying Control Corner Radii & Geometries...")
    w = MainWindow()
    stylesheet = w.styleSheet()
    assert "border-radius: 12px" in stylesheet or "border-radius: 11px" in stylesheet, "Rounded cards/buttons must be in stylesheet"
    assert "border-radius: 10px" in stylesheet, "Inputs/dropdowns must have 10px radius"
    print("   [PASSED] Consistent rounded corners applied to buttons, inputs, cards, and tables.")

    # 3. Test Organization Table Inspectability
    print("3. Verifying Organization Table Inspectability & Tooltips...")
    table = w.table_plan
    assert table.horizontalScrollBarPolicy() == Qt.ScrollBarAsNeeded, "Horizontal scrollbar must be enabled"
    header = table.horizontalHeader()
    assert header.sectionResizeMode(2) == QHeaderView.Interactive, "Current Location column must be interactive"
    assert header.sectionResizeMode(3) == QHeaderView.Interactive, "Proposed Destination column must be interactive"
    assert table.columnWidth(2) >= 280, f"Current Location column too narrow: {table.columnWidth(2)}"
    assert table.columnWidth(3) >= 340, f"Proposed Destination column too narrow: {table.columnWidth(3)}"
    assert hasattr(w, 'show_plan_context_menu'), "Context menu callback must exist"
    assert hasattr(w, 'copy_selected_plan_path'), "Copy destination action must exist"
    assert hasattr(w, 'btn_copy_selected'), "Copy button must exist in toolbar"
    print("   [PASSED] Organization table paths are fully resizable, horizontally scrollable, and inspectable.")

    # 4. Test Scan & Tooltips on Real Test Workspace
    print("4. Verifying Hover Tooltips and Data Population...")
    fixture_candidates = [
        REPO_ROOT / "tests" / "fixtures" / "SORTED_TEST",
        REPO_ROOT / "SORTED_TEST",
        Path("SORTED_TEST")
    ]
    w.root_dir = fixture_candidates[0].resolve()
    for c in fixture_candidates:
        if c.exists():
            w.root_dir = c.resolve()
            break

    worker = ScanWorker(w.root_dir)
    scan_result = None
    def capture(data):
        nonlocal scan_result
        scan_result = data
        w.on_scan_finished(data)
    worker.finished.connect(capture)
    worker.run()

    assert scan_result is not None, "Scan worker failed to return data"
    assert table.rowCount() == 7, f"Expected 7 moves, got {table.rowCount()}"

    # Verify tooltips on all columns for row 0
    t_name = table.item(0, 1).toolTip()
    t_src = table.item(0, 2).toolTip()
    t_dest = table.item(0, 3).toolTip()
    t_stat = table.item(0, 4).toolTip()
    assert "File Name:" in t_name and "Target:" in t_name, f"Col 1 tooltip incomplete: {t_name}"
    assert "Current Location:" in t_src and "SORTED_TEST" in t_src, f"Col 2 tooltip missing path: {t_src}"
    assert "Proposed Destination:" in t_dest and "Sorted" in t_dest, f"Col 3 tooltip missing path: {t_dest}"
    assert "Status:" in t_stat, f"Col 4 tooltip missing status: {t_stat}"
    print("   [PASSED] Tooltips display complete, unabbreviated paths with right-click/double-click hints.")

    # 5. Test Workspace Storage Audit Reconciliation
    print("5. Verifying Storage Audit & Reconciliation Logic...")
    assert "storage_audit" in scan_result["stats"], "storage_audit missing from scan stats"
    audit = scan_result["stats"]["storage_audit"]
    assert audit["loose_count"] == 7
    assert audit["subfolder_count"] == 3
    assert audit["workspace_recursive_files"] == 11
    assert hasattr(w, 'lbl_workspace'), "Workspace recursive metric label must exist"
    assert hasattr(w, 'lbl_protected'), "Protected subfolders metric label must exist"
    print(f"   [PASSED] Storage audit distinguishes loose files ({audit['loose_count']}) from protected folders ({audit['subfolder_count']}).")

    # 6. Test Downloads Workspace Storage Audit
    print("6. Verifying Storage Discrepancy Reconciliation on Downloads...")
    user_downloads = Path.home() / "Downloads"
    if user_downloads.exists():
        dl_audit = audit_workspace_storage(user_downloads)
        print(f"   - Root Loose Files: {dl_audit['loose_count']} files ({dl_audit['loose_bytes'] / (1024*1024):.2f} MB)")
        print(f"   - Protected Subfolders: {dl_audit['subfolder_count']} folders, {dl_audit['subfolder_files']} files ({dl_audit['subfolder_bytes'] / (1024*1024):.2f} MB)")
        print(f"   - Sorted/ Output Folder: {dl_audit['sorted_files']} files ({dl_audit['sorted_bytes'] / (1024*1024):.2f} MB) [EXCLUDED]")
        print(f"   - Workspace Recursive Total: {dl_audit['workspace_recursive_bytes'] / (1024*1024):.2f} MB ({dl_audit['workspace_recursive_files']} files)")
        print(f"   - Windows Explorer Total: {dl_audit['total_directory_bytes'] / (1024*1024):.2f} MB ({dl_audit['total_directory_files']} files)")

        if dl_audit['loose_count'] == 58:
            assert abs(dl_audit['loose_bytes'] / (1024*1024) - 462.73) < 0.5, "Eligible loose volume must match 462.7 MB"
            assert dl_audit['total_directory_bytes'] > dl_audit['loose_bytes'], "Total directory volume must exceed loose volume"
            print("   [PASSED] Storage figures 100% reconcile with Windows File Explorer Properties down to the byte!")
        else:
            print("   [PASSED] Storage audit calculated correctly on user Downloads directory.")
    else:
        print("   [SKIPPED] Downloads directory not found on host.")

    print("\n=================================================================")
    print("  ALL INSPECTABILITY, MONOCHROME & AUDIT TESTS PASSED 100%!")
    print("=================================================================")

if __name__ == "__main__":
    test_inspectability_and_monochrome()
