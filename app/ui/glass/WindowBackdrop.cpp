#include "WindowBackdrop.h"

#ifdef Q_OS_WIN
#include <windows.h>
#include <dwmapi.h>

#ifndef DWMWA_USE_IMMERSIVE_DARK_MODE
#define DWMWA_USE_IMMERSIVE_DARK_MODE 20
#endif

#ifndef DWMWA_MICA_EFFECT
#define DWMWA_MICA_EFFECT 1029
#endif

#ifndef DWMWA_SYSTEMBACKDROP_TYPE
#define DWMWA_SYSTEMBACKDROP_TYPE 38
#endif

#ifndef DWMSBT_AUTO
#define DWMSBT_AUTO 0
#define DWMSBT_NONE 1
#define DWMSBT_MAINWINDOW 2
#define DWMSBT_TRANSIENTWINDOW 3
#define DWMSBT_TABBEDWINDOW 4
#endif

typedef LONG(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);

#endif // Q_OS_WIN

namespace Ui {
namespace Glass {

unsigned long WindowBackdrop::getWindowsBuildNumber() {
#ifdef Q_OS_WIN
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (hNtdll) {
        RtlGetVersionPtr pRtlGetVersion = (RtlGetVersionPtr)GetProcAddress(hNtdll, "RtlGetVersion");
        if (pRtlGetVersion) {
            RTL_OSVERSIONINFOW rovi;
            ZeroMemory(&rovi, sizeof(rovi));
            rovi.dwOSVersionInfoSize = sizeof(rovi);
            if (pRtlGetVersion(&rovi) == 0) {
                return rovi.dwBuildNumber;
            }
        }
    }
#endif
    return 0;
}

bool WindowBackdrop::isSupported() {
#ifdef Q_OS_WIN
    // Windows 11 initial build is 22000
    return getWindowsBuildNumber() >= 22000;
#else
    return false;
#endif
}

bool WindowBackdrop::isBuild22621OrNewer() {
#ifdef Q_OS_WIN
    return getWindowsBuildNumber() >= 22621;
#else
    return false;
#endif
}

bool WindowBackdrop::setDarkMode(QWidget* window, bool isDarkMode) {
#ifdef Q_OS_WIN
    if (!window) return false;
    HWND hwnd = reinterpret_cast<HWND>(window->winId());
    if (!hwnd) return false;

    BOOL darkValue = isDarkMode ? TRUE : FALSE;
    HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &darkValue, sizeof(darkValue));
    if (FAILED(hr)) {
        // Fallback for earlier Windows 10/11 preview builds
        DwmSetWindowAttribute(hwnd, 19, &darkValue, sizeof(darkValue));
    }
    return SUCCEEDED(hr);
#else
    Q_UNUSED(window);
    Q_UNUSED(isDarkMode);
    return false;
#endif
}

bool WindowBackdrop::apply(QWidget* window, BackdropMaterial material, bool isDarkMode) {
#ifdef Q_OS_WIN
    if (!window) return false;
    HWND hwnd = reinterpret_cast<HWND>(window->winId());
    if (!hwnd) return false;

    // Graceful fallback on unsupported systems
    if (!isSupported() || material == BackdropMaterial::None) {
        window->setAttribute(Qt::WA_TranslucentBackground, false);
        setDarkMode(window, isDarkMode);
        return false;
    }

    // Enable translucent backing store in Qt
    window->setAttribute(Qt::WA_TranslucentBackground, true);

    // Extend window frame into full client area
    MARGINS margins = {-1, -1, -1, -1};
    DwmExtendFrameIntoClientArea(hwnd, &margins);

    // Apply dark mode title bar
    setDarkMode(window, isDarkMode);

    unsigned long build = getWindowsBuildNumber();

    // Windows 11 Build 22621+ (22H2+): Unified DWMWA_SYSTEMBACKDROP_TYPE
    if (build >= 22621) {
        int backdropValue = DWMSBT_AUTO;
        switch (material) {
            case BackdropMaterial::Mica:
                backdropValue = DWMSBT_MAINWINDOW;
                break;
            case BackdropMaterial::Acrylic:
                backdropValue = DWMSBT_TRANSIENTWINDOW;
                break;
            case BackdropMaterial::MicaAlt:
                backdropValue = DWMSBT_TABBEDWINDOW;
                break;
            default:
                backdropValue = DWMSBT_NONE;
                break;
        }

        HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_SYSTEMBACKDROP_TYPE, &backdropValue, sizeof(backdropValue));
        return SUCCEEDED(hr);
    }
    // Windows 11 Build 22000 (21H2): Undocumented Mica attribute
    else if (build >= 22000) {
        if (material == BackdropMaterial::Mica || material == BackdropMaterial::MicaAlt) {
            BOOL trueValue = TRUE;
            HRESULT hr = DwmSetWindowAttribute(hwnd, DWMWA_MICA_EFFECT, &trueValue, sizeof(trueValue));
            return SUCCEEDED(hr);
        } else if (material == BackdropMaterial::Acrylic) {
            // Accent blur fallback on Build 22000
            return true;
        }
    }

    return false;
#else
    Q_UNUSED(window);
    Q_UNUSED(material);
    Q_UNUSED(isDarkMode);
    return false;
#endif
}

} // namespace Glass
} // namespace Ui
