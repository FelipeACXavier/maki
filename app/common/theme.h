#pragma once

#include <QHash>
#include <QString>

class QApplication;

namespace Config
{
// ------------------------------------------------------------
// Data types

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
  QString background = "#f1f1f1";
  QString foreground = "#202020";
  QString selection_bg = "#3d6fb4";
  QString selection_fg = "#ffffff";
  QString scrollbar_handle = "#c3c3c3";
  QString scrollbar_handle_hover = "#b0b0b0";
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
  QString combobox_border = "#b0b0b0";
  QString combobox_hover_border = "#6b90c6";
  QString arrow_icon = ":/icons/arrow-down-dark.svg";
  QString tab_bg = "#e0e0e0";
  QString tab_border = "#b0b0b0";
  QString tab_selected_bg = "#ffffff";
  QString menu_bg = "#f4f4f4";
  QString menu_border = "#b0b0b0";
  QString toolbar_bg = "#e8e8e8";
  QString toolbar_border = "#b0b0b0";
  QString toolbutton_hover_bg = "#dcdcdc";
};

// Map key strings to ThemeVars members using the handy pointer to member notation
const QHash<QString, QString ThemeVars::*> THEME_KEY_MAP = {
    {"@background", &ThemeVars::background},
    {"@foreground", &ThemeVars::foreground},
    {"@selection_bg", &ThemeVars::selection_bg},
    {"@selection_fg", &ThemeVars::selection_fg},
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
    {"@combobox_border", &ThemeVars::combobox_border},
    {"@combobox_hover_border", &ThemeVars::combobox_hover_border},
    {"@arrow_icon", &ThemeVars::arrow_icon},
    {"@tab_bg", &ThemeVars::tab_bg},
    {"@tab_border", &ThemeVars::tab_border},
    {"@tab_selected_bg", &ThemeVars::tab_selected_bg},
    {"@menu_bg", &ThemeVars::menu_bg},
    {"@menu_border", &ThemeVars::menu_border},
    {"@toolbar_bg", &ThemeVars::toolbar_bg},
    {"@toolbar_border", &ThemeVars::toolbar_border},
    {"@toolbutton_hover_bg", &ThemeVars::toolbutton_hover_bg},
};

// ------------------------------------------------------------
// Theme loading stuff
QString loadFile(const QString& path);
void applyThemeToApp(QApplication& app, const QString& theme, const QList<Config::ThemeInfo>& availableThemes);
ThemeVars loadThemeVarsFromFile(const QString& filePath);

// ------------------------------------------------------------
// Theme search stuff
QString installThemesDir();
QString userThemesDir();
QList<ThemeInfo> discoverThemes();

}  // namespace Config
