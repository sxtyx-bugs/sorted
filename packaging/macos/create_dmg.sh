#!/usr/bin/env bash
# ==============================================================================
# packaging/macos/create_dmg.sh — Build standalone macOS DMG installer
# ==============================================================================

set -euo pipefail

APP_PATH="${1:-build/SORTED.app}"
OUTPUT_DMG="${2:-dist/macos/SORTED.dmg}"
VOLUME_NAME="SORTED"

echo "=== SORTED macOS DMG Packager ==="
echo "Application bundle: ${APP_PATH}"
echo "Output DMG target:  ${OUTPUT_DMG}"

if [ ! -d "${APP_PATH}" ]; then
    echo "Error: Application bundle not found at ${APP_PATH}" >&2
    exit 1
fi

mkdir -p "$(dirname "${OUTPUT_DMG}")"

# 1. Run macdeployqt to bundle required Qt frameworks and plugins
if command -v macdeployqt >/dev/null 2>&1; then
    echo "--> Running macdeployqt..."
    macdeployqt "${APP_PATH}" -always-overwrite || {
        echo "Warning: macdeployqt exited with non-zero status; proceeding to verify bundle."
    }
else
    echo "Notice: macdeployqt not found in PATH; skipping Qt auto-deployment."
fi

# 2. Code Signing (if identity provided via env var CODESIGN_IDENTITY)
if [ -n "${CODESIGN_IDENTITY:-}" ]; then
    echo "--> Signing application bundle with identity: ${CODESIGN_IDENTITY}"
    ENTITLEMENTS="$(dirname "$0")/entitlements.plist"
    codesign --deep --force --verify --verbose \
        --options runtime \
        --entitlements "${ENTITLEMENTS}" \
        --sign "${CODESIGN_IDENTITY}" \
        "${APP_PATH}"
else
    echo "Notice: CODESIGN_IDENTITY not set. Producing unsigned development/preview package."
fi

# 3. Create DMG structure
STAGING_DIR="$(mktemp -d -t sorted_dmg_staging_XXXXXX)"
echo "--> Staging DMG contents in ${STAGING_DIR}..."

cp -R "${APP_PATH}" "${STAGING_DIR}/SORTED.app"
ln -s /Applications "${STAGING_DIR}/Applications"

# 4. Generate disk image
rm -f "${OUTPUT_DMG}"

if command -v create-dmg >/dev/null 2>&1; then
    echo "--> Creating DMG with create-dmg..."
    create-dmg \
        --volname "${VOLUME_NAME}" \
        --window-pos 200 120 \
        --window-size 600 400 \
        --icon-size 100 \
        --icon "SORTED.app" 150 190 \
        --app-drop-link 450 190 \
        --hide-extension "SORTED.app" \
        --no-internet-enable \
        "${OUTPUT_DMG}" \
        "${STAGING_DIR}" || {
            echo "create-dmg failed, falling back to hdiutil..."
            hdiutil create -volname "${VOLUME_NAME}" -srcfolder "${STAGING_DIR}" -ov -format UDZO "${OUTPUT_DMG}"
        }
else
    echo "--> Creating DMG with native hdiutil..."
    hdiutil create -volname "${VOLUME_NAME}" -srcfolder "${STAGING_DIR}" -ov -format UDZO "${OUTPUT_DMG}"
fi

# Clean up staging directory
rm -rf "${STAGING_DIR}"

# 5. Notarization & Stapling (if credentials provided)
if [ -n "${APPLE_ID:-}" ] && [ -n "${APPLE_ID_PASSWORD:-}" ] && [ -n "${TEAM_ID:-}" ]; then
    echo "--> Submitting DMG for Apple Notarization..."
    xcrun notarytool submit "${OUTPUT_DMG}" \
        --apple-id "${APPLE_ID}" \
        --password "${APPLE_ID_PASSWORD}" \
        --team-id "${TEAM_ID}" \
        --wait
    
    echo "--> Stapling notarization ticket to DMG..."
    xcrun stapler staple "${OUTPUT_DMG}"
fi

echo "=== Successfully generated: ${OUTPUT_DMG} ==="
ls -lh "${OUTPUT_DMG}"
