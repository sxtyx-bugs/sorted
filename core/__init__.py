"""
SORTED Core Engine Package
Exporting fundamental file discovery, sorting, classification, duplicate detection,
and organization planning interfaces.
"""
from core.sorted_cli import (
    scan_directory,
    sort_files,
    detect_duplicates,
    generate_plan,
    classify_extension,
    normalize_extension,
    audit_workspace_storage,
    EXT_RULES,
    main,
)

CATEGORIES = sorted(list(set(EXT_RULES.values()) | {"Other"}))

__all__ = [
    "scan_directory",
    "sort_files",
    "detect_duplicates",
    "generate_plan",
    "classify_extension",
    "normalize_extension",
    "audit_workspace_storage",
    "EXT_RULES",
    "CATEGORIES",
    "main",
]
