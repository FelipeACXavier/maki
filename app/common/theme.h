#pragma once

#include <QColor>
#include <QHash>
#include <QString>

#include "app_configs.h"
#include "itab.h"

#define DECLARE_KEY(KEY)            \
  {                                 \
    "@" #KEY, &maki::ThemeVars::KEY \
  }

class QApplication;

namespace Config
{
// ------------------------------------------------------------
// Data types
static const QString HAS_ACTIVITY = "hasActivity";
static const QString INVALID = "invalid";

struct ThemeInfo
{
  QString id;           // "dark", "light", "gruvbox"
  QString displayName;  // "Dark", "Light", "Gruvbox"
  QString filePath;     // full path to .theme file
  bool isUser = false;
};

// Map key strings to maki::ThemeVars members using the handy pointer to member notation
const QHash<QString, QString maki::ThemeVars::*> THEME_KEY_MAP = {
    // General
    DECLARE_KEY(background),
    DECLARE_KEY(foreground),
    DECLARE_KEY(faded_foreground),
    DECLARE_KEY(highlight),
    DECLARE_KEY(selection_fg),
    DECLARE_KEY(selection_bg),
    DECLARE_KEY(border_radius),
    DECLARE_KEY(icon_size),

    // General Colors
    DECLARE_KEY(info_color),
    DECLARE_KEY(error_color),
    DECLARE_KEY(warning_color),

    // Notification
    DECLARE_KEY(notification_width),
    DECLARE_KEY(min_notification_height),
    DECLARE_KEY(max_notification_height),
    DECLARE_KEY(notification_left_margin),
    DECLARE_KEY(notification_top_margin),
    DECLARE_KEY(notification_between_margin),

    // Scroll bar
    DECLARE_KEY(scrollbar_handle),
    DECLARE_KEY(scrollbar_handle_hover),

    // Button
    DECLARE_KEY(button_size),

    // General input
    DECLARE_KEY(input_bg),
    DECLARE_KEY(input_border),
    DECLARE_KEY(input_focus_border),
    DECLARE_KEY(combobox_bg),
    DECLARE_KEY(control_border),

    // Combo box
    DECLARE_KEY(combobox_hover_border),

    // Icons
    DECLARE_KEY(arrow_icon_up),
    DECLARE_KEY(arrow_icon_down),
    DECLARE_KEY(arrow_icon_right),
    DECLARE_KEY(uncheck_icon),
    DECLARE_KEY(check_icon),
    DECLARE_KEY(limit_icon),
    DECLARE_KEY(eyedropper_icon),

    // Tabs
    DECLARE_KEY(tab_bg),
    DECLARE_KEY(left_tab_width),
    DECLARE_KEY(right_tab_width),
    DECLARE_KEY(canvas_tab_width),
    DECLARE_KEY(canvas_tab_height),
    DECLARE_KEY(info_tab_width),
    DECLARE_KEY(tab_border_size),
    DECLARE_KEY(tab_border_style),
    DECLARE_KEY(tab_border_color),
    DECLARE_KEY(tab_selected_bg),
    DECLARE_KEY(tab_h_padding),
    DECLARE_KEY(tab_w_padding),

    // Menu
    DECLARE_KEY(menu_bg),
    DECLARE_KEY(menu_border),

    // Toolbar
    DECLARE_KEY(toolbar_bg),
    DECLARE_KEY(toolbar_border),
    DECLARE_KEY(toolbutton_hover_bg),

    // Tooltips
    DECLARE_KEY(tooltip_h_padding),
    DECLARE_KEY(tooltip_w_padding),
    DECLARE_KEY(tooltip_border_size),
    DECLARE_KEY(tooltip_border_style),
    DECLARE_KEY(tooltip_border_color),
};

extern maki::ThemeVars SYSTEM_THEME;
extern QColor FOREGROUND;
extern QColor HIGHLIGHT;

QVariant getValueFromTheme(const QString& key);

// ------------------------------------------------------------
// Theme loading stuff
QString loadFile(const QString& path);
void applyThemeToApp(QApplication* app, const QString& theme, const QList<Config::ThemeInfo>& availableThemes);
maki::ThemeVars loadThemeVarsFromFile(const QString& filePath);
VoidResult saveThemeVarsToFile(const QString& name, const maki::ThemeVars& theme);

// ------------------------------------------------------------
// Theme search stuff
QList<ThemeInfo> discoverThemes();

}  // namespace Config
