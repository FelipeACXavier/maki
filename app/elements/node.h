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

  NodeItem(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent = nullptr);

  virtual ~NodeItem();

  int type() const override;

  VoidResult start() override;

  QRectF boundingRect() const override;
  QPainterPath shape() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;

  HelpConfig help() const;
  QString nodeName() const override;
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

  void setEvent(int index, const FlowConfig& event);
  QVector<std::shared_ptr<FlowSaveInfo>> events() const;

  void addChild(NodeItem* node, std::shared_ptr<NodeSaveInfo> info);
  void childRemoved(NodeItem* child);

  Flow* createFlow(const QString& flowName, std::shared_ptr<FlowSaveInfo> info);
  Flow* getFlow(const QString& flowId) const;
  void deleteFlow(const QString& flowId);
  void updateFlow();

  qreal baseScale() const;

  bool canAddTransition() const;
  TransitionConfig nextTransition() const;

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
  std::shared_ptr<NodeSaveInfo> mStorage;

  INode* mParentNode;
  QVector<Flow*> mFlows;
  QVector<INode*> mChildrenNodes;
  QVector<TransitionItem*> mTransitions;

  qreal mBaseScale;
  QSizeF mSize{0, 0};

  bool mIsResizing{false};
  QPointF mResizeStartMousePos{0, 0};
  QSizeF mResizeStartSize{0, 0};

  void updatePosition(const QPointF& position);
  void updateExtrasPosition();
};
