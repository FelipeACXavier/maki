#pragma once

#include <QDataStream>
#include <QString>

class ConfigBase
{
public:
  ConfigBase();
  ConfigBase(ConfigBase& copy);
  ConfigBase(const ConfigBase& copy);

  friend QDataStream& operator<<(QDataStream& out, const ConfigBase& config);
  friend QDataStream& operator>>(QDataStream& in, ConfigBase& config);

  bool isValid() const;

  bool isConfigValid;
  QString errorMessage;

protected:
  void setInvalid(const QString& message);
};
