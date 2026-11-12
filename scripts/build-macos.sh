#!/usr/bin/env bash
# ==============================================================================
# scripts/build-macos.sh — Configure and compile SORTED natively on macOS
# ==============================================================================

set -euo pipefail

BUILD_DIR="${1:-build-macos}"
BUILD_TYPE="${2:-Release}"
ARCH="${3:-arm64}"

echo "=== Building SORTED for macOS (${ARCH}) ==="
echo "Build directory: ${BUILD_DIR}"
echo "Configuration:   ${BUILD_TYPE}"

cmake -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DCMAKE_OSX_ARCHITECTURES="${ARCH}" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cmake --build "${BUILD_DIR}" --config "${BUILD_TYPE}" --parallel

echo "=== Running Verification Tests ==="
ctest --test-dir "${BUILD_DIR}" --output-on-failure

echo "=== macOS Build Completed Successfully ==="
