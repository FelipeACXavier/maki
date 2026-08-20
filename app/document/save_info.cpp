#include "save_info.h"

#include <QJsonArray>

#include "config.h"
#include "json.h"
#include "keys.h"
#include "logging.h"
#include "type_registry.h"

Q_DECLARE_METATYPE(SaveInfo)

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

void SaveInfo::removeNode(const QString& nodeId)
{
  mStructuralNodes.removeIf([nodeId](std::shared_ptr<INode> info) { return info->getid() == nodeId; });
}

QVector<std::shared_ptr<INode>> SaveInfo::getnodes() const
{
  return mStructuralNodes;
}

QVector<koda::types::TypeDefinition> SaveInfo::gettypes() const
{
  QVector<koda::types::TypeDefinition> types;
  for (const auto& type : maki::TypeRegistry::instance().allTypes())
    types.push_back(*type);

  return types;
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
  mMissionParameters.erase(std::remove_if(mMissionParameters.begin(), mMissionParameters.end(),
                                          [parameter](const maki::MissionParameter& p) { return p.name == parameter.name; }));
}

void SaveInfo::clearNodes()
{
  mStructuralNodes.clear();
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

void SaveInfo::findChildrenOfTask(const std::shared_ptr<INode> task, QVector<std::shared_ptr<NodeSaveInfo>>& out,
                                  const Types::ControlTypes type) const
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
    LOG_TRACE("Looking into flow: {}", flow->getname());
    for (const auto& construct : flow->getnodes())
    {
      if (construct->getid() != nodeId)
        continue;

      return std::static_pointer_cast<NodeSaveInfo>(node);
    }
  }

  return nullptr;
}

QVector<std::shared_ptr<IProperty>> SaveInfo::getPossibleStates(const QString& nodeId) const
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

  return QVector<std::shared_ptr<IProperty>>();
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
      if (flow->getid() == flowId)
        return std::static_pointer_cast<FlowSaveInfo>(flow);

    auto found = getFlowWithId(flowId, node->getchildren());
    if (found != nullptr)
      return found;
  }

  return nullptr;
}

QVector<std::shared_ptr<FlowSaveInfo>> SaveInfo::pipelines() const
{
  return mPipelines;
}

void SaveInfo::addPipeline(std::shared_ptr<FlowSaveInfo> pipeline)
{
  mPipelines.push_back(pipeline);
}

void SaveInfo::removePipeline(std::shared_ptr<FlowSaveInfo> pipeline)
{
  mPipelines.removeIf([pipeline](std::shared_ptr<FlowSaveInfo> info) { return info->getid() == pipeline->getid(); });
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