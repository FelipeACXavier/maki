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

  bool isValid() const;
  void setInvalid(const QString& message);

  QJsonObject toJson() const;
  static PropertiesConfig fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const PropertiesConfig& config);
  friend QDataStream& operator>>(QDataStream& in, PropertiesConfig& config);

  bool isConfigValid;
  QString errorMessage;

private:
  QVariant toDefault(const QJsonObject& object, Types::PropertyTypes objectType);
};
