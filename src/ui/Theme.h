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

    // --- Palette Constants (Apple x Notion with Cobalt Blue #3478F6) ---
    // Primary Accent
    static const QString AccentBlue;        // #3478F6
    static const QString AccentHover;       // #2463D8
    static const QString AccentTintLight;   // #EAF1FF
    static const QString AccentTintDark;    // #1E2D4A
    static const QString InfoSurfaceLight;  // #F2F6FF
    static const QString InfoSurfaceDark;   // #172133

    // Light Theme Surfaces & Text
    static const QString LightBgCanvas;     // #FAFAF8 (Notion canvas)
    static const QString LightBgSidebar;    // #F4F4F0 (Subtle sidebar)
    static const QString LightBgSurface;    // #FFFFFF (Main table/card surface)
    static const QString LightBgHover;      // #F0F0EB
    static const QString LightBgSelected;   // #3478F6 (Selected row background)
    static const QString LightTextPrimary;  // #242521 (High-contrast charcoal)
    static const QString LightTextSecondary;// #777A73 (Subtle metadata)
    static const QString LightTextMuted;    // #999A93
    static const QString LightBorder;       // #E6E6E0
    static const QString LightBorderSubtle; // #EDEDE8

    // Dark Theme Surfaces & Text
    static const QString DarkBgCanvas;      // #191919 (Notion dark canvas)
    static const QString DarkBgSidebar;     // #202020 (Sidebar)
    static const QString DarkBgSurface;     // #262626 (Cards / tables)
    static const QString DarkBgHover;       // #2F2F2F
    static const QString DarkBgSelected;    // #3478F6
    static const QString DarkTextPrimary;   // #EDEDEC
    static const QString DarkTextSecondary; // #9B9B97
    static const QString DarkTextMuted;     // #6A6A65
    static const QString DarkBorder;        // #333330
    static const QString DarkBorderSubtle;  // #2B2B28

    // Semantic Status Colors
    static const QString StatusSuccess;     // #2E7D32 (Forest Green)
    static const QString StatusWarning;     // #C07700 (Amber Brown)
    static const QString StatusDanger;      // #D32F2F (Apple Crimson)

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
    static QString getSurface() { return isDark() ? DarkBgSurface : LightBgSurface; }
    static QString getCanvas() { return isDark() ? DarkBgCanvas : LightBgCanvas; }
    static QString getBorder() { return isDark() ? DarkBorder : LightBorder; }
};

} // namespace Ui
