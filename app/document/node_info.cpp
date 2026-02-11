#include "node_info.h"

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
    , mTransitions({})
    , mFlows({})
    , mChildren({})
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
  QVector<std::shared_ptr<IProperty>> args;
  for (auto arg : fields)
    args.emplace_back(std::static_pointer_cast<IProperty>(arg));

  return args;
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

void NodeSaveInfo::addProperty(const QString& key, const QVariant& value)
{
  mProperties[key] = value;
}

void NodeSaveInfo::removeProperty(const QString& key)
{
  mProperties.remove(key);
}

QVariant NodeSaveInfo::getProperty(const QString& key)
{
  if (mProperties.find(key) == mProperties.end())
    return QVariant();

  return mProperties[key];
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

void NodeSaveInfo::removeChild(std::shared_ptr<INode> child)
{
  mChildren.removeIf([child](std::shared_ptr<INode> info) { return info->getid() == child->getid(); });
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
  for (const auto& field : fields)
    fieldArray.append(field->toJson());

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
      info.fields.append(std::make_shared<PropertiesConfig>(PropertiesConfig::fromJson(node.toObject())));
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
  // out << static_cast<qint32>(nodes.size());
  // for (const auto& node : nodes)
  //   out << *node;

  return out;
}

QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<NodeSaveInfo>>& nodes)
{
  // qint32 size;
  // in >> size;

  // nodes.resize(size);
  // for (int i = 0; i < size; ++i)
  //   in >> *nodes[i];

  return in;
}

QDataStream& operator<<(QDataStream& out, const NodeSaveInfo& info)
{
  // out << info.id;
  // out << info.size;
  // out << info.scale;
  // out << info.nodeId;
  // out << info.fields;
  // out << info.position;
  // out << info.properties;
  // out << info.parentId;
  // out << info.transitions;
  // out << info.children;
  // out << info.flows;

  // QByteArray pixmapData;
  // QBuffer buffer(&pixmapData);
  // buffer.open(QIODevice::WriteOnly);
  // info.pixmap.save(&buffer, "PNG");

  // out << pixmapData;

  return out;
}

QDataStream& operator>>(QDataStream& in, NodeSaveInfo& info)
{
  // in >> info.id;
  // in >> info.size;
  // in >> info.scale;
  // in >> info.nodeId;
  // in >> info.fields;
  // in >> info.position;
  // in >> info.properties;
  // in >> info.parentId;
  // in >> info.transitions;
  // in >> info.children;
  // in >> info.flows;

  // QByteArray pixmapData;
  // in >> pixmapData;
  // info.pixmap.loadFromData(pixmapData, "PNG");

  return in;
}
