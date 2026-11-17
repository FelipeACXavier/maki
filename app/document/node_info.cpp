#include "node_info.h"

#include <QBuffer>
#include <QJsonArray>
#include <QJsonObject>

#include "app_configs.h"
#include "flow_info.h"
#include "json.h"
#include "keys.h"
#include "macros.h"
#include "notifications.h"
#include "save_info.h"
#include "transition_info.h"

Q_DECLARE_METATYPE(NodeSaveInfo)

NodeSaveInfo::NodeSaveInfo()
    : mId("")
    , mNodeId("")
    , mParentId("")
    , mParentSubFlow(Constants::MAIN_SUB_FLOW)
    , mPosition(QPointF{0, 0})
    , mSize(QSizeF{0, 0})
    , mScale(1.0)
    , mIconPath("")
    , mProperties({})
    , mChildren({})
    , mFlows({})
    , mEvents({})
    , mTransitions({})
    , mFields({})
    , mPorts({})
{
}

NodeSaveInfo::NodeSaveInfo(const NodeConfig& config)
    : mId("")
    , mNodeId(config.type)
    , mParentId("")
    , mParentSubFlow(Constants::MAIN_SUB_FLOW)
    , mPosition(QPointF{0, 0})
    , mSize(QSizeF{static_cast<qreal>(config.body.width), static_cast<qreal>(config.body.height)})
    , mScale(1)
    , mIconPath(config.body.iconPath)
    , mChildren({})
    , mFlows({})
    , mTransitions({})
    , mFields({})
{
  for (const auto& property : config.properties)
    addProperty(std::make_shared<PropertyInfo>(property));

  for (const auto& event : config.events)
    addEvent(std::make_shared<FlowSaveInfo>(event));

  mPorts = config.ports;
}

void NodeSaveInfo::print(const QString& indent) const
{
  LOG_INFO("{}Node id: {}", indent, getid());
  LOG_INFO("{}Node type: {}", indent, getnodeId());
  LOG_INFO("{}Children: {}", indent, getchildren().size());
  for (const auto& ichild : getchildren())
    if (auto child = std::dynamic_pointer_cast<NodeSaveInfo>(ichild))
      child->print(indent + "  ");

  LOG_INFO("{}Flows: {}", indent, getflows().size());
  for (const auto& iflow : getflows())
    if (auto flow = std::dynamic_pointer_cast<FlowSaveInfo>(iflow))
      flow->print(indent + "  ");

  LOG_INFO("{}Properties: {}", indent, getproperties().size());
  for (const auto& iprop : getproperties())
    if (auto prop = std::dynamic_pointer_cast<PropertyInfo>(iprop); prop)
    {
      LOG_INFO("{}  Id: {}", indent, prop->getid());
      LOG_INFO("{}  Name: {}", indent, prop->getname());
      LOG_INFO("{}  Value: {}", indent, maki::asValue(prop->getvalue())->toReadable());
    }
}

QString NodeSaveInfo::getid() const
{
  return mId;
}

QString NodeSaveInfo::getnodeId() const
{
  return mNodeId;
}

QString NodeSaveInfo::getNodeName() const
{
  if (const auto* name = getProperty(ConfigKeys::NAME))
    return name->getvalue()->toStringValue();

  return QString();
}

QString NodeSaveInfo::getparentId() const
{
  return mParentId;
}

QPointF NodeSaveInfo::getposition() const
{
  return mPosition;
}

qreal NodeSaveInfo::getScale() const
{
  return mScale;
}

QSizeF NodeSaveInfo::getSize() const
{
  return mSize;
}

QString NodeSaveInfo::getIcon() const
{
  return mIconPath;
}

QVector<std::shared_ptr<IParameter>> NodeSaveInfo::getfields() const
{
  return mFields;
}

QVector<std::shared_ptr<IParameter>> NodeSaveInfo::getproperties() const
{
  return mProperties;
}

std::shared_ptr<IParameter> NodeSaveInfo::getproperty(const QString& key) const
{
  for (const auto& prop : mProperties)
    if (prop->getid() == key)
      return prop;

  return nullptr;
}

QVector<std::shared_ptr<IFlow>> NodeSaveInfo::getflows() const
{
  return mFlows;
}

QVector<std::shared_ptr<IFlow>> NodeSaveInfo::getevents() const
{
  return mEvents;
}

QVector<std::shared_ptr<INode>> NodeSaveInfo::getchildren() const
{
  return mChildren;
}

void NodeSaveInfo::setId(const QString& arg)
{
  mId = arg;
}

void NodeSaveInfo::setNodeId(const QString& arg)
{
  mNodeId = arg;
}

void NodeSaveInfo::setParentId(const QString& arg)
{
  mParentId = arg;
}

void NodeSaveInfo::setPosition(const QPointF& arg)
{
  mPosition = arg;
}

void NodeSaveInfo::setIcon(const QString& arg)
{
  mIconPath = arg;
}

void NodeSaveInfo::setSize(const QSizeF& arg)
{
  mSize = arg;
}

void NodeSaveInfo::setScale(qreal arg)
{
  mScale = arg;
}

const PropertyInfo* NodeSaveInfo::getProperty(const QString& key) const
{
  for (const auto& property : mProperties)
    if (property->getid() == key)
      return dynamic_cast<const PropertyInfo*>(property.get());

  return nullptr;
}

void NodeSaveInfo::addProperty(std::shared_ptr<PropertyInfo> parameter)
{
  if (getProperty(parameter->getid()))
    return;

  mProperties.push_back(parameter);
}

void NodeSaveInfo::setProperty(const QString& key, const maki::Value& parameter)
{
  for (auto& property : mProperties)
  {
    if (property->getid() != key)
      continue;

    if (auto* prop = dynamic_cast<PropertyInfo*>(property.get()); prop)
    {
      prop->setDefaultValue(parameter);
      SaveInfo::instance().publish(DocumentEvent{.type = DocumentEvent::Type::NodeModified,
                                                 .data = NodeEvent{
                                                   .id = getid(),
                                                   .propertyId = key,
                                                   .propertyValue = parameter,
                                                 }});
    }
    return;
  }
}

void NodeSaveInfo::removeProperty(const QString& key)
{
  maki::removeIf(mProperties, [key](std::shared_ptr<IParameter> param) { return param->getid() == key; });
  SaveInfo::instance().publish(DocumentEvent{.type = DocumentEvent::Type::NodeModified,
                                             .data = NodeEvent{
                                               .id = getid(),
                                               .propertyId = key,
                                             }});
}

PropertyInfo NodeSaveInfo::getField(const QString& key) const
{
  for (const auto& field : getfields())
    if (field->getid() == key)
      return *std::dynamic_pointer_cast<PropertyInfo>(field);

  return PropertyInfo();
}

void NodeSaveInfo::setField(const QString& key, std::shared_ptr<IParameter> property)
{
  // Check if key exists
  for (auto& field : getfields())
  {
    if (field->getid() != key)
      continue;

    field = property;
    return;
  }

  // If it doesn't exist, add it
  addField(property);
}

void NodeSaveInfo::addField(std::shared_ptr<IParameter> property)
{
  mFields.push_back(property);
}

void NodeSaveInfo::removeField(const QString& key)
{
  mFields.removeIf([key](std::shared_ptr<IParameter> info) { return info->getid() == key; });
}

void NodeSaveInfo::addFlow(std::shared_ptr<IFlow> flow)
{
  mFlows.push_back(flow);
  SaveInfo::instance().publish(DocumentEvent{.type = DocumentEvent::Type::FlowAdded,
                                             .data = FlowEvent{
                                               .id = flow->getid(),
                                               .ownerNodeId = getid(),
                                             }});
}

void NodeSaveInfo::removeFlow(const QString& flowId)
{
  auto flow = maki::findIf(mFlows, [flowId](const auto& info) { return info->getid() == flowId; });
  if (!flow)
    return;

  if ((*flow)->getname() == Constants::MAIN_FLOW)
  {
    NOTIFY_WARNING(Config::APPLICATION_NAME.toStdString(), "Cannot remove the main flow");
    return;
  }

  auto oldFlow = maki::takeIf(mFlows, [flowId](const auto& info) { return info->getid() == flowId; });
  if (oldFlow)
    SaveInfo::instance().publish(DocumentEvent{.type = DocumentEvent::Type::FlowRemoved,
                                               .data = FlowEvent{
                                                 .id = flowId,
                                                 .ownerNodeId = getid(),
                                               }});
}

void NodeSaveInfo::addEvent(std::shared_ptr<IFlow> event)
{
  mEvents.push_back(event);
}

void NodeSaveInfo::removeEvent(std::shared_ptr<IFlow> event)
{
  mEvents.removeIf([event](std::shared_ptr<IFlow> info) { return info->getid() == event->getid(); });
}

void NodeSaveInfo::addChild(std::shared_ptr<INode> child)
{
  mChildren.push_back(child);
}

void NodeSaveInfo::removeChild(const QString& childId)
{
  mChildren.removeIf([childId](std::shared_ptr<INode> info) { return info->getid() == childId; });
}

void NodeSaveInfo::clearChildren()
{
  mChildren.clear();
}

void NodeSaveInfo::addPort(const PortConfig& port)
{
  mPorts.append(port);
}

QVector<PortConfig> NodeSaveInfo::ports() const
{
  return mPorts;
}

QString NodeSaveInfo::getParentSubFlow() const
{
  return mParentSubFlow;
}

void NodeSaveInfo::setParentSubFlow(const QString& subFlowId)
{
  mParentSubFlow = subFlowId;
}

// ==========================================================================
// JSON serialization
QJsonObject NodeSaveInfo::toJson() const
{
  QJsonObject data;

  data[ConfigKeys::ID] = getid();
  data[ConfigKeys::NODE_ID] = getnodeId();
  data[ConfigKeys::PARENT_ID] = getparentId();
  data[ConfigKeys::PARENT_SUB_FLOW_ID] = getParentSubFlow();

  data[ConfigKeys::SCALE] = getScale();
  data[ConfigKeys::SIZE] = JSON::fromSizeF(getSize());
  data[ConfigKeys::POSITION] = JSON::fromPointF(getposition());

  QJsonArray fieldArray;
  for (const auto& field : getfields())
    fieldArray.append(std::dynamic_pointer_cast<PropertyInfo>(field)->toJson());

  QJsonArray childrenArray;
  for (const auto& child : getchildren())
    childrenArray.append(std::dynamic_pointer_cast<NodeSaveInfo>(child)->toJson());

  QJsonArray flowArray;
  for (const auto& flow : getflows())
    flowArray.append(std::dynamic_pointer_cast<FlowSaveInfo>(flow)->toJson());

  QJsonArray eventArray;
  for (const auto& event : getevents())
    eventArray.append(std::dynamic_pointer_cast<FlowSaveInfo>(event)->toJson());

  QJsonArray propertyArray;
  for (const auto& property : getproperties())
    propertyArray.append(std::dynamic_pointer_cast<PropertyInfo>(property)->toJson());

  QJsonArray portArray;
  for (const auto& port : mPorts)
    portArray.append(port.toJson());

  if (propertyArray.size() > 0)
    data[ConfigKeys::PROPERTIES] = propertyArray;
  if (fieldArray.size() > 0)
    data[ConfigKeys::FIELDS] = fieldArray;
  if (childrenArray.size() > 0)
    data[ConfigKeys::CHILDREN] = childrenArray;
  if (flowArray.size() > 0)
    data[ConfigKeys::FLOWS] = flowArray;
  if (eventArray.size() > 0)
    data[ConfigKeys::EVENTS] = eventArray;
  if (portArray.size() > 0)
    data[ConfigKeys::CONNECTORS] = portArray;

  data[ConfigKeys::ICON_PATH] = getIcon();

  return data;
}

NodeSaveInfo NodeSaveInfo::fromJson(const QJsonObject& data)
{
  NodeSaveInfo info;

  // First parse the mandatory arguments
  info.setId(data[ConfigKeys::ID].toString());
  info.setNodeId(data[ConfigKeys::NODE_ID].toString());
  info.setScale(data[ConfigKeys::SCALE].toDouble());
  info.setSize(JSON::toSizeF(data[ConfigKeys::SIZE].toObject()));
  info.setPosition(JSON::toPointF(data[ConfigKeys::POSITION].toObject()));

  if (data.contains(ConfigKeys::PARENT_ID))
    info.setParentId(data[ConfigKeys::PARENT_ID].toString());

  if (data.contains(ConfigKeys::PARENT_SUB_FLOW_ID))
    info.setParentSubFlow(data[ConfigKeys::PARENT_SUB_FLOW_ID].toString());

  if (data.contains(ConfigKeys::FIELDS))
    for (const auto& node : data[ConfigKeys::FIELDS].toArray())
      info.addField(std::make_shared<PropertyInfo>(PropertyInfo::fromJson(node.toObject())));

  if (data.contains(ConfigKeys::EVENTS))
    for (const auto& node : data[ConfigKeys::EVENTS].toArray())
      info.addEvent(std::make_shared<FlowSaveInfo>(FlowSaveInfo::fromJson(node.toObject())));

  if (data.contains(ConfigKeys::CHILDREN))
    for (const auto& child : data[ConfigKeys::CHILDREN].toArray())
      info.addChild(std::make_shared<NodeSaveInfo>(NodeSaveInfo::fromJson(child.toObject())));

  if (data.contains(ConfigKeys::PROPERTIES))
    for (const auto& node : data[ConfigKeys::PROPERTIES].toArray())
      info.addProperty(std::make_shared<PropertyInfo>(PropertyInfo::fromJson(node.toObject())));

  if (data.contains(ConfigKeys::CONNECTORS))
    for (const auto& node : data[ConfigKeys::CONNECTORS].toArray())
      info.addPort(PortConfig(node.toObject()));

  info.setIcon(data[ConfigKeys::ICON_PATH].toString());

  return info;
}

// ==========================================================================
// Stream serialization
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
  {
    nodes[i] = std::make_shared<NodeSaveInfo>();
    in >> *nodes[i];
  }

  return in;
}

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<INode>>& nodes)
{
  out << static_cast<qint32>(nodes.size());
  for (const auto& node : nodes)
    out << *std::dynamic_pointer_cast<NodeSaveInfo>(node);

  return out;
}

QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<INode>>& nodes)
{
  qint32 size;
  in >> size;

  nodes.resize(size);
  for (int i = 0; i < size; ++i)
  {
    nodes[i] = std::make_shared<NodeSaveInfo>();
    in >> *std::dynamic_pointer_cast<NodeSaveInfo>(nodes[i]);
  }

  return in;
}

QDataStream& operator<<(QDataStream& out, const NodeSaveInfo& info)
{
  out << info.getid();
  out << info.getnodeId();
  out << info.getparentId();
  out << info.getParentSubFlow();

  out << info.getposition();
  out << info.getSize();
  out << info.getScale();

  out << info.getproperties();
  out << info.getchildren();
  out << info.getflows();
  out << info.getevents();
  out << info.getfields();

  out << info.getIcon();

  return out;
}

QDataStream& operator>>(QDataStream& in, NodeSaveInfo& info)
{
  // We don't use the setters so that we don't update the event dispatcher
  QString id;
  in >> id;
  info.mId = id;

  QString nodeId;
  in >> nodeId;
  info.mNodeId = nodeId;

  QString parentId;
  in >> parentId;
  info.mParentId = parentId;

  QString parentSubFlowId;
  in >> parentSubFlowId;
  info.mParentSubFlow = parentSubFlowId;

  QPointF position;
  in >> position;
  info.mPosition = position;

  QSizeF size;
  in >> size;
  info.mSize = size;

  qreal scale;
  in >> scale;
  info.mScale = scale;

  QVector<std::shared_ptr<IParameter>> properties;
  in >> properties;
  info.mProperties = properties;

  QVector<std::shared_ptr<INode>> children;
  in >> children;
  info.mChildren = children;

  QVector<std::shared_ptr<IFlow>> flows;
  in >> flows;
  info.mFlows = flows;

  QVector<std::shared_ptr<IFlow>> events;
  in >> events;
  info.mEvents = events;

  QVector<std::shared_ptr<IParameter>> fields;
  in >> fields;
  info.mFields = fields;

  QString iconPath;
  in >> iconPath;
  info.mIconPath = iconPath;

  return in;
}
