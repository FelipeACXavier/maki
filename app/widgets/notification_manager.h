#pragma once

#include <QList>
#include <QObject>
#include <QPointer>

#include "logging.h"
#include "widgets/notification_widget.h"

/**
 * @class NotificationManager
 * @brief Manages the lifecycle and positioning of notification toasts.
 *
 * This class is responsible for:
 * - Creating and displaying NotificationWidget instances
 * - Tracking active notifications (toasts)
 * - Repositioning notifications when the parent window changes
 * - Handling dismissal of notifications
 *
 * It installs an event filter on the parent window to react to resize/move events.
 */
class NotificationManager : public QObject
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a notification manager.
   *
   * @param parentWindow The window relative to which notifications are positioned.
   * @param parent The parent QObject.
   */
  explicit NotificationManager(QWidget* parentWindow, QObject* parent = nullptr);

public slots:
  /**
   * @brief Displays a new notification toast.
   *
   * Creates a NotificationWidget and adds it to the managed list,
   * positioning it relative to the parent window.
   *
   * @param header The title of the notification.
   * @param text The message body of the notification.
   * @param level The log level determining the visual style.
   */
  void showNotification(const QString& header, const QString& text, logging::LogLevel level);

  /**
   * @brief Displays a new long notification toast.
   *
   * Creates a LongNotificationWidget and adds it to the managed list,
   * positioning it relative to the parent window.
   *
   * @param id The id of the long notification widget
   * @param header The title of the notification.
   * @param contents The body of the notification.
   * @param level The log level determining the visual style.
   * @return QString The ID of the created long notification.
   */
  QString showLongNotification(const QString& id, const QString& header, QWidget* contents, logging::LogLevel level);

private slots:
  /**
   * @brief Handles dismissal of a notification toast.
   *
   * Removes the toast from the internal list and updates layout.
   *
   * @param toast The notification widget that was dismissed.
   */
  void onToastDismissed(NotificationWidget* toast);

private:
  /**
   * @brief Repositions all active notification toasts.
   *
   * Typically called after adding/removing a toast or when the parent window changes size.
   */
  void repositionToasts();

  /**
   * @brief Event filter to track parent window changes.
   *
   * Used to reposition notifications when the parent window is resized or moved.
   *
   * @param watched The object being observed.
   * @param event The event that occurred.
   * @return True if the event was handled, false otherwise.
   */
  bool eventFilter(QObject* watched, QEvent* event);

  /**
   * @brief Updates an existing notification with new contents.
   *
   * @param id The ID of the notification to update.
   * @param contents The new body of the notification.
   * @return QString The updated notification's ID.
   */
  QString updateExistingNotification(const QString& id, QWidget* contents, logging::LogLevel level);

  QPointer<QWidget> mParentWindow;     ///< Weak pointer to the parent window used for positioning.
  QList<NotificationWidget*> mToasts;  ///< List of active notification toasts.
};
