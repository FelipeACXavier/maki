#pragma once

#include <QJsonObject>
#include <QList>

#include "types.h"

class PropertiesConfig
{
public:
  PropertiesConfig();
  PropertiesConfig(const QJsonObject& object);

  QString id = "";
  QVariant defaultValue;
  QList<PropertiesConfig> options;
  Types::PropertyTypes type = Types::PropertyTypes::UNKNOWN;

  QString typeToString() const;

  bool isValid() const;
  void setInvalid(const QString& message);

  friend QDataStream& operator<<(QDataStream& out, const PropertiesConfig& config);
  friend QDataStream& operator>>(QDataStream& in, PropertiesConfig& config);

  bool isConfigValid;
  QString errorMessage;

private:
  Types::PropertyTypes toType(const QString& input);
  QVariant toDefault(const QJsonObject& object, Types::PropertyTypes objectType);
};
