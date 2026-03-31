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
