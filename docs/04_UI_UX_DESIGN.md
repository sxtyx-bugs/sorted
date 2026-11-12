# SORTED --- UI / UX Design Specification

## 1. Design direction

SORTED should feel like a calm, premium desktop utility: clear
typography, restrained colour, useful whitespace and precise feedback.

Avoid: - Neon, cyberpunk or futuristic styling. - Gradients and glowing
cards. - Excessive dashboard widgets. - Decorative charts that do not
answer a question. - Destructive actions presented as primary buttons. -
Cluttered file rows with too many columns.

Suggested visual language:

-   Background: warm off-white (`#F7F7F4`).
-   Main text: charcoal (`#242521`).
-   Secondary text: muted grey (`#777A73`).
-   Accent: restrained olive (`#737C5B`) or one muted red for warnings.
-   Borders: subtle warm grey (`#E6E6E0`).
-   Typography: system UI font or a bundled readable sans-serif.
-   Icons: consistent monochrome line icons.

## 2. Application shell

Use a fixed left sidebar and a main content area.

``` text
┌────────────────────────────────────────────────────────────┐
│ sorted.                                      Scan folder   │
├───────────────┬────────────────────────────────────────────┤
│ Overview      │                                            │
│ All files     │             Main page content              │
│ Categories    │                                            │
│ Duplicates    │                                            │
│ Organize      │                                            │
│ History       │                                            │
│               │                                            │
│ Selected path │                                            │
└───────────────┴────────────────────────────────────────────┘
```

The selected root path should remain visible. Provide a clear button to
choose a different folder.

## 3. Pages

### Overview

Show only useful summary information:

-   Number of files scanned.
-   Total size.
-   Number of categories.
-   Duplicate groups and duplicate candidate count.
-   Last scan status.
-   A concise category distribution.

Primary action: `Scan folder` or `Review organization plan`, depending
on state.

### All Files

A table with:

-   Name.
-   Category.
-   Extension.
-   Size.
-   Modified date.
-   Current folder.

Features:

-   Search by filename.
-   Filter by category.
-   Sort by size, name, extension or modified date.
-   Open containing folder.
-   Do not expose full paths in every row by default; show them in
    details.

### Categories

Show category totals and extension subgroups. Selecting a category
filters the file table.

### Duplicates

Display duplicate groups. For each group, show filenames, locations,
sizes and hashes (hash can be truncated visually). Do not provide a
default "delete all" action. The MVP should only identify and report
duplicates.

### Organize

This is the most important page.

Show a reviewable table:

  Include   File         Current location   Proposed destination     Status
  --------- ------------ ------------------ ------------------------ ----------
  Yes       report.pdf   college/           Sorted/Documents/PDF/    Ready
  No        setup.exe    old/               Sorted/Installers/EXE/   Excluded

Allow the user to: - Include or exclude a move. - Lock a file in
place. - Change a destination. - See collision warnings. - Review the
number of planned moves.

Primary action: `Apply approved moves`.

Require a confirmation dialog summarizing the number of moves and
selected root.

### History

Show completed and failed operations with timestamps, source,
destination and status. Provide `Undo` only when the original path is
available and the destination still matches the expected moved file.

## 4. Interaction states

Design explicit states for:

-   No folder selected.
-   Ready to scan.
-   Scanning with progress.
-   Scan completed.
-   Scan completed with warnings.
-   No files found.
-   Plan ready.
-   Plan has conflicts.
-   Executing moves.
-   Execution partially failed.
-   Execution completed.
-   Undo unavailable because the original path is occupied.

Never show a fake progress percentage. Use discovered/processed counts
where possible.

## 5. Confirmation and safety UX

Before execution, display:

-   Selected root directory.
-   Number of files to move.
-   Number excluded.
-   Number of conflicts resolved.
-   Whether cross-volume copy-and-verify is required.
-   A statement that no files will be overwritten.

Use clear verbs: `Preview`, `Apply moves`, `Undo move`. Avoid vague
labels such as `Clean`, `Fix` or `Optimize` for destructive operations.

## 6. Qt widget suggestions

  UI need                   Qt option
  ------------------------- --------------------------------------
  Main window               QMainWindow
  Sidebar                   QListWidget or custom QWidget
  Page navigation           QStackedWidget
  File table                QTableView + QAbstractTableModel
  Search                    QLineEdit
  Category filter           QComboBox
  Progress                  QProgressBar
  Confirmation              QMessageBox or custom QDialog
  Folder selection          QFileDialog
  Background work           QThreadPool / QtConcurrent
  Drag-and-drop overrides   QDrag / QDropEvent, optional for MVP

For a solo implementation, use a normal destination dropdown first. Add
drag-and-drop only after the preview and move planner work reliably.

## 7. Accessibility and usability

-   Maintain readable contrast.
-   Make keyboard navigation possible.
-   Use text labels alongside icons.
-   Show file sizes in consistent units.
-   Make error messages actionable.
-   Preserve the selected folder and last-used settings between
    sessions.
-   Do not rely on colour alone to communicate status.
