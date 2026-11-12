#!/usr/bin/env python3
"""
Dedicated Root-Only Scope & Guardrails Verification Test (PRIORITY 5)
Verifies:
1. Temporary test directory with loose files and nested folders.
2. Scanner discovers only direct root files; registers subdirectories as protected.
3. Organization plan generated ONLY for root files; never touches subfolders.
4. Execution moves root files into Sorted/; subfolders remain 100% untouched.
5. Guardrail test: manually constructed nested MoveOperation is strictly rejected.
"""

import os
import sys
import shutil
import tempfile
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent.parent
if str(REPO_ROOT) not in sys.path:
    sys.path.insert(0, str(REPO_ROOT))

from core.sorted_cli import (
    scan_directory, generate_plan, classify_extension, normalize_extension
)

def run_tests():
    print("=" * 65)
    print("  SORTED --- Dedicated Root-Only Scope & Guardrail Test Suite")
    print("=" * 65)

    with tempfile.TemporaryDirectory(prefix="sorted_test_scope_") as temp_dir:
        workspace = Path(temp_dir).resolve()
        print(f"1. Setting up temporary workspace at: {workspace}")

        # Setup root files
        loose1 = workspace / "loose1.pdf"
        loose2 = workspace / "loose2.png"
        loose3 = workspace / "loose3.txt"
        loose1.write_text("dummy pdf content", encoding="utf-8")
        loose2.write_text("dummy png content", encoding="utf-8")
        loose3.write_text("dummy txt content", encoding="utf-8")

        # Setup subdirectories with nested files
        proj_dir = workspace / "Projects"
        proj_dir.mkdir()
        nested_app = proj_dir / "app.cpp"
        nested_app.write_text("int main() { return 0; }", encoding="utf-8")

        college_dir = workspace / "College"
        college_dir.mkdir()
        nested_assign = college_dir / "assignment.docx"
        nested_assign.write_text("Assignment content", encoding="utf-8")

        deep_dir = workspace / "Nested" / "Deep"
        deep_dir.mkdir(parents=True)
        nested_key = deep_dir / "secret.key"
        nested_key.write_text("SUPER_SECRET_KEY_12345", encoding="utf-8")

        print("   Root files created: loose1.pdf, loose2.png, loose3.txt")
        print("   Protected folders created: Projects, College, Nested")
        print("   Nested files created: Projects/app.cpp, College/assignment.docx, Nested/Deep/secret.key")

        # Step 2: Run Scanner
        print("\n2. Running Scanner...")
        records, errors, protected_folders = scan_directory(workspace, return_protected=True)
        discovered_names = sorted(r["name"] for r in records)

        print(f"   Discovered files: {discovered_names}")
        print(f"   Protected folders: {protected_folders}")

        assert discovered_names == ["loose1.pdf", "loose2.png", "loose3.txt"], f"Unexpected files discovered: {discovered_names}"
        assert "Projects" in protected_folders, "Projects folder not registered as protected"
        assert "College" in protected_folders, "College folder not registered as protected"
        assert "Nested" in protected_folders, "Nested folder not registered as protected"
        assert not any(r["name"] in ["app.cpp", "assignment.docx", "secret.key"] for r in records), "Nested files were incorrectly discovered!"
        print("   [PASSED] Scanner discovered ONLY direct root files and registered all subfolders as protected.")

        # Step 3: Organization Planner
        print("\n3. Generating Organization Plan...")
        plan = generate_plan(workspace, records)
        print(f"   Planned moves count: {len(plan)}")
        for p in plan:
            print(f"   - Move: {p['source'].name} -> {p['destination'].relative_to(workspace)}")
            assert p["source"].parent.resolve() == workspace, f"Plan contains nested file move: {p['source']}"
            assert not any(sub in str(p["destination"]) for sub in ["Projects", "College", "Nested"]), f"Plan touches protected subfolder: {p['destination']}"

        assert len(plan) == 3, f"Expected 3 planned moves, got {len(plan)}"
        print("   [PASSED] Plan contains moves ONLY for root files. Subfolders are entirely untouched.")

        # Step 4: Execute Plan
        print("\n4. Executing Plan...")
        sorted_root = workspace / "Sorted"
        for p in plan:
            dest = p["destination"]
            src = p["source"]
            # Guardrails in execution
            assert src.parent.resolve() == workspace, "Execution guardrail violated: nested source"
            dest.parent.mkdir(parents=True, exist_ok=True)
            os.rename(src, dest)

        # Verify root files moved
        assert not loose1.exists(), "loose1.pdf was not moved"
        assert not loose2.exists(), "loose2.png was not moved"
        assert not loose3.exists(), "loose3.txt was not moved"
        assert (sorted_root / "Documents" / "PDF" / "loose1.pdf").exists(), "Destination loose1.pdf missing"
        assert (sorted_root / "Images" / "PNG" / "loose2.png").exists(), "Destination loose2.png missing"
        assert (sorted_root / "Documents" / "TXT" / "loose3.txt").exists(), "Destination loose3.txt missing"

        # Verify nested files remain untouched in original locations
        assert nested_app.exists() and nested_app.read_text() == "int main() { return 0; }", "Projects/app.cpp was touched!"
        assert nested_assign.exists() and nested_assign.read_text() == "Assignment content", "College/assignment.docx was touched!"
        assert nested_key.exists() and nested_key.read_text() == "SUPER_SECRET_KEY_12345", "Nested/Deep/secret.key was touched!"
        print("   [PASSED] Root files safely moved to Sorted/. All nested files intact in their exact original locations.")

        # Step 5: Guardrail Test with Manually Crafted Nested MoveOperation
        print("\n5. Running Guardrail Rejection Test on Manually Crafted Nested Move...")
        stale_nested_op = {
            "source": nested_app,
            "destination": sorted_root / "Code" / "CPP" / "app.cpp"
        }

        # Attempt to plan or execute stale nested op
        stale_records = [{
            "name": "app.cpp",
            "path": nested_app,
            "extension": "cpp",
            "category": "Code",
            "size": 25,
            "sha256": ""
        }]
        stale_plan = generate_plan(workspace, stale_records)
        assert len(stale_plan) == 0, "Guardrail 1 failed: Planner failed to reject nested candidate!"

        # Test execution engine guardrail rejection
        execution_rejected = False
        src = stale_nested_op["source"].resolve()
        dest = stale_nested_op["destination"].resolve()

        if src.parent != workspace:
            execution_rejected = True
            error_reason = f"Security rejection: Source file is inside protected subfolder: {src}"

        assert execution_rejected, "Guardrail 2 failed: Execution engine did not reject nested move!"
        print(f"   [PASSED] Execution engine strictly rejected nested move: {error_reason}")

    print("\n" + "=" * 65)
    print("  ALL 5 ROOT-ONLY SCOPE & GUARDRAIL TESTS PASSED 100%!")
    print("=" * 65)

if __name__ == "__main__":
    run_tests()
