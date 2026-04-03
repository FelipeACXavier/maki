#pragma once

#include <stdint.h>

#include <chrono>
#include <format>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>

#include "string_helpers.h"

namespace logging
{

enum class LogLevel
{
  Error,
  Warning,
  Info,
  Debugging,
  Trace
};

extern bool gSilentLog;
extern logging::LogLevel gMinLogLevel;
extern std::function<void(std::chrono::system_clock::time_point now, logging::LogLevel level,
                          const std::string& filename, const uint32_t& line,
                          const std::string& message)>
    gLogToStream;

std::string LevelToString(LogLevel logLevel);
std::tm ToLocalTm(std::time_t now);
std::string TimeToString(std::chrono::system_clock::time_point tp);

void Print(std::chrono::system_clock::time_point now, LogLevel level, const std::string& filename,
           const uint32_t& line, const std::string& message);
void Log(LogLevel level, const std::string& filename, const uint32_t& line,
         const std::string& message);

template <typename... Args>
void logRaw(std::string_view fmt, Args&&... args)
{
  std::cout << std::vformat(fmt, std::make_format_args(args...)) << std::endl;
}

}  // namespace logging

#define LOG_ERROR(s, ...)                                    \
  logging::Log(logging::LogLevel::Error, __FILE__, __LINE__, \
               Format(s, ##__VA_ARGS__))
#define LOG_WARNING(s, ...)                                    \
  logging::Log(logging::LogLevel::Warning, __FILE__, __LINE__, \
               Format(s, ##__VA_ARGS__))
#define LOG_INFO(s, ...)                                    \
  logging::Log(logging::LogLevel::Info, __FILE__, __LINE__, \
               Format(s, ##__VA_ARGS__))
#define LOG_INFO_RAW(f, l, s, ...) \
  logging::Log(logging::LogLevel::Info, f, l, Format(s, ##__VA_ARGS__))
#define LOG_DEBUG(s, ...)                                        \
  logging::Log(logging::LogLevel::Debugging, __FILE__, __LINE__, \
               Format(s, ##__VA_ARGS__))
#define LOG_TRACE(s, ...)                                    \
  logging::Log(logging::LogLevel::Trace, __FILE__, __LINE__, \
               Format(s, ##__VA_ARGS__))
#define LOG_RAW(s, ...) \
  logging::logRaw(s, ##__VA_ARGS__)
