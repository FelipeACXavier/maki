#include "notifications.h"

#include <mutex>
#include <sstream>

namespace notification
{

std::function<void(logging::LogLevel level, const std::string& header, const std::string& message)> gNotificationStream = nullptr;
std::function<QString(const QString& id, logging::LogLevel level, const std::string& header, QWidget* body)> gLongNotificationStream = nullptr;

void Notify(logging::LogLevel level, const std::string& header, const NotificationBody& body)
{
  if (!gNotificationStream)
    return;

  if (std::holds_alternative<std::string>(body))
  {
    const auto& message = std::get<std::string>(body);
    gNotificationStream(level, header, message);
  }
}

QString NotifyLongRunning(const QString& id, logging::LogLevel level, const std::string& header, const NotificationBody& body)
{
  if (!gLongNotificationStream)
    return QString();

  if (std::holds_alternative<QWidget*>(body))
  {
    auto widget = std::get<QWidget*>(body);
    return gLongNotificationStream(id, level, header, widget);
  }

  return QString();
}

}  // namespace notification
