#include "flow_info.h"

#include <QJsonArray>
#include <QJsonObject>

#include "document_event.h"
#include "keys.h"
#include "logging.h"
#include "macros.h"
#include "node_info.h"
#include "property_info.h"
#include "save_info.h"
#include "transition_info.h"
#include "type_helpers.h"

Q_DECLARE_METATYPE(FlowSaveInfo)

FlowSaveInfo::FlowSaveInfo()
    : mId("")
    , mName("")
    , mOwner("")
    , mModifiable(true)
    , mLinksTo(0)
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

void FlowSaveInfo::print(const QString& indent) const
{
  LOG_INFO("{}Flow id: {}", indent, getid());
  LOG_INFO("{}Flow name: {}", indent, getname());
  LOG_INFO("{}Flow owner: {}", indent, getowner());
  LOG_INFO("{}Nodes: {}", indent, getnodes().size());
  for (const auto& ichild : getnodes())
    if (auto child = std::dynamic_pointer_cast<NodeSaveInfo>(ichild))
      child->print(indent + "  ");
  LOG_INFO("{}Transitions: {}", indent, gettransitions().size());
  for (const auto& ichild : gettransitions())
    if (auto child = std::dynamic_pointer_cast<TransitionSaveInfo>(ichild))
      child->print(indent + "  ");
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
  SaveInfo::instance().publish(DocumentEvent{.type = DocumentEvent::Type::TransitionAdded,
                                             .data = TransitionEvent{
                                               .id = transition->getid(),
                                               .ownerFlowId = getid(),
                                             }});
}

void FlowSaveInfo::removeTransition(const QString& transitionId)
{
  mTransitions.removeIf([transitionId](std::shared_ptr<ITransition> info) { return info->getid() == transitionId; });

  SaveInfo::instance().publish(DocumentEvent{.type = DocumentEvent::Type::TransitionRemoved,
                                             .data = TransitionEvent{
                                               .id = transitionId,
                                               .ownerFlowId = getid(),
                                             }});
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
  SaveInfo::instance().publish(DocumentEvent{.type = DocumentEvent::Type::FlowModified,
                                             .data = FlowEvent{
                                               .id = getid(),
                                             }});
}

std::shared_ptr<IParameter> FlowSaveInfo::getArgument(uint32_t index)
{
  return mArguments.at(index);
}

void FlowSaveInfo::addArgument(std::shared_ptr<IParameter> arg)
{
  // Thou shall not add repeated arguments
  if (maki::anyOf(mArguments, [name = arg->getname()](const auto& info) { return info->getname() == name; }))
    return;

  mArguments.push_back(arg);
}

void FlowSaveInfo::removeArgument(std::shared_ptr<IParameter> arg)
{
  mArguments.removeIf([arg](std::shared_ptr<IParameter> info) { return info->getid() == arg->getid(); });
  SaveInfo::instance().publish(DocumentEvent{.type = DocumentEvent::Type::FlowModified,
                                             .data = FlowEvent{
                                               .id = getid(),
                                             }});
}

void FlowSaveInfo::addNode(std::shared_ptr<INode> inode)
{
  // Do not add repeated nodes
  if (maki::anyOf(getnodes(), [nodeId = inode->getid()](const auto& info) { return info->getid() == nodeId; }))
    return;

  // Connect the node to the parent on creation
  if (!inode->getparentId().isEmpty())
    if (auto parent = getNode(inode->getparentId()); parent)
      parent->addChild(inode);

  mNodes.push_back(inode);
  SaveInfo::instance().publish(DocumentEvent{.type = DocumentEvent::Type::NodeAdded,
                                             .data = NodeEvent{
                                               .id = inode->getid(),
                                               .nodeType = inode->getnodeId(),
                                               .parentId = inode->getparentId(),
                                               .ownerFlowId = getid(),
                                             }});
}

void FlowSaveInfo::removeNode(const QString& nodeId)
{
  // The node is removed from the list if it exists, and this copy is destroyed at the end of the scope
  auto node = maki::takeIf(mNodes, [nodeId](const auto& info) { return info->getid() == nodeId; });
  if (!node)
    return;

  if (!node->getparentId().isEmpty())
  {
    auto parent = getNode(node->getparentId());
    if (!parent)
      return;

    // Verify the immediate parent actually contains it.
    // We don't want to emit the signal in a fluke
    const bool found = maki::anyOf(parent->getchildren(), [&nodeId](const auto& child) { return child && child->getid() == nodeId; });
    if (!found)
      return;

    // Removes the root of the requested subtree.
    parent->removeChild(nodeId);
  }

  SaveInfo::instance().publish(DocumentEvent{.type = DocumentEvent::Type::NodeRemoved,
                                             .data = NodeEvent{
                                               .id = nodeId,
                                               .parentId = node ? node->getparentId() : "",
                                               .ownerFlowId = getid(),
                                             }});
}

std::shared_ptr<NodeSaveInfo> FlowSaveInfo::getNode(const QString& nodeId) const
{
  auto node = maki::findIf(getnodes(), [nodeId](const auto& node) { return node->getid() == nodeId; });
  if (node)
    return std::dynamic_pointer_cast<NodeSaveInfo>(*node);

  return nullptr;
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
  data[ConfigKeys::LINKS_TO] = getlinksTo();

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

  if (data.contains(ConfigKeys::LINKS_TO))
    info.setLinksTo(data[ConfigKeys::LINKS_TO].toInt());

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
  // We don't use the setters so that we don't update the event dispatcher
  QString id;
  in >> id;
  info.mId = id;

  QString name;
  in >> name;
  info.mName = name;

  QVector<std::shared_ptr<INode>> nodes;
  in >> nodes;
  info.mNodes = nodes;

  bool modifiable;
  in >> modifiable;
  info.mModifiable = modifiable;

  Types::CallType type;
  in >> type;
  info.mType = type;

  koda::types::TypeReference returnType;
  in >> returnType;
  info.mReturnType = returnType;

  QString owner;
  in >> owner;
  info.mOwner = owner;

  int linksTo;
  in >> linksTo;
  info.mLinksTo = linksTo;

  QVector<std::shared_ptr<IParameter>> arguments;
  in >> arguments;
  info.mArguments = arguments;

  QVector<std::shared_ptr<ITransition>> transitions;
  in >> transitions;
  info.mTransitions = transitions;

  return in;
}
