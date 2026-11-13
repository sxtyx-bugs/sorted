#include "Theme.h"
#include <QApplication>

namespace Ui {

ColorTheme Theme::currentTheme = ColorTheme::Light;

// --- Typography: Native OS First with Apple & Windows Cross-Platform Fallback ---
#if defined(Q_OS_WIN) || defined(_WIN32)
const QString Theme::FontFamilyDisplay = "'Segoe UI Variable Display', 'Segoe UI Variable Text', 'Segoe UI', 'SF Pro Display', -apple-system, BlinkMacSystemFont, system-ui, sans-serif";
const QString Theme::FontFamilyText    = "'Segoe UI Variable Text', 'Segoe UI', 'SF Pro Text', -apple-system, BlinkMacSystemFont, system-ui, sans-serif";
const QString Theme::FontFamilyMono    = "'Cascadia Code', 'Consolas', 'SF Mono', monospace";
#elif defined(Q_OS_MAC) || defined(__APPLE__)
const QString Theme::FontFamilyDisplay = "'SF Pro Display', 'SF Pro Text', 'SF Pro', -apple-system, BlinkMacSystemFont, system-ui, sans-serif";
const QString Theme::FontFamilyText    = "'SF Pro Text', 'SF Pro Display', 'SF Pro', -apple-system, BlinkMacSystemFont, system-ui, sans-serif";
const QString Theme::FontFamilyMono    = "'SF Mono', 'Menlo', 'Courier New', monospace";
#else
const QString Theme::FontFamilyDisplay = "system-ui, -apple-system, sans-serif";
const QString Theme::FontFamilyText    = "system-ui, -apple-system, sans-serif";
const QString Theme::FontFamilyMono    = "monospace";
#endif

const QString Theme::AccentCharcoal    = "#1D1D1F";
const QString Theme::AccentHover       = "#3A3A3C";
const QString Theme::AccentTintLight   = "#F1F1EF";
const QString Theme::AccentTintDark    = "#333333";
const QString Theme::InfoSurfaceLight  = "#F7F7F5";
const QString Theme::InfoSurfaceDark   = "#222222";

const QString Theme::LightBgCanvas     = "#F7F7F5";
const QString Theme::LightBgSidebar    = "#F2F2F0";
const QString Theme::LightBgSurface    = "#FFFFFF";
const QString Theme::LightBgHover      = "#F1F1EF";
const QString Theme::LightBgSelected   = "#EBEBEB";
const QString Theme::LightTextPrimary  = "#1D1D1F";
const QString Theme::LightTextSecondary= "#86868B";
const QString Theme::LightTextMuted    = "#A1A1A6";
const QString Theme::LightBorder       = "#E5E5E5";
const QString Theme::LightBorderSubtle = "#EDEDE8";

const QString Theme::DarkBgCanvas      = "#191919";
const QString Theme::DarkBgSidebar     = "#202020";
const QString Theme::DarkBgSurface     = "#262626";
const QString Theme::DarkBgHover       = "#2F2F2F";
const QString Theme::DarkBgSelected    = "#383838";
const QString Theme::DarkTextPrimary   = "#F5F5F7";
const QString Theme::DarkTextSecondary = "#A1A1A6";
const QString Theme::DarkTextMuted     = "#6E6E73";
const QString Theme::DarkBorder        = "#3A3A3C";
const QString Theme::DarkBorderSubtle  = "#303032";

const QString Theme::StatusSuccess     = "#1D1D1F";
const QString Theme::StatusWarning     = "#48484A";
const QString Theme::StatusDanger      = "#D32F2F";

// Liquid Glass Translucent RGBA definitions
const QString Theme::GlassLightCanvas   = "rgba(247, 247, 245, 0.75)";
const QString Theme::GlassLightSidebar  = "rgba(242, 242, 240, 0.78)";
const QString Theme::GlassLightSurface  = "rgba(255, 255, 255, 0.82)";
const QString Theme::GlassLightCard     = "rgba(255, 255, 255, 0.85)";
const QString Theme::GlassLightBorder   = "rgba(255, 255, 255, 0.80)";

const QString Theme::GlassDarkCanvas    = "rgba(25, 25, 25, 0.78)";
const QString Theme::GlassDarkSidebar   = "rgba(32, 32, 32, 0.75)";
const QString Theme::GlassDarkSurface   = "rgba(38, 38, 38, 0.80)";
const QString Theme::GlassDarkCard      = "rgba(42, 42, 45, 0.78)";
const QString Theme::GlassDarkBorder    = "rgba(255, 255, 255, 0.16)";

QPalette Theme::getPalette(ColorTheme theme) {
    QPalette p;
    if (theme == ColorTheme::Light) {
        p.setColor(QPalette::Window, QColor(LightBgCanvas));
        p.setColor(QPalette::WindowText, QColor(LightTextPrimary));
        p.setColor(QPalette::Base, QColor(LightBgSurface));
        p.setColor(QPalette::AlternateBase, QColor(LightBgCanvas));
        p.setColor(QPalette::ToolTipBase, QColor(LightBgSurface));
        p.setColor(QPalette::ToolTipText, QColor(LightTextPrimary));
        p.setColor(QPalette::Text, QColor(LightTextPrimary));
        p.setColor(QPalette::Button, QColor(LightBgSurface));
        p.setColor(QPalette::ButtonText, QColor(LightTextPrimary));
        p.setColor(QPalette::BrightText, QColor(StatusDanger));
        p.setColor(QPalette::Link, QColor(LightTextPrimary));
        p.setColor(QPalette::Highlight, QColor(LightBgSelected));
        p.setColor(QPalette::HighlightedText, QColor(LightTextPrimary));
        p.setColor(QPalette::PlaceholderText, QColor(LightTextMuted));
        p.setColor(QPalette::Disabled, QPalette::Text, QColor(LightTextMuted));
        p.setColor(QPalette::Disabled, QPalette::WindowText, QColor(LightTextMuted));
        p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(LightTextMuted));
    } else {
        p.setColor(QPalette::Window, QColor(DarkBgCanvas));
        p.setColor(QPalette::WindowText, QColor(DarkTextPrimary));
        p.setColor(QPalette::Base, QColor(DarkBgSurface));
        p.setColor(QPalette::AlternateBase, QColor(DarkBgCanvas));
        p.setColor(QPalette::ToolTipBase, QColor(DarkBgSurface));
        p.setColor(QPalette::ToolTipText, QColor(DarkTextPrimary));
        p.setColor(QPalette::Text, QColor(DarkTextPrimary));
        p.setColor(QPalette::Button, QColor(DarkBgSurface));
        p.setColor(QPalette::ButtonText, QColor(DarkTextPrimary));
        p.setColor(QPalette::BrightText, QColor("#FF6B6B"));
        p.setColor(QPalette::Link, QColor(DarkTextPrimary));
        p.setColor(QPalette::Highlight, QColor(DarkBgSelected));
        p.setColor(QPalette::HighlightedText, QColor(DarkTextPrimary));
        p.setColor(QPalette::PlaceholderText, QColor(DarkTextMuted));
        p.setColor(QPalette::Disabled, QPalette::Text, QColor(DarkTextMuted));
        p.setColor(QPalette::Disabled, QPalette::WindowText, QColor(DarkTextMuted));
        p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(DarkTextMuted));
    }
    return p;
}

QString Theme::getGlobalStyleSheet(ColorTheme theme) {
    bool dark = (theme == ColorTheme::Dark);
    QString bgCanvas = dark ? DarkBgCanvas : LightBgCanvas;
    QString bgSurface = dark ? DarkBgSurface : LightBgSurface;
    QString bgHover = dark ? DarkBgHover : LightBgHover;
    QString bgSelected = dark ? DarkBgSelected : LightBgSelected;
    QString textPri = dark ? DarkTextPrimary : LightTextPrimary;
    QString textSec = dark ? DarkTextSecondary : LightTextSecondary;
    QString textMuted = dark ? DarkTextMuted : LightTextMuted;
    QString border = dark ? DarkBorder : LightBorder;
    QString tint = dark ? AccentTintDark : AccentTintLight;
    QString infoSurface = dark ? InfoSurfaceDark : InfoSurfaceLight;

    return QString(
        "QMainWindow { background-color: %1; color: %2; font-family: %11; } "
        "QWidget { font-family: %11; color: %2; } "
        
        // Buttons
        "QPushButton { "
        "  background-color: %3; "
        "  color: %2; "
        "  border: 1px solid %4; "
        "  border-radius: 6px; "
        "  padding: 6px 14px; "
        "  font-size: 13px; "
        "  font-weight: 500; "
        "} "
        "QPushButton:hover { "
        "  background-color: %5; "
        "  border-color: %4; "
        "} "
        "QPushButton:pressed { "
        "  background-color: %6; "
        "} "
        "QPushButton:disabled { "
        "  color: %7; "
        "  border-color: %4; "
        "  opacity: 0.5; "
        "} "
        
        // Primary Cobalt Blue Button
        "QPushButton#btnPrimary { "
        "  background-color: %8; "
        "  color: #FFFFFF; "
        "  border: none; "
        "  font-weight: 600; "
        "  border-radius: 10px; "
        "  min-height: 24px; "
        "  padding: 6px 16px; "
        "} "
        "QPushButton#btnPrimary:hover { "
        "  background-color: %9; "
        "} "
        "QPushButton#btnPrimary:pressed { "
        "  background-color: #1A54BD; "
        "} "
        "QPushButton#btnPrimary:disabled { "
        "  background-color: %4; "
        "  color: %7; "
        "} "

        // Secondary / Subtle Button
        "QPushButton#btnSubtle { "
        "  background-color: %3; "
        "  border: 1px solid %4; "
        "  color: %2; "
        "  border-radius: 10px; "
        "  min-height: 24px; "
        "  padding: 6px 14px; "
        "} "
        "QPushButton#btnSubtle:hover { "
        "  background-color: %5; "
        "  color: %2; "
        "} "
        "QPushButton#btnSubtle:pressed { "
        "  background-color: %4; "
        "} "

        // Icon Button (Collapse/Expand)
        "QPushButton#btnIconButton { "
        "  background-color: transparent; "
        "  border: 1px solid %4; "
        "  border-radius: 9px; "
        "  min-width: 32px; "
        "  max-width: 34px; "
        "  min-height: 32px; "
        "  max-height: 34px; "
        "  padding: 0px; "
        "} "
        "QPushButton#btnIconButton:hover { "
        "  background-color: %5; "
        "  border-color: %8; "
        "} "

        // Theme Toggle Button
        "QPushButton#btnTheme { "
        "  background-color: %3; "
        "  border: 1px solid %4; "
        "  color: %2; "
        "  border-radius: 10px; "
        "  padding: 6px 12px; "
        "  font-size: 12px; "
        "} "
        "QPushButton#btnTheme:hover { "
        "  background-color: %5; "
        "} "

        // Inputs
        "QLineEdit { "
        "  background-color: %3; "
        "  color: %2; "
        "  border: 1px solid %4; "
        "  border-radius: 10px; "
        "  padding: 7px 12px; "
        "  font-size: 13px; "
        "} "
        "QLineEdit:focus { "
        "  border: 1.5px solid %8; "
        "  background-color: %3; "
        "} "
        "QComboBox { "
        "  background-color: %3; "
        "  color: %2; "
        "  border: 1px solid %4; "
        "  border-radius: 10px; "
        "  padding: 6px 12px; "
        "  font-size: 13px; "
        "} "
        "QComboBox:focus { "
        "  border: 1.5px solid %8; "
        "} "
        "QComboBox QAbstractItemView { "
        "  background-color: %3; "
        "  color: %2; "
        "  border: 1px solid %4; "
        "  border-radius: 8px; "
        "  selection-background-color: %10; "
        "  selection-color: %8; "
        "  padding: 4px; "
        "} "

        // CheckBoxes
        "QCheckBox { "
        "  spacing: 8px; "
        "  font-size: 13px; "
        "  color: %2; "
        "} "
        "QCheckBox::indicator { "
        "  width: 16px; "
        "  height: 16px; "
        "  border: 1px solid %4; "
        "  border-radius: 4px; "
        "  background-color: %3; "
        "} "
        "QCheckBox::indicator:hover { "
        "  border-color: %8; "
        "} "
        "QCheckBox::indicator:checked { "
        "  background-color: %8; "
        "  border-color: %8; "
        "} "

        // Progress Bar
        "QProgressBar { "
        "  background-color: %4; "
        "  border: none; "
        "  border-radius: 3px; "
        "} "
        "QProgressBar::chunk { "
        "  background-color: %8; "
        "  border-radius: 3px; "
        "} "

        // ScrollBars
        "QScrollBar:vertical { "
        "  border: none; "
        "  background: transparent; "
        "  width: 7px; "
        "  margin: 0px; "
        "} "
        "QScrollBar::handle:vertical { "
        "  background: %4; "
        "  min-height: 24px; "
        "  border-radius: 3px; "
        "} "
        "QScrollBar::handle:vertical:hover { "
        "  background: %7; "
        "} "
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { "
        "  height: 0px; "
        "} "

        // Menus & Tooltips
        "QMenu { "
        "  background-color: %3; "
        "  color: %2; "
        "  border: 1px solid %4; "
        "  border-radius: 8px; "
        "  padding: 4px; "
        "} "
        "QMenu::item { "
        "  padding: 6px 14px; "
        "  border-radius: 4px; "
        "  font-size: 13px; "
        "} "
        "QMenu::item:selected { "
        "  background-color: %10; "
        "  color: %8; "
        "} "
        "QToolTip { "
        "  background-color: %3; "
        "  color: %2; "
        "  border: 1px solid %4; "
        "  border-radius: 6px; "
        "  padding: 4px 8px; "
        "  font-size: 12px; "
        "} "
    ).arg(bgCanvas, textPri, bgSurface, border, bgHover, bgSelected, textMuted, AccentCharcoal, AccentHover, tint, FontFamilyText);
}

QString Theme::getTableStyleSheet(ColorTheme theme) {
    bool dark = (theme == ColorTheme::Dark);
    QString bgSurface = dark ? DarkBgSurface : LightBgSurface;
    QString bgCanvas = dark ? DarkBgCanvas : LightBgCanvas;
    QString bgHover = dark ? DarkBgHover : LightBgHover;
    QString textPri = dark ? DarkTextPrimary : LightTextPrimary;
    QString textSec = dark ? DarkTextSecondary : LightTextSecondary;
    QString border = dark ? DarkBorder : LightBorder;
    QString borderSubtle = dark ? DarkBorderSubtle : LightBorderSubtle;

    return QString(
        "QTableView, QTableWidget { "
        "  background-color: %1; "
        "  color: %2; "
        "  border: 1px solid %3; "
        "  border-radius: 8px; "
        "  gridline-color: %4; "
        "  selection-background-color: %5; "
        "  selection-color: #FFFFFF; "
        "  outline: none; "
        "  font-variant-numeric: tabular-nums; "
        "} "
        "QTableView::item, QTableWidget::item { "
        "  color: %2; "
        "  padding: 6px 12px; "
        "  border: none; "
        "} "
        "QTableView::item:hover, QTableWidget::item:hover { "
        "  background-color: %6; "
        "  color: %2; "
        "} "
        "QTableView::item:selected, QTableWidget::item:selected { "
        "  background-color: %5; "
        "  color: #FFFFFF; "
        "} "
        "QHeaderView::section { "
        "  background-color: %7; "
        "  color: %8; "
        "  font-size: 12px; "
        "  font-weight: 600; "
        "  padding: 8px 12px; "
        "  border: none; "
        "  border-bottom: 1px solid %3; "
        "  border-right: 1px solid %4; "
        "} "
        "QTableCornerButton::section { "
        "  background-color: %7; "
        "  border: none; "
        "  border-bottom: 1px solid %3; "
        "  border-right: 1px solid %4; "
        "} "
    ).arg(bgSurface, textPri, border, borderSubtle, AccentCharcoal, bgHover, bgCanvas, textSec);
}

QString Theme::getTreeStyleSheet(ColorTheme theme) {
    bool dark = (theme == ColorTheme::Dark);
    QString bgSurface = dark ? DarkBgSurface : LightBgSurface;
    QString bgCanvas = dark ? DarkBgCanvas : LightBgCanvas;
    QString bgHover = dark ? DarkBgHover : LightBgHover;
    QString textPri = dark ? DarkTextPrimary : LightTextPrimary;
    QString textSec = dark ? DarkTextSecondary : LightTextSecondary;
    QString border = dark ? DarkBorder : LightBorder;

    return QString(
        "QTreeView, QTreeWidget { "
        "  background-color: %1; "
        "  color: %2; "
        "  border: 1px solid %3; "
        "  border-radius: 8px; "
        "  selection-background-color: %4; "
        "  selection-color: #FFFFFF; "
        "  outline: none; "
        "  padding: 4px; "
        "  font-variant-numeric: tabular-nums; "
        "} "
        "QTreeView::item, QTreeWidget::item { "
        "  color: %2; "
        "  padding: 6px 8px; "
        "  border-radius: 5px; "
        "} "
        "QTreeView::item:hover, QTreeWidget::item:hover { "
        "  background-color: %5; "
        "  color: %2; "
        "} "
        "QTreeView::item:selected, QTreeWidget::item:selected { "
        "  background-color: %4; "
        "  color: #FFFFFF; "
        "} "
        "QHeaderView::section { "
        "  background-color: %6; "
        "  color: %7; "
        "  font-size: 12px; "
        "  font-weight: 600; "
        "  padding: 8px 12px; "
        "  border: none; "
        "  border-bottom: 1px solid %3; "
        "  border-right: 1px solid %3; "
        "} "
    ).arg(bgSurface, textPri, border, AccentCharcoal, bgHover, bgCanvas, textSec);
}

QString Theme::getSidebarStyleSheet(ColorTheme theme) {
    bool dark = (theme == ColorTheme::Dark);
    QString bgSidebar = dark ? DarkBgSidebar : LightBgSidebar;
    QString bgHover = dark ? DarkBgHover : LightBgHover;
    QString tint = dark ? AccentTintDark : AccentTintLight;
    QString textPri = dark ? DarkTextPrimary : LightTextPrimary;
    QString textSec = dark ? DarkTextSecondary : LightTextSecondary;
    QString border = dark ? DarkBorder : LightBorder;

    return QString(
        "QWidget#sidebarWidget { "
        "  background-color: %1; "
        "  border-right: 1px solid %2; "
        "} "
        "QListWidget { "
        "  border: none; "
        "  background-color: transparent; "
        "  outline: none; "
        "} "
        "QListWidget::item { "
        "  padding: 8px 12px; "
        "  border-radius: 6px; "
        "  font-size: 13px; "
        "  font-weight: 500; "
        "  color: %3; "
        "  margin-bottom: 2px; "
        "} "
        "QListWidget::item:hover { "
        "  background-color: %4; "
        "  color: %5; "
        "} "
        "QListWidget::item:selected { "
        "  background-color: %6; "
        "  color: %7; "
        "  font-weight: 600; "
        "} "
    ).arg(bgSidebar, border, textSec, bgHover, textPri, tint, AccentCharcoal);
}

QString Theme::getCardStyleSheet(ColorTheme theme) {
    bool dark = (theme == ColorTheme::Dark);
    QString bgSurface = dark ? DarkBgSurface : LightBgSurface;
    QString bgCanvas = dark ? DarkBgCanvas : LightBgCanvas;
    QString border = dark ? DarkBorder : LightBorder;

    return QString(
        "QFrame#folderPill { "
        "  background-color: %4; "
        "  border: 1px solid %2; "
        "  border-radius: 10px; "
        "} "
        "QWidget#statCard, QFrame#statCard { "
        "  background-color: %1; "
        "  border: 1px solid %2; "
        "  border-radius: 10px; "
        "} "
        "QFrame#noticeBanner { "
        "  background-color: %3; "
        "  border: 1px solid %2; "
        "  border-radius: 10px; "
        "} "
        "QFrame#bottomActionBar { "
        "  background-color: %1; "
        "  border-top: 1px solid %2; "
        "  border-radius: 0px 0px 10px 10px; "
        "} "
    ).arg(bgSurface, border, dark ? InfoSurfaceDark : InfoSurfaceLight, bgCanvas);
}

QString Theme::getBadgeStyleSheet(const QString& type, ColorTheme theme) {
    bool dark = (theme == ColorTheme::Dark);
    if (type == "success") {
        return dark ? "background-color: #173822; color: #66BB6A; border-radius: 4px; padding: 2px 8px; font-weight: 600; font-size: 11px;"
                    : "background-color: #E8F5E9; color: #2E7D32; border-radius: 4px; padding: 2px 8px; font-weight: 600; font-size: 11px;";
    } else if (type == "warning") {
        return dark ? "background-color: #382A12; color: #FFA726; border-radius: 4px; padding: 2px 8px; font-weight: 600; font-size: 11px;"
                    : "background-color: #FFF3E0; color: #C07700; border-radius: 4px; padding: 2px 8px; font-weight: 600; font-size: 11px;";
    } else if (type == "danger") {
        return dark ? "background-color: #3B1C1C; color: #EF5350; border-radius: 4px; padding: 2px 8px; font-weight: 600; font-size: 11px;"
                    : "background-color: #FFEBEE; color: #D32F2F; border-radius: 4px; padding: 2px 8px; font-weight: 600; font-size: 11px;";
    } else if (type == "info") {
        return dark ? "background-color: #1E2D4A; color: #64B5F6; border-radius: 4px; padding: 2px 8px; font-weight: 600; font-size: 11px;"
                    : "background-color: #EAF1FF; color: #3478F6; border-radius: 4px; padding: 2px 8px; font-weight: 600; font-size: 11px;";
    }
    return dark ? "background-color: #2F2F2F; color: #9B9B97; border-radius: 4px; padding: 2px 8px; font-size: 11px;"
                : "background-color: #F0F0EB; color: #777A73; border-radius: 4px; padding: 2px 8px; font-size: 11px;";
}

void Theme::applyTheme(ColorTheme theme) {
    currentTheme = theme;
    if (qApp) {
        qApp->setPalette(getPalette(theme));
        qApp->setStyleSheet(getGlobalStyleSheet(theme));
    }
}

void Theme::toggleTheme() {
    applyTheme(currentTheme == ColorTheme::Light ? ColorTheme::Dark : ColorTheme::Light);
}

} // namespace Ui
