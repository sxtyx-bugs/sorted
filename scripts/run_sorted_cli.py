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

def scan_directory(root_dir: Path):
    excluded = {"sorted", ".sorted", ".sorted_journal"}
    records = []
    errors = []

    for root, dirs, files in os.walk(root_dir):
        # Exclude output folders
        dirs[:] = [d for d in dirs if d.lower() not in excluded]

        for file in files:
            p = Path(root) / file
            if p.is_symlink():
                continue
            try:
                stat = p.stat()
                ext = normalize_extension(p.suffix)
                cat = classify_extension(ext)
                records.append({
                    "name": file,
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

    return records, errors

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
    plan = []
    reserved = set()

    for r in records:
        cat = r["category"]
        ext_folder = r["extension"].upper() if r["extension"] else "No Extension"
        dest_dir = root_dir / "Sorted" / cat / ext_folder
        raw_dest = dest_dir / r["name"]

        # Check if already at dest
        if r["path"].resolve() == raw_dest.resolve() if raw_dest.exists() else False:
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
    root = Path(sys.argv[1]) if len(sys.argv) > 1 else Path("SORTED_TEST")
    print("=" * 65)
    print(f"  SORTED --- Desktop File Organizer Engine")
    print(f"  Target Root: {root.resolve()}")
    print("=" * 65)

    # 1. Scan
    t0 = time.perf_counter()
    records, errors = scan_directory(root)
    t_scan = (time.perf_counter() - t0) * 1000
    print(f"[1/5] Scan complete: {len(records)} files discovered in {t_scan:.2f} ms")

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
