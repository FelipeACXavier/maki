#pragma once
#include <QGraphicsScene>

#include "result.h"

namespace maki
{

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
  QString notification_top_margin = "55";
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

struct ThemeFonts
{
  QFont main;
  QFont property;
  QFont label;
  QFont hint;
  QFont monoSpace;
};

class ITab
{
public:
  virtual ~ITab() = default;
  virtual void updateScene(const QString& name) = 0;
  virtual void openScene(const QString& name) = 0;
  virtual ThemeVars currentTheme() = 0;
  virtual ThemeFonts labelFont() = 0;
  virtual void registerAppearenceUpdate(const QString& name, std::function<VoidResult(QGraphicsScene* scene)> callback) = 0;
};

}  // namespace maki
