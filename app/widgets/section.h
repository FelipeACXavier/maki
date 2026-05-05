#pragma once

#include <QToolButton>
#include <QVBoxLayout>
#include <QWidget>
#include <oclero/qlementine/Common.hpp>

#include "collapsible_area.h"

class ClickableIcon;
class QPropertyAnimation;

/**
 * @brief A widget that represents a section with a collapsible content area.
 */
class SectionWidget : public QWidget
{
  Q_OBJECT

public:
  /**
   * @brief Constructs a new SectionWidget.
   *
   * @param parent The parent widget.
   */
  SectionWidget(QWidget* parent = nullptr);

  /**
   * @brief Adds an item to the section with a title.
   *
   * @param container The widget to add as content.
   * @param title The title of the item.
   */
  void addItem(QWidget* container, const QString& title);

  /**
   * @brief Adds an item to the section with a title and text role.
   *
   * @param container The widget to add as content.
   * @param title The title of the item.
   * @param role The text role for the title.
   */
  void addItem(QWidget* container, const QString& title, oclero::qlementine::TextRole role);

  /**
   * @brief Sets whether the section is expanded or collapsed.
   *
   * @param expanded True to expand, false to collapse.
   */
  void setExpanded(bool expanded);

  /**
   * @brief Returns whether the section is currently expanded.
   *
   * @return True if expanded, false otherwise.
   */
  bool isExpanded() const;

  /**
   * @brief Sets the duration of the animation for expanding and collapsing.
   *
   * @param duration The duration in milliseconds.
   */
  void setDuration(int duration);

  /**
   * @brief Returns the content widget of the section.
   *
   * @return The content widget.
   */
  QWidget* content() const;

  /**
   * @brief Updates the height of the content area.
   *
   * @param height The new height in pixels.
   */
  void updateContentHeight(int height);

private:
  ClickableIcon* mToggleButton = nullptr; /// Pointer to the toggle button for expanding/collapsing.
  QWidget* mContent = nullptr; /// Pointer to the content widget.
  CollapsibleAreaHeight* mContentArea = nullptr; /// Pointer to the collapsible area.
  bool mExpanded = true; /// Current state of the section (expanded or collapsed).
  std::optional<int> mDuration = std::nullopt; /// Duration of the animation for expanding and collapsing.
  QPropertyAnimation* mAnimation = nullptr; /// Pointer to the property animation.

  /**
   * @brief Calculates the duration of the animation based on target and current heights.
   *
   * @param target The target height.
   * @param current The current height.
   * @return The calculated duration in milliseconds.
   */
  inline int getAnimationDuration(int target, int current) const;

  /**
   * @brief Slot called when the toggle button is toggled.
   *
   * @param checked True if the button is checked (expanded), false otherwise.
   */
  void toggled(bool checked);
};
