#pragma once

#include <QHash>
#include <QPointF>
#include <QString>
#include <QVector>
#include <memory>
#include <optional>

#include "node_info.h"

class FlowSaveInfo;
class TransitionSaveInfo;
class IParameter;

class DocumentCloner final
{
public:
  // Produces a detached, fully cloned node subtree.
  // Does not modify SaveInfo or publish events.
  static NodeSaveInfo cloneNodeForPaste(const NodeSaveInfo& original, const QString& newParentId, const QPointF& position);

private:
  DocumentCloner() = default;

  // Pass 1: assign IDs to all objects before cloning references.
  void indexNode(const NodeSaveInfo& node);
  void indexFlow(const FlowSaveInfo& flow);

  // Pass 2: clone the objects and remap their references.
  std::shared_ptr<NodeSaveInfo> cloneNode(const NodeSaveInfo& original, const std::optional<QString>& parentOverride, const QPointF& positionOffset);

  std::shared_ptr<FlowSaveInfo> cloneFlow(const FlowSaveInfo& original, const QString& newOwnerId);

  std::shared_ptr<TransitionSaveInfo> cloneTransition(const TransitionSaveInfo& original, const QPointF& positionOffset);

  static QVector<std::shared_ptr<IParameter>> cloneParameters(const QVector<std::shared_ptr<IParameter>>& parameters);

  QString mappedNodeId(const QString& originalId) const;

  // Maps are shared by the entire clone operation.
  QHash<QString, QString> mNodeIds;
  QHash<QString, QString> mFlowIds;

  // Preserve shared identity if a node/flow is referenced
  // more than once inside the source graph.
  QHash<QString, std::shared_ptr<NodeSaveInfo>> mClonedNodes;
  QHash<QString, std::shared_ptr<FlowSaveInfo>> mClonedFlows;
};