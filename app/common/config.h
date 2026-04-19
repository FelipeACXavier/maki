#pragma once

#include <QColor>
#include <QJsonObject>
#include <QString>
#include <QVector>

#include "config_base.h"
#include "types.h"

class PropertyConfig : public ConfigBase
{
public:
  PropertyConfig();
  PropertyConfig(const QJsonObject& object);

  QString id = "";
  QVariant defaultValue;
  QList<PropertyConfig> options = {};
  Types::PropertyTypes type = Types::PropertyTypes::UNKNOWN;
  QString info = "";

  friend QDataStream& operator<<(QDataStream& out, const PropertyConfig& config);
  friend QDataStream& operator>>(QDataStream& in, PropertyConfig& config);

private:
  QVariant toDefault(const QJsonObject& object, Types::PropertyTypes objectType);
};

class TransitionConfig : public ConfigBase
{
public:
  TransitionConfig();
  TransitionConfig(const QJsonObject& object);

  QString id = "";
  QString label = "";
  QString event = "";
  bool modifiable = true;

  friend QDataStream& operator<<(QDataStream& out, const TransitionConfig& config);
  friend QDataStream& operator>>(QDataStream& in, TransitionConfig& config);
};

class FlowConfig : public ConfigBase
{
public:
  FlowConfig();
  FlowConfig(const QJsonObject& object);

  QString name = "";
  Types::CallType type = Types::CallType::UNKNOWN;
  Types::PropertyTypes returnType = Types::PropertyTypes::UNKNOWN;
  QList<PropertyConfig> arguments = {};
  bool modifiable = true;
  QString info = "";
  int linksTo = INT32_MAX;

  friend QDataStream& operator<<(QDataStream& out, const FlowConfig& config);
  friend QDataStream& operator>>(QDataStream& in, FlowConfig& config);
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
  QColor iconColor = Qt::black;
  QColor backgroundColor = Qt::lightGray;
  QColor borderColor = Qt::black;

  int width = 100;
  int height = 50;
  int zIndex = 1;
  int borderRadius = 10;

  QString iconPath = "";
  // nodeSvg replaces shape + icon if set
  QString nodeSvg = "";
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
  QVector<PropertyConfig> properties;
  QVector<FlowConfig> events;
  QVector<TransitionConfig> transitions;

  Types::LibraryTypes libraryType = Types::LibraryTypes::UNKNOWN;

  friend QDataStream& operator<<(QDataStream& out, const NodeConfig& config);
  friend QDataStream& operator>>(QDataStream& in, NodeConfig& config);
};
