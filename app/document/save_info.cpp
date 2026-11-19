#include "save_info.h"

#include <QJsonArray>

#include "app_configs.h"
#include "config_table.h"
#include "keys.h"
#include "logging.h"
#include "macros.h"
#include "type_registry.h"

Q_DECLARE_METATYPE(SaveInfo)

SaveInfo& SaveInfo::instance()
{
  static SaveInfo storage;
  return storage;
}

void SaveInfo::reset()
{
  mCanvasInfo = CanvasSaveInfo();
  mStructuralNodes.clear();
  mPipelines.clear();
  mMissionParameters.clear();

  // Do not destroy the scheduler
}

CanvasSaveInfo SaveInfo::canvasInfo() const
{
  return mCanvasInfo;
}

void SaveInfo::setCanvasInfo(const CanvasSaveInfo& info)
{
  mCanvasInfo = info;
}

void SaveInfo::print() const
{
  LOG_INFO("Document: {} {}", name, version);
  for (const auto& inode : getnodes())
    if (auto node = std::dynamic_pointer_cast<NodeSaveInfo>(inode))
      node->print("  ");
}

std::shared_ptr<NodeSaveInfo> createNode(const QString& nodeType, const QPointF& position)
{
  auto config = ConfigurationTable::instance().get(nodeType);
  if (config == nullptr)
    return nullptr;

  auto info = std::make_shared<NodeSaveInfo>(*config);
  info->setId(QUuid::createUuid().toString());
  info->setPosition(position);
  return info;
}

std::shared_ptr<TransitionSaveInfo> createTransition(PNodeSaveInfo src, PNodeSaveInfo dst)
{
  auto transition = std::make_shared<TransitionSaveInfo>();
  transition->setId(QUuid::createUuid().toString());

  transition->setSrcId(src->getid());
  transition->setDstId(dst->getid());

  transition->setSrcPoint(src->getposition());
  transition->setDstPoint(dst->getposition());
  transition->setSrcShift({0, 0});
  transition->setDstShift({0, 0});
  transition->setSrcPort(Types::Port::OUT);
  transition->setDstPort(Types::Port::IN);

  return transition;
}

void SaveInfo::addFlowToNode(const QString& nodeId, std::shared_ptr<FlowSaveInfo> flow, const QList<QPointF>& initialNodePositions)
{
  if (auto node = getNodeWithId(nodeId))
  {
    // Do not add the flow if it already exists
    if (maki::anyOf(node->getflows(), [flow](const auto& f) { return f->getid() == flow->getid(); }))
      return;

    flow->setId(QUuid::createUuid().toString());
    flow->setOwner(nodeId);
    flow->setType(Types::CallType::USER);
    flow->setModifiable(true);
    flow->setReturnType(koda::types::TypeReference::createVoid());

    node->addFlow(flow);

    if (flow->getnodes().isEmpty() && initialNodePositions.size() >= 2)
    {
      auto src = createNode(ConfigKeys::START_NODE, initialNodePositions.at(0));
      auto dst = createNode(ConfigKeys::SUCCESS_NODE, initialNodePositions.at(1));
      if (!src || !dst)
        return;

      flow->addNode(src);
      flow->addNode(dst);
      flow->addTransition(createTransition(src, dst));
    }
  }
}

void SaveInfo::removeFlowFromNode(const QString& flowId, const QString& nodeId)
{
  if (auto node = getNodeWithId(nodeId))
    node->removeFlow(flowId);
}

void SaveInfo::addNodeToFlow(const QString& flowId, std::shared_ptr<INode> node)
{
  if (auto flow = getFlowWithId(flowId))
  {
    // Do not add the node if it already exists
    if (maki::anyOf(flow->getnodes(), [node](const auto& n) { return n->getid() == node->getid(); }))
      return;

    flow->addNode(node);
  }
}

std::shared_ptr<NodeSaveInfo> SaveInfo::getPipelineNodeWithId(const QString& nodeId) const
{
  for (const auto& pipeline : mPipelines)
  {
    auto node = maki::findIf(pipeline->getnodes(), [nodeId](const auto& info) { return info->getid() == nodeId; });
    if (node.has_value())
      return std::dynamic_pointer_cast<NodeSaveInfo>(*node);
  }
  return nullptr;
}

std::shared_ptr<FlowSaveInfo> SaveInfo::getPipelineWithId(const QString& pipelineId) const
{
  auto pipeline = maki::findIf(mPipelines, [pipelineId](const auto& info) { return info->getid() == pipelineId; });
  return pipeline.value_or(nullptr);
}

void SaveInfo::addTransitionToFlow(const QString& flowId, std::shared_ptr<ITransition> transition)
{
  if (auto flow = getFlowWithId(flowId))
  {
    if (maki::anyOf(flow->getnodes(), [transition](const auto& t) { return t->getid() == transition->getid(); }))
      return;

    flow->addTransition(transition);
  }
}

void SaveInfo::addNodeToPipeline(const QString& flowId, std::shared_ptr<INode> node)
{
  if (auto pipeline = getPipelineWithId(flowId))
  {
    if (maki::anyOf(pipeline->getnodes(), [node](const auto& n) { return n->getid() == node->getid(); }))
      return;

    pipeline->addNode(node);
  }
}

void SaveInfo::addTransitionToPipeline(const QString& flowId, std::shared_ptr<ITransition> transition)
{
  if (auto pipeline = getPipelineWithId(flowId))
  {
    if (maki::anyOf(pipeline->getnodes(), [transition](const auto& t) { return t->getid() == transition->getid(); }))
      return;

    pipeline->addTransition(transition);
  }
}

void SaveInfo::addNode(std::shared_ptr<NodeSaveInfo> node)
{
  // If this is a child, we must add it to the parent
  if (const auto parent = getNodeWithId(node->getparentId()); parent)
    parent->addChild(node);
  else
    mStructuralNodes.push_back(node);

  if (!node->getid().isEmpty())
    publish(DocumentEvent{.type = DocumentEvent::Type::NodeAdded,
                          .data = NodeEvent{
                            .id = node->getid(),
                            .nodeType = node->getnodeId(),
                            .parentId = node->getparentId(),
                            .ownerFlowId = Config::MAIN_CANVAS,
                          }});
}

void SaveInfo::removeNode(const QString& nodeId)
{
  auto node = getNodeWithId(nodeId);
  if (!node)
    return;

  const QString parentId = node->getparentId();
  if (!parentId.isEmpty())
  {
    // The node belongs to another structural node.
    auto parent = getNodeWithId(parentId);
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
  else
  {
    // Root structural node.
    mStructuralNodes.removeIf([nodeId](std::shared_ptr<INode> info) { return info->getid() == nodeId; });
  }

  // The model has already been updated.
  // The graphical representation may still exist.
  publish(DocumentEvent{.type = DocumentEvent::Type::NodeRemoved,
                        .data = NodeEvent{
                          .id = nodeId,
                          .parentId = parentId,
                          .ownerFlowId = Config::MAIN_CANVAS,
                        }});
}

QVector<std::shared_ptr<INode>> SaveInfo::getnodes() const
{
  return mStructuralNodes;
}

const koda::types::TypeRegistry* SaveInfo::getTypesRegistry() const
{
  return &maki::TypeRegistry::instance();
}

QVector<const IParameter*> SaveInfo::getparameters() const
{
  QVector<const IParameter*> out;
  out.reserve(mMissionParameters.size());

  for (const auto& parameter : mMissionParameters)
    out.push_back(&parameter);

  return out;
}

QVector<maki::MissionParameter> SaveInfo::missionParameters() const
{
  return mMissionParameters;
}

void SaveInfo::setParameters(const QVector<maki::MissionParameter>& parameters)
{
  mMissionParameters = std::move(parameters);
}

maki::MissionParameter SaveInfo::getParameter(int index) const
{
  if (index < missionParameters().size())
    return missionParameters().at(index);

  return maki::MissionParameter{};
}

void SaveInfo::addParameter(const maki::MissionParameter& parameter)
{
  mMissionParameters.push_back(parameter);
}

void SaveInfo::setParameter(int index, const maki::MissionParameter& parameter)
{
  if (index < missionParameters().size())
    mMissionParameters[index] = parameter;
}

void SaveInfo::removeParameter(const maki::MissionParameter& parameter)
{
  mMissionParameters.erase(
    std::remove_if(mMissionParameters.begin(), mMissionParameters.end(), [parameter](const maki::MissionParameter& p) { return p.name == parameter.name; }));
}

QVector<std::pair<QString, QString>> SaveInfo::getLinkNodes() const
{
  QVector<std::pair<QString, QString>> result;
  for (const auto& node : getnodes())
    for (const auto& flow : node->getflows())
      for (const auto& child : flow->getnodes())
        if (child && child->getnodeId() == ConfigKeys::LINK_IN_NODE)
          result.append(std::make_pair(child->getid(), maki::getProperty(ConfigKeys::NAME, *child)->toString()));

  return result;
}

QVector<std::shared_ptr<NodeSaveInfo>> SaveInfo::findFamilyOfFlowNode(const QString& nodeId, const QVector<std::shared_ptr<INode>>& nodes,
                                                                      const Types::ControlTypes type) const
{
  for (const auto& node : nodes)
  {
    LOG_TRACE("Looking into: {} {}", node->getid(), node->getnodeId());

    // Find the task which owns the given behaviour node
    // If the task is found, then we start building the family
    auto parent = findOwnerTaskOfFlowNode(nodeId, node);
    if (parent)
    {
      QVector<std::shared_ptr<NodeSaveInfo>> out = {};
      if (type == Types::ControlTypes::FLOW_CALL)
        out.push_back(std::static_pointer_cast<NodeSaveInfo>(node));

      findChildrenOfTask(node, out, type);
      return out;
    }

    // If not found in the parent, maybe we can find it in the children (sub-tasks)
    auto family = findFamilyOfFlowNode(nodeId, node->getchildren(), type);
    if (!family.empty())
      return family;
  }

  return {};
}

void SaveInfo::findChildrenOfTask(const std::shared_ptr<INode> task, QVector<std::shared_ptr<NodeSaveInfo>>& out, const Types::ControlTypes type) const
{
  for (const auto& child : task->getchildren())
  {
    // If we are looking for flows, we should only return children that actually have flows
    // Basically, we should ignore capabilities or any subtask that has no flows defined yet
    if (type != Types::ControlTypes::FLOW_CALL || !child->getflows().empty())
      out.push_back(std::static_pointer_cast<NodeSaveInfo>(child));

    findChildrenOfTask(child, out, type);
  }
}

std::shared_ptr<NodeSaveInfo> SaveInfo::findOwnerTaskOfFlowNode(const QString& nodeId, const std::shared_ptr<INode> node) const
{
  if (!node)
    return nullptr;

  LOG_TRACE("Node info: {} {} {}", node->getnodeId(), node->getchildren().size(), node->getflows().size());

  for (const auto& flow : node->getflows())
  {
    LOG_TRACE("Looking into flow: {} with {} nodes", flow->getname(), flow->getnodes().size());
    for (const auto& construct : flow->getnodes())
    {
      if (construct->getid() != nodeId)
        continue;

      return std::static_pointer_cast<NodeSaveInfo>(node);
    }
  }

  return nullptr;
}

QVector<std::shared_ptr<IParameter>> SaveInfo::getPossibleStates(const QString& nodeId) const
{
  for (const auto& node : getnodes())
  {
    for (const auto& flow : node->getflows())
    {
      for (const auto& child : flow->getnodes())
      {
        if (child->getid() != nodeId)
          continue;

        return node->getfields();
      }
    }
  }

  return QVector<std::shared_ptr<IParameter>>();
}

QVector<std::shared_ptr<NodeSaveInfo>> SaveInfo::getPossibleCallers(const QString& nodeId, const Types::ControlTypes type) const
{
  // Get the parent
  return findFamilyOfFlowNode(nodeId, getnodes(), type);
}

QVector<std::shared_ptr<FlowSaveInfo>> SaveInfo::getEventsFromNode(const QString& nodeId) const
{
  return getEventsFromNode(nodeId, getnodes());
}

QVector<std::shared_ptr<FlowSaveInfo>> SaveInfo::getEventsFromNode(const QString& taskId, QVector<std::shared_ptr<INode>> nodes) const
{
  for (const auto& node : nodes)
  {
    if (node->getid() == taskId)
    {
      QVector<std::shared_ptr<FlowSaveInfo>> out;
      for (auto& flow : node->getevents())
        out.push_back(std::static_pointer_cast<FlowSaveInfo>(flow));

      return out;
    }

    auto events = getEventsFromNode(taskId, node->getchildren());
    if (!events.empty())
      return events;
  }

  return {};
}

QVector<std::shared_ptr<FlowSaveInfo>> SaveInfo::getEventsOfTypeFromNode(const QString& nodeId, const QList<Types::CallType>& types) const
{
  auto events = getEventsFromNode(nodeId, getnodes());
  for (auto it = events.begin(); it != events.end();)
    if (types.contains((*it)->gettype()))
      ++it;
    else
      it = events.erase(it);

  return events;
}

QVector<std::shared_ptr<FlowSaveInfo>> SaveInfo::getFlowsFromNode(const QString& nodeId) const
{
  return getFlowsFromNode(nodeId, getnodes());
}

QVector<std::shared_ptr<FlowSaveInfo>> SaveInfo::getFlowsFromNode(const QString& taskId, QVector<std::shared_ptr<INode>> nodes) const
{
  for (const auto& node : nodes)
  {
    if (node->getid() == taskId)
    {
      QVector<std::shared_ptr<FlowSaveInfo>> out;
      for (auto& flow : node->getflows())
        out.push_back(std::static_pointer_cast<FlowSaveInfo>(flow));

      return out;
    }

    auto events = getFlowsFromNode(taskId, node->getchildren());
    if (!events.empty())
      return events;
  }

  return {};
}

std::shared_ptr<FlowSaveInfo> SaveInfo::getEventFromNode(const QString& nodeId, const QString& flowName) const
{
  auto events = getEventsFromNode(nodeId, getnodes());
  for (const auto& event : events)
    if (event->getname() == flowName)
      return event;

  return nullptr;
}

std::shared_ptr<FlowSaveInfo> SaveInfo::getFlowFromNode(const QString& nodeId, const QString& flowName) const
{
  auto flows = getFlowsFromNode(nodeId, getnodes());
  for (const auto& flow : flows)
    if (flow->getname() == flowName)
      return flow;

  return nullptr;
}

std::shared_ptr<NodeSaveInfo> SaveInfo::getNodeWithId(const QString& nodeId)
{
  auto node = getNodeWithId(nodeId, getnodes());
  if (node)
    return node;

  return getPipelineNodeWithId(nodeId);
}

std::shared_ptr<NodeSaveInfo> SaveInfo::getNodeWithId(const QString& nodeId, const QVector<std::shared_ptr<INode>>& nodes)
{
  for (const auto& node : nodes)
  {
    // LOG_DEBUG("Trying to find node: {} vs {}. Children={} Flows={}", nodeId, node->getid(), node->getchildren().size(), node->getflows().size());
    if (node->getid() == nodeId)
      return std::static_pointer_cast<NodeSaveInfo>(node);

    auto found = getNodeWithId(nodeId, node->getchildren());
    if (found != nullptr)
      return found;

    for (const auto& flow : node->getflows())
    {
      // LOG_DEBUG("Trying to find node: {} in flow: {}", nodeId, flow->getname());
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
      if (flow->getid() == flowId)
        return std::static_pointer_cast<FlowSaveInfo>(flow);

    auto found = getFlowWithId(flowId, node->getchildren());
    if (found != nullptr)
      return found;
  }

  return nullptr;
}

std::shared_ptr<TransitionSaveInfo> SaveInfo::getTransitionWithId(const QString& transitionId) const
{
  auto transition = getTransitionWithId(transitionId, getnodes());
  if (transition)
    return transition;

  return getPipelineTransitionWithId(transitionId);
}

std::shared_ptr<TransitionSaveInfo> SaveInfo::getTransitionWithId(const QString& transitionId, const QVector<std::shared_ptr<INode>>& nodes) const
{
  for (const auto& node : nodes)
    for (const auto& flow : node->getflows())
      for (const auto& transition : flow->gettransitions())
        if (transition->getid() == transitionId)
          return std::static_pointer_cast<TransitionSaveInfo>(transition);

  return nullptr;
}

std::shared_ptr<TransitionSaveInfo> SaveInfo::getPipelineTransitionWithId(const QString& transitionId) const
{
  for (const auto& flow : mPipelines)
    for (const auto& transition : flow->gettransitions())
      if (transition->getid() == transitionId)
        return std::static_pointer_cast<TransitionSaveInfo>(transition);

  return nullptr;
}

QVector<std::shared_ptr<FlowSaveInfo>> SaveInfo::pipelines() const
{
  return mPipelines;
}

void SaveInfo::addPipeline(std::shared_ptr<FlowSaveInfo> pipeline)
{
  if (maki::anyOf(mPipelines, [pipeline](const auto& info) { return info->getid() == pipeline->getid() || info->getname() == pipeline->getname(); }))
    return;

  mPipelines.push_back(pipeline);
  publish(DocumentEvent{.type = DocumentEvent::Type::PipelineAdded,
                        .data = PipelineEvent{
                          .id = pipeline->getid(),
                          .name = pipeline->getname(),
                        }});
}

void SaveInfo::removePipeline(std::shared_ptr<FlowSaveInfo> pipeline)
{
  auto removed = maki::takeIf(mPipelines, [pipeline](std::shared_ptr<FlowSaveInfo> info) { return info->getid() == pipeline->getid(); });
  if (removed)
    publish(DocumentEvent{.type = DocumentEvent::Type::PipelineRemoved,
                          .data = PipelineEvent{
                            .id = removed->getid(),
                            .name = removed->getname(),
                          }});
}

bool SaveInfo::taskHasCapability(const QString& taskId, const QString& capabilityId) const
{
  for (const auto& node : getnodes())
  {
    if (node->getid() != taskId)
      continue;

    for (const auto& child : node->getchildren())
      if (child->getnodeId() == capabilityId)
        return true;
  }

  return false;
}

void SaveInfo::setScheduler(std::shared_ptr<EventDispatcher<DocumentEvent>> scheduler)
{
  mScheduler = std::move(scheduler);
}

EventDispatcher<DocumentEvent>::Subscription SaveInfo::subscribe(EventDispatcher<DocumentEvent>::Callback callback)
{
  if (mScheduler)
    return mScheduler->subscribe(callback);
  else
    return EventDispatcher<DocumentEvent>::Subscription();
}

void SaveInfo::publish(const DocumentEvent& event)
{
  if (mScheduler)
    mScheduler->publish(event);
}

void SaveInfo::blockEvents()
{
  if (mScheduler)
    mScheduler->blockEvents();
}

void SaveInfo::unblockEvents()
{
  if (mScheduler)
    mScheduler->unblockEvents();
}

// ==========================================================================
// JSON serialization
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

// ==========================================================================
// Stream serialization
QDataStream& operator<<(QDataStream& out, const SaveInfo& info)
{
  out << info.mCanvasInfo;
  out << info.mStructuralNodes;

  return out;
}

QDataStream& operator>>(QDataStream& in, SaveInfo& info)
{
  in >> info.mCanvasInfo;
  in >> info.mStructuralNodes;

  return in;
}
