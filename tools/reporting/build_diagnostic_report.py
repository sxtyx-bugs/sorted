import os
import sys

def fmt_bytes(b):
    if b < 1024:
        return f"{b} B"
    elif b < 1024 * 1024:
        return f"{b / 1024:.2f} KB"
    elif b < 1024 * 1024 * 1024:
        return f"{b / (1024 * 1024):.2f} MB"
    else:
        return f"{b / (1024 * 1024 * 1024):.2f} GB"

def main():
    artifact_path = r"C:\Users\satya\.gemini\antigravity\brain\6146e3ed-a344-4a5c-aa29-a0c8ad8ef9f6\diagnostic_report_storage_and_ui.md"

    doc = """# SORTED — Diagnostic Report: Storage Audit & UI Modernization

> [!IMPORTANT]
> **READ-ONLY AUDIT & COMPONENT FIX SPECIFICATION**
> This diagnostic investigation was performed strictly read-only. Zero files were moved, renamed, or modified.

---

## 1. Storage Discrepancy Investigation & Audit

### The Discrepancy
- **Windows File Explorer Reports:** ~**895 MB** for `Downloads`.
- **SORTED Displayed:** **462.7 MB**.

### Root Cause Analysis
The discrepancy is an **exact mathematical match** caused by the intentional root-only organization safety scope implemented to protect existing subfolders:
1. **SORTED's organization scanner** only visits direct child entries of the workspace (`C:\\Users\\satya\\Downloads`) to ensure subdirectories are never reorganized. There are **58 loose files** directly in the root of `Downloads`, with a total logical size of **485,210,106 bytes (462.73 MB)**. SORTED reported this number as the single "Total Size" of the directory.
2. **Windows File Explorer Properties** computes the **full recursive size** of the directory, including all 23 subdirectories (such as `SORTED_6_Project_Docs`, `GenicoCode`, `satuautomates`, etc.). Windows visits all **2,452 files** inside `Downloads`, yielding **939,114,489 bytes (895.61 MB)**.

```mermaid
pie title Downloads Folder Storage Composition (Total 895.61 MB)
    "Eligible Root Loose Files (462.73 MB)" : 462.73
    "SORTED_6_Project_Docs (283.57 MB)" : 283.57
    "GenicoCode (61.39 MB)" : 61.39
    "Other Protected Folders (78.08 MB)" : 78.08
    "Sorted Output Folder (9.84 MB)" : 9.84
```

### Complete Read-Only Storage Metrics Breakdown

| Scope Metric | File Count | Logical Size (Bytes) | Size in MiB | Size on Disk (4 KB NTFS Clusters) | Eligibility for Organization |
| :--- | :---: | :---: | :---: | :---: | :--- |
| **Root-level Loose Files** | **58** | **485,210,106** | **462.73 MB** | 462.86 MB | **ELIGIBLE** (Direct root children) |
| **Protected Subfolders (22 folders)** | 2,308 | 443,583,146 | 423.03 MB | 428.18 MB | **PROTECTED / UNTOUCHED** |
| **SORTED Output Folder (`Sorted/`)** | 86 | 10,321,237 | 9.84 MB | 9.53 MB | **EXCLUDED** (Output folder) |
| **Total Workspace (Recursive)** | **2,452** | **939,114,489** | **895.61 MB** | **900.57 MB** | Informational indexing only |
| **Total Workspace (Excl. `Sorted/`)** | **2,366** | **928,793,252** | **885.76 MB** | **891.04 MB** | Future indexing baseline |

### Itemized Subfolder Storage Audit (Top 10)

| Subfolder | Files | Logical Size | % of Subfolder Storage | Safety Status |
| :--- | :---: | :---: | :---: | :--- |
| `SORTED_6_Project_Docs/` | 1,371 | 283.57 MB | 62.5% | Preserved / Untouched |
| `GenicoCode/` | 508 | 61.39 MB | 13.5% | Preserved / Untouched |
| `About _ Maria Vasilyeva_files/` | 2 | 19.71 MB | 4.3% | Preserved / Untouched |
| `garden-satydev-breathe-frames/` | 90 | 10.90 MB | 2.4% | Preserved / Untouched |
| `Sorted/` (Output folder) | 86 | 9.84 MB | 2.2% | Excluded from future indexing |
| `task9_1602643917499_frames/` | 47 | 8.95 MB | 2.0% | Preserved / Untouched |
| `satuautomates/` | 196 | 8.90 MB | 2.0% | Preserved / Untouched |
| `custom_id_cards/` | 2 | 6.23 MB | 1.4% | Preserved / Untouched |
| `custom_id_cards (1)/` | 2 | 4.32 MB | 1.0% | Preserved / Untouched |
| `dafi-tropicdane-huy-phan.../` | 2 | 4.20 MB | 0.9% | Preserved / Untouched |

### Special File Attributes Audit
- **Hidden Files:** 0 hidden files skipped at root.
- **System Files:** 0 system files skipped at root.
- **Symlinks / Junctions:** 0 symlinks encountered.
- **Inaccessible / Permission Denied:** 0 files inaccessible.
- **Duplicate Detection Filtering:** Duplicate detection did **not** exclude any files from counting.
- **Calculation Type:** The application calculates storage using exact **logical file size** (`st_size`), formatted with binary prefixes ($1024^2$). Windows Explorer displays logical size as primary ("Size") and allocated cluster size as secondary ("Size on disk: 900 MB").

---

## 2. Architecture for Correct Counting Logic

To reconcile the display with the filesystem while rigorously preserving the safety rule:
1. **Dual-Metric Storage Display:**
   - **Workspace Storage (Recursive):** Displays total disk footprint of the workspace (`885.8 MB` across `2,366 files`, explicitly excluding `Sorted/` to prevent double-counting).
   - **Eligible Loose Files (Root Only):** Displays `462.7 MB` across `58 files` as the only files eligible for organization.
   - **Protected Storage (Untouched):** Displays `423.0 MB` across `22 subfolders` with clear badge indicating subfolders will not be traversed.
2. **Exclusion of `Sorted/`:**
   - The scanner explicitly ignores any directory named `Sorted` or matching `workspace/Sorted` to avoid indexing previously organized outputs or double-counting storage.

---

## 3. Exact UI Components Requiring Modernization

### Component 1: Organization Plan Table (`table_plan`)
- **Current Issue:** Current Location and Proposed Destination columns use `QHeaderView.Stretch` with no horizontal scrolling or manual resizing, clipping paths like `C:\\Users\\satya\\Downloads\\Sorted\\Documents\\PDF\\...`.
- **Fixes:**
  - Enable interactive column resizing via header handles (`QHeaderView.Interactive`).
  - Enable horizontal scroll bar (`setHorizontalScrollBarPolicy(Qt.ScrollBarAsNeeded)`).
  - Set generous, readable default column widths (Include: 64px, File Name: 220px, Current Location: 280px, Proposed Destination: 340px, Status: 120px).
  - Add full-path tooltips to `Current Location` and `Proposed Destination` cells.
  - Implement a **Context Menu (Right-Click)** with:
    - `Copy Proposed Destination`
    - `Copy Current Location`
    - `Copy File Name`
    - `Open Containing Folder in Explorer`
  - Add a dedicated quick-copy action icon on hover/selection or table action bar.

### Component 2: Design System & Color Palette
- **Current Issue:** Prominent cobalt blue (`#3478F6`) is used for primary buttons, highlights, badges, and accents.
- **Fixes (Monochrome Apple / Notion Palette):**
  - **Canvas:** `#F7F7F5`
  - **Surface:** `#FFFFFF`
  - **Primary Text:** `#1D1D1F`
  - **Secondary Text:** `#86868B`
  - **Borders:** `#E5E5E5`
  - **Hover:** `#F1F1EF`
  - **Selected Background:** `#EBEBEB` (with dark text `#1D1D1F`)
  - **Primary Action Button:** Charcoal background (`#1D1D1F` / `#242521`) with clean white text (`#FFFFFF`) and smooth hover (`#3A3A3C`).
  - **Eliminate Colored Badges:** Replace bright green/blue status tags with subtle monochrome pills (`#F1F1EF` background with `#1D1D1F` text and `#E5E5E5` border).

### Component 3: Geometry & Radius Consistency
- **Buttons:** 10–12px border radius.
- **Inputs & Dropdowns:** 10px border radius, `#E5E5E5` border, `#F7F7F5` focus state.
- **Cards & Stat Panels:** 12–14px border radius with subtle `#E5E5E5` border and 0 harsh shadows.
- **Tables & Headers:** Clean flat headers with subtle divider lines, no Windows 95 3D bevels.
- **Typography:** Multi-tier font stack prioritizing `'SF Pro Text', 'SF Pro Display', 'Segoe UI Variable Text', 'Segoe UI', system-ui, sans-serif`.
"""

    os.makedirs(os.path.dirname(artifact_path), exist_ok=True)
    with open(artifact_path, "w", encoding="utf-8") as f:
        f.write(doc)

    print("Diagnostic report written successfully to:", artifact_path)

if __name__ == "__main__":
    main()
