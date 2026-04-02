#include "logger.h"

Logger::Logger(QObject* parent)
    : QObject(parent)
{
}

void Logger::registerPlugin(const QString& /* name */, maki::LogFunction& callback)
{
  callback = logging::gLogToStream;
}
