# SORTED --- Software Architecture

## 1. Architecture overview

Use a layered architecture:

``` text
┌──────────────────────────────────────────┐
│                 Qt UI                    │
│ MainWindow / Pages / Models / Dialogs    │
└────────────────────┬─────────────────────┘
                     │ signals / slots
┌────────────────────▼─────────────────────┐
│              Application Layer           │
│ ScanController / PlanController          │
│ ExecutionController / HistoryController  │
└────────────────────┬─────────────────────┘
                     │
┌────────────────────▼─────────────────────┐
│                 Core                     │
│ Scanner / Classifier / MergeSort         │
│ DuplicateDetector / OrganizationPlanner  │
└────────────────────┬─────────────────────┘
                     │
┌────────────────────▼─────────────────────┐
│             Infrastructure               │
│ std::filesystem / QCryptographicHash     │
│ JournalRepository / FileOperationService│
└──────────────────────────────────────────┘
```

The core layer must not depend on Qt widgets. This makes algorithms
easier to unit test and explain in the DAA viva.

## 2. Suggested project structure

``` text
SORTED/
├── CMakeLists.txt
├── README.md
├── docs/
│   ├── 00_PROJECT_BRIEF.md
│   ├── 01_REQUIREMENTS_PRD.md
│   ├── 02_ARCHITECTURE.md
│   ├── 03_ALGORITHMS_DAA.md
│   ├── 04_UI_UX_DESIGN.md
│   └── 05_IMPLEMENTATION_AND_TESTING.md
├── src/
│   ├── main.cpp
│   ├── models/
│   │   ├── FileRecord.h
│   │   ├── MoveOperation.h
│   │   └── ScanResult.h
│   ├── core/
│   │   ├── FileScanner.h
│   │   ├── FileScanner.cpp
│   │   ├── FileClassifier.h
│   │   ├── FileClassifier.cpp
│   │   ├── MergeSort.h
│   │   ├── DuplicateDetector.h
│   │   ├── DuplicateDetector.cpp
│   │   ├── OrganizationPlanner.h
│   │   └── OrganizationPlanner.cpp
│   ├── operations/
│   │   ├── FileOperationService.h
│   │   ├── FileOperationService.cpp
│   │   ├── OperationJournal.h
│   │   └── OperationJournal.cpp
│   └── ui/
│       ├── MainWindow.h
│       ├── MainWindow.cpp
│       ├── OverviewPage.h
│       ├── FilesPage.h
│       ├── DuplicatesPage.h
│       ├── OrganizePage.h
│       └── HistoryPage.h
└── tests/
    ├── test_mergesort.cpp
    ├── test_classifier.cpp
    ├── test_duplicates.cpp
    ├── test_planner.cpp
    └── test_file_operations.cpp
```

## 3. Core data models

### FileRecord

``` cpp
struct FileRecord {
    std::string name;
    std::string extension;       // normalized, no leading dot
    std::filesystem::path path;
    std::uintmax_t size = 0;
    std::filesystem::file_time_type modifiedTime;
    std::string category;
    std::string sha256;           // empty until calculated
    bool isDuplicate = false;
    bool eligible = true;
    std::string scanError;
};
```

### MoveOperation

``` cpp
struct MoveOperation {
    std::filesystem::path source;
    std::filesystem::path destination;
    std::string status;           // planned, completed, failed, undone
    std::string errorMessage;
};
```

Keep status values as an enum in the actual implementation rather than
free-form strings.

## 4. Module responsibilities

### FileScanner

-   Accepts one selected root directory.
-   Traverses recursively.
-   Collects regular files only.
-   Does not follow symlinks by default.
-   Skips the configured output and staging directories.
-   Records errors rather than crashing the entire scan.
-   Emits progress through a controller.

### FileClassifier

-   Normalizes the extension.
-   Looks up the extension in a rule table.
-   Returns a category and extension group.
-   Assigns unknown or missing extensions to `Other`.

### MergeSort

-   Sorts `FileRecord` collections by a selected key.
-   Implements merge sort explicitly for the DAA requirement.
-   Uses a stable comparison so equal keys preserve relative order.

### DuplicateDetector

-   Groups candidates by file size.
-   Hashes only groups containing two or more files.
-   Reads files in fixed-size chunks.
-   Groups identical SHA-256 results.
-   Reports hash/read errors separately.

### OrganizationPlanner

-   Converts classified records into proposed destination paths.
-   Excludes files already in their intended destination.
-   Detects destination collisions within the plan and on disk.
-   Generates unique destination names without overwriting.
-   Does not perform filesystem changes.

### FileOperationService

-   Revalidates each source immediately before moving.
-   Creates destination directories only during execution.
-   Performs safe same-volume moves.
-   Handles cross-volume moves using copy, verify, then remove.
-   Records each completed operation.
-   Never deletes duplicate files.

### OperationJournal

-   Persists source, destination, status, timestamp and error.
-   Writes updates atomically where possible.
-   Supports history display and undo.
-   A JSON journal is sufficient for the MVP; SQLite can be added later.

## 5. Threading model

The GUI thread handles rendering and user interaction only.

Run recursive scanning and hashing in a worker using `QThreadPool` /
`QtConcurrent`. Send progress and results back through queued Qt
signals/slots. Do not update widgets directly from a worker thread.

Disable conflicting actions while a scan or execution is active. Provide
cancellation for scanning; cancellation must not leave partially moved
files because scanning itself is read-only.

## 6. Data flow

``` text
Select root
   ↓
Validate root and exclusions
   ↓
Recursive scan
   ↓
FileRecord collection
   ├── Merge Sort for user-selected ordering
   ├── Extension classification
   └── Duplicate detection
           ↓
     Organization plan
           ↓
       User review
           ↓
     Execute approved moves
           ↓
     Persist operation journal
           ↓
       History / Undo
```

## 7. Error handling

Errors should be attached to individual files or operations where
possible. Examples:

-   Permission denied.
-   File disappeared after scan.
-   File changed after scan.
-   Destination is not writable.
-   Destination collision.
-   Insufficient disk space.
-   Hash read failure.
-   Cross-volume verification failure.

A failure for one file should not silently abort the entire batch.
Continue with independent operations, then show a summary.

## 8. Key invariants

1.  Scanning never changes the filesystem.
2.  Planning never changes the filesystem.
3.  Only user-approved plan entries can execute.
4.  No operation overwrites an existing file.
5.  The scanner never follows symlinks outside the selected root.
6.  The output and staging directories are excluded from scanning.
7.  A move is journaled as completed only after the destination exists
    and the operation is verified.
8.  Undo checks that the original path is free before restoring.
