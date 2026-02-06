#include "notifications.h"

#include <mutex>
#include <sstream>

namespace notification
{

std::function<void(logging::LogLevel level, const std::string& header, const std::string& message)> gNotificationStream = nullptr;

void Notify(logging::LogLevel level, const std::string& header, const std::string& message)
{
  if (gNotificationStream)
    gNotificationStream(level, header, message);
}

}  // namespace notification
