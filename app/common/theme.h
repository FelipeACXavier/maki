#pragma once

#include <QColor>
#include <QHash>
#include <QString>

#include "app_configs.h"

class QApplication;

namespace Config
{
// ------------------------------------------------------------
// Data types
static const QString HAS_ACTIVITY = "hasActivity";

struct ThemeInfo
{
  QString id;           // "dark", "light", "gruvbox"
  QString displayName;  // "Dark", "Light", "Gruvbox"
  QString filePath;     // full path to .theme file
  bool isUser = false;
};

// We also set the default values here
struct ThemeVars
{
  // General
  QString background = "#f1f1f1";
  QString foreground = "#202020";
  QString highlight = "#ff6600";
  QString selection_bg = "#3d6fb4";
  QString selection_fg = "#ffffff";
  QString border_radius = "5px";
  QString faded_foreground = "#afafaf";

  QString info_color = "#7CC183";
  QString error_color = "#DC6C6D";
  QString warning_color = "#E5C07B";

  // Notifications
  QString notification_width = "300";
  QString min_notification_height = "100";
  QString max_notification_height = "500";
  QString notification_top_margin = "60";
  QString notification_left_margin = "10";
  QString notification_between_margin = "10";

  // Scrollbar
  QString scrollbar_handle = "#c3c3c3";
  QString scrollbar_handle_hover = "#b0b0b0";

  // QButton
  QString button_bg = "#e0e0e0";
  QString button_border = "#b0b0b0";
  QString button_bg_hover = "#d4d4d4";
  QString button_pressed_bg = "#4a6ea9";
  QString button_pressed_border = "#315b9b";
  QString button_pressed_fg = "#ffffff";

  QString input_bg = "#ffffff";
  QString input_border = "#b0b0b0";
  QString input_focus_border = "#638ecb";
  QString combobox_bg = "#e0e0e0";
  QString control_border = "#b0b0b0";
  QString combobox_hover_border = "#6b90c6";

  // Icons
  QString arrow_icon_up = ":/icons/arrow-up.svg";
  QString arrow_icon_down = ":/icons/arrow-down.svg";
  QString arrow_icon_right = ":/icons/arrow-right.svg";
  QString check_icon = ":/icons/check.svg";
  QString uncheck_icon = ":/icons/uncheck.svg";
  QString limit_icon = ":/icons/limit.svg";

  // QTabBar
  QString left_tab_width = "100px";
  QString canvas_tab_width = "200px";
  QString canvas_tab_height = "24px";
  QString info_tab_width = "50px";
  QString right_tab_width = "100px";
  QString tab_border_size = "1px";
  QString tab_border_style = "solid";
  QString tab_border_color = "#b0b0b0";
  QString tab_bg = "#e0e0e0";
  QString tab_h_padding = "6px";
  QString tab_w_padding = "12px";
  QString tab_selected_bg = "#ffffff";

  // Menu
  QString menu_bg = "#f4f4f4";
  QString menu_border = "#b0b0b0";

  // Toolbar
  QString toolbar_bg = "#e8e8e8";
  QString toolbar_border = "#b0b0b0";
  QString toolbutton_hover_bg = "#dcdcdc";

  // Tooltip
  QString tooltip_h_padding = "2px";
  QString tooltip_w_padding = "5px";
  QString tooltip_border_size = "1px";
  QString tooltip_border_style = "solid";
  QString tooltip_border_color = "#b0b0b0";
};

// Map key strings to ThemeVars members using the handy pointer to member notation
const QHash<QString, QString ThemeVars::*> THEME_KEY_MAP = {
    // General
    {"@background", &ThemeVars::background},
    {"@foreground", &ThemeVars::foreground},
    {"@faded_foreground", &ThemeVars::faded_foreground},
    {"@highlight", &ThemeVars::highlight},
    {"@selection_bg", &ThemeVars::selection_bg},
    {"@selection_fg", &ThemeVars::selection_fg},
    {"@border_radius", &ThemeVars::border_radius},

    {"@info_color", &ThemeVars::info_color},
    {"@error_color", &ThemeVars::error_color},
    {"@warning_color", &ThemeVars::warning_color},

    // Notification
    {"@notification_width", &ThemeVars::notification_width},
    {"@min_notification_height", &ThemeVars::min_notification_height},
    {"@max_notification_height", &ThemeVars::max_notification_height},
    {"@notification_left_margin", &ThemeVars::notification_left_margin},
    {"@notification_top_margin", &ThemeVars::notification_top_margin},
    {"@notification_between_margin", &ThemeVars::notification_between_margin},

    {"@scrollbar_handle", &ThemeVars::scrollbar_handle},
    {"@scrollbar_handle_hover", &ThemeVars::scrollbar_handle_hover},
    {"@button_bg", &ThemeVars::button_bg},
    {"@button_border", &ThemeVars::button_border},
    {"@button_bg_hover", &ThemeVars::button_bg_hover},
    {"@button_pressed_bg", &ThemeVars::button_pressed_bg},
    {"@button_pressed_border", &ThemeVars::button_pressed_border},
    {"@button_pressed_fg", &ThemeVars::button_pressed_fg},
    {"@input_bg", &ThemeVars::input_bg},
    {"@input_border", &ThemeVars::input_border},
    {"@input_focus_border", &ThemeVars::input_focus_border},
    {"@combobox_bg", &ThemeVars::combobox_bg},
    {"@control_border", &ThemeVars::control_border},
    {"@combobox_hover_border", &ThemeVars::combobox_hover_border},

    // Icons
    {"@arrow_icon_up", &ThemeVars::arrow_icon_up},
    {"@arrow_icon_down", &ThemeVars::arrow_icon_down},
    {"@arrow_icon_right", &ThemeVars::arrow_icon_right},
    {"@uncheck_icon", &ThemeVars::uncheck_icon},
    {"@check_icon", &ThemeVars::check_icon},
    {"@limit_icon", &ThemeVars::limit_icon},

    // QTabBar
    {"@tab_bg", &ThemeVars::tab_bg},
    {"@left_tab_width", &ThemeVars::left_tab_width},
    {"@right_tab_width", &ThemeVars::right_tab_width},
    {"@canvas_tab_width", &ThemeVars::canvas_tab_width},
    {"@canvas_tab_height", &ThemeVars::canvas_tab_height},
    {"@info_tab_width", &ThemeVars::info_tab_width},
    {"@tab_border_size", &ThemeVars::tab_border_size},
    {"@tab_border_style", &ThemeVars::tab_border_style},
    {"@tab_border_color", &ThemeVars::tab_border_color},
    {"@tab_selected_bg", &ThemeVars::tab_selected_bg},
    {"@tab_h_padding", &ThemeVars::tab_h_padding},
    {"@tab_w_padding", &ThemeVars::tab_w_padding},

    // QMenu
    {"@menu_bg", &ThemeVars::menu_bg},
    {"@menu_border", &ThemeVars::menu_border},

    // QToolbar
    {"@toolbar_bg", &ThemeVars::toolbar_bg},
    {"@toolbar_border", &ThemeVars::toolbar_border},
    {"@toolbutton_hover_bg", &ThemeVars::toolbutton_hover_bg},

    // QToolTip
    {"@tooltip_h_padding", &ThemeVars::tooltip_h_padding},
    {"@tooltip_w_padding", &ThemeVars::tooltip_w_padding},
    {"@tooltip_border_size", &ThemeVars::tooltip_border_size},
    {"@tooltip_border_style", &ThemeVars::tooltip_border_style},
    {"@tooltip_border_color", &ThemeVars::tooltip_border_color},
};

static ThemeVars SYSTEM_THEME = ThemeVars{};
extern QColor FOREGROUND;
extern QColor HIGHLIGHT;

QVariant getValueFromTheme(const QString& key);

// ------------------------------------------------------------
// Theme loading stuff
QString loadFile(const QString& path);
void applyThemeToApp(QApplication* app, const QString& theme, const QList<Config::ThemeInfo>& availableThemes);
ThemeVars loadThemeVarsFromFile(const QString& filePath);

// ------------------------------------------------------------
// Theme search stuff
QString installThemesDir();
QString userThemesDir();
QList<ThemeInfo> discoverThemes();

}  // namespace Config
