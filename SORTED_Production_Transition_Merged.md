# SORTED --- Production Transition & Native macOS Release

## Objective

We are officially ending development/test-suite mode. The current UI is
good enough. Do not spend more time implementing Liquid Glass or
redesigning the interface.

Turn SORTED into a real, distributable desktop application for macOS,
with a clean production repository and a one-click installation
experience, while maintaining Windows support.

## Development Environment: Windows Only

The developer works entirely on Windows 11 and does not have a Mac or
Xcode installed. Do not ask the developer to install Xcode on Windows or
attempt to compile a macOS binary locally.

Use GitHub Actions with GitHub-hosted macOS runners for native macOS
builds:

1.  Develop and test on Windows.
2.  Push the source repository to GitHub.
3.  Trigger GitHub Actions manually or through a version tag.
4.  A GitHub-hosted macOS runner installs/selects Xcode tools, Qt and
    dependencies.
5.  The runner configures CMake, compiles SORTED, bundles dependencies
    and creates a DMG.
6.  GitHub Actions uploads the artifacts.

Select currently supported macOS runner labels from GitHub's official
documentation. Do not assume a runner label or architecture is available
without checking. Use separate Apple Silicon and Intel jobs where
supported. Start with one supported architecture if dependencies make
multi-architecture builds impractical.

A successful remote build does not replace testing on a real Mac.
Arrange at least one actual macOS launch and functionality test before
public release.

## Existing Technology

-   C++17
-   Qt 6 Widgets
-   CMake
-   `std::filesystem`
-   Qt Concurrent
-   `QCryptographicHash`
-   `QSaveFile`
-   JSON operation journal

Preserve the existing application and working functionality. Do not
rewrite it unnecessarily. First inspect the repository and verify that
these technologies match the actual code.

------------------------------------------------------------------------

# PHASE 1 --- Audit the Entire Project

Before changing anything:

1.  Inspect the complete repository.
2.  Identify the actual application entry point.
3.  Identify all source files and dependencies.
4.  Inspect `CMakeLists.txt` and build configurations.
5.  Identify test suites, demo data, temporary files, generated
    binaries, build directories and unused assets.
6.  Identify application resources, icons, fonts and licenses.
7.  Verify operation journal and settings storage locations.
8.  Identify platform-specific Windows assumptions.
9.  Identify hardcoded paths, environment assumptions and
    current-working-directory dependencies.
10. Determine the exact Qt version, modules, plugins and third-party
    libraries required at runtime.
11. Verify whether dependencies support the intended macOS
    architectures.

Produce a concise inventory showing files to retain, reorganize,
regenerate or exclude from release packages, plus risks and dependencies
requiring migration.

**Do not delete or move anything during the audit.** Back up important
project data before later restructuring. Preserve `sorted_journal.json`,
settings, operation history and test datasets until their purpose and
dependencies are understood.

# PHASE 2 --- Production Repository Structure

Refactor toward this structure, adapting it to the real code:

``` text
SORTED/
├── CMakeLists.txt
├── CMakePresets.json
├── README.md
├── LICENSE
├── CHANGELOG.md
├── .gitignore
├── .editorconfig
├── cmake/
│   ├── Dependencies.cmake
│   └── Packaging.cmake
├── src/
│   ├── main.cpp
│   ├── app/
│   ├── core/
│   ├── application/
│   ├── infrastructure/
│   └── ui/
├── resources/
│   ├── icons/
│   ├── images/
│   ├── fonts/
│   └── resources.qrc
├── tests/
│   ├── unit/
│   ├── integration/
│   └── CMakeLists.txt
├── benchmarks/
│   └── reports/
├── docs/
├── packaging/
│   ├── macos/
│   └── windows/
├── scripts/
│   ├── build-macos.sh
│   ├── build-windows.ps1
│   └── package-macos.sh
└── .github/
    └── workflows/
        └── release.yml
```

Do not create empty folders just to match the example. Keep useful test
and benchmark infrastructure in the source repository, but exclude
development-only files and test datasets from production distributions.
Do not commit generated build directories or compiled test executables
without a specific reason.

Update CMake paths, scripts, tests, documentation and resource paths
after migration. Use robust paths rather than relying on the repository
being the current working directory.

**Never delete actual user files or application history.** Do not reset
or overwrite the operation journal.

# PHASE 3 --- Native macOS Application Bundle

Create a proper macOS application bundle:

``` text
SORTED.app
```

Requirements:

-   Native Finder application bundle.
-   Correct `Info.plist`.
-   Application name: SORTED.
-   Configurable bundle identifier, default `com.satydev.sorted`.
-   Version and build number.
-   High-quality `.icns` application icon.
-   Correct application menu and window behaviour.
-   Appropriate macOS keyboard shortcuts.
-   Native file and folder selection dialogs.
-   Native open/save behaviour where appropriate.
-   High-DPI support.
-   No console window on normal launch.
-   Correct application activation, closing and reopening behaviour.

Support Apple Silicon `arm64` and Intel `x86_64`. Prefer Universal 2
only if Qt and every required dependency support both architectures and
the combined binary is verified.

Use the existing C++ and Qt application. Do not convert it into
Electron, Python or a web application.

# PHASE 4 --- Bundle All Runtime Dependencies

The application must work on a Mac that does not have Qt or a
development environment installed.

-   Bundle required Qt frameworks and plugins.
-   Include required application resources and third-party runtime
    dependencies.
-   Resolve dynamic library paths correctly.
-   Verify plugin loading.
-   Inspect linked libraries and runtime dependencies.
-   Avoid developer-specific absolute paths.
-   Verify the deployment target and minimum supported macOS version.

Use `macdeployqt` where appropriate for the selected Qt version and
packaging workflow. Do not ship debug builds, test datasets or
development-only dependencies.

# PHASE 5 --- macOS Installer

Produce:

``` text
dist/macos/SORTED.app
dist/macos/SORTED.dmg
```

The DMG should contain `SORTED.app`, an Applications shortcut for
drag-and-drop installation, and a clean, simple installer presentation.

Expected user experience:

1.  Download `SORTED.dmg`.
2.  Open the DMG.
3.  Drag SORTED into Applications.
4.  Launch SORTED from Finder or Launchpad.

No terminal commands, source folders, Python installation or manual Qt
dependency installation should be required.

# PHASE 6 --- Application Data and Safety

The application must not store mutable data inside its application
bundle or installation directory. Use `QStandardPaths` and suitable
per-user locations.

On macOS:

-   Settings: `~/Library/Application Support/SORTED/`
-   Operation journal and history:
    `~/Library/Application Support/SORTED/`
-   Logs: `~/Library/Logs/SORTED/`
-   Temporary files: appropriate system temporary directory.

On Windows, use appropriate per-user application data and log locations.

Preserve workspace preferences, operation history, undo information,
settings and user-selected folder paths. Do not silently reset,
overwrite or discard existing history during migration or upgrades. The
app must never assume its installation directory is writable.

## Mandatory file-organization safety restrictions

-   Only direct child files of the selected workspace are eligible for
    organization.
-   Existing subdirectories are protected.
-   Never recursively reorganize the contents of existing
    subdirectories.
-   Never move, rename, delete or empty existing subdirectories.
-   Exclude the application's own output directory from eligible inputs.
-   Never delete duplicate files automatically.
-   Require explicit user approval before moving files.
-   Revalidate source and destination paths immediately before each
    operation.
-   Never overwrite an existing destination.
-   Preserve source files if a copy, verification or move fails.
-   Keep operation journaling and undo/recovery safeguards intact.

Packaging, installation and release workflows must never run
organization operations against a user's Downloads folder or another
personal workspace.

# PHASE 7 --- Cross-Platform Support

Maintain Windows support and produce separate native artifacts.

## Windows

-   `SORTED.exe`
-   Required Qt DLLs and plugins.
-   Required runtime resources.
-   Prefer a standalone installer or packaged distribution.
-   Build using a Windows GitHub Actions runner.
-   Verify the package on a clean Windows environment without a Qt
    development installation.

## macOS

-   `SORTED.app`
-   `SORTED.dmg`
-   Bundle required Qt frameworks, plugins and runtime dependencies.
-   Build natively on a macOS runner.

Do not mix platform-specific binaries or libraries. Keep shared
application logic in the existing core.

# PHASE 8 --- Automated Build Pipeline from Windows

Create:

``` text
.github/workflows/release.yml
```

Support manual workflow dispatch and builds triggered by version tags
such as `v1.0.0`. Use independent Windows and macOS jobs, clear logs,
reproducible dependency versions and artifact uploads.

## macOS jobs

Run on currently supported GitHub-hosted macOS runners:

1.  Check out the repository.
2.  Install/select the required Xcode toolchain available on the runner.
3.  Install the pinned Qt version and required modules.
4.  Configure CMake for Release.
5.  Build the application.
6.  Run automated tests.
7.  Bundle Qt dependencies with `macdeployqt` where appropriate.
8.  Verify the app bundle, linked libraries and Qt plugins.
9.  Generate the DMG.
10. Upload the `.app` and `.dmg` as workflow artifacts.

Start with a single architecture if necessary. Add the second only after
confirming dependency compatibility. Do not claim Universal 2 support
until both architectures are verified.

## Windows job

1.  Use a Windows GitHub-hosted runner.
2.  Install/select the pinned compiler and Qt version.
3.  Configure CMake for Release.
4.  Build the application.
5.  Run automated tests.
6.  Deploy required Qt dependencies.
7.  Generate a Windows release package.
8.  Upload the package as a workflow artifact.

Pin dependency versions and make builds reproducible. Never commit
signing certificates, passwords, API keys or other secrets.

The developer must be able to trigger this pipeline from GitHub while
continuing to work entirely on Windows.

# PHASE 9 --- Code Signing and Notarization

Prepare macOS distribution for:

-   Developer ID Application signing.
-   Hardened Runtime.
-   Secure timestamps.
-   Apple notarization.
-   Stapling the notarization ticket.

Use GitHub Actions secrets or another secure secret store for signing
credentials. Never commit certificates, private keys, passwords or API
keys.

If Apple Developer credentials are unavailable, produce a clearly
labelled unsigned development build and document the exact remaining
signing and notarization steps. Do not claim the application is signed
or notarized. A signed and notarized public release requires appropriate
Apple Developer credentials and successful verification by Apple's
services.

# PHASE 10 --- Production Quality Assurance

Test the actual packaged application, not only the development build.

Verify:

-   Fresh installation and launch without Qt installed.
-   Correct application icon and name.
-   Workspace selection.
-   File scanning and root-level-only scan behaviour.
-   Existing subdirectories remain untouched.
-   Organization preview and explicit approval before moving.
-   File collision handling.
-   Undo and history persistence.
-   Restart and recovery behaviour.
-   Light and dark themes.
-   Settings persistence.
-   Closing and reopening.
-   Upgrade and uninstall/reinstall behaviour.
-   Missing permissions and inaccessible folders.
-   Large files and large directory listings.
-   Required Qt plugin loading.
-   No missing dynamic libraries.
-   No dependency on the source repository or build directory.
-   No unexpected console window or terminal requirement.

Use isolated test fixtures and temporary directories. Do not use actual
user Downloads contents as test fixtures.

A successful CI compilation is not sufficient proof of production
readiness. Arrange at least one actual macOS launch and functionality
test before public release.

# PHASE 11 --- Final Deliverables

Provide:

1.  Clean production repository.
2.  Updated CMake configuration.
3.  CMake presets where appropriate.
4.  macOS build scripts.
5.  Windows build scripts.
6.  GitHub Actions workflow.
7.  `SORTED.app`.
8.  `SORTED.dmg`.
9.  Windows release package.
10. README with installation and developer build instructions.
11. Release checklist.
12. Known limitations and uncompleted release requirements.
13. Build logs and test results for each produced artifact.

## Execution Rules

-   Inspect before editing.
-   Back up before moving or removing files.
-   Never delete user data.
-   Never reset the operation journal or history.
-   Never run file organization during packaging.
-   Do not redesign the UI or spend time implementing Liquid Glass.
-   Do not rewrite working algorithms or application architecture
    unnecessarily.
-   Do not fabricate a successful macOS build from a Windows
    environment.
-   If macOS compilation requires a Mac runner, configure and use GitHub
    Actions.
-   Do not declare production readiness until the packaged application
    has passed the required tests.
-   Report what was actually built, tested, signed and notarized,
    distinguishing completed work from pending work.

## Required Work Sequence

### Stage A --- Audit only

-   Inspect the repository.
-   Produce the file inventory and dependency map.
-   Identify platform-specific assumptions.
-   Present the proposed migration plan.
-   Make no destructive changes.

### Stage B --- Controlled repository migration

-   Back up important project data.
-   Restructure the repository.
-   Update references and build configuration.
-   Verify the Windows development build and existing tests.

### Stage C --- Remote macOS and Windows builds

-   Implement GitHub Actions.
-   Build and package the native applications.
-   Upload release artifacts.
-   Fix build and packaging errors based on actual logs.

### Stage D --- Release verification

-   Test the packaged builds.
-   Verify installation and persistent application data.
-   Document signing/notarization status.
-   Publish a versioned release only after the required checks pass.

**Start with Stage A. Do not skip the audit or begin deleting files.**
