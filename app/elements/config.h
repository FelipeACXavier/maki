#pragma once

#include <QColor>
#include <QJsonObject>
#include <QString>
#include <QVector>

#include "result.h"
#include "types.h"

class ConfigBase
{
public:
  ConfigBase();
  ConfigBase(ConfigBase& copy);
  ConfigBase(const ConfigBase& copy);

  virtual bool isValid() const;
  VoidResult result() const;

protected:
  void setInvalid(const QString& message);

private:
  VoidResult mIsValid;
};

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

QDataStream& operator<<(QDataStream& out, const ConnectorConfig& config);
QDataStream& operator>>(QDataStream& in, ConnectorConfig& config);

class ControlConfig : public ConfigBase
{
public:
  ControlConfig();
  ControlConfig(const QJsonObject& object);

  QString id = "";
  Types::ControlTypes type = Types::ControlTypes::UNKNOWN;
  QString format = "";

private:
  Types::ControlTypes toType(const QString& config) const;
};

class ControlsConfig : public ConfigBase
{
public:
  ControlsConfig();
  ControlsConfig(const QJsonObject& object);

  QString id = "";
  Types::ControlTypes type = Types::ControlTypes::UNKNOWN;
  QString format = "";

private:
  Types::ControlTypes toType(const QString& config) const;
};

class PropertiesConfig : public ConfigBase
{
public:
  PropertiesConfig();
  PropertiesConfig(const QJsonObject& object);

  QString id = "";
  QVariant defaultValue;
  QList<PropertiesConfig> options;
  Types::PropertyTypes type = Types::PropertyTypes::UNKNOWN;

  bool isValid() const override;

  QString typeToString() const;

private:
  Types::PropertyTypes toType(const QString& input);
  QVariant toDefault(const QJsonObject& object, Types::PropertyTypes objectType);
};

class BehaviourConfig : public ConfigBase
{
public:
  BehaviourConfig();
  BehaviourConfig(const QJsonObject& object);

  QString code = "";
};

class HelpConfig : public ConfigBase
{
public:
  HelpConfig();
  HelpConfig(const QJsonObject& object);

  QString message = "";
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

QDataStream& operator<<(QDataStream& out, const BodyConfig& config);
QDataStream& operator>>(QDataStream& in, BodyConfig& config);

class NodeConfig : public ConfigBase
{
public:
  NodeConfig(const QJsonObject& object);

  QString type;
  BodyConfig body;
  HelpConfig help;
  BehaviourConfig behaviour;
  QVector<ControlsConfig> controls;
  QVector<ConnectorConfig> connectors;
  QVector<PropertiesConfig> properties;

  Types::LibraryTypes libraryType = Types::LibraryTypes::UNKNOWN;

  friend QDataStream& operator<<(QDataStream& out, const NodeConfig& config);
  friend QDataStream& operator>>(QDataStream& in, NodeConfig& config);
};
