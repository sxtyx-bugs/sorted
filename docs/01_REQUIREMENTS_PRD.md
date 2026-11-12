# SORTED --- Product Requirements

## 1. Users and use case

**Primary user:** someone whose Downloads directory contains a mixture
of file types and nested folders.

**Primary use case:** organize files recursively while preserving user
control and avoiding accidental data loss.

## 2. Functional requirements

  -----------------------------------------------------------------------
  ID                      Requirement             Priority
  ----------------------- ----------------------- -----------------------
  FR-01                   User can select a root  Must
                          directory               

  FR-02                   App recursively         Must
                          discovers eligible      
                          regular files           

  FR-03                   App reports scan        Must
                          progress and errors     

  FR-04                   App records file name,  Must
                          extension, path, size   
                          and modified time       

  FR-05                   App categorizes files   Must
                          using configurable      
                          extension rules         

  FR-06                   User can view and sort  Must
                          the file list           

  FR-07                   App detects exact       Must
                          duplicates using        
                          content hashes          

  FR-08                   App generates a         Must
                          proposed organization   
                          plan                    

  FR-09                   User can exclude a file Must
                          or override its         
                          destination             

  FR-10                   User can preview source Must
                          and destination paths   

  FR-11                   App never overwrites an Must
                          existing destination    

  FR-12                   App records completed   Must
                          moves in a journal      

  FR-13                   User can undo a move if Must
                          the original path is    
                          available               

  FR-14                   App can export a CSV    Should
                          summary                 

  FR-15                   User can edit           Should
                          extension-to-category   
                          rules                   

  FR-16                   User can choose whether Could
                          hidden files are        
                          included                
  -----------------------------------------------------------------------

## 3. Non-functional requirements

-   **Safety:** no file move occurs before explicit user confirmation.
-   **Data integrity:** cross-volume moves use copy, verification and
    then source removal.
-   **Responsiveness:** scanning and hashing run outside the GUI thread.
-   **Portability:** use standard C++17 filesystem APIs and Qt
    abstractions where possible.
-   **Maintainability:** algorithms must be independent of Qt widgets.
-   **Transparency:** every skipped file and failed operation has a
    visible reason.
-   **Performance:** avoid loading file contents fully into memory for
    hashing.

## 4. User stories

1.  As a user, I can choose my Downloads folder so SORTED knows what to
    scan.
2.  As a user, I can see files inside nested folders, not just the root.
3.  As a user, I can see how many files belong to each category.
4.  As a user, I can identify exact duplicate files without relying on
    filenames.
5.  As a user, I can see every proposed move before it happens.
6.  As a user, I can keep a file in its current location.
7.  As a user, I can change a proposed destination.
8.  As a user, I can inspect what changed and reverse a move when safe.

## 5. Classification rules

Use lowercase extensions without the leading dot as lookup keys.
Normalize extension case before lookup.

Suggested initial rules:

  Category        Extensions
  --------------- -------------------------------------------------------------
  Documents       pdf, doc, docx, odt, rtf, txt
  Spreadsheets    csv, xls, xlsx, ods
  Presentations   ppt, pptx, odp
  Images          png, jpg, jpeg, webp, gif, bmp, svg, tif, tiff
  Videos          mp4, mkv, mov, avi, webm
  Audio           mp3, wav, flac, m4a, ogg
  Archives        zip, rar, 7z, tar, gz
  Installers      exe, msi, msix, dmg, pkg, deb, rpm
  Code            c, cpp, h, hpp, java, py, js, ts, html, css, json, xml, sql
  Other           everything not matched

These are starter rules, not an exhaustive taxonomy. A file without an
extension belongs in `Other`. Do not infer type from a filename
substring.

## 6. Proposed destination policy

Default destination template:

`<root>/Sorted/<Category>/<Extension>/`

For files without an extension, use:

`<root>/Sorted/<Category>/No Extension/`

The destination template must be configurable in a later version. In the
MVP, it is acceptable to keep it fixed and expose the category rules in
one configuration file.

## 7. Acceptance criteria

-   A recursive scan finds files in nested directories.
-   The `Sorted` output folder is excluded from subsequent scans.
-   The same content with different names is detected as duplicate.
-   Different content with the same name is not marked duplicate.
-   Unknown extensions are placed in `Other`.
-   A move plan is visible before execution.
-   Existing destination files are never overwritten.
-   Failed moves are reported and do not prevent unrelated moves from
    continuing.
-   A journal entry is written for each successful move.
-   Undo never overwrites a file at the original path.
