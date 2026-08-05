// notificationwidget.h
#pragma once

#include <QFrame>
#include <QLabel>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <oclero/qlementine/widgets/StatusBadgeWidget.hpp>

#include "clickable_icon.h"
#include "expanding_widget.h"
#include "frame.h"
#include "logging.h"

/**
 * @class NotificationWidget
 * @brief A transient UI widget for displaying notifications with animation and auto-dismiss support.
 *
 * This widget displays a message with a title and severity level, and supports:
 * - Fade-in and fade-out animations
 * - Automatic dismissal via timer
 * - Manual dismissal via a close button
 *
 * The widget exposes an opacity property used for animation.
 */
class NotificationWidget : public StyledFrame
{
  Q_OBJECT

  /**
   * @property opacity
   * @brief Controls the visual opacity of the widget.
   *
   * This property is typically animated using QPropertyAnimation
   * to create fade-in and fade-out effects.
   */
  Q_PROPERTY(qreal opacity READ opacity WRITE setOpacity)

public:
  /**
   * @brief Constructs a notification widget.
   *
   * @param title The title of the notification.
   * @param text The main message text.
   * @param level The log level determining the visual style (e.g., info, warning, error).
   * @param parent The parent widget.
   */
  NotificationWidget(const QString& title, const QString& text, logging::LogLevel level, QWidget* parent = nullptr);

  /**
   * @brief Returns the current opacity of the widget.
   * @return The opacity value in the range [0.0, 1.0].
   */
  qreal opacity() const;

  /**
   * @brief Sets the opacity of the widget.
   * @param o The opacity value to set (typically between 0.0 and 1.0).
   */
  void setOpacity(qreal o);

  virtual bool disappearing() const;

  int duration() const;
  void setBadge(oclero::qlementine::StatusBadge badge);

  void minimize(bool minimize);

signals:
  /**
   * @brief Emitted when the notification is dismissed.
   * @param self Pointer to this notification widget.
   */
  void dismissed(NotificationWidget* self);

public slots:
  /**
   * @brief Shows the notification with a fade-in animation.
   */
  void showAnimated();

  /**
   * @brief Hides the notification with a fade-out animation.
   */
  void hideAnimated();

protected:
  bool mAlarmSetup;
  QTimer mAutoCloseTimer;         ///< Timer used to automatically dismiss the notification.
  QPropertyAnimation* mFadeAnim;  ///< Animation used to control opacity transitions.

  ExpandingWidget* mBody;

  void setupAlarm(int msec);

private:
  ClickableIcon* mMinimizeButton;  ///< Button used to manually minimize the notification.
  ClickableIcon* mCloseButton;     ///< Button used to manually dismiss the notification.

  bool mMinimized;  ///< Whether the widget is expanded or minimized
  qreal mOpacity;   ///< Current opacity value of the widget.

  oclero::qlementine::StatusBadgeWidget* mStatusBadge;

  void toggleMinimized();
  void setMinimized(bool minimized);
};