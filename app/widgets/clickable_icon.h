#pragma once

#include <QMouseEvent>
#include <oclero/qlementine/widgets/IconWidget.hpp>

/**
 * @brief A clickable icon widget that can be checked or unchecked.
 */
class ClickableIcon : public oclero::qlementine::IconWidget
{
  Q_OBJECT

public:
  /**
   * @brief Constructs a new ClickableIcon with the given icon and parent widget.
   *
   * @param icon The QIcon to display on the button.
   * @param parent The parent QWidget of this ClickableIcon.
   */
  ClickableIcon(const QIcon& icon, QWidget* parent = nullptr);

  /**
   * @brief Constructs a new ClickableIcon with the given icon, size, and parent widget.
   *
   * @param icon The QIcon to display on the button.
   * @param size The QSize of the button.
   * @param parent The parent QWidget of this ClickableIcon.
   */
  ClickableIcon(const QIcon& icon, const QSize& size, QWidget* parent = nullptr);

  /**
   * @brief Sets whether the icon is checkable.
   *
   * @param checkable True if the icon should be checkable, false otherwise.
   */
  void setCheckable(bool checkable);

  /**
   * @brief Sets the checked state of the icon.
   *
   * @param check True to check the icon, false to uncheck it.
   */
  void setChecked(bool check);

  /**
   * @brief Returns whether the icon is currently checked.
   *
   * @return True if the icon is checked, false otherwise.
   */
  bool isChecked();

signals:
  /**
   * @brief Emitted when the icon is clicked.
   */
  void clicked();

  /**
   * @brief Emitted when the checked state of the icon changes.
   *
   * @param check The new checked state of the icon.
   */
  void toggled(bool check);

protected:
  /**
   * @brief Handles mouse press events.
   *
   * @param event The QMouseEvent to handle.
   */
  void mousePressEvent(QMouseEvent* event) override;
  void enterEvent(QEnterEvent* event) override;
  void leaveEvent(QEvent* event) override;

  void paintEvent(QPaintEvent* event) override;

private:
  bool mChecked;    /// True if the icon is currently checked, false otherwise.
  bool mCheckable;  /// True if the icon can be checked or unchecked, false otherwise.
  bool mHovered;    /// True when the button is hovered
};
