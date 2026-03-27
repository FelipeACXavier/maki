#include "save_info.h"

#include <QJsonArray>

#include "config.h"
#include "json.h"
#include "keys.h"
#include "logging.h"

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

void SaveInfo::removeNode(std::shared_ptr<NodeSaveInfo> node)
{
  mStructuralNodes.removeIf([node](std::shared_ptr<INode> info) { return info->getid() == node->getid(); });
}

QVector<std::shared_ptr<INode>> SaveInfo::getnodes() const
{
  return mStructuralNodes;
};

QVector<std::shared_ptr<NodeSaveInfo>> SaveInfo::findFamilyOfConstruct(const QString& nodeId, QVector<std::shared_ptr<INode>> nodes) const
{
  for (const auto& node : nodes)
  {
    LOG_TRACE("Looking into: %s %s", qPrintable(node->getid()), qPrintable(node->getnodeId()));
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

  LOG_TRACE("Node info: %s %d %d", qPrintable(node->getnodeId()), node->getchildren().size(), node->getflows().size());

  for (const auto& flow : node->getflows())
  {
    LOG_TRACE("Looking into flow: %s", qPrintable(flow->getname()));
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
  // for (const auto& node : nodes)
  // {
  //   if (!node->getfields().isEmpty())
  //     toReturn.push_back(std::static_pointer_cast<NodeSaveInfo>(node));

  //   findStatesOfConstruct(toReturn, node->getchildren());
  // }
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

QVector<std::shared_ptr<FlowSaveInfo>> SaveInfo::getEventsOfTypeFromNode(const QString& nodeId, Types::CallType type) const
{
  auto events = getEventsFromNode(nodeId, getnodes());
  for (auto it = events.begin(); it != events.end();)
  {
    if ((*it)->gettype() != type)
      it = events.erase(it);
    else
      ++it;
  }

  return events;
}

std::shared_ptr<FlowSaveInfo> SaveInfo::getEventFromNode(const QString& nodeId, const QString& flowName) const
{
  auto events = getEventsFromNode(nodeId, getnodes());
  for (const auto& event : events)
  {
    if (event->getname() == flowName)
      return event;
  }

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