#pragma once

#include <QColor>
#include <QHash>
#include <QString>

#include "app_configs.h"
#include "itab.h"

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
    {"@background", &maki::ThemeVars::background},
    {"@foreground", &maki::ThemeVars::foreground},
    {"@faded_foreground", &maki::ThemeVars::faded_foreground},
    {"@highlight", &maki::ThemeVars::highlight},
    {"@selection_bg", &maki::ThemeVars::selection_bg},
    {"@selection_fg", &maki::ThemeVars::selection_fg},
    {"@border_radius", &maki::ThemeVars::border_radius},

    {"@info_color", &maki::ThemeVars::info_color},
    {"@error_color", &maki::ThemeVars::error_color},
    {"@warning_color", &maki::ThemeVars::warning_color},

    // Notification
    {"@notification_width", &maki::ThemeVars::notification_width},
    {"@min_notification_height", &maki::ThemeVars::min_notification_height},
    {"@max_notification_height", &maki::ThemeVars::max_notification_height},
    {"@notification_left_margin", &maki::ThemeVars::notification_left_margin},
    {"@notification_top_margin", &maki::ThemeVars::notification_top_margin},
    {"@notification_between_margin", &maki::ThemeVars::notification_between_margin},

    {"@scrollbar_handle", &maki::ThemeVars::scrollbar_handle},
    {"@scrollbar_handle_hover", &maki::ThemeVars::scrollbar_handle_hover},
    {"@button_bg", &maki::ThemeVars::button_bg},
    {"@button_border", &maki::ThemeVars::button_border},
    {"@button_bg_hover", &maki::ThemeVars::button_bg_hover},
    {"@button_pressed_bg", &maki::ThemeVars::button_pressed_bg},
    {"@button_pressed_border", &maki::ThemeVars::button_pressed_border},
    {"@button_pressed_fg", &maki::ThemeVars::button_pressed_fg},
    {"@input_bg", &maki::ThemeVars::input_bg},
    {"@input_border", &maki::ThemeVars::input_border},
    {"@input_focus_border", &maki::ThemeVars::input_focus_border},
    {"@combobox_bg", &maki::ThemeVars::combobox_bg},
    {"@control_border", &maki::ThemeVars::control_border},
    {"@combobox_hover_border", &maki::ThemeVars::combobox_hover_border},

    // Icons
    {"@arrow_icon_up", &maki::ThemeVars::arrow_icon_up},
    {"@arrow_icon_down", &maki::ThemeVars::arrow_icon_down},
    {"@arrow_icon_right", &maki::ThemeVars::arrow_icon_right},
    {"@uncheck_icon", &maki::ThemeVars::uncheck_icon},
    {"@check_icon", &maki::ThemeVars::check_icon},
    {"@limit_icon", &maki::ThemeVars::limit_icon},

    // QTabBar
    {"@tab_bg", &maki::ThemeVars::tab_bg},
    {"@left_tab_width", &maki::ThemeVars::left_tab_width},
    {"@right_tab_width", &maki::ThemeVars::right_tab_width},
    {"@canvas_tab_width", &maki::ThemeVars::canvas_tab_width},
    {"@canvas_tab_height", &maki::ThemeVars::canvas_tab_height},
    {"@info_tab_width", &maki::ThemeVars::info_tab_width},
    {"@tab_border_size", &maki::ThemeVars::tab_border_size},
    {"@tab_border_style", &maki::ThemeVars::tab_border_style},
    {"@tab_border_color", &maki::ThemeVars::tab_border_color},
    {"@tab_selected_bg", &maki::ThemeVars::tab_selected_bg},
    {"@tab_h_padding", &maki::ThemeVars::tab_h_padding},
    {"@tab_w_padding", &maki::ThemeVars::tab_w_padding},

    // QMenu
    {"@menu_bg", &maki::ThemeVars::menu_bg},
    {"@menu_border", &maki::ThemeVars::menu_border},

    // QToolbar
    {"@toolbar_bg", &maki::ThemeVars::toolbar_bg},
    {"@toolbar_border", &maki::ThemeVars::toolbar_border},
    {"@toolbutton_hover_bg", &maki::ThemeVars::toolbutton_hover_bg},

    // QToolTip
    {"@tooltip_h_padding", &maki::ThemeVars::tooltip_h_padding},
    {"@tooltip_w_padding", &maki::ThemeVars::tooltip_w_padding},
    {"@tooltip_border_size", &maki::ThemeVars::tooltip_border_size},
    {"@tooltip_border_style", &maki::ThemeVars::tooltip_border_style},
    {"@tooltip_border_color", &maki::ThemeVars::tooltip_border_color},
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

// ------------------------------------------------------------
// Theme search stuff
QString installThemesDir();
QString userThemesDir();
QList<ThemeInfo> discoverThemes();

}  // namespace Config
