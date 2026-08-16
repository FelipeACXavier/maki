#pragma once

#include <QBrush>
#include <QColor>
#include <QDataStream>
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <memory>

#include "config.h"
#include "node_base.h"
#include "port.h"
#include "save_info.h"
#include "transition.h"
#include "types.h"

class NodeItem;

class Flow;
class QGraphicsSceneMouseEvent;
class StructureCanvas;

/**
 * @brief Represents a graphical node item in a flowchart.
 *
 * Shared base for structural, behavioural, and pipeline nodes. Subclasses
 * override virtual hooks for type-specific initialization, painting, and interaction.
 */
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

  virtual VoidResult start() override;

  QRectF nodeRect() const override;
  QRectF sceneNodeRect() const;

  virtual QPainterPath shape() const override;

  virtual void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;

  HelpConfig help() const;
  QString nodeName() const;
  QString nodeType() const;
  QString behaviour() const;
  QVector<ControlsConfig> controls() const;
  QMap<QString, QVariant> properties() const;
  QVector<PropertyConfig> configurationProperties() const;
  Types::LibraryTypes function() const;

  QVariant getProperty(const QString& key) const;
  virtual void setProperty(const QString& key, QVariant value);
  void renameNode(const QString& name);

  /** True when @p scenePos hits the painted name label under the node body. */
  bool labelContainsScenePoint(const QPointF& scenePos) const;

  /** Prompt to rename this node (sets name_auto_generated false on accept). */
  void beginRenameFromLabel();

  NodeItem* parentNode() const;

  QVector<NodeItem*> children() const;

  QPointF edgePointToward(const QPointF& targetScenePos, bool fromOutgoingPort) const;
  QPointF outgoingPortAnchorForEvent(const QString& event) const;
  PortItem::Kind outgoingPortKindForEvent(const QString& event) const;
  QPointF incomingPortAnchor() const;

  QVector<std::shared_ptr<IProperty>> fields() const;
  PropertyInfo getField(const QString& key) const;
  VoidResult setField(const QString& key, const QJsonObject& property);
  VoidResult setField(const QString& key, std::shared_ptr<PropertyInfo> property);
  void removeField(const QString& key);

  void setEvent(int index, const FlowConfig& event);
  QVector<std::shared_ptr<IFlow>> events() const;

  virtual void addParent(NodeItem* node);
  void addChild(NodeItem* node, std::shared_ptr<NodeSaveInfo> info);
  virtual void childRemoved(NodeItem* child);
  /** Clear logical parent pointer (does not notify the former parent). */
  void clearParentNode();

  /** Structural "Task" node from library (container with capability slots + subtasks). */
  virtual bool isTaskContainer() const { return false; }
  /** Inline subflow container attached to Repeat / Within nodes in the behaviour canvas. */
  virtual bool isSubflowContainer() const { return false; }
  /** True when this subflow container is currently collapsed (contents hidden). */
  virtual bool isCollapsedSubflow() const { return false; }
  /** Owning Repeat/Within for a subflow container; nullptr otherwise. */
  virtual NodeItem* subflowHost() const { return nullptr; }
  /** Detach owned subflow containers before this node is deleted (caller owns the result). */
  virtual QVector<NodeItem*> detachOwnedSubflowBlocks() { return {}; }
  /** Grow/shrink a subflow container around its children; no-op for other nodes. */
  virtual void expandSubflowToFitChildren() {}
  /** Task nested under another Task in the system view. */
  virtual bool isStructuralSubtask() const { return false; }
  /** Non-Task structural child of a Task (capability / timer / etc.): drawn as inset circle. */
  virtual bool rendersAsInsetCapability() const { return false; }

  virtual QVector<NodeItem*> structuralSubtaskChildren() const { return {}; }
  virtual QVector<NodeItem*> structuralCapabilityChildren() const { return {}; }

  virtual void layoutSubtasks() {}
  virtual void relayoutCapabilitySlots() {}
  virtual void swapCapabilityOrder(NodeItem* a, NodeItem* b);
  virtual NodeItem* capabilityAtScenePos(const QPointF& scenePos, NodeItem* exclude = nullptr) const;

  virtual QRectF placeholderSlotSceneRect() const { return {}; }
  virtual bool placeholderSlotContainsScenePoint(const QPointF& scenePos) const;

  virtual void ensureSubtaskConnector(StructureCanvas* canvas);
  virtual void destroySubtaskConnector();
  virtual void syncSubtaskConnector();

  QVector<Flow*> flows() const;
  Flow* createFlow(const QString& flowName, std::shared_ptr<FlowSaveInfo> info);
  Flow* getFlow(const QString& flowId) const;
  void deleteFlow(const QString& flowId);
  void updateFlow();

  qreal baseScale() const;
  QVector<TransitionConfig> configTransitions() const;

  virtual void applySize(const QSizeF& size);
  virtual void updatePosition(const QPointF& position);

  virtual void setHoverPreview(const QString& iconPath, const QColor& color, bool active);

  /** Used by structural subtree layout and parent resize passes. */
  void fitInsideParent(qreal padding);
  virtual void applyStructuralLayoutTopLeft(const QPointF& topLeftScene);
  virtual void finalizeStructuralPackedPositions(const QPointF& subtreeSceneTopLeft);

  std::function<void(NodeItem* item)> nodeModified;
  std::function<void(Flow* flow, NodeItem* item)> flowAdded;
  std::function<void(const QString& id)> nodeMoved;
  std::function<void(NodeItem* item, bool hovered)> nodeHovered;

  void onProperties();

  NodeSaveInfo saveInfo() const;

  friend QDataStream& operator<<(QDataStream& out, const NodeItem& config);
  friend QDataStream& operator>>(QDataStream& in, NodeItem& config);

protected:
  std::shared_ptr<NodeSaveInfo> mStorage;
  QVector<Flow*> mFlows;
  NodeItem* mParentNode = nullptr;
  QVector<NodeItem*> mChildrenNodes;

  PortItem* mInPort = nullptr;
  PortItem* mOutPort = nullptr;
  PortItem* mAbortPort = nullptr;
  PortItem* mErrorPort = nullptr;

  qreal mBaseScale = 1.0;
  QSizeF mSize{0, 0};
  QPointF mDragStartPos{0, 0};
  QPointF mLastPosition{0, 0};

  /** Reentrancy guard so setPos inside updatePosition does not recurse via itemChange. */
  bool mInUpdatePosition = false;
  /** When true, updatePosition does not cascade to logical children (used by resize passes). */
  bool mSuppressChildCascade = false;

  /** Called after mSize is loaded from storage; subclasses adjust dimensions. */
  virtual void initializeNodeSize() {}

  /** Called from constructor to create transition ports when applicable. */
  virtual void configurePorts();

  /** Default node body painting shared by behavioural and pipeline nodes. */
  void paintDefaultNode(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget);

  virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  virtual void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

  QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

  void updateExtrasPosition();
  virtual void updatePortPositions();

  QSizeF clampSize(qreal width, qreal height) const;
  QPointF clampPosInside(const QRectF& inner, const QRectF& childSceneRect) const;
  QRectF parentInnerSceneRect(qreal padding) const;

  virtual NodeItem* rootStructuralTask() const { return const_cast<NodeItem*>(this); }
};
