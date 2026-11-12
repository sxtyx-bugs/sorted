# -*- mode: python ; coding: utf-8 -*-
# =============================================================================
# PyInstaller Build Specification for SORTED Desktop Application
# =============================================================================

import sys
import os
from pathlib import Path

block_cipher = None
REPO_ROOT = Path(os.path.abspath(SPEC)).parent.parent

a = Analysis(
    [str(REPO_ROOT / 'app' / 'main.py')],
    pathex=[str(REPO_ROOT)],
    binaries=[],
    datas=[
        (str(REPO_ROOT / 'config' / 'default_settings.json'), 'config'),
    ],
    hiddenimports=[
        'PySide6.QtCore',
        'PySide6.QtGui',
        'PySide6.QtWidgets',
        'PySide6.QtSvg',
        'core',
        'core.sorted_cli',
        'app',
        'app.desktop_app',
    ],
    hookspath=[],
    hooksconfig={},
    runtime_hooks=[],
    excludes=[
        'tests',
        'benchmarks',
        'tools',
        'SORTED_TEST',
        'SORTED_TEST_100',
        'BENCHMARK_SUITE',
        'tkinter',
        'matplotlib',
        'numpy',
        'pandas',
        'scipy',
    ],
    win_no_prefer_redirects=False,
    win_private_assemblies=False,
    cipher=block_cipher,
    noarchive=False,
)

pyz = PYZ(a.pure, a.zipped_data, cipher=block_cipher)

exe = EXE(
    pyz,
    a.scripts,
    [],
    exclude_binaries=True,
    name='SORTED',
    debug=False,
    bootloader_ignore_signals=False,
    strip=False,
    upx=True,
    console=False,
    disable_windowed_traceback=False,
    argv_emulation=False,
    target_arch=None,
    codesign_identity=None,
    entitlements_file=None,
)

coll = COLLECT(
    exe,
    a.binaries,
    a.zipfiles,
    a.datas,
    strip=False,
    upx=True,
    upx_exclude=[],
    name='SORTED',
)
