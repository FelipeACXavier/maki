#pragma once

#include <stdint.h>

#include <QWidget>
#include <chrono>
#include <functional>
#include <string>

#include "logging.h"

namespace notification {

/**
 * @brief Represents the body of a notification which can be either a string or a QWidget pointer.
 */
using NotificationBody = std::variant<std::string, QWidget*>;

/**
 * @brief Global function to set the stream for logging notifications.
 *
 * @param level The log level.
 * @param header The header of the notification.
 * @param message The message of the notification.
 */
extern std::function<void(logging::LogLevel level,
                          const std::string& header,
                          const std::string& message)>
    gNotificationStream;

/**
 * @brief Global function to set the stream for long-running notifications.
 *
 * @param id The unique identifier for the long-running notification.
 * @param level The log level.
 * @param header The header of the notification.
 * @param body The body of the notification, which can be a QWidget pointer.
 * @return QString The formatted notification message.
 */
extern std::function<QString(const QString& id,
                             logging::LogLevel level,
                             const std::string& header,
                             QWidget* body)>
    gLongNotificationStream;

/**
 * @brief Notify the user with a message at a specified log level.
 *
 * @param level The log level.
 * @param header The header of the notification.
 * @param message The message of the notification, which can be either a string or a QWidget pointer.
 */
void Notify(logging::LogLevel level, const std::string& header, const NotificationBody& message);

/**
 * @brief Notify the user with a long-running message at a specified log level.
 *
 * @param id The unique identifier for the long-running notification.
 * @param level The log level.
 * @param header The header of the notification.
 * @param body The body of the notification, which can be either a string or a QWidget pointer.
 * @return QString The formatted notification message.
 */
QString NotifyLongRunning(const QString& id, logging::LogLevel level, const std::string& header, const NotificationBody& body);

/**
 * @brief Format a string using format specifiers and arguments.
 *
 * @param fmt The format string.
 * @param args The arguments to be formatted into the string.
 * @return std::string The formatted string.
 */
template <typename... Args>
std::string format(std::string_view fmt, Args&&... args)
{
  return std::vformat(fmt, std::make_format_args(args...));
}

}  // namespace notification

#define NOTIFY_ERROR(header, s, ...) \
  notification::Notify(logging::LogLevel::Error, header, notification::format(s, ##__VA_ARGS__))
#define NOTIFY_WARNING(header, s, ...) \
  notification::Notify(logging::LogLevel::Warning, header, notification::format(s, ##__VA_ARGS__))
#define NOTIFY_INFO(header, s, ...) \
  notification::Notify(logging::LogLevel::Info, header, notification::format(s, ##__VA_ARGS__))

#define NOTIFY_LONG_ERROR(id, header, content) \
  notification::NotifyLongRunning(id, logging::LogLevel::Error, header, content)
#define NOTIFY_LONG_WARNING(id, header, content) \
  notification::NotifyLongRunning(id, logging::LogLevel::Warning, header, content)
#define NOTIFY_LONG_INFO(id, header, content) \
  notification::NotifyLongRunning(id, logging::LogLevel::Info, header, content)
