#include "save_info.h"

#include <QBuffer>
#include <QJsonArray>

#include "json.h"
#include "keys.h"
#include "logging.h"

Q_DECLARE_METATYPE(TransitionSaveInfo)
Q_DECLARE_METATYPE(ConnectorSaveInfo)
Q_DECLARE_METATYPE(ConnectionSaveInfo)
Q_DECLARE_METATYPE(NodeSaveInfo)
Q_DECLARE_METATYPE(SaveInfo)

// ==========================================================================================================
// TransitionSaveInfo
QDataStream& operator<<(QDataStream& out, const TransitionSaveInfo& info)
{
  out << info.srcId;
  out << info.srcPoint;
  out << info.srcShift;

  out << info.dstId;
  out << info.dstPoint;
  out << info.dstShift;

  return out;
}

QDataStream& operator>>(QDataStream& in, TransitionSaveInfo& info)
{
  in >> info.srcId;
  in >> info.srcPoint;
  in >> info.srcShift;

  in >> info.dstId;
  in >> info.dstPoint;
  in >> info.dstShift;

  return in;
}

QJsonObject TransitionSaveInfo::toJson() const
{
  QJsonObject data;

  QJsonObject source;
  source[ConfigKeys::ID] = srcId;
  source[ConfigKeys::POSITION] = JSON::fromPointF(srcPoint);
  source[ConfigKeys::SHIFT] = JSON::fromPointF(srcShift);
  data[ConfigKeys::SOURCE] = source;

  QJsonObject destination;
  destination[ConfigKeys::ID] = dstId;
  destination[ConfigKeys::POSITION] = JSON::fromPointF(dstPoint);
  destination[ConfigKeys::SHIFT] = JSON::fromPointF(dstShift);
  data[ConfigKeys::DESTINATION] = destination;

  return data;
}

TransitionSaveInfo TransitionSaveInfo::fromJson(const QJsonObject& data)
{
  TransitionSaveInfo info;

  info.srcId = data[ConfigKeys::SOURCE][ConfigKeys::ID].toString();
  info.srcPoint = JSON::toPointF(data[ConfigKeys::SOURCE][ConfigKeys::POSITION].toObject());
  info.srcShift = JSON::toPointF(data[ConfigKeys::SOURCE][ConfigKeys::SHIFT].toObject());

  info.dstId = data[ConfigKeys::DESTINATION][ConfigKeys::ID].toString();
  info.dstPoint = JSON::toPointF(data[ConfigKeys::DESTINATION][ConfigKeys::POSITION].toObject());
  info.dstShift = JSON::toPointF(data[ConfigKeys::DESTINATION][ConfigKeys::SHIFT].toObject());

  return info;
}

// ==========================================================================================================
// ConnectorSaveInfo
QDataStream& operator<<(QDataStream& out, const ConnectorSaveInfo& info)
{
  out << info.connectorId;
  out << info.configId;

  return out;
}

QDataStream& operator>>(QDataStream& in, ConnectorSaveInfo& info)
{
  in >> info.connectorId;
  in >> info.configId;

  return in;
}

QJsonObject ConnectorSaveInfo::toJson() const
{
  QJsonObject data;
  data[ConfigKeys::CONFIG_ID] = configId;
  data[ConfigKeys::CONNECTOR_ID] = connectorId;

  return data;
}

ConnectorSaveInfo ConnectorSaveInfo::fromJson(const QJsonObject& data)
{
  ConnectorSaveInfo info;
  info.configId = data[ConfigKeys::CONFIG_ID].toString();
  info.connectorId = data[ConfigKeys::CONNECTOR_ID].toString();

  return info;
}

// ==========================================================================================================
// NodeSaveInfo
QDataStream& operator<<(QDataStream& out, const QVector<NodeSaveInfo>& nodes)
{
  out << static_cast<qint32>(nodes.size());
  for (const auto& node : nodes)
    out << node;

  return out;
}

QDataStream& operator>>(QDataStream& in, QVector<NodeSaveInfo>& nodes)
{
  qint32 size;
  in >> size;

  nodes.resize(size);
  for (int i = 0; i < size; ++i)
    in >> nodes[i];

  return in;
}

QDataStream& operator<<(QDataStream& out, const NodeSaveInfo& info)
{
  out << info.id;
  out << info.size;
  out << info.scale;
  out << info.nodeId;
  out << info.fields;
  out << info.events;
  out << info.position;
  out << info.connectors;
  out << info.properties;
  out << info.parentId;

  QByteArray pixmapData;
  QBuffer buffer(&pixmapData);
  buffer.open(QIODevice::WriteOnly);
  info.pixmap.save(&buffer, "PNG");

  out << pixmapData;

  return out;
}

QDataStream& operator>>(QDataStream& in, NodeSaveInfo& info)
{
  in >> info.id;
  in >> info.size;
  in >> info.scale;
  in >> info.nodeId;
  in >> info.fields;
  in >> info.events;
  in >> info.position;
  in >> info.connectors;
  in >> info.properties;
  in >> info.parentId;

  QByteArray pixmapData;
  in >> pixmapData;
  info.pixmap.loadFromData(pixmapData, "PNG");

  return in;
}

QJsonObject NodeSaveInfo::toJson() const
{
  QJsonObject data;

  data[ConfigKeys::ID] = id;
  data[ConfigKeys::NODE_ID] = nodeId;
  data[ConfigKeys::PARENT_ID] = parentId;

  data[ConfigKeys::SCALE] = scale;
  data[ConfigKeys::SIZE] = JSON::fromSizeF(size);
  data[ConfigKeys::POSITION] = JSON::fromPointF(position);

  QJsonArray connectorArray;
  for (const auto& connector : connectors)
    connectorArray.append(connector.toJson());

  QJsonArray fieldArray;
  for (const auto& field : fields)
    fieldArray.append(field.toJson());

  QJsonArray eventArray;
  for (const auto& event : events)
    eventArray.append(event.toJson());

  QJsonObject propertiesObject;
  for (auto it = properties.constBegin(); it != properties.constEnd(); ++it)
    propertiesObject[it.key()] = it.value().toJsonValue();

  if (properties.size() > 0)
    data[ConfigKeys::PROPERTIES] = propertiesObject;
  if (connectorArray.size() > 0)
    data[ConfigKeys::CONNECTORS] = connectorArray;
  if (fieldArray.size() > 0)
    data[ConfigKeys::FIELDS] = fieldArray;
  if (eventArray.size() > 0)
    data[ConfigKeys::EVENTS] = eventArray;

  data[ConfigKeys::PIXMAP] = JSON::fromPixmap(pixmap);

  // Currently, we don't save the children.

  return data;
}

NodeSaveInfo NodeSaveInfo::fromJson(const QJsonObject& data)
{
  NodeSaveInfo info;

  // First parse the mandatory arguments
  info.id = data[ConfigKeys::ID].toString();
  info.nodeId = data[ConfigKeys::NODE_ID].toString();

  info.scale = data[ConfigKeys::SCALE].toDouble();
  info.size = JSON::toSizeF(data[ConfigKeys::SIZE].toObject());
  info.position = JSON::toPointF(data[ConfigKeys::POSITION].toObject());

  if (data.contains(ConfigKeys::PARENT_ID))
    info.parentId = data[ConfigKeys::PARENT_ID].toString();

  if (data.contains(ConfigKeys::FIELDS))
  {
    for (const auto& node : data[ConfigKeys::FIELDS].toArray())
      info.fields.append(PropertiesConfig::fromJson(node.toObject()));
  }

  if (data.contains(ConfigKeys::EVENTS))
  {
    for (const auto& node : data[ConfigKeys::EVENTS].toArray())
      info.events.append(EventConfig::fromJson(node.toObject()));
  }

  if (data.contains(ConfigKeys::CONNECTORS))
  {
    for (const auto& node : data[ConfigKeys::CONNECTORS].toArray())
      info.connectors.append(ConnectorSaveInfo::fromJson(node.toObject()));
  }

  if (data.contains(ConfigKeys::PROPERTIES))
  {
    const auto propertiesObject = data[ConfigKeys::PROPERTIES].toObject();
    for (const QString& key : propertiesObject.keys())
      info.properties[key] = propertiesObject.value(key);
  }

  info.pixmap = JSON::toPixmap(data[ConfigKeys::PIXMAP].toObject());

  return info;
}

// ==========================================================================================================
// ConnectionSaveInfo
QDataStream& operator<<(QDataStream& out, const ConnectionSaveInfo& info)
{
  out << info.srcId;
  out << info.srcPoint;
  out << info.srcShift;

  out << info.dstId;
  out << info.dstPoint;
  out << info.dstShift;

  return out;
}

QDataStream& operator>>(QDataStream& in, ConnectionSaveInfo& info)
{
  in >> info.srcId;
  in >> info.srcPoint;
  in >> info.srcShift;

  in >> info.dstId;
  in >> info.dstPoint;
  in >> info.dstShift;

  return in;
}

QJsonObject ConnectionSaveInfo::toJson() const
{
  QJsonObject data;

  QJsonObject source;
  source[ConfigKeys::ID] = srcId;
  source[ConfigKeys::POSITION] = JSON::fromPointF(srcPoint);
  source[ConfigKeys::SHIFT] = JSON::fromPointF(srcShift);
  data[ConfigKeys::SOURCE] = source;

  QJsonObject destination;
  destination[ConfigKeys::ID] = dstId;
  destination[ConfigKeys::POSITION] = JSON::fromPointF(dstPoint);
  destination[ConfigKeys::SHIFT] = JSON::fromPointF(dstShift);
  data[ConfigKeys::DESTINATION] = destination;

  return data;
}

ConnectionSaveInfo ConnectionSaveInfo::fromJson(const QJsonObject& data)
{
  ConnectionSaveInfo info;

  info.srcId = data[ConfigKeys::SOURCE][ConfigKeys::ID].toString();
  info.srcPoint = JSON::toPointF(data[ConfigKeys::SOURCE][ConfigKeys::POSITION].toObject());
  info.srcShift = JSON::toPointF(data[ConfigKeys::SOURCE][ConfigKeys::SHIFT].toObject());

  info.dstId = data[ConfigKeys::DESTINATION][ConfigKeys::ID].toString();
  info.dstPoint = JSON::toPointF(data[ConfigKeys::DESTINATION][ConfigKeys::POSITION].toObject());
  info.dstShift = JSON::toPointF(data[ConfigKeys::DESTINATION][ConfigKeys::SHIFT].toObject());

  return info;
}

// ==========================================================================================================
// CanvasSaveInfo
QDataStream& operator<<(QDataStream& out, const CanvasSaveInfo& info)
{
  out << info.scale;
  out << info.center;

  return out;
}

QDataStream& operator>>(QDataStream& in, CanvasSaveInfo& info)
{
  in >> info.scale;
  in >> info.center;

  return in;
}

QJsonObject CanvasSaveInfo::toJson() const
{
  QJsonObject data;

  data[ConfigKeys::POSITION] = JSON::fromPointF(center);
  data[ConfigKeys::SCALE] = scale;

  return data;
}

CanvasSaveInfo CanvasSaveInfo::fromJson(const QJsonObject& data)
{
  CanvasSaveInfo info;

  info.center = JSON::toPointF(data[ConfigKeys::POSITION].toObject());
  info.scale = data[ConfigKeys::SCALE].toDouble();

  return info;
}

// ==========================================================================================================
// SaveInfo
QDataStream& operator<<(QDataStream& out, const SaveInfo& info)
{
  out << info.canvasInfo;
  out << info.structuralNodes;
  out << info.behaviouralNodes;
  out << info.connections;
  out << info.transitions;

  return out;
}

QDataStream& operator>>(QDataStream& in, SaveInfo& info)
{
  in >> info.canvasInfo;
  in >> info.structuralNodes;
  in >> info.behaviouralNodes;
  in >> info.connections;
  in >> info.transitions;

  return in;
}

QJsonObject SaveInfo::toJson() const
{
  QJsonObject data;

  data[ConfigKeys::CANVAS] = canvasInfo.toJson();

  QJsonArray structuralArray;
  for (const auto& node : structuralNodes)
    structuralArray.append(node.toJson());

  QJsonArray behaviouralArray;
  for (const auto& node : behaviouralNodes)
    behaviouralArray.append(node.toJson());

  QJsonArray connectionArray;
  for (const auto& node : connections)
    connectionArray.append(node.toJson());

  QJsonArray transitionArray;
  for (const auto& node : transitions)
    transitionArray.append(node.toJson());

  if (structuralArray.size() > 0)
    data[ConfigKeys::STRUCTURAL] = structuralArray;
  if (behaviouralArray.size() > 0)
    data[ConfigKeys::BEHAVIOURAL] = behaviouralArray;
  if (connectionArray.size() > 0)
    data[ConfigKeys::CONNECTIONS] = connectionArray;
  if (transitionArray.size() > 0)
    data[ConfigKeys::TRANSITIONS] = transitionArray;

  return data;
}

SaveInfo SaveInfo::fromJson(const QJsonObject& data)
{
  SaveInfo info;
  info.canvasInfo = CanvasSaveInfo::fromJson(data[ConfigKeys::CANVAS].toObject());

  for (const auto& node : data[ConfigKeys::STRUCTURAL].toArray())
    info.structuralNodes.append(NodeSaveInfo::fromJson(node.toObject()));

  for (const auto& node : data[ConfigKeys::BEHAVIOURAL].toArray())
    info.behaviouralNodes.append(NodeSaveInfo::fromJson(node.toObject()));

  for (const auto& node : data[ConfigKeys::CONNECTIONS].toArray())
    info.connections.append(ConnectionSaveInfo::fromJson(node.toObject()));

  for (const auto& node : data[ConfigKeys::TRANSITIONS].toArray())
    info.transitions.append(TransitionSaveInfo::fromJson(node.toObject()));

  return info;
}
