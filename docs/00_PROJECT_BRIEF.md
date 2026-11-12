# SORTED --- Project Brief

**Project type:** Second-year DAA desktop application\
**Working title:** SORTED\
**Language:** C++17\
**GUI:** Qt 6 Widgets\
**Platform target:** Windows 10/11 first; keep filesystem code portable
where practical\
**Team:** Solo\
**Status:** MVP specification

## 1. One-line description

SORTED is a preview-first desktop application that recursively scans a
selected folder, classifies files, detects exact duplicates, and
proposes a safer, cleaner folder structure that the user can review
before applying.

## 2. The problem

Downloads folders accumulate documents, images, videos, archives,
installers, code files and nested folders. Finding files becomes
difficult, and manually reorganizing them is tedious and risky.

SORTED reduces this friction by turning an unstructured folder into a
proposed organization plan. It does not silently move or delete files.

## 3. Product principles

1.  **Preview before action:** scanning and planning never modify files.
2.  **User remains in control:** the user approves the move plan.
3.  **No destructive automation:** no automatic deletion or overwriting.
4.  **Traceability:** every completed move is recorded.
5.  **Recoverability:** provide an undo operation when the original path
    is available.
6.  **Honest classification:** unknown extensions go into an `Other`
    category; do not guess.
7.  **Meaningful DAA:** algorithms solve visible tasks and can be
    benchmarked independently.

## 4. MVP workflow

1.  User selects a root directory, normally `Downloads`.
2.  SORTED recursively scans eligible files and nested directories.
3.  The scanner records metadata: name, path, extension, size and
    modified time.
4.  The classifier assigns a category and extension group.
5.  The duplicate detector groups exact-content matches.
6.  The planner proposes destinations.
7.  The user reviews, excludes, locks or changes individual moves.
8.  The executor applies approved moves and writes an operation journal.
9.  The user can inspect history and undo completed moves where safe.

## 5. Default folder proposal

The proposed default is:

``` text
Downloads/
└── Sorted/
    ├── Documents/
    │   ├── PDF/
    │   ├── DOCX/
    │   └── TXT/
    ├── Images/
    │   ├── PNG/
    │   └── JPG/
    ├── Videos/
    ├── Audio/
    ├── Archives/
    ├── Installers/
    ├── Code/
    └── Other/
```

The folder name and category rules should be configurable. The `Sorted`
output directory and the application's staging/journal directory must be
excluded from future scans to prevent rescanning its own output.

## 6. MVP feature boundary

### Included

-   Recursive file scanning.
-   File metadata collection.
-   Extension-based categorization.
-   Custom Merge Sort for at least one visible sorting operation.
-   Exact duplicate detection using file size as a prefilter and SHA-256
    as the content check.
-   Organization preview.
-   Per-file include/exclude and destination override.
-   Collision-safe file moves.
-   Operation history and basic undo.
-   Error reporting and scan progress.

### Not included in MVP

-   AI-based semantic classification.
-   Similar-image or near-duplicate detection.
-   Automatic deletion.
-   Cloud drives or network shares.
-   Background real-time monitoring.
-   Archive extraction.
-   Full disk indexing.
-   Automatic cleanup of empty folders.
-   Complex user scripting or rule language.

## 7. DAA learning outcomes

The project demonstrates:

-   Divide and Conquer through Merge Sort.
-   Hashing through exact duplicate grouping.
-   Greedy decision-making through deterministic file classification and
    destination planning.
-   Empirical analysis by measuring scan, sort, hash and planning stages
    separately.

**Important academic note:** extension classification is a rule-based
greedy assignment problem, not a sophisticated global optimization
problem. The report must describe it accurately. Do not claim that the
application uses an optimal global arrangement algorithm unless one is
actually implemented and tested.

## 8. Definition of done

The MVP is complete when a user can select a test directory, recursively
scan it, inspect categories and duplicate groups, preview proposed
moves, apply selected moves without overwriting files, view a journal,
and undo a move when the original destination is free.

The application must pass the safety tests in
`05_IMPLEMENTATION_AND_TESTING.md` before it is tested on a real
Downloads folder.
