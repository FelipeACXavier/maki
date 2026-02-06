#pragma once

#include <stdint.h>

#include <chrono>
#include <functional>
#include <string>

#include "logging.h"

namespace notification
{

extern std::function<void(logging::LogLevel level,
                          const std::string& header,
                          const std::string& message)>
    gNotificationStream;

void Notify(logging::LogLevel level, const std::string& header, const std::string& message);

}  // namespace notification

#define NOTIFY_ERROR(header, s, ...) \
  notification::Notify(logging::LogLevel::Error, header, Format(s, ##__VA_ARGS__))
#define NOTIFY_WARNING(header, s, ...) \
  notification::Notify(logging::LogLevel::Warning, header, Format(s, ##__VA_ARGS__))
#define NOTIFY_INFO(header, s, ...) \
  notification::Notify(logging::LogLevel::Info, header, Format(s, ##__VA_ARGS__))
