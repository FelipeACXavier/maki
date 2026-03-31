#pragma once
#include <QStringList>

#include "logging.h"

namespace maki
{

typedef std::function<void(std::chrono::system_clock::time_point now, logging::LogLevel level,
                           const std::string& filename, const uint32_t& line,
                           const std::string& message)>
    LogFunction;

class ILogging
{
public:
  virtual ~ILogging() = default;
  virtual void registerPlugin(const QString& name, maki::LogFunction& callback) = 0;
};

}  // namespace maki
