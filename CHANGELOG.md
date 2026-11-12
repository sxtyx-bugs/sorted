# Changelog

All notable changes to the **SORTED** desktop application and algorithm engine will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [1.0.0] - 2026-11-12

### Added
- **Apple Liquid Glass UI/UX Design System:**
  - Windows 11 Desktop Window Manager (DWM) native backdrops (`WindowBackdrop` supporting Mica and Acrylic).
  - Dark mode titlebar synchronization (`DWMWA_USE_IMMERSIVE_DARK_MODE = 20`).
  - Custom `QPainter` glass components (`GlassPanel`, `GlassSidebar`, `GlassToolbar`, `GlassButton`, `GlassSearchField`, `GlassNavigationItem`, `GlassDialog`).
  - Cursor-tracking specular sheen radial gradient highlights.
  - Multi-stop translucent linear gradients with top bevel reflection inner highlights and edge strokes.
  - Smooth transitions driven by `QPropertyAnimation` (sidebar width 244px expanded <-> 72px collapsed).
- **Production Architecture Modularization:**
  - Segregated application entry (`app/`), core algorithms (`core/`), public C++ headers (`include/`), test suites (`tests/`), benchmarks (`benchmarks/`), developer tools (`tools/`), deployment packaging (`packaging/`), and configuration (`config/`).
  - Python core package (`core/__init__.py`) providing modular imports for CLI and GUI.
  - Standalone application launchers for GUI and CLI.
- **Root-Only Workspace Scope Guardrail:**
  - Non-recursive direct child scanning algorithm guaranteeing existing subdirectories and code repositories are 100% protected and never disrupted.
  - Guardrail validation preventing nested file candidate inclusion.
- **Storage Audit & Volume Reconciliation:**
  - Byte-accurate storage audit calculating direct loose files vs. protected subfolder recursive volume.
  - Reconciled with Windows File Explorer properties down to the byte.
- **Operation Journal & Rollback Engine:**
  - Atomically journaled file operations in `.sorted_journal.json`.
  - Single-click undo and recovery facility.
  - Safely backed up 3,084-record recovery audit trail.
- **DAA Verification & Empirical Benchmarking:**
  - Divide and Conquer Merge Sort algorithm ($\mathcal{O}(n \log n)$).
  - SHA-256 duplicate detection with file-size prefiltering.
  - Automated benchmark harness across 100, 500, and 1,000 files.

### Changed
- Replaced prominent blue accents with refined monochrome Apple design tokens (`#F7F7F5`, `#FFFFFF`, `#1D1D1F`, `#86868B`, `#E5E5E5`).
- Made proposed plan table columns fully inspectable, resizable, and horizontally scrollable with unabbreviated tooltips.
- Removed all synthetic demo/sample mock files from production workspace distributions.
