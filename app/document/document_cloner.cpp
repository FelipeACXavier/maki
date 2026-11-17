#include "document_cloner.h"

#include <QUuid>
#include <stdexcept>

#include "flow_info.h"
#include "property_info.h"
#include "transition_info.h"

namespace
{
QString newId()
{
  return QUuid::createUuid().toString();
}
}  // namespace

// =============================================================
// Public entry point
// =============================================================

NodeSaveInfo DocumentCloner::cloneNodeForPaste(const NodeSaveInfo& original, const QString& newParentId, const QPointF& position)
{
  DocumentCloner cloner;

  // Register every ID before attempting to remap references.
  cloner.indexNode(original);

  // Positions of structural children are absolute scene
  // coordinates, so move the whole visual subtree.
  const QPointF offset = position - original.getposition();

  auto result = cloner.cloneNode(original, std::optional<QString>{newParentId}, offset);

  return *result;
}

// =============================================================
// Pass 1: collect and regenerate IDs
// =============================================================

void DocumentCloner::indexNode(const NodeSaveInfo& node)
{
  if (node.mId.isEmpty())
    throw std::logic_error("Cannot clone a node without an ID");

  if (mNodeIds.contains(node.mId))
    return;

  mNodeIds.insert(node.mId, newId());

  // Structural / subflow children.
  for (const auto& child : node.mChildren)
  {
    auto info = std::dynamic_pointer_cast<NodeSaveInfo>(child);

    if (!info)
      throw std::logic_error("Unsupported node implementation");

    indexNode(*info);
  }

  // Owned behavioral flows.
  for (const auto& flow : node.mFlows)
  {
    auto info = std::dynamic_pointer_cast<FlowSaveInfo>(flow);

    if (!info)
      throw std::logic_error("Unsupported flow implementation");

    indexFlow(*info);
  }

  // Owned event flows.
  for (const auto& event : node.mEvents)
  {
    auto info = std::dynamic_pointer_cast<FlowSaveInfo>(event);

    if (!info)
      throw std::logic_error("Unsupported event implementation");

    indexFlow(*info);
  }
}

void DocumentCloner::indexFlow(const FlowSaveInfo& flow)
{
  if (flow.mId.isEmpty())
    throw std::logic_error("Cannot clone a flow without an ID");

  if (mFlowIds.contains(flow.mId))
    return;

  mFlowIds.insert(flow.mId, newId());

  // A flow has its own node hierarchy.
  for (const auto& node : flow.mNodes)
  {
    auto info = std::dynamic_pointer_cast<NodeSaveInfo>(node);

    if (!info)
      throw std::logic_error("Unsupported node implementation");

    indexNode(*info);
  }
}

QString DocumentCloner::mappedNodeId(const QString& originalId) const
{
  // An ID not present in this map is outside the cloned
  // subtree. We do not implicitly attach to the old parent.
  return mNodeIds.value(originalId);
}

// =============================================================
// Clone independent property objects
// =============================================================

QVector<std::shared_ptr<IParameter>> DocumentCloner::cloneParameters(const QVector<std::shared_ptr<IParameter>>& parameters)
{
  QVector<std::shared_ptr<IParameter>> result;
  result.reserve(parameters.size());

  for (const auto& parameter : parameters)
  {
    auto property = std::dynamic_pointer_cast<PropertyInfo>(parameter);

    if (!property)
      throw std::logic_error("Unsupported parameter implementation");

    // Property IDs such as "name" and "color" are keys,
    // not unique document object IDs. Preserve them.
    result.append(std::make_shared<PropertyInfo>(*property));
  }

  return result;
}

// =============================================================
// Clone nodes
// =============================================================

std::shared_ptr<NodeSaveInfo> DocumentCloner::cloneNode(const NodeSaveInfo& original, const std::optional<QString>& parentOverride,
                                                        const QPointF& positionOffset)
{
  if (auto it = mClonedNodes.constFind(original.mId); it != mClonedNodes.cend())
    return it.value();

  // Copy the scalar properties first.
  // Shared-pointer collections will be replaced below.
  auto result = std::make_shared<NodeSaveInfo>(original);

  const QString newNodeId = mappedNodeId(original.mId);

  result->mId = newNodeId;
  result->mParentId = parentOverride.has_value() ? *parentOverride : mappedNodeId(original.mParentId);

  result->mPosition = original.mPosition + positionOffset;

  // Keep original size, scale, icon, node type, ports,
  // and parent subflow metadata.

  mClonedNodes.insert(original.mId, result);

  // ---------------------------------------------------------
  // Properties and fields
  // ---------------------------------------------------------
  result->mProperties = cloneParameters(original.mProperties);
  result->mFields = cloneParameters(original.mFields);

  // ---------------------------------------------------------
  // Children
  // ---------------------------------------------------------
  result->mChildren.clear();

  for (const auto& child : original.mChildren)
  {
    auto info = std::dynamic_pointer_cast<NodeSaveInfo>(child);

    if (!info)
      throw std::logic_error("Unsupported node implementation");

    result->mChildren.append(cloneNode(*info, std::optional<QString>{newNodeId}, positionOffset));
  }

  // ---------------------------------------------------------
  // Owned flows
  // ---------------------------------------------------------
  result->mFlows.clear();

  for (const auto& flow : original.mFlows)
  {
    auto info = std::dynamic_pointer_cast<FlowSaveInfo>(flow);

    if (!info)
      throw std::logic_error("Unsupported flow implementation");

    result->mFlows.append(cloneFlow(*info, newNodeId));
  }

  // ---------------------------------------------------------
  // Owned event flows
  // ---------------------------------------------------------
  result->mEvents.clear();

  for (const auto& event : original.mEvents)
  {
    auto info = std::dynamic_pointer_cast<FlowSaveInfo>(event);

    if (!info)
      throw std::logic_error("Unsupported event implementation");

    result->mEvents.append(cloneFlow(*info, newNodeId));
  }

  // ---------------------------------------------------------
  // Node-owned transitions, if present
  // ---------------------------------------------------------
  result->mTransitions.clear();

  for (const auto& transition : original.mTransitions)
  {
    auto info = std::dynamic_pointer_cast<TransitionSaveInfo>(transition);

    if (!info)
      throw std::logic_error("Unsupported transition implementation");

    if (auto cloned = cloneTransition(*info, positionOffset))
      result->mTransitions.append(cloned);
  }

  return result;
}

// =============================================================
// Clone behavioral flows
// =============================================================

std::shared_ptr<FlowSaveInfo> DocumentCloner::cloneFlow(const FlowSaveInfo& original, const QString& newOwnerId)
{
  if (auto it = mClonedFlows.constFind(original.mId); it != mClonedFlows.cend())
    return it.value();

  auto result = std::make_shared<FlowSaveInfo>(original);

  result->mId = mFlowIds.value(original.mId);
  result->mOwner = newOwnerId;

  // References from outside the copied subtree must not
  // be counted as references to the new flow.
  result->mLinksTo = original.mLinksTo;

  mClonedFlows.insert(original.mId, result);

  // Arguments are independent mutable model objects.
  result->mArguments = cloneParameters(original.mArguments);

  // ---------------------------------------------------------
  // Nodes
  // ---------------------------------------------------------
  result->mNodes.clear();

  for (const auto& node : original.mNodes)
  {
    auto info = std::dynamic_pointer_cast<NodeSaveInfo>(node);

    if (!info)
      throw std::logic_error("Unsupported node implementation");

    // Behavioral flow nodes have their own canvas coordinates.
    // Moving a structural task does NOT move the nodes inside
    // that task's separately owned behavioral flow.
    result->mNodes.append(cloneNode(*info, std::nullopt, QPointF{}));
  }

  // ---------------------------------------------------------
  // Transitions
  // ---------------------------------------------------------
  result->mTransitions.clear();

  for (const auto& transition : original.mTransitions)
  {
    auto info = std::dynamic_pointer_cast<TransitionSaveInfo>(transition);

    if (!info)
      throw std::logic_error("Unsupported transition implementation");

    // Flow coordinates remain unchanged.
    if (auto cloned = cloneTransition(*info, QPointF{}))
      result->mTransitions.append(cloned);
  }

  return result;
}

// =============================================================
// Clone transitions and remap endpoints
// =============================================================

std::shared_ptr<TransitionSaveInfo> DocumentCloner::cloneTransition(const TransitionSaveInfo& original, const QPointF& positionOffset)
{
  // Copy only transitions whose endpoints are both
  // inside the cloned graph.
  //
  // A transition connected to a node outside the copied
  // subtree should not accidentally reconnect to the
  // original node.
  if (!mNodeIds.contains(original.getsrcId()) || !mNodeIds.contains(original.getdstId()))
    return nullptr;

  auto result = std::make_shared<TransitionSaveInfo>(original);

  result->setId(newId());

  result->setSrcId(mappedNodeId(original.getsrcId()));
  result->setDstId(mappedNodeId(original.getdstId()));

  result->setSrcPoint(original.srcPoint() + positionOffset);
  result->setDstPoint(original.dstPoint() + positionOffset);

  // Existing event, label, port types, port subflow
  // names, and control-point shifts are preserved.

  return result;
}