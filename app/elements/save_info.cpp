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
TransitionSaveInfo::TransitionSaveInfo()
    : mId("")
    , mLabel("")
    , mEvent("")
    , mSrcId("")
    , mSrcPoint(QPointF{0, 0})
    , mSrcShift(QPointF{0, 0})
    , mDstId("")
    , mDstPoint(QPointF{0, 0})
    , mDstShift(QPointF{0, 0})
{
}

QString TransitionSaveInfo::getid() const
{
  return mId;
}

QString TransitionSaveInfo::getlabel() const
{
  return mLabel;
}

QString TransitionSaveInfo::getevent() const
{
  return mEvent;
}

QString TransitionSaveInfo::getsrcId() const
{
  return mSrcId;
}

QString TransitionSaveInfo::getdstId() const
{
  return mDstId;
}

QPointF TransitionSaveInfo::srcPoint() const
{
  return mSrcPoint;
}

QPointF TransitionSaveInfo::srcShift() const
{
  return mSrcShift;
}

QPointF TransitionSaveInfo::dstPoint() const
{
  return mDstPoint;
}

QPointF TransitionSaveInfo::dstShift() const
{
  return mDstShift;
}

void TransitionSaveInfo::setId(const QString& arg)
{
  mId = arg;
}

void TransitionSaveInfo::setLabel(const QString& arg)
{
  mLabel = arg;
}

void TransitionSaveInfo::setEvent(const QString& arg)
{
  mEvent = arg;
}

void TransitionSaveInfo::setSrcId(const QString& arg)
{
  mSrcId = arg;
}

void TransitionSaveInfo::setSrcPoint(const QPointF& arg)
{
  mSrcPoint = arg;
}

void TransitionSaveInfo::setSrcShift(const QPointF& arg)
{
  mSrcShift = arg;
}

void TransitionSaveInfo::setDstId(const QString& arg)
{
  mDstId = arg;
}

void TransitionSaveInfo::setDstPoint(const QPointF& arg)
{
  mDstPoint = arg;
}

void TransitionSaveInfo::setDstShift(const QPointF& arg)
{
  mDstPoint = arg;
}

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<TransitionSaveInfo>>& transitions)
{
  // out << static_cast<qint32>(transitions.size());
  // for (const auto& transition : transitions)
  //   out << *transition;

  return out;
}

QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<TransitionSaveInfo>>& transitions)
{
  // qint32 size;
  // in >> size;

  // transitions.resize(size);
  // for (int i = 0; i < size; ++i)
  //   in >> *transitions[i];

  return in;
}

QDataStream& operator<<(QDataStream& out, const TransitionSaveInfo& info)
{
  // out << info.id;
  // out << info.label;
  // out << info.event;

  // out << info.srcId;
  // out << info.srcPoint;
  // out << info.srcShift;

  // out << info.dstId;
  // out << info.dstPoint;
  // out << info.dstShift;

  return out;
}

QDataStream& operator>>(QDataStream& in, TransitionSaveInfo& info)
{
  // in >> info.id;
  // in >> info.label;
  // in >> info.event;

  // in >> info.srcId;
  // in >> info.srcPoint;
  // in >> info.srcShift;

  // in >> info.dstId;
  // in >> info.dstPoint;
  // in >> info.dstShift;

  return in;
}

QJsonObject TransitionSaveInfo::toJson() const
{
  QJsonObject data;
  data[ConfigKeys::ID] = getid();
  data[ConfigKeys::LABEL] = getlabel();
  data[ConfigKeys::EVENTS] = getevent();

  QJsonObject source;
  source[ConfigKeys::ID] = getsrcId();
  source[ConfigKeys::POSITION] = JSON::fromPointF(srcPoint());
  source[ConfigKeys::SHIFT] = JSON::fromPointF(srcShift());
  data[ConfigKeys::SOURCE] = source;

  QJsonObject destination;
  destination[ConfigKeys::ID] = getdstId();
  destination[ConfigKeys::POSITION] = JSON::fromPointF(dstPoint());
  destination[ConfigKeys::SHIFT] = JSON::fromPointF(dstShift());
  data[ConfigKeys::DESTINATION] = destination;

  return data;
}

TransitionSaveInfo TransitionSaveInfo::fromJson(const QJsonObject& data)
{
  TransitionSaveInfo info;
  info.setId(data[ConfigKeys::ID].toString());
  info.setLabel(data[ConfigKeys::LABEL].toString());
  info.setEvent(data[ConfigKeys::EVENTS].toString());

  info.setSrcId(data[ConfigKeys::SOURCE][ConfigKeys::ID].toString());
  info.setSrcPoint(JSON::toPointF(data[ConfigKeys::SOURCE][ConfigKeys::POSITION].toObject()));
  info.setSrcShift(JSON::toPointF(data[ConfigKeys::SOURCE][ConfigKeys::SHIFT].toObject()));

  info.setDstId(data[ConfigKeys::DESTINATION][ConfigKeys::ID].toString());
  info.setDstPoint(JSON::toPointF(data[ConfigKeys::DESTINATION][ConfigKeys::POSITION].toObject()));
  info.setDstShift(JSON::toPointF(data[ConfigKeys::DESTINATION][ConfigKeys::SHIFT].toObject()));

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

  QJsonArray fieldArray;
  for (const auto& field : fields)
    fieldArray.append(field->toJson());

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

  if (data.contains(ConfigKeys::PARENT_ID))
    info.parentId = data[ConfigKeys::PARENT_ID].toString();

  if (data.contains(ConfigKeys::FIELDS))
  {
    for (const auto& node : data[ConfigKeys::FIELDS].toArray())
      info.fields.append(std::make_shared<PropertiesConfig>(PropertiesConfig::fromJson(node.toObject())));
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
qreal CanvasSaveInfo::scale() const
{
  return mScale;
}

QPointF CanvasSaveInfo::center() const
{
  return mCenter;
}

void CanvasSaveInfo::setScale(qreal scale)
{
  mScale = scale;
}

void CanvasSaveInfo::setCenter(const QPointF& center)
{
  mCenter = center;
}

// CanvasSaveInfo
QDataStream& operator<<(QDataStream& out, const CanvasSaveInfo& info)
{
  // out << info.scale();
  // out << info.center();

  return out;
}

QDataStream& operator>>(QDataStream& in, CanvasSaveInfo& info)
{
  // in >> info.scale;
  // in >> info.center;

  return in;
}

QJsonObject CanvasSaveInfo::toJson() const
{
  QJsonObject data;

  data[ConfigKeys::POSITION] = JSON::fromPointF(center());
  data[ConfigKeys::SCALE] = scale();

  return data;
}

CanvasSaveInfo CanvasSaveInfo::fromJson(const QJsonObject& data)
{
  CanvasSaveInfo info;

  info.setCenter(JSON::toPointF(data[ConfigKeys::POSITION].toObject()));
  info.setScale(data[ConfigKeys::SCALE].toDouble());

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

CanvasSaveInfo SaveInfo::canvasInfo() const
{
  return mCanvasInfo;
}

void SaveInfo::setCanvasInfo(const CanvasSaveInfo& info)
{
  mCanvasInfo = info;
}

void SaveInfo::addNode(std::shared_ptr<NodeSaveInfo> node)
{
  mStructuralNodes.push_back(node);
}

void SaveInfo::removeNode(std::shared_ptr<NodeSaveInfo> node)
{
  mStructuralNodes.removeIf([node](std::shared_ptr<INode> info) { return info->getid() == node->getid(); });
}

QVector<std::shared_ptr<INode>> SaveInfo::getnodes() const
{
  return mStructuralNodes;
};

QJsonObject SaveInfo::toJson() const
{
  QJsonObject data;

  data[ConfigKeys::CANVAS] = canvasInfo().toJson();

  QJsonArray structuralArray;
  for (const auto& node : getnodes())
    structuralArray.append(std::dynamic_pointer_cast<NodeSaveInfo>(node)->toJson());

  if (structuralArray.size() > 0)
    data[ConfigKeys::STRUCTURAL] = structuralArray;

  return data;
}

SaveInfo SaveInfo::fromJson(const QJsonObject& data)
{
  SaveInfo info;
  auto canvasInfo = CanvasSaveInfo::fromJson(data[ConfigKeys::CANVAS].toObject());
  info.setCanvasInfo(canvasInfo);

  for (const auto& node : data[ConfigKeys::STRUCTURAL].toArray())
    info.addNode(std::make_shared<NodeSaveInfo>(NodeSaveInfo::fromJson(node.toObject())));

  return info;
}

QVector<std::shared_ptr<NodeSaveInfo>> SaveInfo::findFamilyOfConstruct(const QString& nodeId, QVector<std::shared_ptr<INode>> nodes) const
{
  for (const auto& node : nodes)
  {
    LOG_INFO("Looking into: %s %s", qPrintable(node->getid()), qPrintable(node->getnodeId()));
    auto parent = findParentOfConstruct(nodeId, node);
    if (parent)
    {
      QVector<std::shared_ptr<NodeSaveInfo>> out = {std::static_pointer_cast<NodeSaveInfo>(node)};
      for (auto& child : node->getchildren())
        out.push_back(std::static_pointer_cast<NodeSaveInfo>(child));

      return out;
    }

    auto family = findFamilyOfConstruct(nodeId, node->getchildren());
    if (!family.empty())
      return family;
  }

  return {};
}

std::shared_ptr<NodeSaveInfo> SaveInfo::findParentOfConstruct(const QString& nodeId, const std::shared_ptr<INode> node) const
{
  if (!node)
    return nullptr;

  LOG_INFO("Node info: %s %d %d", qPrintable(node->getnodeId()), node->getchildren().size(), node->getflows().size());

  for (const auto& flow : node->getflows())
  {
    LOG_INFO("Looking into flow: %s", qPrintable(flow->getname()));
    for (const auto& construct : flow->getnodes())
    {
      if (construct->getid() != nodeId)
        continue;

      return std::static_pointer_cast<NodeSaveInfo>(node);
    }
  }

  return nullptr;
}

void SaveInfo::findStatesOfConstruct(QVector<std::shared_ptr<NodeSaveInfo>>& toReturn, QVector<std::shared_ptr<INode>> nodes) const
{
  for (const auto& node : nodes)
  {
    if (!node->getfields().isEmpty())
      toReturn.push_back(std::static_pointer_cast<NodeSaveInfo>(node));

    findStatesOfConstruct(toReturn, node->getchildren());
  }
}

QVector<std::shared_ptr<NodeSaveInfo>> SaveInfo::getPossibleStates(const QString& nodeId) const
{
  QVector<std::shared_ptr<NodeSaveInfo>> toReturn;
  findStatesOfConstruct(toReturn, getnodes());
  return toReturn;
}

QVector<std::shared_ptr<NodeSaveInfo>> SaveInfo::getPossibleCallers(const QString& nodeId) const
{
  // Get the parent
  return findFamilyOfConstruct(nodeId, getnodes());
}

QVector<std::shared_ptr<FlowSaveInfo>> SaveInfo::getEventsFromNode(const QString& nodeId, QVector<std::shared_ptr<INode>> nodes) const
{
  for (const auto& node : nodes)
  {
    if (node->getid() == nodeId)
    {
      QVector<std::shared_ptr<FlowSaveInfo>> out;
      for (auto& flow : node->getflows())
        out.push_back(std::static_pointer_cast<FlowSaveInfo>(flow));

      return out;
    }

    auto events = getEventsFromNode(nodeId, node->getchildren());
    if (!events.empty())
      return events;
  }

  return {};
}

QVector<std::shared_ptr<FlowSaveInfo>> SaveInfo::getEventsFromNode(const QString& nodeId) const
{
  return getEventsFromNode(nodeId, getnodes());
}

std::shared_ptr<NodeSaveInfo> SaveInfo::getNodeWithId(const QString& nodeId)
{
  return getNodeWithId(nodeId, getnodes());
}

std::shared_ptr<NodeSaveInfo> SaveInfo::getNodeWithId(const QString& nodeId, const QVector<std::shared_ptr<INode>>& nodes)
{
  for (const auto& node : nodes)
  {
    if (node->getid() == nodeId)
      return std::static_pointer_cast<NodeSaveInfo>(node);

    auto found = getNodeWithId(nodeId, node->getchildren());
    if (found != nullptr)
      return found;

    for (const auto& flow : node->getflows())
    {
      found = getNodeWithId(nodeId, flow->getnodes());
      if (found != nullptr)
        return found;
    }
  }

  return nullptr;
}

std::shared_ptr<FlowSaveInfo> SaveInfo::getFlowWithId(const QString& flowId)
{
  return getFlowWithId(flowId, getnodes());
}

std::shared_ptr<FlowSaveInfo> SaveInfo::getFlowWithId(const QString& flowId, const QVector<std::shared_ptr<INode>>& nodes)
{
  for (const auto& node : nodes)
  {
    for (const auto& flow : node->getflows())
    {
      if (flow->getid() == flowId)
        return std::static_pointer_cast<FlowSaveInfo>(flow);
    }

    auto found = getFlowWithId(flowId, node->getchildren());
    if (found != nullptr)
      return found;
  }

  return nullptr;
}
