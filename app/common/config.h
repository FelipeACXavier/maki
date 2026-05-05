#pragma once

#include <QColor>
#include <QJsonObject>
#include <QString>
#include <QVector>

#include "config_base.h"
#include "types.h"

/**
 * @brief Configuration for a property.
 */
class PropertyConfig : public ConfigBase
{
public:
  /**
   * @brief Default constructor.
   */
  PropertyConfig();

  /**
   * @brief Constructor from JSON object.
   * @param object The JSON object to initialize the configuration from.
   */
  PropertyConfig(const QJsonObject& object);

  QString id = "";  /// Unique identifier for the property.
  QVariant defaultValue;  /// Default value of the property.
  QList<PropertyConfig> options = {};  /// List of possible options for the property.
  Types::PropertyTypes type = Types::PropertyTypes::UNKNOWN;  /// Type of the property.
  QString info = "";  /// Additional information about the property.

  friend QDataStream& operator<<(QDataStream& out, const PropertyConfig& config);
  friend QDataStream& operator>>(QDataStream& in, PropertyConfig& config);

private:
  QVariant toDefault(const QJsonObject& object, Types::PropertyTypes objectType);  /// Convert JSON object to default value.
};

/**
 * @brief Configuration for a transition between nodes.
 */
class TransitionConfig : public ConfigBase
{
public:
  /**
   * @brief Default constructor.
   */
  TransitionConfig();

  /**
   * @brief Constructor from JSON object.
   * @param object The JSON object to initialize the configuration from.
   */
  TransitionConfig(const QJsonObject& object);

  QString id = "";  /// Unique identifier for the transition.
  QString label = "";  /// Label of the transition.
  QString event = "";  /// Event that triggers the transition.
  bool modifiable = true;  /// Whether the transition is modifiable.

  friend QDataStream& operator<<(QDataStream& out, const TransitionConfig& config);
  friend QDataStream& operator>>(QDataStream& in, TransitionConfig& config);
};

/**
 * @brief Configuration for a flow or process.
 */
class FlowConfig : public ConfigBase
{
public:
  /**
   * @brief Default constructor.
   */
  FlowConfig();

  /**
   * @brief Constructor from JSON object.
   * @param object The JSON object to initialize the configuration from.
   */
  FlowConfig(const QJsonObject& object);

  QString name = "";  /// Name of the flow.
  Types::CallType type = Types::CallType::UNKNOWN;  /// Type of the call.
  Types::PropertyTypes returnType = Types::PropertyTypes::UNKNOWN;  /// Return type of the call.
  QList<PropertyConfig> arguments = {};  /// List of arguments for the call.
  bool modifiable = true;  /// Whether the flow is modifiable.
  QString info = "";  /// Additional information about the flow.
  int linksTo = INT32_MAX;  /// Number of links to this flow.

  friend QDataStream& operator<<(QDataStream& out, const FlowConfig& config);
  friend QDataStream& operator>>(QDataStream& in, FlowConfig& config);
};

/**
 * @brief Configuration for controls within a node.
 */
class ControlsConfig : public ConfigBase
{
public:
  /**
   * @brief Default constructor.
   */
  ControlsConfig();

  /**
   * @brief Constructor from JSON object.
   * @param object The JSON object to initialize the configuration from.
   */
  ControlsConfig(const QJsonObject& object);

  QString id = "";  /// Unique identifier for the control.
  Types::ControlTypes type = Types::ControlTypes::UNKNOWN;  /// Type of the control.
  QString format = "";  /// Format of the control.

  friend QDataStream& operator<<(QDataStream& out, const ControlsConfig& config);
  friend QDataStream& operator>>(QDataStream& in, ControlsConfig& config);
};

/**
 * @brief Configuration for a behaviour associated with a node.
 */
class BehaviourConfig : public ConfigBase
{
public:
  /**
   * @brief Default constructor.
   */
  BehaviourConfig();

  /**
   * @brief Constructor from JSON object.
   * @param object The JSON object to initialize the configuration from.
   */
  BehaviourConfig(const QJsonObject& object);

  QString code = "";  /// Code associated with the behaviour.

  friend QDataStream& operator<<(QDataStream& out, const BehaviourConfig& config);
  friend QDataStream& operator>>(QDataStream& in, BehaviourConfig& config);
};

/**
 * @brief Configuration for help information.
 */
class HelpConfig : public ConfigBase
{
public:
  /**
   * @brief Default constructor.
   */
  HelpConfig();

  /**
   * @brief Constructor from JSON object.
   * @param object The JSON object to initialize the configuration from.
   */
  HelpConfig(const QJsonObject& object);

  QString message = "";  /// Message containing help information.

  friend QDataStream& operator<<(QDataStream& out, const HelpConfig& config);
  friend QDataStream& operator>>(QDataStream& in, HelpConfig& config);
};

/**
 * @brief Configuration for the body of a node.
 */
class BodyConfig : public ConfigBase
{
public:
  /**
   * @brief Default constructor.
   */
  BodyConfig();

  /**
   * @brief Constructor from JSON object.
   * @param object The JSON object to initialize the configuration from.
   */
  BodyConfig(const QJsonObject& object);

  Types::Shape shape = Types::Shape::ROUNDED_RECTANGLE;  /// Shape of the body.

  QColor textColor = Qt::black;  /// Text color.
  QColor iconColor = Qt::black;  /// Icon color.
  QColor backgroundColor = Qt::lightGray;  /// Background color.
  QColor borderColor = Qt::black;  /// Border color.

  int width = 100;  /// Width of the body.
  int height = 50;  /// Height of the body.
  int zIndex = 1;  /// Z-index for stacking order.
  int borderRadius = 10;  /// Border radius for rounded corners.

  QString iconPath = "";  /// Path to the icon image.
  qreal iconScale = 1;  /// Scale factor for the icon.

  friend QDataStream& operator<<(QDataStream& out, const BodyConfig& config);
  friend QDataStream& operator>>(QDataStream& in, BodyConfig& config);

private:
  Types::Shape toShape(const QString& config) const;  /// Convert string representation of shape to enum.
};

/**
 * @brief Configuration for a node.
 */
class NodeConfig : public ConfigBase
{
public:
  /**
   * @brief Default constructor.
   */
  NodeConfig();

  /**
   * @brief Constructor from JSON object.
   * @param object The JSON object to initialize the configuration from.
   */
  NodeConfig(const QJsonObject& object);

  QString type;  /// Type of the node.
  BodyConfig body;  /// Configuration for the body of the node.
  HelpConfig help;  /// Configuration for help information.
  BehaviourConfig behaviour;  /// Configuration for the behaviour associated with the node.
  QVector<ControlsConfig> controls = {};  /// List of controls within the node.
  QVector<PropertyConfig> properties = {};  /// List of properties associated with the node.
  QVector<FlowConfig> events = {};  /// List of events associated with the node.
  QVector<TransitionConfig> transitions = {};  /// List of transitions associated with the node.

  Types::LibraryTypes libraryType = Types::LibraryTypes::UNKNOWN;  /// Type of the library.

  friend QDataStream& operator<<(QDataStream& out, const NodeConfig& config);
  friend QDataStream& operator>>(QDataStream& in, NodeConfig& config);
};
