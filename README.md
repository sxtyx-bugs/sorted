<div align="center">

# **sorted.**

### **Safe, Preview-First Desktop File Organizer & DAA Algorithm Suite**

*Native C++17 & Qt 6 Widgets Desktop Application for macOS & Windows 11*

[![macOS](https://img.shields.io/badge/macOS-Apple%20Silicon%20%7C%20Intel-000000?style=flat-square&logo=apple)](https://github.com/satya/SORTED)
[![Windows 11](https://img.shields.io/badge/Windows-11%20x64-0078D4?style=flat-square&logo=windows)](https://github.com/satya/SORTED)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-00599C?style=flat-square&logo=c%2B%2B)](https://en.cppreference.com/w/cpp/17)
[![Qt 6](https://img.shields.io/badge/GUI-Qt%206%20Widgets-41CD52?style=flat-square&logo=qt)](https://www.qt.io/)
[![CI/CD](https://img.shields.io/badge/Build-GitHub%20Actions-2088FF?style=flat-square&logo=githubactions)](.github/workflows/release.yml)
[![Safety](https://img.shields.io/badge/Safety-Zero%20Overwrite%20Policy-2E7D32?style=flat-square)](#-safety-architecture--root-only-scope-policy)
[![License](https://img.shields.io/badge/License-MIT-gray?style=flat-square)](LICENSE)

[**Installation**](#-installation) • [**Architecture**](#-architecture--design-system) • [**DAA Algorithms**](#-daa-algorithms--complexity) • [**Build Instructions**](#-building-from-source) • [**CI/CD Pipeline**](#-automated-github-actions-release-pipeline) • [**Safety Policy**](#-safety-architecture--root-only-scope-policy)

---

</div>

## 📌 Executive Summary

**SORTED** is a high-performance, native desktop productivity application engineered to organize cluttered directories (such as `Downloads/` or `Desktop/`) without the destructive risks of traditional file clean-up utilities. 

Built on a foundation of classic **Design and Analysis of Algorithms (DAA)** principles—including **stable Divide-and-Conquer Merge Sort**, **two-stage streaming SHA-256 duplicate detection**, and **deterministic greedy collision planning**—SORTED enforces a strict **preview-first** architecture:
* **Root-Only Scope:** Only direct child files of the selected workspace are eligible for organization. Existing subdirectories are strictly protected and never recursed into or modified.
* **Preview-First Execution:** No file is ever moved without user review and explicit approval.
* **Zero-Overwrite Policy:** Existing destination files are never overwritten. Path collisions are resolved with clean incremental suffixes (`report (1).pdf`).
* **Non-Destructive Duplicates:** Exact duplicate files are detected byte-for-byte and never deleted automatically.
* **Persistent Journaling:** Every move is journaled in `.sorted_journal.json` (with automatic migration to per-user `QStandardPaths::AppDataLocation`) and can be reverted with a single click.

---

## 📦 Installation

### macOS (Apple Silicon & Intel)
1. Download **`SORTED-1.0.0-macOS-arm64.dmg`** (for M1/M2/M3/M4 Macs) or **`SORTED-1.0.0-macOS-x86_64.dmg`** (for Intel Macs) from [GitHub Releases](https://github.com/satya/SORTED/releases).
2. Double-click to mount the disk image.
3. Drag **`SORTED.app`** into your **`Applications`** folder.
4. Launch **SORTED** from Launchpad or Finder.

> [!NOTE]
> For unsigned preview builds on macOS, right-click `SORTED.app` and choose **Open**, then confirm the prompt, or run `xattr -cr /Applications/SORTED.app`. Fully notarized production builds pass Gatekeeper automatically.

### Windows 11 / 10 (64-bit)
1. Download **`SORTED-1.0.0-windows-x64-Setup.exe`** or the portable archive **`SORTED-1.0.0-windows-x64.zip`** from [GitHub Releases](https://github.com/satya/SORTED/releases).
2. Run the Setup Wizard to install SORTED with Start Menu and Desktop shortcuts, or extract the portable ZIP and run `SORTED.exe`.

---

## 🎨 Architecture & Design System

SORTED pairs high-performance native C++17 logic with an Apple Liquid Glass-inspired visual interface:

```
┌────────────────────────────────────────────────────────────────────────────────────────┐
│  ● ● ●  SORTED — Safe Desktop File Organizer (Qt 6)              [Ready] [🌙 Theme] [Scan]  │
├───────────────┬────────────────────────────────────────────────────────────────────────┤
│ sorted.  [◀]  │  Overview                                           Merge Sort: O(n log n)│
│ Preview first │  Workspace metrics, storage footprint, and category distribution       │
│               ├────────────────────────────────────────────────────────────────────────┤
│ ⊞ Overview    │  ┌──────────────┐ ┌──────────────┐ ┌──────────────┐ ┌──────────────┐   │
│ ▤ All Files   │  │ TOTAL FILES  │ │ TOTAL VOLUME │ │ CATEGORIES   │ │ DUPLICATES   │   │
│ 📁 Categories │  │ 100          │ │ 4.82 MB      │ │ 8            │ │ 3 groups     │   │
│ ⧉ Duplicates │  └──────────────┘ └──────────────┘ └──────────────┘ └──────────────┘   │
│ ⚡ Organize    │                                                                        │
│ ⏱ History     │  Category Distribution (44px row heights, high-contrast typography)    │
│               │  ┌───────────────────────┬────────────┬──────────────┬───────────────┐ │
│ Target:       │  │ Category              │ Files      │ Volume       │ % Share       │ │
│ ~/Downloads   │  ├───────────────────────┼────────────┼──────────────┼───────────────┤ │
│ [Change]      │  │ 📄 Documents          │ 24         │ 1.2 MB       │ 24.0%         │ │
│               │  │ 🖼️ Images             │ 20         │ 1.8 MB       │ 20.0%         │ │
│ 🌙 Dark Mode  │  │ 💻 Code               │ 18         │ 240 KB       │ 18.0%         │ │
└───────────────┴──┴───────────────────────┴────────────┴──────────────┴───────────────┴─┘
```

### Key UI Features
* **Restrained Monochrome Palette:** Apple-inspired neutrals (`#F7F7F5` canvas, `#FFFFFF` surfaces, `#1D1D1F` charcoal typography, `#86868B` secondary text, `#E5E5E5` borders).
* **Fully Inspectable Paths:** Organization and review tables provide horizontally scrollable, expansive current and destination path columns with hover tooltips and instant full-path copy.
* **Liquid Glass Components:** Reusable Qt Widgets (`GlassPanel`, `GlassSidebar`, `GlassToolbar`, `GlassButton`, `GlassSearchField`, `GlassDialog`) featuring native Windows 11 DWM Mica/Acrylic backdrops and graceful macOS fallbacks.
* **Collapsible Sidebar:** Animated toggle between 240px expanded navigation and 68px compact icon bar.

---

## 📂 Production Repository Structure

```text
SORTED/
├── CMakeLists.txt                    # Multi-platform CMake 3.21+ build configuration
├── CMakePresets.json                 # Standardized CMake build & test presets
├── README.md                         # Release, installation, and architecture guide
├── LICENSE                           # MIT License
├── CHANGELOG.md                      # Release changelog
├── .gitignore                        # Release-ready ignore rules
├── .editorconfig                     # Cross-platform editor formatting
├── cmake/
│   ├── Dependencies.cmake            # Qt 6 / Qt 5 discovery and link configurations
│   └── Packaging.cmake               # macOS bundle & CPack release definitions
├── src/                              # Consolidated C++17 Application Source
│   ├── main.cpp                      # Native application entrypoint
│   ├── core/                         # DAA algorithms & scanning engines
│   │   ├── FileScanner.h / .cpp      # Root-only discovery & guardrails
│   │   ├── FileClassifier.h / .cpp   # Semantic taxonomy classification
│   │   ├── DuplicateDetector.h / .cpp# Streaming SHA-256 duplicate verification
│   │   ├── OrganizationPlanner.h/.cpp# Collision-free greedy destination planner
│   │   └── MergeSort.h               # Stable Divide-and-Conquer Merge Sort
│   ├── models/                       # Domain data transfer objects
│   │   ├── FileRecord.h
│   │   ├── MoveOperation.h
│   │   └── ScanResult.h
│   ├── operations/                   # Operation journal and execution service
│   │   ├── FileOperationService.h/.cpp
│   │   └── OperationJournal.h/.cpp   # Thread-safe persistent JSON journal
│   └── ui/                           # Qt 6 Widgets user interface
│       ├── MainWindow.h / .cpp       # Main window frame & state machine
│       ├── OverviewPage.h / .cpp     # Workspace metrics & telemetry
│       ├── FilesPage.h / .cpp        # 44px file catalogue table
│       ├── CategoriesPage.h / .cpp   # Folder taxonomy grid
│       ├── DuplicatesPage.h / .cpp   # Duplicate content inspector
│       ├── OrganizePage.h / .cpp     # Inspectable move review table
│       ├── HistoryPage.h / .cpp      # Operation audit trail & instant undo
│       ├── Theme.h / .cpp            # Apple monochrome design tokens
│       └── glass/                    # Liquid Glass component system
├── resources/                        # Bundled application icons & assets
│   ├── icons/
│   │   ├── SORTED.png                # 1024x1024 master icon
│   │   ├── SORTED.icns               # Multi-resolution Apple icon bundle
│   │   ├── SORTED.ico                # Multi-resolution Windows icon bundle
│   │   └── SORTED.rc                 # Windows PE version and icon resource
│   └── resources.qrc                 # Qt resource XML definition
├── tests/                            # Automated verification test suites
│   ├── CMakeLists.txt                # C++ test targets
│   ├── cpp/                          # C++ algorithm & unit tests
│   ├── python/                       # Full pipeline & UI regression tests
│   └── fixtures/                     # Isolated test directory fixtures
├── benchmarks/                       # Empirical DAA benchmarks & synthetic data
├── docs/                             # Engineering specifications & design manuals
├── packaging/                        # Packaging assets and specifications
│   ├── macos/
│   │   ├── Info.plist.in             # macOS bundle property list template
│   │   ├── entitlements.plist        # Apple hardened runtime entitlements
│   │   └── create_dmg.sh             # DMG creation script with macdeployqt
│   └── windows/
│       ├── installer.iss             # Inno Setup 64-bit installer script
│       └── package_windows.ps1       # Automated windeployqt packaging script
├── scripts/                          # Platform build & packaging helper scripts
│   ├── build-macos.sh                # macOS native build runner
│   ├── build-windows.ps1             # Windows build runner
│   └── package-macos.sh              # macOS packaging runner
└── .github/
    └── workflows/
        └── release.yml               # Multi-platform CI/CD release workflow
```

---

## 🛠️ Building from Source

### macOS (Apple Silicon / Intel)

Requirements: macOS 12.0+, Xcode Command Line Tools, CMake 3.21+, Qt 6.6+ (`brew install qt@6 ninja`).

```bash
# Using helper script
chmod +x scripts/build-macos.sh scripts/package-macos.sh packaging/macos/create_dmg.sh
./scripts/build-macos.sh build-macos Release arm64

# Package DMG
./scripts/package-macos.sh build-macos dist/macos arm64
```

Or using CMake directly:
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="$(brew --prefix qt@6)"
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

### Windows 11 (64-bit)

Requirements: Windows 10/11, CMake 3.21+, MinGW (g++ 13+) or MSVC 2022, Qt 6.6+.

```powershell
# Build and run tests using helper script
.\scripts\build-windows.ps1 -BuildDir build -Config Release

# Build and assemble full installer (requires windeployqt and Inno Setup)
.\scripts\build-windows.ps1 -BuildDir build -Config Release -Package
```

### Standalone C++ Algorithm Verification (No Qt SDK Required)

To verify the core DAA algorithms on a workstation without Qt:
```powershell
.\run_cpp_test.bat
```
Output:
```text
============================================
  SORTED --- C++17 Core Verification Suite   
============================================
[Test 1/5] Running DAA MergeSort verification... PASSED!
[Test 2/5] Running FileClassifier rules verification... PASSED!
[Test 3/5] Running OrganizationPlanner collision handling... PASSED!
[Test 4/5] Running FileScanner root-only discovery... PASSED! Discovered 7 root files, preserved 3 protected folders.
[Test 5/5] Running Guardrail 1 planner validation for nested files... PASSED! Nested file candidate was successfully rejected.
============================================
  ALL C++17 VERIFICATION TESTS PASSED!      
============================================
```

---

## 🚀 Automated GitHub Actions Release Pipeline

The remote CI/CD workflow (`.github/workflows/release.yml`) builds, tests, and packages release binaries across macOS and Windows simultaneously:

* **Triggering a Release:**
  * Push a version tag: `git tag v1.0.0 && git push origin v1.0.0`
  * Or trigger manually from GitHub UI: **Actions** -> **SORTED Production Build & Multi-Platform Release** -> **Run workflow**.

* **Matrix of Jobs:**
  1. **`build-macos-arm64` (`macos-14` runner):** Compiles native Apple Silicon `arm64` binary, runs verification tests, executes `macdeployqt`, packages `SORTED-1.0.0-macOS-arm64.dmg`, and uploads artifact.
  2. **`build-macos-x86_64` (`macos-13` runner):** Compiles native Intel `x86_64` binary, runs tests, executes `macdeployqt`, packages `SORTED-1.0.0-macOS-x86_64.dmg`, and uploads artifact.
  3. **`build-windows` (`windows-latest` runner):** Compiles 64-bit MSVC binary, runs tests, runs `windeployqt`, compiles Inno Setup installer `SORTED-1.0.0-windows-x64-Setup.exe` and portable ZIP.
  4. **`publish-release`:** Collects all artifacts and publishes a unified GitHub Release.

### Code Signing & Apple Notarization
To enable official Apple Developer ID signing and Gatekeeper notarization in CI:
* Set GitHub Secrets:
  * `MACOS_CERTIFICATE_NAME`: Developer ID Application identity.
  * `APPLE_ID`: Apple developer email.
  * `APPLE_APP_SPECIFIC_PASSWORD`: App-specific password from appleid.apple.com.
  * `APPLE_TEAM_ID`: 10-character Apple Developer Team ID.

When secrets are omitted, the workflow cleanly generates unsigned preview DMGs ready for testing and distribution.

---

## 🛡️ Safety Architecture & Root-Only Scope Policy

1. **Root-Level Isolation:** Only direct child files of the selected workspace are processed. Existing subdirectories are strictly protected.
2. **Preview-First Execution:** Files are only moved after the user inspects the proposed destination and clicks "Apply Approved Moves".
3. **In-Memory Collision Reservation:** Colliding file names automatically receive deterministic numeric suffixes (`file (1).ext`) to ensure zero file loss.
4. **Pre-Move Disk Verification:** The filesystem is revalidated immediately before each file is moved.
5. **Atomic Reversible Operations:** Moves are performed via atomic filesystem operations and recorded in `.sorted_journal.json` for one-click rollback.

---

## 📄 License

Distributed under the **MIT License**. See [`LICENSE`](LICENSE) for details.
