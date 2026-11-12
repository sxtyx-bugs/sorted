#!/usr/bin/env bash
# ==============================================================================
# scripts/package-macos.sh — Bundle and package SORTED.app into SORTED.dmg
# ==============================================================================

set -euo pipefail

BUILD_DIR="${1:-build-macos}"
OUTPUT_DIR="${2:-dist/macos}"
ARCH="${3:-arm64}"

APP_BUNDLE="${BUILD_DIR}/SORTED.app"
DMG_NAME="SORTED-1.0.0-macOS-${ARCH}.dmg"
TARGET_DMG="${OUTPUT_DIR}/${DMG_NAME}"

echo "=== Packaging SORTED for macOS (${ARCH}) ==="
mkdir -p "${OUTPUT_DIR}"

if [ ! -d "${APP_BUNDLE}" ]; then
    echo "Error: ${APP_BUNDLE} not found. Run scripts/build-macos.sh first." >&2
    exit 1
fi

# Run create_dmg script
bash packaging/macos/create_dmg.sh "${APP_BUNDLE}" "${TARGET_DMG}"

echo "=== Packaging Complete: ${TARGET_DMG} ==="
