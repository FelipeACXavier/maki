#pragma once

#include <QBrush>
#include <QColor>
#include <QDataStream>
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <memory>

#include "config.h"
// #include "inode.h"
#include "node_base.h"
#include "save_info.h"
#include "transition.h"
#include "types.h"

class PortItem;
class Flow;
class QGraphicsSceneMouseEvent;
class SubtaskConnector;
class StructureCanvas;

class NodeItem : public NodeBase
{
public:
  enum
  {
    Type = Types::NODE
  };

  NodeItem(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent = nullptr);

  virtual ~NodeItem();

  int type() const override;

  VoidResult start() override;

  QRectF boundingRect() const override;
  QPainterPath shape() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;

  HelpConfig help() const;
  QString nodeName() const;
  QString nodeType() const;
  QString behaviour() const;
  QVector<ControlsConfig> controls() const;
  QMap<QString, QVariant> properties() const;
  QVector<PropertyConfig> configurationProperties() const;

  Types::LibraryTypes function() const;

  QVariant getProperty(const QString& key) const;
  void setProperty(const QString& key, QVariant value);

  void renameNode(const QString& name);

  NodeItem* parentNode() const;
  QVector<NodeItem*> children() const;

  QVector<TransitionItem*> transitions() const;
  void addTransition(TransitionItem* transition);
  void removeTransition(TransitionItem* transition);
  /** @param fromOutgoingPort true = out-port anchor (source side), false = in-port anchor (target side). */
  QPointF edgePointToward(const QPointF& targetScenePos, bool fromOutgoingPort) const;

  QVector<std::shared_ptr<IProperty>> fields() const;
  PropertyInfo getField(const QString& key) const;
  VoidResult setField(const QString& key, const QJsonObject& property);
  VoidResult setField(const QString& key, std::shared_ptr<PropertyInfo> property);
  void removeField(const QString& key);

  void setEvent(int index, const FlowConfig& event);
  QVector<std::shared_ptr<IFlow>> events() const;

  void addParent(NodeItem* node);
  void addChild(NodeItem* node, std::shared_ptr<NodeSaveInfo> info);
  void childRemoved(NodeItem* child);

  /** Structural "Task" node from library (container with capability slots + subtasks). */
  bool isTaskContainer() const;
  /** Task nested under another Task in the system view. */
  bool isStructuralSubtask() const;
  /** Non-Task structural child of a Task (capability / timer / etc.): drawn as inset circle. */
  bool rendersAsInsetCapability() const;

  QVector<NodeItem*> structuralSubtaskChildren() const;
  QVector<NodeItem*> structuralCapabilityChildren() const;

  void layoutSubtasks();
  void relayoutCapabilitySlots();
  void ensureSubtaskConnector(StructureCanvas* canvas);
  void destroySubtaskConnector();
  void syncSubtaskConnector();

  QVector<Flow*> flows() const;
  Flow* createFlow(const QString& flowName, std::shared_ptr<FlowSaveInfo> info);
  Flow* getFlow(const QString& flowId) const;
  void deleteFlow(const QString& flowId);
  void updateFlow();

  qreal baseScale() const;

  bool canAddTransition() const;
  TransitionConfig nextTransition() const;
  QVector<TransitionConfig> configTransitions() const;

  void applySize(const QSizeF& size);
  void updatePosition(const QPointF& position);

  void setHoverPreview(const QString& iconPath, const QColor& color, bool active);

  // "signals":
  std::function<void(NodeItem* item)> nodeModified;
  std::function<void(Flow* flow, NodeItem* item)> flowAdded;

  // "slots":
  void onProperties();

  // Serialization functions
  NodeSaveInfo saveInfo() const;

  friend QDataStream& operator<<(QDataStream& out, const NodeItem& config);
  friend QDataStream& operator>>(QDataStream& in, NodeItem& config);

protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
  std::shared_ptr<NodeSaveInfo> mStorage;

  QVector<Flow*> mFlows;
  NodeItem* mParentNode;
  QVector<NodeItem*> mChildrenNodes;
  QVector<TransitionItem*> mTransitions;

  PortItem* mInPort = nullptr;
  PortItem* mOutPort = nullptr;

  qreal mBaseScale;
  QSizeF mSize{0, 0};
  QPointF mDragStartPos{0, 0};
  QPointF mLastPosition{0, 0};

  bool mIsResizing{false};
  QPointF mResizeStartMousePos{0, 0};
  QSizeF mResizeStartSize{0, 0};

  void updateExtrasPosition();
  void updatePortPositions();

  QSizeF clampSize(qreal width, qreal height) const;
  QPointF clampPosInside(const QRectF& inner, const QRectF& childSceneRect) const;
  void fitInsideParent(qreal padding);
  QRectF parentInnerSceneRect(qreal padding) const;
  NodeItem* rootStructuralTask() const;

  SubtaskConnector* mSubtaskConnector = nullptr;
  QPointF mTreeDragRootStartPos{0, 0};
  QPointF mTreeDragStartScenePos{0, 0};

  bool mHoverPreviewActive = false;
  QString mHoverPreviewIcon;
  QColor mHoverPreviewColor;
};
