#pragma once

#include <QColor>
#include <QFrame>
#include <QPainter>
#include <QPainterPath>
#include <oclero/qlementine.hpp>

class StyledFrame : public QFrame
{
  Q_OBJECT

public:
  enum class BackgroundRole
  {
    Window,
    Base,
    Button,
    Midlight,
    Custom
  };

  enum class BorderRole
  {
    None,
    Mid,
    Highlight,
    Custom
  };

  /**
   * @brief Constructs a StyledFrame with the given parent.
   *
   * @param parent The parent widget.
   */
  StyledFrame(QWidget* parent = nullptr);

  /**
   * @brief Sets the background role of the frame.
   *
   * @param role The new background role.
   */
  void setBackgroundRole(BackgroundRole role);

  /**
   * @brief Sets the border role of the frame.
   *
   * @param role The new border role.
   */
  void setBorderRole(BorderRole role);

  /**
   * @brief Sets a custom background color for the frame.
   *
   * @param color The new custom background color.
   */
  void setCustomBackgroundColor(const QColor& color);

  /**
   * @brief Sets a custom border color for the frame.
   *
   * @param color The new custom border color.
   */
  void setCustomBorderColor(const QColor& color);

  /**
   * @brief Sets the width of the border.
   *
   * @param width The new border width.
   */
  void setBorderWidth(int width);

  /**
   * @brief Sets the radius for rounded corners.
   *
   * @param radius The new radius value.
   */
  void setRadius(qreal radius);

protected:
  /**
   * @brief Handles the paint event to draw the styled frame.
   *
   * @param event The paint event.
   */
  void paintEvent(QPaintEvent* event) override;

private:
  BackgroundRole mBackgroundRole = BackgroundRole::Base;  /// Current background role.
  BorderRole mBorderRole = BorderRole::Mid;               /// Current border role.

  QColor mCustomBackgroundColor;  /// Custom background color.
  QColor mCustomBorderColor;      /// Custom border color.

  int mBorderWidth = 1;  /// Width of the border.
  qreal mRadius = 0.0;   /// Radius for rounded corners.

  /**
   * @brief Returns the current background color based on the role.
   *
   * @return The background color.
   */
  QColor backgroundColor() const;

  /**
   * @brief Returns the current border color based on the role.
   *
   * @return The border color.
   */
  QColor borderColor() const;
};
