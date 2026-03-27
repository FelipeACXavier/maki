#pragma once

#include <QMap>
#include <QPixmap>
#include <QPointF>
#include <QString>
#include <QVariant>
#include <QVector>

#include "config.h"
#include "idocument.h"

// Single header for the document stuff
#include "canvas_info.h"
#include "flow_info.h"
#include "node_info.h"
#include "property_info.h"
#include "transition_info.h"

class SaveInfo : public IDocument
{
public:
  SaveInfo() = default;

  CanvasSaveInfo canvasInfo() const;
  void setCanvasInfo(const CanvasSaveInfo& info);

  QVector<std::shared_ptr<INode>> getnodes() const override;
  void addNode(std::shared_ptr<NodeSaveInfo> node);
  void removeNode(std::shared_ptr<NodeSaveInfo> node);

  QJsonObject toJson() const;
  static SaveInfo fromJson(const QJsonObject& data);

  friend QDataStream& operator<<(QDataStream& out, const SaveInfo& info);
  friend QDataStream& operator>>(QDataStream& in, SaveInfo& info);

  QVector<std::shared_ptr<NodeSaveInfo>> getPossibleStates(const QString& nodeId) const;
  QVector<std::shared_ptr<NodeSaveInfo>> getPossibleCallers(const QString& nodeId) const;
  QVector<std::shared_ptr<FlowSaveInfo>> getEventsFromNode(const QString& nodeId) const;
  QVector<std::shared_ptr<FlowSaveInfo>> getEventsOfTypeFromNode(const QString& nodeId, Types::CallType type) const;
  std::shared_ptr<FlowSaveInfo> getEventFromNode(const QString& nodeId, const QString& flowName) const;

  std::shared_ptr<NodeSaveInfo> getNodeWithId(const QString& nodeId);
  std::shared_ptr<FlowSaveInfo> getFlowWithId(const QString& flowId);

private:
  CanvasSaveInfo mCanvasInfo;
  QVector<std::shared_ptr<INode>> mStructuralNodes;

  QVector<std::shared_ptr<NodeSaveInfo>> findFamilyOfConstruct(const QString& nodeId, QVector<std::shared_ptr<INode>> nodes) const;
  std::shared_ptr<NodeSaveInfo> findParentOfConstruct(const QString& nodeId, const std::shared_ptr<INode> node) const;
  void findStatesOfConstruct(QVector<std::shared_ptr<NodeSaveInfo>>& toReturn, QVector<std::shared_ptr<INode>> nodes) const;

  QVector<std::shared_ptr<FlowSaveInfo>> getEventsFromNode(const QString& nodeId, QVector<std::shared_ptr<INode>> nodes) const;
  std::shared_ptr<NodeSaveInfo> getNodeWithId(const QString& nodeId, const QVector<std::shared_ptr<INode>>& nodes);
  std::shared_ptr<FlowSaveInfo> getFlowWithId(const QString& flowId, const QVector<std::shared_ptr<INode>>& nodes);
};
