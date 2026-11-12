#pragma once

#include <QWidget>
#include <QString>

namespace Ui {
namespace Glass {

enum class BackdropMaterial {
    None,       // Classic opaque surface (fallback)
    Mica,       // Windows 11 Mica (desktop wallpaper blended)
    Acrylic,    // Windows 11 Acrylic (active behind-window blur)
    MicaAlt     // Windows 11 Tabbed Mica Alt
};

/**
 * @brief WindowBackdrop manages native Windows 11 Desktop Window Manager (DWM)
 * system backdrop materials (Mica, Acrylic, Mica Alt) with proper version checks
 * and graceful fallback on unsupported operating systems.
 */
class WindowBackdrop {
public:
    /**
     * @brief Checks if the running OS is Windows 11 with DWM backdrop support.
     * @return true if Windows 11 Build 22000 or newer.
     */
    static bool isSupported();

    /**
     * @brief Checks if Windows 11 22H2 (Build 22621) or newer is available,
     * which introduced the unified DWMWA_SYSTEMBACKDROP_TYPE attribute.
     */
    static bool isBuild22621OrNewer();

    /**
     * @brief Retrieves the current Windows build number.
     */
    static unsigned long getWindowsBuildNumber();

    /**
     * @brief Applies the selected native backdrop material to the target widget window.
     * @param window Top-level QWidget / QMainWindow.
     * @param material Mica, Acrylic, MicaAlt, or None.
     * @param isDarkMode true to set dark mode window title bar and material tint.
     * @return true if successfully applied.
     */
    static bool apply(QWidget* window, BackdropMaterial material, bool isDarkMode);

    /**
     * @brief Updates the immersive dark mode attribute for the window frame.
     */
    static bool setDarkMode(QWidget* window, bool isDarkMode);
};

} // namespace Glass
} // namespace Ui
