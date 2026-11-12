#!/usr/bin/env python3
"""
SORTED CLI Entry Wrapper (Legacy/Convenience Script)
Delegates to core/sorted_cli.py.
"""
import sys
import os
from pathlib import Path

# Add project root to sys.path
sys.path.insert(0, str(Path(__file__).resolve().parent.parent))

from core.sorted_cli import (
    main, scan_directory, sort_files, detect_duplicates, generate_plan,
    classify_extension, normalize_extension, audit_workspace_storage,
    EXT_RULES
)

if __name__ == "__main__":
    main()
