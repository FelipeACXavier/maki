#pragma once

#include <QBrush>
#include <QDataStream>
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <memory>

#include "config.h"
#include "inode.h"
#include "node_base.h"
#include "save_info.h"
#include "transition.h"
#include "types.h"

class Flow;
class QGraphicsSceneMouseEvent;

class NodeItem : public INode, public NodeBase
{
public:
  enum
  {
    Type = Types::NODE
  };

  NodeItem(const QString& id, const NodeSaveInfo& info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent = nullptr);

  virtual ~NodeItem();

  int type() const override;

  VoidResult start() override;

  QRectF boundingRect() const override;
  QPainterPath shape() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;

  HelpConfig help() const;
  QString nodeName() const;
  QString nodeType() const override;
  QString behaviour() const;
  QVector<ControlsConfig> controls() const;
  QVector<PropertiesConfig> fields() const override;
  QMap<QString, QVariant> properties() const override;
  QVector<PropertiesConfig> configurationProperties() const;

  Types::LibraryTypes function() const override;

  QVariant getProperty(const QString& key) const override;
  void setProperty(const QString& key, QVariant value);

  PropertiesConfig getField(const QString& key) const override;
  VoidResult setField(const QString& key, const QJsonObject& value);
  void removeField(const QString& key);

  void renameNode(const QString& name);

  INode* parentNode() const override;
  QVector<INode*> children() const override;

  QVector<TransitionItem*> transitions() const;
  void addTransition(TransitionItem* transition);
  void removeTransition(TransitionItem* transition);
  QPointF edgePointToward(const QPointF& targetScenePos) const;

  void setEvent(int index, const EventConfig& event);
  QVector<EventConfig> events() const;

  void addChild(NodeItem* child);
  void setParent(NodeItem* parent);
  void childRemoved(NodeItem* child);

  void addFlow(Flow* flow);
  Flow* getFlow(const QString& flowId) const;
  void deleteFlow(Flow* flow);
  void updateFlow();

  qreal baseScale() const;

  // "signals":
  std::function<void(NodeItem* item)> nodeDeleted;
  std::function<void(NodeItem* item)> nodeModified;
  std::function<void(Flow* flow, NodeItem* item)> flowAdded;

  // "slots":
  void deleteNode();
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
  QVector<PropertiesConfig> mFields;
  QVector<EventConfig> mEvents;
  QMap<QString, QVariant> mProperties;

  QVector<Flow*> mFlows;

  QVector<TransitionItem*> mTransitions;

  INode* mParentNode;
  QVector<INode*> mChildrenNodes;

  qreal mBaseScale;
  QSizeF mSize{0, 0};

  bool mIsResizing{false};
  QPointF mResizeStartMousePos{0, 0};
  QSizeF mResizeStartSize{0, 0};

  void updatePosition(const QPointF& position);
  void updateExtrasPosition();
};
