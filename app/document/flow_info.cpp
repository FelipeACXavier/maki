#include "flow_info.h"

#include <QJsonArray>
#include <QJsonObject>

#include "keys.h"
#include "logging.h"
#include "node_info.h"
#include "property_info.h"
#include "transition_info.h"
#include "type_helpers.h"

Q_DECLARE_METATYPE(FlowSaveInfo)

FlowSaveInfo::FlowSaveInfo()
    : mId("")
    , mName("")
    , mOwner("")
    , mModifiable(true)
    , mType(Types::CallType::UNKNOWN)
    , mReturnType(koda::types::TypeReference::createVoid())
    , mNodes({})
    , mArguments({})
    , mTransitions({})
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

koda::types::TypeReference FlowSaveInfo::getreturnType() const
{
  return mReturnType;
}

QVector<std::shared_ptr<IParameter>> FlowSaveInfo::getarguments() const
{
  return mArguments;
}

QVector<std::shared_ptr<INode>> FlowSaveInfo::getnodes() const
{
  return mNodes;
}

QVector<std::shared_ptr<ITransition>> FlowSaveInfo::gettransitions() const
{
  return mTransitions;
}

QVector<std::shared_ptr<ITransition>> FlowSaveInfo::gettransitions(const QString& id) const
{
  QVector<std::shared_ptr<ITransition>> transitions = {};
  for (const auto& t : mTransitions)
    if (t->getsrcId() == id)
      transitions.push_back(t);

  return transitions;
}

void FlowSaveInfo::addTransition(std::shared_ptr<ITransition> transition)
{
  mTransitions.push_back(transition);
}

void FlowSaveInfo::removeTransition(std::shared_ptr<ITransition> transition)
{
  mTransitions.removeIf([transition](std::shared_ptr<ITransition> info) { return info->getid() == transition->getid(); });
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

void FlowSaveInfo::setReturnType(const koda::types::TypeReference& arg)
{
  mReturnType = arg;
}

void FlowSaveInfo::setArgument(uint32_t index, std::shared_ptr<IParameter> arg)
{
  mArguments[index] = arg;
}

std::shared_ptr<IParameter> FlowSaveInfo::getArgument(uint32_t index)
{
  return mArguments.at(index);
}

void FlowSaveInfo::addArgument(std::shared_ptr<IParameter> arg)
{
  mArguments.push_back(arg);
}

void FlowSaveInfo::removeArgument(std::shared_ptr<IParameter> arg)
{
  mArguments.removeIf([arg](std::shared_ptr<IParameter> info) { return info->getid() == arg->getid(); });
}

void FlowSaveInfo::addNode(std::shared_ptr<INode> arg)
{
  // Do not add repeated nodes
  if (!arg->getid().isEmpty())
  {
    for (const auto& node : mNodes)
      if (node->getid() == arg->getid())
        return;
  }

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
  data[ConfigKeys::RETURN_TYPE] = maki::typeReferenceToJson(getreturnType());
  data[ConfigKeys::OWNER] = getowner();
  data["linksTo"] = getlinksTo();

  QJsonArray optionArray;
  for (const auto& arg : getarguments())
    optionArray.append(std::dynamic_pointer_cast<PropertyInfo>(arg)->toJson());

  data[ConfigKeys::ARGUMENTS] = optionArray;

  QJsonArray nodesArray;
  for (const auto& node : getnodes())
    nodesArray.append(std::dynamic_pointer_cast<NodeSaveInfo>(node)->toJson());

  QJsonArray transitionArray;
  for (const auto& transition : gettransitions())
    transitionArray.append(std::dynamic_pointer_cast<TransitionSaveInfo>(transition)->toJson());

  if (nodesArray.size() > 0)
    data[ConfigKeys::NODES] = nodesArray;
  if (transitionArray.size() > 0)
    data[ConfigKeys::TRANSITIONS] = transitionArray;

  return data;
}

FlowSaveInfo FlowSaveInfo::fromJson(const QJsonObject& data)
{
  FlowSaveInfo info;
  info.setId(data[ConfigKeys::ID].toString());
  info.setName(data[ConfigKeys::NAME].toString());
  info.setModifiable(data[ConfigKeys::MODIFIABLE].toBool());

  if (data.contains("linksTo"))
    info.setLinksTo(data["linksTo"].toInt());

  info.setType(Types::StringToCallType(data[ConfigKeys::TYPE].toString()));
  auto ref = maki::typeReferenceFromJson(data[ConfigKeys::TYPE].toObject());
  if (ref.IsSuccess())
    info.setReturnType(ref.Value());

  info.setOwner(data[ConfigKeys::OWNER].toString());

  if (data.contains(ConfigKeys::ARGUMENTS))
    for (const auto& argument : data[ConfigKeys::ARGUMENTS].toArray())
      info.addArgument(std::make_shared<PropertyInfo>(PropertyInfo::fromJson(argument.toObject())));

  if (data.contains(ConfigKeys::NODES))
    for (const auto& node : data[ConfigKeys::NODES].toArray())
      info.addNode(std::make_shared<NodeSaveInfo>(NodeSaveInfo::fromJson(node.toObject())));

  if (data.contains(ConfigKeys::TRANSITIONS))
    for (const auto& node : data[ConfigKeys::TRANSITIONS].toArray())
      info.addTransition(std::make_shared<TransitionSaveInfo>(TransitionSaveInfo::fromJson(node.toObject())));

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
  {
    flows[i] = std::make_shared<FlowSaveInfo>();
    in >> *flows[i];
  }

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
  out << info.gettransitions();

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
  for (const auto& node : nodes)
    info.addNode(std::dynamic_pointer_cast<NodeSaveInfo>(node));

  bool modifiable;
  in >> modifiable;
  info.setModifiable(modifiable);

  Types::CallType type;
  in >> type;
  info.setType(type);

  koda::types::TypeReference returnType;
  in >> returnType;
  info.setReturnType(returnType);

  QString owner;
  in >> owner;
  info.setOwner(owner);

  int linksTo;
  in >> linksTo;
  info.setLinksTo(linksTo);

  QVector<std::shared_ptr<IParameter>> arguments;
  in >> arguments;
  for (const auto& argument : arguments)
    info.addArgument(std::dynamic_pointer_cast<PropertyInfo>(argument));

  QVector<std::shared_ptr<TransitionSaveInfo>> transitions;
  in >> transitions;
  for (const auto& transition : transitions)
    info.addTransition(std::dynamic_pointer_cast<ITransition>(transition));

  return in;
}
