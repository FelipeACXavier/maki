#pragma once

#include <QColor>
#include <QJsonObject>
#include <QString>
#include <QVector>

#include "config_base.h"
#include "event_config.h"
#include "property_config.h"
#include "types.h"

class ConnectorConfig : public ConfigBase
{
public:
  ConnectorConfig();
  ConnectorConfig(const QJsonObject& object);

  QString id = "";
  QString position = "";
  Types::ConnectorType type = Types::ConnectorType::UNKNOWN;

  QPointF getPosition(const QRectF& bounds) const;
  QPointF getShift(const QString& config = "") const;
  QPointF getMirrorShift() const;

  friend QDataStream& operator<<(QDataStream& out, const ConnectorConfig& config);
  friend QDataStream& operator>>(QDataStream& in, ConnectorConfig& config);

private:
  Types::ConnectorType fromString(const QString& config) const;
};

class ControlsConfig : public ConfigBase
{
public:
  ControlsConfig();
  ControlsConfig(const QJsonObject& object);

  QString id = "";
  Types::ControlTypes type = Types::ControlTypes::UNKNOWN;
  QString format = "";

  friend QDataStream& operator<<(QDataStream& out, const ControlsConfig& config);
  friend QDataStream& operator>>(QDataStream& in, ControlsConfig& config);

private:
  Types::ControlTypes toType(const QString& config) const;
};

class BehaviourConfig : public ConfigBase
{
public:
  BehaviourConfig();
  BehaviourConfig(const QJsonObject& object);

  QString code = "";

  friend QDataStream& operator<<(QDataStream& out, const BehaviourConfig& config);
  friend QDataStream& operator>>(QDataStream& in, BehaviourConfig& config);
};

class HelpConfig : public ConfigBase
{
public:
  HelpConfig();
  HelpConfig(const QJsonObject& object);

  QString message = "";

  friend QDataStream& operator<<(QDataStream& out, const HelpConfig& config);
  friend QDataStream& operator>>(QDataStream& in, HelpConfig& config);
};

class BodyConfig : public ConfigBase
{
public:
  BodyConfig();
  BodyConfig(const QJsonObject& object);

  Types::Shape shape = Types::Shape::ROUNDED_RECTANGLE;

  QColor textColor = Qt::black;
  QColor backgroundColor = Qt::lightGray;
  QColor borderColor = Qt::black;

  int width = 100;
  int height = 50;
  int zIndex = 1;
  int borderRadius = 10;

  QString iconPath = "";
  qreal iconScale = 1;

  friend QDataStream& operator<<(QDataStream& out, const BodyConfig& config);
  friend QDataStream& operator>>(QDataStream& in, BodyConfig& config);

private:
  Types::Shape toShape(const QString& config) const;
};

class NodeConfig : public ConfigBase
{
public:
  NodeConfig();
  NodeConfig(const QJsonObject& object);

  QString type;
  BodyConfig body;
  HelpConfig help;
  BehaviourConfig behaviour;
  QVector<ControlsConfig> controls;
  QVector<ConnectorConfig> connectors;
  QVector<PropertiesConfig> properties;
  QVector<EventConfig> events;

  Types::LibraryTypes libraryType = Types::LibraryTypes::UNKNOWN;

  friend QDataStream& operator<<(QDataStream& out, const NodeConfig& config);
  friend QDataStream& operator>>(QDataStream& in, NodeConfig& config);
};
