#pragma once

#include <stdint.h>

#include <QWidget>
#include <chrono>
#include <functional>
#include <string>

#include "logging.h"

namespace notification
{

using NotificationBody = std::variant<std::string, QWidget*>;

extern std::function<void(logging::LogLevel level,
                          const std::string& header,
                          const std::string& message)>
    gNotificationStream;
extern std::function<QString(const QString& id,
                             logging::LogLevel level,
                             const std::string& header,
                             QWidget* body)>
    gLongNotificationStream;

void Notify(logging::LogLevel level, const std::string& header, const NotificationBody& message);
QString NotifyLongRunning(const QString& id, logging::LogLevel level, const std::string& header, const NotificationBody& body);

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
