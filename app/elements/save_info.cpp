#include "save_info.h"

#include <QBuffer>
#include <QJsonArray>

#include "config.h"
#include "json.h"
#include "keys.h"
#include "logging.h"

Q_DECLARE_METATYPE(TransitionSaveInfo)
Q_DECLARE_METATYPE(FlowSaveInfo)
Q_DECLARE_METATYPE(NodeSaveInfo)
Q_DECLARE_METATYPE(SaveInfo)

// ==========================================================================================================
// FlowSaveInfo
QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<FlowSaveInfo>>& flows)
{
  out << static_cast<qint32>(flows.size());
  for (const auto& flow : flows)
    out << *flow;

  return out;
}

QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<FlowSaveInfo>>& flows)
{
  qint32 size;
  in >> size;

  flows.resize(size);
  for (int i = 0; i < size; ++i)
    in >> *flows[i];

  return in;
}

QDataStream& operator<<(QDataStream& out, const FlowSaveInfo& info)
{
  out << info.id;
  out << info.name;
  out << info.nodes;
  out << info.modifiable;
  out << info.type;
  out << info.returnType;
  out << info.arguments;
  out << info.owner;

  return out;
}

QDataStream& operator>>(QDataStream& in, FlowSaveInfo& info)
{
  in >> info.id;
  in >> info.name;
  in >> info.nodes;
  in >> info.modifiable;
  in >> info.type;
  in >> info.returnType;
  in >> info.arguments;
  in >> info.owner;

  return in;
}

FlowSaveInfo::FlowSaveInfo(const FlowConfig& config)
{
  id = QUuid::createUuid().toString();
  name = config.name;
  type = config.type;
  returnType = config.returnType;
  arguments = config.arguments;
  modifiable = config.modifiable;
}

QJsonObject FlowSaveInfo::toJson() const
{
  QJsonObject data;
  data[ConfigKeys::ID] = id;
  data[ConfigKeys::NAME] = name;
  data[ConfigKeys::MODIFIABLE] = modifiable;
  data[ConfigKeys::TYPE] = Types::ConnectorTypeToString(type);
  data[ConfigKeys::RETURN_TYPE] = Types::PropertyTypesToString(returnType);
  data[ConfigKeys::OWNER] = owner;

  QJsonArray optionArray;
  for (const auto& arg : arguments)
    optionArray.append(arg.toJson());

  data[ConfigKeys::ARGUMENTS] = optionArray;

  QJsonArray nodesArray;
  for (const auto& node : nodes)
    nodesArray.append(node->toJson());

  if (nodesArray.size() > 0)
    data[ConfigKeys::NODES] = nodesArray;

  return data;
}

FlowSaveInfo FlowSaveInfo::fromJson(const QJsonObject& data)
{
  FlowSaveInfo info;
  info.id = data[ConfigKeys::ID].toString();
  info.name = data[ConfigKeys::NAME].toString();
  info.modifiable = data[ConfigKeys::MODIFIABLE].toBool();
  info.type = Types::StringToConnectorType(data[ConfigKeys::TYPE].toString());
  info.returnType = Types::StringToPropertyTypes(data[ConfigKeys::RETURN_TYPE].toString());
  info.owner = data[ConfigKeys::OWNER].toString();

  for (const auto& argument : data[ConfigKeys::ARGUMENTS].toArray())
    info.arguments.append(PropertiesConfig::fromJson(argument.toObject()));

  for (const auto& node : data[ConfigKeys::NODES].toArray())
    info.nodes.append(std::make_shared<NodeSaveInfo>(NodeSaveInfo::fromJson(node.toObject())));

  return info;
}

// ==========================================================================================================
// TransitionSaveInfo
QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<TransitionSaveInfo>>& transitions)
{
  out << static_cast<qint32>(transitions.size());
  for (const auto& transition : transitions)
    out << *transition;

  return out;
}

QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<TransitionSaveInfo>>& transitions)
{
  qint32 size;
  in >> size;

  transitions.resize(size);
  for (int i = 0; i < size; ++i)
    in >> *transitions[i];

  return in;
}

QDataStream& operator<<(QDataStream& out, const TransitionSaveInfo& info)
{
  out << info.id;
  out << info.label;

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
  in >> info.id;
  in >> info.label;

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
  data[ConfigKeys::ID] = id;
  data[ConfigKeys::LABEL] = label;

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
  info.id = data[ConfigKeys::ID].toString();
  info.label = data[ConfigKeys::LABEL].toString();

  info.srcId = data[ConfigKeys::SOURCE][ConfigKeys::ID].toString();
  info.srcPoint = JSON::toPointF(data[ConfigKeys::SOURCE][ConfigKeys::POSITION].toObject());
  info.srcShift = JSON::toPointF(data[ConfigKeys::SOURCE][ConfigKeys::SHIFT].toObject());

  info.dstId = data[ConfigKeys::DESTINATION][ConfigKeys::ID].toString();
  info.dstPoint = JSON::toPointF(data[ConfigKeys::DESTINATION][ConfigKeys::POSITION].toObject());
  info.dstShift = JSON::toPointF(data[ConfigKeys::DESTINATION][ConfigKeys::SHIFT].toObject());

  return info;
}

// ==========================================================================================================
// NodeSaveInfo
QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<NodeSaveInfo>>& nodes)
{
  out << static_cast<qint32>(nodes.size());
  for (const auto& node : nodes)
    out << *node;

  return out;
}

QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<NodeSaveInfo>>& nodes)
{
  qint32 size;
  in >> size;

  nodes.resize(size);
  for (int i = 0; i < size; ++i)
    in >> *nodes[i];

  return in;
}

QDataStream& operator<<(QDataStream& out, const NodeSaveInfo& info)
{
  out << info.id;
  out << info.size;
  out << info.scale;
  out << info.nodeId;
  out << info.fields;
  out << info.position;
  out << info.properties;
  out << info.parentId;
  out << info.transitions;
  out << info.children;
  out << info.flows;
  if (info.behaviour)
    out << *info.behaviour;

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
  in >> info.position;
  in >> info.properties;
  in >> info.parentId;
  in >> info.transitions;
  in >> info.children;
  in >> info.flows;

  info.behaviour = std::make_shared<FlowSaveInfo>();
  in >> *info.behaviour;

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

  data[ConfigKeys::BEHAVIOUR] = behaviour->toJson();

  QJsonArray fieldArray;
  for (const auto& field : fields)
    fieldArray.append(field.toJson());

  QJsonArray transitionArray;
  for (const auto& transition : transitions)
    transitionArray.append(transition->toJson());

  QJsonArray childrenArray;
  for (const auto& child : children)
    childrenArray.append(child->toJson());

  QJsonArray flowArray;
  for (const auto& flow : flows)
    flowArray.append(flow->toJson());

  QJsonObject propertiesObject;
  for (auto it = properties.constBegin(); it != properties.constEnd(); ++it)
    propertiesObject[it.key()] = it.value().toJsonValue();

  if (properties.size() > 0)
    data[ConfigKeys::PROPERTIES] = propertiesObject;
  if (fieldArray.size() > 0)
    data[ConfigKeys::FIELDS] = fieldArray;
  if (transitionArray.size() > 0)
    data[ConfigKeys::TRANSITIONS] = transitionArray;
  if (childrenArray.size() > 0)
    data[ConfigKeys::CHILDREN] = childrenArray;
  if (flowArray.size() > 0)
    data[ConfigKeys::FLOWS] = flowArray;

  data[ConfigKeys::PIXMAP] = JSON::fromPixmap(pixmap);

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

  info.behaviour = std::make_shared<FlowSaveInfo>(FlowSaveInfo::fromJson(data[ConfigKeys::BEHAVIOUR].toObject()));

  if (data.contains(ConfigKeys::PARENT_ID))
    info.parentId = data[ConfigKeys::PARENT_ID].toString();

  if (data.contains(ConfigKeys::FIELDS))
  {
    for (const auto& node : data[ConfigKeys::FIELDS].toArray())
      info.fields.append(PropertiesConfig::fromJson(node.toObject()));
  }

  if (data.contains(ConfigKeys::TRANSITIONS))
  {
    for (const auto& node : data[ConfigKeys::TRANSITIONS].toArray())
      info.transitions.append(std::make_shared<TransitionSaveInfo>(TransitionSaveInfo::fromJson(node.toObject())));
  }

  if (data.contains(ConfigKeys::CHILDREN))
  {
    for (const auto& node : data[ConfigKeys::CHILDREN].toArray())
      info.children.append(std::make_shared<NodeSaveInfo>(NodeSaveInfo::fromJson(node.toObject())));
  }

  if (data.contains(ConfigKeys::FLOWS))
  {
    for (const auto& node : data[ConfigKeys::FLOWS].toArray())
      info.flows.append(std::make_shared<FlowSaveInfo>(FlowSaveInfo::fromJson(node.toObject())));
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
  // out << info.canvasInfo;
  // out << info.structuralNodes;
  // out << info.behaviouralNodes;

  return out;
}

QDataStream& operator>>(QDataStream& in, SaveInfo& info)
{
  // in >> info.canvasInfo;
  // in >> info.structuralNodes;
  // in >> info.behaviouralNodes;

  return in;
}

QJsonObject SaveInfo::toJson() const
{
  QJsonObject data;

  data[ConfigKeys::CANVAS] = canvasInfo.toJson();

  QJsonArray structuralArray;
  for (const auto& node : structuralNodes)
    structuralArray.append(node->toJson());

  QJsonArray behaviouralArray;
  for (const auto& node : behaviouralNodes)
    behaviouralArray.append(node->toJson());

  if (structuralArray.size() > 0)
    data[ConfigKeys::STRUCTURAL] = structuralArray;
  if (behaviouralArray.size() > 0)
    data[ConfigKeys::BEHAVIOURAL] = behaviouralArray;

  return data;
}

SaveInfo SaveInfo::fromJson(const QJsonObject& data)
{
  SaveInfo info;
  info.canvasInfo = CanvasSaveInfo::fromJson(data[ConfigKeys::CANVAS].toObject());

  for (const auto& node : data[ConfigKeys::STRUCTURAL].toArray())
    info.structuralNodes.append(std::make_shared<NodeSaveInfo>(NodeSaveInfo::fromJson(node.toObject())));

  for (const auto& node : data[ConfigKeys::BEHAVIOURAL].toArray())
    info.behaviouralNodes.append(std::make_shared<NodeSaveInfo>(NodeSaveInfo::fromJson(node.toObject())));

  return info;
}

QVector<std::shared_ptr<NodeSaveInfo>> SaveInfo::findFamilyOfConstruct(const QString& nodeId, QVector<std::shared_ptr<NodeSaveInfo>> nodes) const
{
  for (const auto& node : nodes)
  {
    auto parent = findParentOfConstruct(nodeId, node);
    if (parent)
      return nodes + node->children;

    auto family = findFamilyOfConstruct(nodeId, node->children);
    if (!family.empty())
      return family;
  }

  return {};
}

std::shared_ptr<NodeSaveInfo> SaveInfo::findParentOfConstruct(const QString& nodeId, const std::shared_ptr<NodeSaveInfo> node) const
{
  if (node->behaviour != nullptr)
  {
    for (const auto& construct : node->behaviour->nodes)
    {
      if (construct->id != nodeId)
        continue;

      return node;
    }
  }

  for (const auto& flow : node->flows)
  {
    for (const auto& construct : flow->nodes)
    {
      if (construct->id != nodeId)
        continue;

      return node;
    }
  }

  return nullptr;
}

QVector<std::shared_ptr<NodeSaveInfo>> SaveInfo::getPossibleCallers(const QString& nodeId) const
{
  // Get the parent
  return findFamilyOfConstruct(nodeId, structuralNodes);
}

QVector<std::shared_ptr<FlowSaveInfo>> SaveInfo::getEventsFromNode(const QString& nodeId, QVector<std::shared_ptr<NodeSaveInfo>> nodes) const
{
  for (const auto& node : nodes)
  {
    if (node->id == nodeId)
      return node->flows;

    auto events = getEventsFromNode(nodeId, node->children);
    if (!events.empty())
      return events;
  }

  return {};
}

QVector<std::shared_ptr<FlowSaveInfo>> SaveInfo::getEventsFromNode(const QString& nodeId) const
{
  return getEventsFromNode(nodeId, structuralNodes);
}

std::shared_ptr<NodeSaveInfo> SaveInfo::getNodeWithId(const QString& nodeId)
{
  return getNodeWithId(nodeId, structuralNodes);
}

std::shared_ptr<NodeSaveInfo> SaveInfo::getNodeWithId(const QString& nodeId, const QVector<std::shared_ptr<NodeSaveInfo>>& nodes)
{
  for (const auto& node : nodes)
  {
    if (node->id == nodeId)
      return node;

    auto found = getNodeWithId(nodeId, node->children);
    if (found != nullptr)
      return found;

    for (const auto& flow : node->flows)
    {
      found = getNodeWithId(nodeId, flow->nodes);
      if (found != nullptr)
        return found;
    }
  }

  return nullptr;
}

std::shared_ptr<FlowSaveInfo> SaveInfo::getFlowWithId(const QString& flowId)
{
  return getFlowWithId(flowId, structuralNodes);
}

std::shared_ptr<FlowSaveInfo> SaveInfo::getFlowWithId(const QString& flowId, const QVector<std::shared_ptr<NodeSaveInfo>>& nodes)
{
  for (const auto& node : nodes)
  {
    for (const auto& flow : node->flows)
    {
      if (flow->id == flowId)
        return flow;
    }

    auto found = getFlowWithId(flowId, node->children);
    if (found != nullptr)
      return found;
  }

  return nullptr;
}
