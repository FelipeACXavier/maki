#pragma once

#include <QBrush>
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

class Flow;
class QGraphicsSceneMouseEvent;

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
  QVector<std::shared_ptr<PropertiesConfig>> fields() const;
  QMap<QString, QVariant> properties() const;
  QVector<PropertiesConfig> configurationProperties() const;

  Types::LibraryTypes function() const;

  QVariant getProperty(const QString& key) const;
  void setProperty(const QString& key, QVariant value);

  PropertiesConfig getField(const QString& key) const;
  VoidResult setField(const QString& key, const QJsonObject& value);
  VoidResult setField(const QString& key, const PropertiesConfig& property);
  void removeField(const QString& key);

  void renameNode(const QString& name);

  NodeItem* parentNode() const;
  QVector<NodeItem*> children() const;

  QVector<TransitionItem*> transitions() const;
  void addTransition(TransitionItem* transition);
  void removeTransition(TransitionItem* transition);
  QPointF edgePointToward(const QPointF& targetScenePos) const;

  void setEvent(int index, const FlowConfig& event);
  QVector<std::shared_ptr<FlowSaveInfo>> events() const;

  void addParent(NodeItem* node);
  void addChild(NodeItem* node, std::shared_ptr<NodeSaveInfo> info);
  void childRemoved(NodeItem* child);

  QVector<Flow*> flows() const;
  Flow* createFlow(const QString& flowName, std::shared_ptr<FlowSaveInfo> info);
  Flow* getFlow(const QString& flowId) const;
  void deleteFlow(const QString& flowId);
  void updateFlow();

  qreal baseScale() const;

  bool canAddTransition() const;
  TransitionConfig nextTransition() const;

  void applySize(const QSizeF& size);
  void updatePosition(const QPointF& position);

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

  qreal mBaseScale;
  QSizeF mSize{0, 0};
  QPointF mDragStartPos{0, 0};
  QPointF mLastPosition{0, 0};

  bool mIsResizing{false};
  QPointF mResizeStartMousePos{0, 0};
  QSizeF mResizeStartSize{0, 0};

  void updateExtrasPosition();

  QSizeF clampSize(qreal width, qreal height) const;
  QPointF clampPosInside(const QRectF& inner, const QRectF& childSceneRect) const;
  void fitInsideParent(qreal padding);
  QRectF parentInnerSceneRect(qreal padding) const;
};
