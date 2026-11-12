import os
import sys
import datetime
import math

def entropy(data):
    if not data:
        return 0.0
    occ = {}
    for b in data:
        occ[b] = occ.get(b, 0) + 1
    ent = 0.0
    for count in occ.values():
        p = count / len(data)
        ent -= p * math.log2(p)
    return ent

def main():
    dry_run = "--dry-run" in sys.argv
    downloads_dir = r"C:\Users\satya\Downloads"
    sorted_dir = r"C:\Users\satya\Downloads\Sorted"

    to_delete = []

    # 1. Inspect Sorted/ for demo files
    for root, dirs, fnames in os.walk(sorted_dir):
        for f in fnames:
            p = os.path.join(root, f)
            st = os.stat(p)
            ctime = datetime.datetime.fromtimestamp(st.st_ctime).strftime("%Y-%m-%d %H:%M:%S")
            mtime = datetime.datetime.fromtimestamp(st.st_mtime).strftime("%Y-%m-%d %H:%M:%S")

            with open(p, "rb") as fp:
                head = fp.read(4096)
            ent = entropy(head)

            if (f.startswith("file_") and ("2026-11-12 15:" in ctime or "2026-11-12 15:" in mtime or ent > 7.5)) \
               or f == "aqtinstall.log" or f.endswith(".pyc"):
                to_delete.append((p, f"Sorted demo file (entropy: {ent:.2f}, {f})"))

    # 2. Inspect Downloads root for demo files
    dummy_known_names = {"loose.pdf", "photo.png", "report (1).pdf", "report.pdf", "video.mp4"}
    for entry in os.scandir(downloads_dir):
        if entry.is_file():
            f = entry.name
            p = entry.path
            st = entry.stat()
            ctime = datetime.datetime.fromtimestamp(st.st_ctime).strftime("%Y-%m-%d %H:%M:%S")
            mtime = datetime.datetime.fromtimestamp(st.st_mtime).strftime("%Y-%m-%d %H:%M:%S")

            if f.startswith("file_") and ("2026-11-12 15:" in ctime or "2026-11-12 15:" in mtime):
                to_delete.append((p, f"Downloads benchmark demo file ({f})"))
            elif f in dummy_known_names:
                with open(p, "rb") as fp:
                    content = fp.read(100)
                if b"test" in content or b"simulation" in content:
                    to_delete.append((p, f"Downloads dummy test file ({f})"))

    print(f"Total confirmed demo files to delete: {len(to_delete)}")
    for p, reason in to_delete[:15]:
        print(f"  [TARGET] {p} -> {reason}")
    if len(to_delete) > 15:
        print(f"  ... and {len(to_delete) - 15} more files.")

    if dry_run:
        print("\n[DRY RUN COMPLETE] No files were deleted.")
        return

    # Execute deletion
    deleted_count = 0
    failed = []
    for p, reason in to_delete:
        try:
            os.remove(p)
            deleted_count += 1
        except Exception as e:
            failed.append((p, str(e)))

    print(f"\nSuccessfully deleted: {deleted_count} demo files.")
    if failed:
        print(f"Failed to delete {len(failed)} files:")
        for p, err in failed:
            print(f"  {p}: {err}")

    # Clean up empty directories in Sorted/
    removed_dirs = 0
    for root, dirs, files in os.walk(sorted_dir, topdown=False):
        for d in dirs:
            dir_path = os.path.join(root, d)
            try:
                if not os.listdir(dir_path):
                    os.rmdir(dir_path)
                    removed_dirs += 1
            except Exception:
                pass

    print(f"Cleaned up {removed_dirs} empty subdirectories in Sorted/.")

if __name__ == "__main__":
    main()
