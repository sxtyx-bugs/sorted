# SORTED --- Implementation Plan, Safety and Testing

## 1. Build environment

Recommended setup:

-   C++17 compiler.
-   Qt 6 Widgets.
-   CMake.
-   Qt Test for unit tests.
-   Qt Concurrent for background tasks.
-   `QCryptographicHash` for SHA-256.
-   `QSaveFile` for safer journal/config writes.

For the MVP, prefer Qt's built-in hashing rather than adding OpenSSL as
another dependency.

## 2. Suggested CMake configuration

``` cmake
cmake_minimum_required(VERSION 3.21)
project(SORTED LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent Test)

qt_add_executable(SORTED
    src/main.cpp
    src/ui/MainWindow.cpp
    src/core/FileScanner.cpp
    src/core/FileClassifier.cpp
    src/core/DuplicateDetector.cpp
    src/core/OrganizationPlanner.cpp
    src/operations/FileOperationService.cpp
    src/operations/OperationJournal.cpp
)

target_link_libraries(SORTED PRIVATE
    Qt6::Widgets
    Qt6::Concurrent
)
```

Add source files and headers as they are created. Add a separate test
executable linked to `Qt6::Test`.

## 3. Build milestones

### Milestone 1 --- Scanner

-   Create the Qt Widgets project.
-   Add folder selection.
-   Implement recursive traversal.
-   Collect metadata into `FileRecord`.
-   Display results in a basic table.
-   Add scan progress and per-file errors.

**Exit condition:** scan a test directory containing nested folders and
show all eligible files.

### Milestone 2 --- Algorithms

-   Implement Merge Sort and unit tests.
-   Implement extension normalization and classification.
-   Implement size prefiltering and chunked SHA-256 duplicate detection.
-   Add category and duplicate views.
-   Add simple timing instrumentation.

**Exit condition:** algorithm tests pass on deterministic test data.

### Milestone 3 --- Planner

-   Generate destination paths.
-   Exclude the output directory and staging directory from scanning.
-   Detect duplicate destination paths within the proposed plan.
-   Resolve existing destination names without overwriting.
-   Display the proposed move list.
-   Add include/exclude and destination override.

**Exit condition:** user can inspect a complete plan without any
filesystem changes.

### Milestone 4 --- Safe operations

-   Revalidate source files before each move.
-   Create destination directories.
-   Move same-volume files safely.
-   Implement cross-volume copy-to-temporary, verify, final rename, then
    source removal.
-   Write journal entries.
-   Implement undo with conflict checks.
-   Add partial-failure summary.

**Exit condition:** test moves and undo only in disposable test
directories.

### Milestone 5 --- UI polish and submission

-   Finish Overview, All Files, Categories, Duplicates, Organize and
    History.
-   Add empty, loading, warning and error states.
-   Add CSV export if time permits.
-   Record benchmark results.
-   Prepare screenshots and viva explanation.

## 4. File-operation safety design

### Same-volume move

Use a filesystem rename where supported. Revalidate that the source
exists and the destination does not exist immediately before the
operation. Do not use an API mode that silently replaces the
destination.

### Cross-volume move

A rename may fail across volumes. Use this sequence:

1.  Copy source to a uniquely named temporary file in the destination
    directory.
2.  Flush and close the destination file.
3.  Verify copied size and SHA-256 against the source.
4.  Rename the temporary file to the final destination, ensuring the
    final destination does not already exist.
5.  Remove the original source only after successful verification and
    finalization.
6.  Journal the completed operation.

If any step fails, preserve the source. Remove temporary output only
when it is safe and the application can identify it as its own
incomplete temporary file.

### Collision handling

If `report.pdf` already exists, try `report (1).pdf`, then
`report (2).pdf`, and so on. Check both the filesystem and an in-memory
set of destinations reserved by the current plan.

### Undo

Undo in reverse execution order:

1.  Confirm the recorded destination still exists.
2.  Confirm the original source path is currently free.
3.  Verify the destination still represents the moved file, using
    recorded size and hash where available.
4.  Move it back without overwriting.
5.  Mark the journal entry as undone.

If the original path is occupied, do not overwrite it. Ask the user to
resolve the conflict.

### Important limitations

Filesystem operations are not a universal transaction across all
platforms. A crash, power loss, permissions change or disk failure can
interrupt a batch. The journal supports recovery and transparency, but
do not promise perfect atomic rollback of an entire batch.

## 5. Test data preparation

Create a disposable test directory:

``` text
SORTED_TEST/
├── loose.pdf
├── photo.png
├── duplicate-a.txt
├── duplicate-b.txt
├── same-name/
│   └── report.pdf
├── nested/
│   └── deeper/
│       ├── video.mp4
│       └── unknown.xyz
└── empty-folder/
```

Make `duplicate-a.txt` and `duplicate-b.txt` contain identical bytes.
Create a second `report.pdf` with different content to test filename
collision handling.

Never use the real Downloads directory for initial testing.

## 6. Test cases

  -----------------------------------------------------------------------
  ID                      Scenario                Expected result
  ----------------------- ----------------------- -----------------------
  TC-01                   Root contains files     All eligible root files
                                                  discovered

  TC-02                   Files exist several     Nested files discovered
                          folders deep            

  TC-03                   Extension uses          Correct normalized
                          uppercase letters       category

  TC-04                   File has no extension   Classified as Other

  TC-05                   Unknown extension       Classified as Other

  TC-06                   Same bytes, different   Same duplicate group
                          names                   

  TC-07                   Same name, different    Not a duplicate
                          bytes                   

  TC-08                   Same size, different    Not a duplicate
                          bytes                   

  TC-09                   Destination name        Unique destination
                          already exists          generated

  TC-10                   File is excluded by     No move
                          user                    proposed/executed

  TC-11                   Source disappears after Operation fails safely
                          scan                    and is reported

  TC-12                   Destination is not      Operation fails; source
                          writable                remains

  TC-13                   Undo destination path   Undo blocked; no
                          is occupied             overwrite

  TC-14                   Output folder exists    Output is excluded from
                          during rescan           scan

  TC-15                   Symlink points outside  Symlink is not followed
                          selected root           

  TC-16                   One move fails in a     Other independent moves
                          batch                   can continue

  TC-17                   Cross-volume move       Copy is verified before
                                                  source removal

  TC-18                   Scan is cancelled       No files have been
                                                  modified
  -----------------------------------------------------------------------

## 7. Performance experiments

Use 100, 500, 1,000 and 5,000 files. Vary the distribution of file sizes
and duplicate frequency.

Record:

  Metric                    Measurement
  ------------------------- ---------------------------------------------------
  Scan time                 Recursive traversal duration
  Sort time                 Merge Sort duration
  Classification time       Rule lookup duration
  Duplicate grouping time   Size and hash grouping overhead
  Hashing time              Time and total bytes read
  Planning time             Destination generation duration
  Memory                    Approximate peak application memory if measurable

Run repeated trials. Report median or mean and document the machine,
operating system, storage type and test dataset. Avoid presenting a
single run as a universal benchmark.

## 8. Submission and viva preparation

Be ready to explain:

1.  Why recursive traversal is required.
2.  Why Merge Sort is a Divide and Conquer algorithm.
3.  Its recurrence and time/space complexity.
4.  Why file size is a useful duplicate prefilter.
5.  What SHA-256 does and why filenames are insufficient.
6.  What the greedy classification decision is.
7.  How destination collisions are handled.
8.  Why preview and explicit approval are important.
9.  How undo uses the operation journal.
10. What happens when a cross-volume move fails.

## 9. MVP completion checklist

-   [ ] Folder selection works.
-   [ ] Recursive scan works.
-   [ ] Nested files appear in the table.
-   [ ] Scan errors are reported.
-   [ ] Merge Sort is implemented and tested.
-   [ ] Extension classification works.
-   [ ] SHA-256 duplicate detection works in chunks.
-   [ ] Planner generates destinations.
-   [ ] Output directory is excluded from rescans.
-   [ ] Preview supports exclusion and destination override.
-   [ ] Existing files are never overwritten.
-   [ ] Approved moves execute.
-   [ ] Journal records successful and failed operations.
-   [ ] Undo checks for conflicts.
-   [ ] All safety test cases pass.
-   [ ] Complexity analysis and benchmark results are documented.
