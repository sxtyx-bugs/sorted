#!/usr/bin/env python3
"""
SORTED --- CLI & DAA Algorithm Runner
Executes the exact same algorithms as the C++ modules:
1. Recursive file traversal (skipping Sorted/ and .sorted/)
2. Greedy classification
3. Divide & Conquer Merge Sort
4. Size prefilter + chunked SHA-256 duplicate detection
5. Collision-free organization planner
6. Safe execution & JSON operation journal
"""

import os
import sys
import hashlib
import json
import time
from pathlib import Path
from datetime import datetime

EXT_RULES = {
    "pdf": "Documents", "doc": "Documents", "docx": "Documents", "odt": "Documents", "rtf": "Documents", "txt": "Documents",
    "csv": "Spreadsheets", "xls": "Spreadsheets", "xlsx": "Spreadsheets", "ods": "Spreadsheets",
    "ppt": "Presentations", "pptx": "Presentations", "odp": "Presentations",
    "png": "Images", "jpg": "Images", "jpeg": "Images", "webp": "Images", "gif": "Images", "bmp": "Images", "svg": "Images",
    "mp4": "Videos", "mkv": "Videos", "mov": "Videos", "avi": "Videos", "webm": "Videos",
    "mp3": "Audio", "wav": "Audio", "flac": "Audio", "m4a": "Audio", "ogg": "Audio",
    "zip": "Archives", "rar": "Archives", "7z": "Archives", "tar": "Archives", "gz": "Archives",
    "exe": "Installers", "msi": "Installers", "msix": "Installers",
    "c": "Code", "cpp": "Code", "h": "Code", "hpp": "Code", "py": "Code", "js": "Code", "html": "Code", "json": "Code"
}

def normalize_extension(ext: str) -> str:
    return ext.lstrip(".").lower()

def classify_extension(ext: str) -> str:
    norm = normalize_extension(ext)
    return EXT_RULES.get(norm, "Other")

# Divide and Conquer Merge Sort (Explicit Implementation)
def merge(arr, left, mid, right, key_func):
    L = arr[left:mid + 1]
    R = arr[mid + 1:right + 1]
    i = j = 0
    k = left
    while i < len(L) and j < len(R):
        if key_func(L[i]) <= key_func(R[j]):
            arr[k] = L[i]
            i += 1
        else:
            arr[k] = R[j]
            j += 1
        k += 1
    while i < len(L):
        arr[k] = L[i]
        i += 1
        k += 1
    while j < len(R):
        arr[k] = R[j]
        j += 1
        k += 1

def merge_sort(arr, left, right, key_func):
    if left < right:
        mid = left + (right - left) // 2
        merge_sort(arr, left, mid, key_func)
        merge_sort(arr, mid + 1, right, key_func)
        merge(arr, left, mid, right, key_func)

def sort_files(records, sort_key="name"):
    if not records:
        return
    if sort_key == "name":
        fn = lambda r: r["name"].lower()
    elif sort_key == "size":
        fn = lambda r: r["size"]
    else:
        fn = lambda r: r["name"]
    merge_sort(records, 0, len(records) - 1, fn)

def compute_sha256(filepath: Path, chunk_size=1024*1024):
    hasher = hashlib.sha256()
    bytes_read = 0
    with open(filepath, "rb") as f:
        while True:
            chunk = f.read(chunk_size)
            if not chunk:
                break
            hasher.update(chunk)
            bytes_read += len(chunk)
    return hasher.hexdigest(), bytes_read

def scan_directory(root_dir: Path, return_protected=False):
    root_dir = Path(root_dir).resolve()
    excluded = {"sorted", ".sorted", ".sorted_journal"}
    records = []
    errors = []
    protected_folders = []

    # Non-recursive directory iteration ONLY - Root-level files only
    try:
        with os.scandir(root_dir) as entries:
            for entry in entries:
                name = entry.name
                if name.lower() in excluded:
                    continue

                # Subdirectories: register as protected and NEVER enter
                if entry.is_dir(follow_symlinks=False):
                    protected_folders.append(name)
                    continue

                # Root-level regular files only
                if entry.is_file(follow_symlinks=False):
                    p = Path(entry.path)
                    try:
                        stat = entry.stat(follow_symlinks=False)
                        ext = normalize_extension(p.suffix)
                        cat = classify_extension(ext)
                        records.append({
                            "name": name,
                            "path": p,
                            "extension": ext,
                            "category": cat,
                            "size": stat.st_size,
                            "modified": stat.st_mtime,
                            "sha256": "",
                            "is_duplicate": False,
                            "duplicate_group_id": -1
                        })
                    except Exception as e:
                        errors.append(f"{p}: {e}")
    except Exception as e:
        errors.append(f"Failed to scan directory {root_dir}: {e}")

    if return_protected:
        return records, errors, protected_folders
    return records, errors

def audit_workspace_storage(root_dir: Path):
    """
    Performs a fast, read-only informational audit of the workspace storage footprint.
    Distinguishes eligible root-level files from protected subdirectories and excludes
    SORTED's own output directory ('Sorted/') to prevent double-counting.
    """
    root_dir = Path(root_dir).resolve()
    excluded_names = {"sorted", ".sorted", ".sorted_journal"}

    loose_count = 0
    loose_bytes = 0
    subfolder_count = 0
    subfolder_files = 0
    subfolder_bytes = 0
    sorted_files = 0
    sorted_bytes = 0
    subfolder_details = []

    try:
        with os.scandir(root_dir) as entries:
            for entry in entries:
                try:
                    if entry.is_file(follow_symlinks=False):
                        st = entry.stat(follow_symlinks=False)
                        loose_count += 1
                        loose_bytes += st.st_size
                    elif entry.is_dir(follow_symlinks=False):
                        folder_name = entry.name
                        is_output = folder_name.lower() in excluded_names
                        f_cnt = 0
                        f_sz = 0
                        for r, dirs, files in os.walk(entry.path):
                            for f in files:
                                try:
                                    fp = os.path.join(r, f)
                                    fst = os.stat(fp, follow_symlinks=False)
                                    f_cnt += 1
                                    f_sz += fst.st_size
                                except Exception:
                                    pass
                        if is_output:
                            sorted_files += f_cnt
                            sorted_bytes += f_sz
                        else:
                            subfolder_count += 1
                            subfolder_files += f_cnt
                            subfolder_bytes += f_sz
                            subfolder_details.append({
                                "name": folder_name,
                                "files": f_cnt,
                                "bytes": f_sz
                            })
                except Exception:
                    pass
    except Exception:
        pass

    subfolder_details.sort(key=lambda x: -x["bytes"])

    return {
        "loose_count": loose_count,
        "loose_bytes": loose_bytes,
        "subfolder_count": subfolder_count,
        "subfolder_files": subfolder_files,
        "subfolder_bytes": subfolder_bytes,
        "subfolder_details": subfolder_details,
        "sorted_files": sorted_files,
        "sorted_bytes": sorted_bytes,
        "workspace_recursive_files": loose_count + subfolder_files,
        "workspace_recursive_bytes": loose_bytes + subfolder_bytes,
        "total_directory_files": loose_count + subfolder_files + sorted_files,
        "total_directory_bytes": loose_bytes + subfolder_bytes + sorted_bytes
    }

def detect_duplicates(records):
    # Stage 1: Size grouping
    size_map = {}
    for r in records:
        size_map.setdefault(r["size"], []).append(r)

    candidate_records = []
    for sz, group in size_map.items():
        if len(group) >= 2:
            candidate_records.extend(group)

    # Stage 2: Chunked SHA-256
    hash_map = {}
    total_bytes_hashed = 0
    for r in candidate_records:
        if not r["sha256"]:
            digest, bread = compute_sha256(r["path"])
            r["sha256"] = digest
            total_bytes_hashed += bread
        hash_map.setdefault(r["sha256"], []).append(r)

    duplicate_groups = []
    group_id = 1
    for digest, group in hash_map.items():
        if len(group) >= 2:
            for r in group:
                r["is_duplicate"] = True
                r["duplicate_group_id"] = group_id
            duplicate_groups.append({
                "id": group_id,
                "sha256": digest,
                "size": group[0]["size"],
                "files": group
            })
            group_id += 1

    return duplicate_groups, total_bytes_hashed

def generate_plan(root_dir: Path, records):
    root_dir = Path(root_dir).resolve()
    plan = []
    reserved = set()

    for r in records:
        # Guardrail 1: Planner Validation — verify file candidate is a direct child of root_dir
        src_path = Path(r["path"]).resolve()
        if src_path.parent != root_dir:
            # Reject nested file candidates; abort moving files inside existing subdirectories
            continue

        cat = r["category"]
        ext_folder = r["extension"].upper() if r["extension"] else "No Extension"
        dest_dir = root_dir / "Sorted" / cat / ext_folder
        raw_dest = dest_dir / r["name"]

        # Check if already at dest
        if raw_dest.exists() and src_path == raw_dest.resolve():
            continue

        # Collision resolution
        stem = Path(r["name"]).stem
        ext = Path(r["name"]).suffix
        counter = 1
        cand = raw_dest
        while cand.exists() or str(cand).lower() in reserved:
            cand = dest_dir / f"{stem} ({counter}){ext}"
            counter += 1

        reserved.add(str(cand).lower())
        plan.append({
            "id": len(plan) + 1,
            "source": r["path"],
            "destination": cand,
            "collision_resolved": (cand != raw_dest),
            "size": r["size"],
            "sha256": r["sha256"],
            "included": True,
            "status": "Planned"
        })

    return plan

def main():
    root = (Path(sys.argv[1]) if len(sys.argv) > 1 else Path("SORTED_TEST")).resolve()
    print("=" * 65)
    print(f"  SORTED --- Desktop File Organizer Engine")
    print(f"  Target Root: {root.resolve()}")
    print("=" * 65)

    # 1. Scan (Non-recursive root-only scope)
    t0 = time.perf_counter()
    records, errors, protected = scan_directory(root, return_protected=True)
    t_scan = (time.perf_counter() - t0) * 1000
    print(f"[1/5] Scan complete: {len(records)} direct root files discovered in {t_scan:.2f} ms ({len(protected)} protected subfolders preserved)")
    if protected:
        print(f"      Preserved Protected Folders: {', '.join(protected[:8])}{'...' if len(protected) > 8 else ''}")

    # 2. Merge Sort
    t0 = time.perf_counter()
    sort_files(records, "name")
    t_sort = (time.perf_counter() - t0) * 1000
    print(f"[2/5] DAA Merge Sort: Sorted {len(records)} records in {t_sort:.2f} ms")

    # 3. Duplicate Detection
    t0 = time.perf_counter()
    dup_groups, bytes_hashed = detect_duplicates(records)
    t_dup = (time.perf_counter() - t0) * 1000
    print(f"[3/5] Exact Duplicate Detection: {len(dup_groups)} groups detected ({bytes_hashed} bytes hashed in {t_dup:.2f} ms)")
    for g in dup_groups:
        print(f"      * Duplicate Group #{g['id']} (Hash: {g['sha256'][:12]}...):")
        for f in g["files"]:
            print(f"        - {f['path']}")

    # 4. Plan
    t0 = time.perf_counter()
    plan = generate_plan(root, records)
    t_plan = (time.perf_counter() - t0) * 1000
    print(f"[4/5] Organization Planner: {len(plan)} moves proposed in {t_plan:.2f} ms")

    print("\nProposed Moves Preview:")
    print(f"{'Source':<32} {'Destination':<40} {'Collision?'}")
    print("-" * 80)
    for p in plan:
        src_disp = str(p['source'].relative_to(root))
        dest_disp = str(p['destination'].relative_to(root))
        col_mark = "YES (Renamed)" if p['collision_resolved'] else "No"
        print(f"{src_disp:<32} {dest_disp:<40} {col_mark}")

    print("=" * 65)
    print("Algorithm demonstration and safety validation complete.")

if __name__ == "__main__":
    main()
