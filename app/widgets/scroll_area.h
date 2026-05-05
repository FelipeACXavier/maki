#pragma once

#include <QPainter>
#include <QPainterPath>
#include <QScrollArea>

/**
 * @brief A custom QScrollArea with styled background.
 */
class StyledScrollArea : public QScrollArea
{
  Q_OBJECT

public:
  /**
   * @brief Enumeration for different background roles.
   */
  enum class BackgroundRole
  {
    Window,     /// The window background role.
    Base,       /// The base background role.
    Button,     /// The button background role.
    Midlight,   /// The midlight background role.
    Custom      /// A custom background role.
  };

  /**
   * @brief Constructs a StyledScrollArea with the given parent.
   *
   * @param parent The parent widget.
   */
  StyledScrollArea(QWidget* parent = nullptr);

  /**
   * @brief Sets the background role for the scroll area.
   *
   * @param role The new background role.
   */
  void setBackgroundRole(BackgroundRole role);

  /**
   * @brief Sets a custom background color for the scroll area.
   *
   * @param color The new custom background color.
   */
  void setCustomBackgroundColor(const QColor& color);

protected:
  /**
   * @brief Handles viewport events.
   *
   * @param event The event to handle.
   * @return true if the event is handled, false otherwise.
   */
  bool viewportEvent(QEvent* event) override;

private:
  BackgroundRole mBackgroundRole = BackgroundRole::Base; /// The current background role.
  QColor mCustomBackgroundColor; /// The custom background color.

  /**
   * @brief Returns the current background color based on the role.
   *
   * @return The background color.
   */
  QColor backgroundColor() const;
};
