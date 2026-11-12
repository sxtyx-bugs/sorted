#pragma once

#include <QString>
#include <QColor>
#include <QPalette>
#include <QFont>

namespace Ui {

enum class ColorTheme {
    Light,
    Dark
};

class Theme {
public:
    static ColorTheme currentTheme;

    // --- Font Families (Apple SF Pro with native Windows fallbacks) ---
    static const QString FontFamilyDisplay;
    static const QString FontFamilyText;
    static const QString FontFamilyMono;

    // --- Palette Constants (Monochrome Apple Design System) ---
    // Primary Action (Charcoal in Light, White in Dark)
    static const QString AccentCharcoal;    // #1D1D1F
    static const QString AccentHover;       // #3A3A3C
    static const QString AccentTintLight;   // #F1F1EF
    static const QString AccentTintDark;    // #333333
    static const QString InfoSurfaceLight;  // #F7F7F5
    static const QString InfoSurfaceDark;   // #222222

    // Light Theme Surfaces & Text
    static const QString LightBgCanvas;     // #F7F7F5 (Canvas)
    static const QString LightBgSidebar;    // #F2F2F0 (Sidebar)
    static const QString LightBgSurface;    // #FFFFFF (Surface)
    static const QString LightBgHover;      // #F1F1EF (Hover)
    static const QString LightBgSelected;   // #EBEBEB (Selected row background)
    static const QString LightTextPrimary;  // #1D1D1F (High-contrast charcoal)
    static const QString LightTextSecondary;// #86868B (Subtle metadata)
    static const QString LightTextMuted;    // #A1A1A6
    static const QString LightBorder;       // #E5E5E5
    static const QString LightBorderSubtle; // #EDEDE8

    // Dark Theme Surfaces & Text
    static const QString DarkBgCanvas;      // #191919 (Dark canvas)
    static const QString DarkBgSidebar;     // #202020 (Sidebar)
    static const QString DarkBgSurface;     // #262626 (Cards / tables)
    static const QString DarkBgHover;       // #2F2F2F
    static const QString DarkBgSelected;    // #383838
    static const QString DarkTextPrimary;   // #F5F5F7
    static const QString DarkTextSecondary; // #A1A1A6
    static const QString DarkTextMuted;     // #6E6E73
    static const QString DarkBorder;        // #3A3A3C
    static const QString DarkBorderSubtle;  // #303032

    // Semantic Status Colors (Restrained Monochrome)
    static const QString StatusSuccess;     // #1D1D1F
    static const QString StatusWarning;     // #48484A
    static const QString StatusDanger;      // #D32F2F

    // Liquid Glass Translucent RGBA definitions
    static const QString GlassLightCanvas;
    static const QString GlassLightSidebar;
    static const QString GlassLightSurface;
    static const QString GlassLightCard;
    static const QString GlassLightBorder;
    static const QString GlassDarkCanvas;
    static const QString GlassDarkSidebar;
    static const QString GlassDarkSurface;
    static const QString GlassDarkCard;
    static const QString GlassDarkBorder;

    // Helpers
    static void applyTheme(ColorTheme theme);
    static void toggleTheme();
    static QPalette getPalette(ColorTheme theme);
    static QString getGlobalStyleSheet(ColorTheme theme);
    static QString getTableStyleSheet(ColorTheme theme);
    static QString getTreeStyleSheet(ColorTheme theme);
    static QString getSidebarStyleSheet(ColorTheme theme);
    static QString getCardStyleSheet(ColorTheme theme);
    static QString getBadgeStyleSheet(const QString& type, ColorTheme theme);

    static bool isDark() { return currentTheme == ColorTheme::Dark; }
    static QString getTextPrimary() { return isDark() ? DarkTextPrimary : LightTextPrimary; }
    static QString getTextSecondary() { return isDark() ? DarkTextSecondary : LightTextSecondary; }
    static QString getTextMuted() { return isDark() ? DarkTextMuted : LightTextMuted; }
    static QString getSurface() { return isDark() ? DarkBgSurface : LightBgSurface; }
    static QString getCanvas() { return isDark() ? DarkBgCanvas : LightBgCanvas; }
    static QString getBorder() { return isDark() ? DarkBorder : LightBorder; }
    static QString getHover() { return isDark() ? DarkBgHover : LightBgHover; }
};

} // namespace Ui
