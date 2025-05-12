#pragma once

#include <QJsonObject>
#include <QList>

#include "property_config.h"
#include "types.h"

class EventConfig
{
public:
  EventConfig();
  EventConfig(const QJsonObject& object);

  QString id = "";
  Types::ConnectorType type = Types::ConnectorType::UNKNOWN;
  Types::PropertyTypes returnType = Types::PropertyTypes::UNKNOWN;
  QList<PropertiesConfig> arguments = {};

  bool isValid() const;
  void setInvalid(const QString& message);

  QJsonObject toJson() const;
  static EventConfig fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const EventConfig& config);
  friend QDataStream& operator>>(QDataStream& in, EventConfig& config);

  bool isConfigValid;
  QString errorMessage;
};
