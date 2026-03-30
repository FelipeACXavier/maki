#include "flow_info.h"

#include <QJsonArray>
#include <QJsonObject>

#include "keys.h"
#include "node_info.h"

Q_DECLARE_METATYPE(FlowSaveInfo)

FlowSaveInfo::FlowSaveInfo()
    : mId("")
    , mName("")
    , mOwner("")
    , mModifiable(true)
    , mType(Types::CallType::UNKNOWN)
    , mReturnType(Types::PropertyTypes::UNKNOWN)
    , mNodes({})
    , mArguments({})
{
}

FlowSaveInfo::FlowSaveInfo(const FlowConfig& config)
{
  setId(QUuid::createUuid().toString());
  setName(config.name);
  setType(config.type);
  setReturnType(config.returnType);
  setModifiable(config.modifiable);
  setLinksTo(config.linksTo);
  for (const auto& arg : config.arguments)
    addArgument(std::make_shared<PropertyInfo>(arg));
}

QString FlowSaveInfo::getid() const
{
  return mId;
}

QString FlowSaveInfo::getname() const
{
  return mName;
}

QString FlowSaveInfo::getowner() const
{
  return mOwner;
}

bool FlowSaveInfo::getmodifiable() const
{
  return mModifiable;
}

Types::CallType FlowSaveInfo::gettype() const
{
  return mType;
}

int FlowSaveInfo::getlinksTo() const
{
  return mLinksTo;
}

Types::PropertyTypes FlowSaveInfo::getreturnType() const
{
  return mReturnType;
}

QVector<std::shared_ptr<IProperty>> FlowSaveInfo::getarguments() const
{
  return mArguments;
}

QVector<std::shared_ptr<INode>> FlowSaveInfo::getnodes() const
{
  return mNodes;
}

void FlowSaveInfo::setId(const QString& arg)
{
  mId = arg;
}

void FlowSaveInfo::setName(const QString& arg)
{
  mName = arg;
}

void FlowSaveInfo::setOwner(const QString& arg)
{
  mOwner = arg;
}

void FlowSaveInfo::setModifiable(bool arg)
{
  mModifiable = arg;
}

void FlowSaveInfo::setLinksTo(int arg)
{
  mLinksTo = arg;
}

void FlowSaveInfo::setType(Types::CallType arg)
{
  mType = arg;
}

void FlowSaveInfo::setReturnType(Types::PropertyTypes arg)
{
  mReturnType = arg;
}

void FlowSaveInfo::setArgument(uint32_t index, std::shared_ptr<IProperty> arg)
{
  mArguments[index] = arg;
}

std::shared_ptr<IProperty> FlowSaveInfo::getArgument(uint32_t index)
{
  return mArguments.at(index);
}

void FlowSaveInfo::addArgument(std::shared_ptr<IProperty> arg)
{
  mArguments.push_back(arg);
}

void FlowSaveInfo::removeArgument(std::shared_ptr<IProperty> arg)
{
  mArguments.removeIf([arg](std::shared_ptr<IProperty> info) { return info->getid() == arg->getid(); });
}

void FlowSaveInfo::addNode(std::shared_ptr<INode> arg)
{
  mNodes.push_back(arg);
}

void FlowSaveInfo::removeNode(std::shared_ptr<INode> node)
{
  mNodes.removeIf([node](std::shared_ptr<INode> info) { return info->getid() == node->getid(); });
}

// ==========================================================================
// JSON serialization
QJsonObject FlowSaveInfo::toJson() const
{
  QJsonObject data;
  data[ConfigKeys::ID] = getid();
  data[ConfigKeys::NAME] = getname();
  data[ConfigKeys::MODIFIABLE] = getmodifiable();
  data[ConfigKeys::TYPE] = Types::CallTypeToString(gettype());
  data[ConfigKeys::RETURN_TYPE] = Types::PropertyTypesToString(getreturnType());
  data[ConfigKeys::OWNER] = getowner();
  data["linksTo"] = getlinksTo();

  QJsonArray optionArray;
  for (const auto& arg : getarguments())
    optionArray.append(std::dynamic_pointer_cast<PropertyInfo>(arg)->toJson());

  data[ConfigKeys::ARGUMENTS] = optionArray;

  QJsonArray nodesArray;
  for (const auto& node : getnodes())
    nodesArray.append(std::dynamic_pointer_cast<NodeSaveInfo>(node)->toJson());

  if (nodesArray.size() > 0)
    data[ConfigKeys::NODES] = nodesArray;

  return data;
}

FlowSaveInfo FlowSaveInfo::fromJson(const QJsonObject& data)
{
  FlowSaveInfo info;
  info.setId(data[ConfigKeys::ID].toString());
  info.setName(data[ConfigKeys::NAME].toString());
  info.setModifiable(data[ConfigKeys::MODIFIABLE].toBool());
  info.setLinksTo(data["linksTo"].toInt());
  info.setType(Types::StringToCallType(data[ConfigKeys::TYPE].toString()));
  info.setReturnType(Types::StringToPropertyTypes(data[ConfigKeys::RETURN_TYPE].toString()));
  info.setOwner(data[ConfigKeys::OWNER].toString());

  for (const auto& argument : data[ConfigKeys::ARGUMENTS].toArray())
    info.addArgument(std::make_shared<PropertyInfo>(PropertyInfo::fromJson(argument.toObject())));

  for (const auto& node : data[ConfigKeys::NODES].toArray())
    info.addNode(std::make_shared<NodeSaveInfo>(NodeSaveInfo::fromJson(node.toObject())));

  return info;
}

// ==========================================================================
// Stream serialization
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

QDataStream& operator<<(QDataStream& out, const QVector<std::shared_ptr<IFlow>>& flows)
{
  out << static_cast<qint32>(flows.size());
  for (const auto& flow : flows)
    out << *std::dynamic_pointer_cast<FlowSaveInfo>(flow);

  return out;
}

QDataStream& operator>>(QDataStream& in, QVector<std::shared_ptr<IFlow>>& flows)
{
  qint32 size;
  in >> size;

  flows.resize(size);
  for (int i = 0; i < size; ++i)
  {
    flows[i] = std::make_shared<FlowSaveInfo>();
    in >> *std::dynamic_pointer_cast<FlowSaveInfo>(flows[i]);
  }

  return in;
}

QDataStream& operator<<(QDataStream& out, const FlowSaveInfo& info)
{
  out << info.getid();
  out << info.getname();
  out << info.getnodes();
  out << info.getmodifiable();
  out << info.gettype();
  out << info.getreturnType();
  out << info.getowner();
  out << info.getlinksTo();

  out << info.getarguments();

  return out;
}

QDataStream& operator>>(QDataStream& in, FlowSaveInfo& info)
{
  QString id;
  in >> id;
  info.setId(id);

  QString name;
  in >> name;
  info.setName(name);

  QVector<std::shared_ptr<INode>> nodes;
  in >> nodes;

  bool modifiable;
  in >> modifiable;
  info.setModifiable(modifiable);

  Types::CallType type;
  in >> type;
  info.setType(type);

  Types::PropertyTypes returnType;
  in >> returnType;
  info.setReturnType(returnType);

  QString owner;
  in >> owner;
  info.setOwner(owner);

  int linksTo;
  in >> linksTo;
  info.setLinksTo(linksTo);

  QVector<std::shared_ptr<IProperty>> arguments;
  in >> arguments;

  return in;
}
