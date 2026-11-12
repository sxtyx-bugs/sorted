#!/usr/bin/env python3
"""
SORTED — Production Distribution Packaging & Verification Tool
Assembles a standalone, portable Windows application release package in dist/
and strictly validates that no test datasets, fixtures, or user files are included.
"""

import os
import sys
import shutil
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent

def build_distribution():
    print("=================================================================")
    print("  SORTED --- Production Distribution Packaging Tool")
    print("=================================================================")

    dist_root = REPO_ROOT / "dist"
    package_dir = dist_root / "SORTED_v1.0.0_win64"

    # Clean existing package dir
    if package_dir.exists():
        shutil.rmtree(package_dir)
    package_dir.mkdir(parents=True, exist_ok=True)

    print(f"1. Target Distribution Directory: {package_dir}")

    # Copy application files
    app_target = package_dir / "app"
    app_target.mkdir(parents=True, exist_ok=True)
    shutil.copy2(REPO_ROOT / "app" / "desktop_app.py", app_target / "desktop_app.py")
    shutil.copy2(REPO_ROOT / "app" / "main.py", app_target / "main.py")
    shutil.copy2(REPO_ROOT / "app" / "__init__.py", app_target / "__init__.py")

    # Copy UI files
    ui_target = app_target / "ui"
    ui_target.mkdir(parents=True, exist_ok=True)
    for f in (REPO_ROOT / "app" / "ui").glob("*.*"):
        if f.is_file():
            shutil.copy2(f, ui_target / f.name)
    glass_target = ui_target / "glass"
    glass_target.mkdir(parents=True, exist_ok=True)
    for f in (REPO_ROOT / "app" / "ui" / "glass").glob("*.*"):
        if f.is_file():
            shutil.copy2(f, glass_target / f.name)

    # Copy core engine
    core_target = package_dir / "core"
    core_target.mkdir(parents=True, exist_ok=True)
    shutil.copy2(REPO_ROOT / "core" / "sorted_cli.py", core_target / "sorted_cli.py")
    shutil.copy2(REPO_ROOT / "core" / "__init__.py", core_target / "__init__.py")

    # Copy configuration
    config_target = package_dir / "config"
    config_target.mkdir(parents=True, exist_ok=True)
    shutil.copy2(REPO_ROOT / "config" / "default_settings.json", config_target / "default_settings.json")

    # Copy resources & license
    if (REPO_ROOT / "resources").exists():
        shutil.copytree(REPO_ROOT / "resources", package_dir / "resources", dirs_exist_ok=True)
    shutil.copy2(REPO_ROOT / "LICENSE", package_dir / "LICENSE")
    shutil.copy2(REPO_ROOT / "CHANGELOG.md", package_dir / "CHANGELOG.md")

    # Create root production launcher
    launcher_path = package_dir / "SORTED.bat"
    with open(launcher_path, "w", encoding="utf-8") as f:
        f.write("@echo off\r\ntitle SORTED --- Safe Desktop File Organizer\r\npython app\\main.py\r\n")

    print("2. Application files assembled successfully.")

    # 3. Strict Audit of Production Distribution Package
    print("3. Auditing Production Package for Developer / Test Isolation...")
    forbidden_tokens = [
        "SORTED_TEST", "SORTED_TEST_100", "BENCHMARK_SUITE",
        ".sorted_journal.json", ".git", ".kombai", "tests",
        "benchmarks", "tools", "__pycache__", ".bak"
    ]

    all_files = list(package_dir.rglob("*"))
    for file_path in all_files:
        name_str = str(file_path.relative_to(package_dir))
        for token in forbidden_tokens:
            if token in name_str:
                raise AssertionError(f"FORBIDDEN ARTIFACT IN PRODUCTION PACKAGE: {name_str}")

    print(f"   [PASSED] Package contains {len([f for f in all_files if f.is_file()])} production files.")
    print("   [PASSED] Zero test datasets, zero fixtures, zero user history included.")
    print("\n=================================================================")
    print("  PRODUCTION PACKAGING VERIFICATION COMPLETE 100%!")
    print("=================================================================")

if __name__ == "__main__":
    build_distribution()
