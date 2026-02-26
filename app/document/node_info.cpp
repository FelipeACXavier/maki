#include "node_info.h"

#include <QBuffer>
#include <QJsonArray>
#include <QJsonObject>

#include "flow_info.h"
#include "json.h"
#include "keys.h"
#include "transition_info.h"

Q_DECLARE_METATYPE(NodeSaveInfo)

NodeSaveInfo::NodeSaveInfo()
    : mId("")
    , mNodeId("")
    , mParentId("")
    , mPosition(QPointF{0, 0})
    , mSize(QSizeF{0, 0})
    , mScale(1.0)
    , mProperties({})
    , mChildren({})
    , mFlows({})
    , mTransitions({})
{
}

QString NodeSaveInfo::getid() const
{
  return mId;
}

QString NodeSaveInfo::getnodeId() const
{
  return mNodeId;
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

QPixmap NodeSaveInfo::getPixmap() const
{
  return mPixmap;
}

QVector<std::shared_ptr<IProperty>> NodeSaveInfo::getfields() const
{
  return mFields;
}

QMap<QString, QVariant> NodeSaveInfo::getproperties() const
{
  return mProperties;
}

QVector<std::shared_ptr<ITransition>> NodeSaveInfo::gettransitions() const
{
  return mTransitions;
}

QVector<std::shared_ptr<IFlow>> NodeSaveInfo::getflows() const
{
  return mFlows;
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

void NodeSaveInfo::setPixmap(const QPixmap& arg)
{
  mPixmap = arg;
}

void NodeSaveInfo::setSize(const QSizeF& arg)
{
  mSize = arg;
}

void NodeSaveInfo::setScale(qreal arg)
{
  mScale = arg;
}

QVariant NodeSaveInfo::getProperty(const QString& key) const
{
  if (mProperties.find(key) == mProperties.end())
    return QVariant();

  return mProperties[key];
}

void NodeSaveInfo::addProperty(const QString& key, const QVariant& value)
{
  mProperties[key] = value;
}

void NodeSaveInfo::removeProperty(const QString& key)
{
  mProperties.remove(key);
}

PropertyInfo NodeSaveInfo::getField(const QString& key) const
{
  for (const auto& field : getfields())
  {
    if (field->getid() == key)
      return *std::dynamic_pointer_cast<PropertyInfo>(field);
  }

  return PropertyInfo();
}

void NodeSaveInfo::setField(const QString& key, std::shared_ptr<IProperty> property)
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

void NodeSaveInfo::addField(std::shared_ptr<IProperty> property)
{
  mFields.push_back(property);
}

void NodeSaveInfo::removeField(const QString& key)
{
  mFields.removeIf([key](std::shared_ptr<IProperty> info) { return info->getid() == key; });
}

void NodeSaveInfo::addTransition(std::shared_ptr<ITransition> transition)
{
  mTransitions.push_back(transition);
}

void NodeSaveInfo::removeTransition(std::shared_ptr<ITransition> transition)
{
  mTransitions.removeIf([transition](std::shared_ptr<ITransition> info) { return info->getid() == transition->getid(); });
}

void NodeSaveInfo::addFlow(std::shared_ptr<IFlow> flow)
{
  mFlows.push_back(flow);
}

void NodeSaveInfo::removeFlow(std::shared_ptr<IFlow> flow)
{
  mFlows.removeIf([flow](std::shared_ptr<IFlow> info) { return info->getid() == flow->getid(); });
}

void NodeSaveInfo::removeFlow(const QString& flowId)
{
  mFlows.removeIf([flowId](std::shared_ptr<IFlow> info) { return info->getid() == flowId; });
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

// ==========================================================================
// JSON serialization
QJsonObject NodeSaveInfo::toJson() const
{
  QJsonObject data;

  data[ConfigKeys::ID] = getid();
  data[ConfigKeys::NODE_ID] = getnodeId();
  data[ConfigKeys::PARENT_ID] = getparentId();

  data[ConfigKeys::SCALE] = getScale();
  data[ConfigKeys::SIZE] = JSON::fromSizeF(getSize());
  data[ConfigKeys::POSITION] = JSON::fromPointF(getposition());

  QJsonArray fieldArray;
  for (const auto& field : getfields())
    fieldArray.append(std::dynamic_pointer_cast<PropertyInfo>(field)->toJson());

  QJsonArray transitionArray;
  for (const auto& transition : gettransitions())
    transitionArray.append(std::dynamic_pointer_cast<TransitionSaveInfo>(transition)->toJson());

  QJsonArray childrenArray;
  for (const auto& child : getchildren())
    childrenArray.append(std::dynamic_pointer_cast<NodeSaveInfo>(child)->toJson());

  QJsonArray flowArray;
  for (const auto& flow : getflows())
    flowArray.append(std::dynamic_pointer_cast<FlowSaveInfo>(flow)->toJson());

  QJsonObject propertiesObject;
  for (auto it = mProperties.constBegin(); it != mProperties.constEnd(); ++it)
    propertiesObject[it.key()] = it.value().toJsonValue();

  if (mProperties.size() > 0)
    data[ConfigKeys::PROPERTIES] = propertiesObject;
  if (fieldArray.size() > 0)
    data[ConfigKeys::FIELDS] = fieldArray;
  if (transitionArray.size() > 0)
    data[ConfigKeys::TRANSITIONS] = transitionArray;
  if (childrenArray.size() > 0)
    data[ConfigKeys::CHILDREN] = childrenArray;
  if (flowArray.size() > 0)
    data[ConfigKeys::FLOWS] = flowArray;

  data[ConfigKeys::PIXMAP] = JSON::fromPixmap(getPixmap());

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

  if (data.contains(ConfigKeys::FIELDS))
  {
    for (const auto& node : data[ConfigKeys::FIELDS].toArray())
      info.addField(std::make_shared<PropertyInfo>(PropertyInfo::fromJson(node.toObject())));
  }

  if (data.contains(ConfigKeys::TRANSITIONS))
  {
    for (const auto& node : data[ConfigKeys::TRANSITIONS].toArray())
      info.addTransition(std::make_shared<TransitionSaveInfo>(TransitionSaveInfo::fromJson(node.toObject())));
  }

  if (data.contains(ConfigKeys::CHILDREN))
  {
    for (const auto& node : data[ConfigKeys::CHILDREN].toArray())
      info.addChild(std::make_shared<NodeSaveInfo>(NodeSaveInfo::fromJson(node.toObject())));
  }

  if (data.contains(ConfigKeys::FLOWS))
  {
    for (const auto& node : data[ConfigKeys::FLOWS].toArray())
      info.addFlow(std::make_shared<FlowSaveInfo>(FlowSaveInfo::fromJson(node.toObject())));
  }

  if (data.contains(ConfigKeys::PROPERTIES))
  {
    const auto propertiesObject = data[ConfigKeys::PROPERTIES].toObject();
    for (const QString& key : propertiesObject.keys())
      info.addProperty(key, propertiesObject.value(key));
  }

  info.setPixmap(JSON::toPixmap(data[ConfigKeys::PIXMAP].toObject()));

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
    in >> *nodes[i];

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

  out << info.getposition();
  out << info.getSize();
  out << info.getScale();

  out << info.getproperties();
  out << info.getchildren();
  out << info.getflows();
  out << info.gettransitions();
  out << info.getfields();

  QByteArray pixmapData;
  QBuffer buffer(&pixmapData);
  buffer.open(QIODevice::WriteOnly);
  info.getPixmap().save(&buffer, "PNG");

  out << pixmapData;

  return out;
}

QDataStream& operator>>(QDataStream& in, NodeSaveInfo& info)
{
  QString id;
  in >> id;
  info.setId(id);

  QString nodeId;
  in >> nodeId;
  info.setNodeId(nodeId);

  QString parentId;
  in >> parentId;
  info.setParentId(parentId);

  QPointF position;
  in >> position;
  info.setPosition(position);

  QSizeF size;
  in >> size;
  info.setSize(size);

  qreal scale;
  in >> scale;
  info.setScale(scale);

  QMap<QString, QVariant> properties;
  in >> properties;
  for (auto iter = properties.keyValueBegin(); iter != properties.keyValueEnd(); ++iter)
    info.addProperty(iter->first, iter->second);

  QVector<std::shared_ptr<NodeSaveInfo>> children;
  in >> children;
  for (const auto& child : children)
    info.addChild(std::dynamic_pointer_cast<INode>(child));

  QVector<std::shared_ptr<FlowSaveInfo>> flows;
  in >> flows;
  for (const auto& flow : flows)
    info.addFlow(std::dynamic_pointer_cast<IFlow>(flow));

  QVector<std::shared_ptr<TransitionSaveInfo>> transitions;
  in >> transitions;
  for (const auto& transition : transitions)
    info.addTransition(std::dynamic_pointer_cast<ITransition>(transition));

  QVector<std::shared_ptr<PropertyInfo>> fields;
  in >> fields;
  for (const auto& field : fields)
    info.addField(std::dynamic_pointer_cast<IProperty>(field));

  QPixmap pixmap;
  QByteArray pixmapData;
  in >> pixmapData;

  pixmap.loadFromData(pixmapData, "PNG");
  info.setPixmap(pixmap);

  return in;
}
