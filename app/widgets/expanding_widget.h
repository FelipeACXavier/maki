#pragma once

#include <QHBoxLayout>
#include <QLineEdit>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QWidget>

#include "collapsible_area.h"

class ClickableIcon;

/**
 * @brief A widget that expands and collapses its content area.
 */
class ExpandingWidget : public QWidget
{
  Q_OBJECT

public:
  /**
   * @brief Enumerates the possible directions for expanding and collapsing.
   */
  enum class Direction
  {
    Right,  /// Expands to the right.
    Left    /// Expands to the left.
  };

  /**
   * @brief Constructs an ExpandingWidget with a specified direction.
   *
   * @param direction The direction in which the widget expands and collapses.
   * @param parent The parent widget of this ExpandingWidget.
   */
  ExpandingWidget(Direction direction, QWidget* parent = nullptr);

  /**
   * @brief Sets the icon for the button that controls the expansion/collapse.
   *
   * @param icon The QIcon to set as the button's icon.
   */
  void setButtonIcon(const QIcon& icon);

  /**
   * @brief Sets the tooltip for the button that controls the expansion/collapse.
   *
   * @param tooltip The QString to set as the button's tooltip.
   */
  void setButtonTooltip(const QString& tooltip);

  /**
   * @brief Adds a collapsible widget to the expanding area.
   *
   * @param widget The QWidget to add to the collapsible area.
   */
  void addCollapsableWidget(QWidget* widget);

  /**
   * @brief Sets the width of the expanded state.
   *
   * @param width The int width to set for the expanded state.
   */
  void setExpandedWidth(int width);

  /**
   * @brief Expands the area controlled by the button.
   */
  void expandArea();

  /**
   * @brief Collapses the area controlled by the button.
   */
  void collapseArea();

signals:
  /**
   * @brief Emitted when the text changes.
   *
   * @param text The QString containing the new text.
   */
  void textChanged(const QString& text);

  /**
   * @brief Emitted when the area is expanded.
   *
   * @param button The ClickableIcon* that controls the expansion.
   */
  void areaExpanded(ClickableIcon* button);

  /**
   * @brief Emitted when the area is collapsed.
   *
   * @param button The ClickableIcon* that controls the collapse.
   */
  void areaCollapsed(ClickableIcon* button);

public slots:
  /**
   * @brief Sets whether the area is expanded or collapsed.
   *
   * @param expanded A bool indicating whether to expand (true) or collapse (false).
   */
  void setExpanded(bool expanded);

private:
  ClickableIcon* mButton = nullptr;  /// Pointer to the button that controls expansion/collapse.
  CollapsibleAreaWidth* mSearchArea = nullptr;  /// Pointer to the collapsible area widget.
  QPropertyAnimation* mAnimation = nullptr;  /// Pointer to the property animation for expanding/collapsing.

  int mExpandedWidth;  /// The width of the expanded state.
};
