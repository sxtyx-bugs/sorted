# SORTED --- DAA Algorithms and Complexity

## 1. Algorithm selection

The MVP uses three clearly explainable techniques:

1.  **Merge Sort (Divide and Conquer):** sort the file table.
2.  **Hash-based grouping:** identify exact duplicate file contents.
3.  **Greedy classification and planning:** assign each file to a
    destination using a deterministic rule priority.

Do not describe extension lookup as AI or as a globally optimal
optimization algorithm. The planner is a deterministic rule-based greedy
process.

## 2. Merge Sort

### Problem

The user needs to sort discovered files by name, size, extension or
modification time.

### Divide and Conquer strategy

1.  Divide the array into two halves.
2.  Recursively sort each half.
3.  Merge the two sorted halves.

### Pseudocode

``` text
MERGE-SORT(A, left, right)
    if left >= right
        return

    mid = left + (right - left) / 2

    MERGE-SORT(A, left, mid)
    MERGE-SORT(A, mid + 1, right)
    MERGE(A, left, mid, right)
```

### Complexity

Recurrence:

`T(n) = 2T(n/2) + O(n)`

-   Best: O(n log n)
-   Average: O(n log n)
-   Worst: O(n log n)
-   Auxiliary space: O(n)

### Demonstration

Generate file records with known sizes and verify that the output is
sorted ascending and descending. Compare results with a trusted
reference sort in tests, but retain the custom Merge Sort in the
application for the DAA demonstration.

## 3. Exact duplicate detection using hashing

### Problem

Two files may have different names and paths but contain identical
bytes. Filename comparison is not sufficient.

### Pipeline

1.  Group file records by file size.
2.  Ignore size groups with fewer than two members.
3.  Calculate SHA-256 for each candidate.
4.  Group candidates by hash.
5.  Treat groups with two or more matching hashes as duplicate
    candidates.

### Why size prefiltering?

Files with different byte lengths cannot be byte-for-byte identical.
Size grouping avoids hashing files that cannot match another file.

### Pseudocode

``` text
DETECT-DUPLICATES(files)
    sizeGroups = group files by size
    duplicateGroups = empty

    for each group in sizeGroups
        if group.count < 2
            continue

        for each file in group
            digest = SHA256-STREAM(file)
            hashGroups[digest].append(file)

    for each hashGroup in hashGroups
        if hashGroup.count > 1
            duplicateGroups.append(hashGroup)

    return duplicateGroups
```

### Streaming hash

Read a file in chunks (for example, 1 MiB) and update the SHA-256
context for each chunk. Do not read a multi-gigabyte file into one
memory buffer.

Qt's `QCryptographicHash` supports SHA-256 and incremental updates.
Check file read errors and do not treat a partial hash as valid.

### Complexity

Let:

-   `n` = number of files.
-   `B` = total bytes read from candidate files.
-   `H` = number of candidate files hashed.

Expected hash-table grouping cost is O(H). File reading and SHA-256
processing is O(B), so a useful overall description is expected O(H +
B), with O(H) grouping storage plus the records themselves.

Hash collisions are theoretically possible. For a practical duplicate
finder, SHA-256 is a strong content fingerprint, but a high-assurance
implementation can perform a final byte-by-byte comparison inside
matching hash groups before declaring files identical.

### Demonstration

-   Same bytes, different filenames: detected.
-   Same filename, different bytes: not detected as duplicate.
-   Same size, different bytes: not detected as duplicate.
-   Different sizes: never placed in the same candidate group.
-   Unreadable file: reported as a hash error, not classified as unique.

## 4. Greedy classification and destination planning

### Problem

Each eligible file needs a proposed destination based on a set of
user-understandable extension rules.

### Greedy decision

For each file, choose the first matching rule according to configured
rule priority. If no rule matches, choose `Other`.

### Example

``` text
report.pdf   → Documents/PDF/
photo.png    → Images/PNG/
movie.mp4    → Videos/MP4/
source.cpp   → Code/CPP/
unknown.xyz  → Other/XYZ/
```

### Pseudocode

``` text
BUILD-PLAN(files, rules, outputRoot)
    plan = empty

    for each file in files
        if file is excluded
            continue

        extension = NORMALIZE(file.extension)
        category = FIRST-MATCHING-CATEGORY(extension, rules)

        if no category
            category = "Other"

        destination = outputRoot / category / extension / file.name
        destination = RESOLVE-COLLISION-WITHOUT-OVERWRITE(destination)

        if destination is not the current path
            plan.append(file.path, destination)

    return plan
```

### Complexity

With a hash-map extension lookup and a fixed number of rules,
classification and initial planning are expected O(n). Destination
collision checks should use an in-memory set of reserved destination
paths to avoid repeatedly scanning the plan.

Filesystem existence checks add I/O cost and must be repeated
immediately before execution because the filesystem may change after
preview.

## 5. Combined pipeline

The application does not need to run every algorithm in one monolithic
function.

``` text
FileScanner
    ↓
FileRecord list
    ├── MergeSort → sorted file view
    ├── Classifier → category metadata
    └── DuplicateDetector → duplicate groups
                  ↓
           OrganizationPlanner
                  ↓
             User approval
                  ↓
            FileOperationService
```

## 6. Benchmark methodology

Use generated test directories with 100, 500, 1,000 and 5,000 files.
Include a mixture of small and large files.

Measure separately:

-   Recursive traversal duration.
-   Merge Sort duration.
-   Classification duration.
-   Duplicate candidate grouping duration.
-   Actual hashing duration and total bytes read.
-   Planning duration.

Do not combine disk I/O and algorithm time into one number when
explaining complexity. Disk speed, cache state, file sizes and operating
system behaviour influence wall-clock measurements.

For Merge Sort, report input size and elapsed time. For duplicate
detection, report both candidate count and bytes hashed. Repeat trials
and report a mean or median.

## 7. What not to claim

-   Do not claim O(n) duplicate detection without mentioning
    file-reading cost.
-   Do not claim Merge Sort is O(n) in the best case.
-   Do not claim the greedy classifier finds a globally optimal folder
    arrangement.
-   Do not claim SHA-256 proves mathematical identity; it provides a
    very strong practical fingerprint.
-   Do not compare algorithm timings unless the test data and machine
    conditions are recorded.
