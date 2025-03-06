#include "config_base.h"

#include <QString>

ConfigBase::ConfigBase()
    : isConfigValid(true)
    , errorMessage("")
{
}

ConfigBase::ConfigBase(ConfigBase& copy)
    : isConfigValid(copy.isConfigValid)
    , errorMessage(copy.errorMessage)
{
}

ConfigBase::ConfigBase(const ConfigBase& copy)
    : isConfigValid(copy.isConfigValid)
    , errorMessage(copy.errorMessage)
{
}

bool ConfigBase::isValid() const
{
  return isConfigValid;
}

void ConfigBase::setInvalid(const QString& message)
{
  isConfigValid = false;
  errorMessage = message;
}

// ===========================================================================================================
// ConfigBase
QDataStream& operator<<(QDataStream& out, const ConfigBase& config)
{
  out << config.isConfigValid;
  out << config.errorMessage;

  return out;
}

QDataStream& operator>>(QDataStream& in, ConfigBase& config)
{
  in >> config.isConfigValid;
  in >> config.errorMessage;

  return in;
}
