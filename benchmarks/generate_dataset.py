#!/usr/bin/env python3
"""
SORTED --- Test Dataset & Benchmark Generator
Generates reproducible test directories with 100, 500, 1,000, or 5,000 files
featuring controlled distributions of file sizes, extensions, and exact duplicates.
"""

import os
import sys
import random
import argparse
from pathlib import Path

EXTENSIONS_BY_CATEGORY = {
    "Documents": ["pdf", "docx", "txt", "rtf"],
    "Images": ["png", "jpg", "webp", "gif"],
    "Videos": ["mp4", "mkv", "webm"],
    "Audio": ["mp3", "wav", "flac"],
    "Archives": ["zip", "tar.gz", "7z"],
    "Installers": ["exe", "msi"],
    "Code": ["cpp", "h", "py", "js", "html"],
    "Other": ["xyz", "dat", "bin"]
}

ALL_EXTENSIONS = [ext for cat in EXTENSIONS_BY_CATEGORY.values() for ext in cat]

def create_dataset(target_dir: Path, count: int, duplicate_ratio: float = 0.15):
    target_dir.mkdir(parents=True, exist_ok=True)
    random.seed(42)  # Deterministic seed for reproducible benchmarks

    # Create pool of duplicate contents
    dup_pool_size = max(2, int(count * duplicate_ratio // 2))
    dup_contents = [
        os.urandom(random.choice([1024, 4096, 65536, 1048576]))
        for _ in range(dup_pool_size)
    ]

    print(f"Generating {count} files in {target_dir} (duplicate ratio: {duplicate_ratio:.0%})...")

    subfolders = [
        target_dir,
        target_dir / "folder_a",
        target_dir / "folder_b",
        target_dir / "nested" / "sub_1",
        target_dir / "nested" / "sub_2" / "deep",
    ]
    for sub in subfolders:
        sub.mkdir(parents=True, exist_ok=True)

    for i in range(count):
        sub = random.choice(subfolders)
        ext = random.choice(ALL_EXTENSIONS)
        filename = f"file_{i:05d}.{ext}"
        filepath = sub / filename

        if random.random() < duplicate_ratio:
            content = random.choice(dup_contents)
        else:
            # Random size between 500 bytes and 500 KB
            size = random.randint(500, 500 * 1024)
            content = os.urandom(size)

        with open(filepath, "wb") as f:
            f.write(content)

    print(f"Successfully generated {count} files across nested directories.")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate benchmark test datasets for SORTED.")
    parser.add_argument("--count", type=int, default=500, help="Number of files to generate (e.g. 100, 500, 1000, 5000)")
    parser.add_argument("--out", type=str, default="benchmark_dataset", help="Output directory")
    args = parser.parse_args()

    create_dataset(Path(args.out), args.count)
