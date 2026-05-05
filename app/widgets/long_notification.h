#pragma once

#include "notification_widget.h"

namespace maki
{
/**
 * @brief A notification widget that can display long content.
 */
class LongNotificationWidget : public NotificationWidget
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a new LongNotificationWidget.
   *
   * @param id The unique identifier for the widget.
   * @param title The title of the notification.
   * @param level The logging level associated with the notification.
   * @param parent The parent widget, if any.
   */
  LongNotificationWidget(const QString& id, const QString& title, logging::LogLevel level, QWidget* parent = nullptr);

  /**
   * @brief Updates the content of the notification widget.
   *
   * @param content The new content to display.
   */
  void updateContent(QWidget* content);

  /**
   * @brief Retrieves the unique identifier for the widget.
   *
   * @return The ID as a QString.
   */
  QString id() const;

  /**
   * @brief Checks if the notification is set to disappear automatically.
   *
   * @return True if the notification will disappear, false otherwise.
   */
  bool disappearing() const override;

private:
  /// Unique identifier for the widget.
  const QString mId;
};
}  // namespace maki
