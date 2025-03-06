#pragma once

#include <QBrush>
#include <QDataStream>
#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <memory>

#include "config.h"
#include "node_base.h"
#include "save_info.h"
#include "types.h"

class Connector;
class ConnectionItem;
class QGraphicsSceneMouseEvent;

class NodeItem : public NodeBase
{
public:
  enum
  {
    Type = UserType + Types::NODE
  };

  NodeItem(const QString& id, const NodeSaveInfo& info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent = nullptr);

  virtual ~NodeItem();

  // QString id() const;
  int type() const override;

  VoidResult start() override;

  QRectF boundingRect() const override;
  QPainterPath shape() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;

  HelpConfig help() const;
  QString nodeType() const override;
  QString behaviour() const;
  QVector<ControlsConfig> controls() const;
  QVector<PropertiesConfig> fields() const;
  QMap<QString, QVariant> properties() const;
  QVector<std::shared_ptr<Connector>> connectors() const;
  QVector<PropertiesConfig> configurationProperties() const;

  Types::LibraryTypes function() const;

  Result<QVariant> getProperty(const QString& key) const;
  void setProperty(const QString& key, QVariant value);

  Result<PropertiesConfig> getField(const QString& key) const;
  VoidResult setField(const QString& key, const QJsonObject& value);

  NodeItem* parentNode() const;
  QVector<NodeItem*> children() const;

  void addChild(NodeItem* child);
  void setParent(NodeItem* parent);
  void childRemoved(NodeItem* child);

  std::function<void(NodeItem* item)> nodeCopied;
  std::function<void(NodeItem* item)> nodeDeleted;
  std::function<void(NodeItem* item)> nodeSeletected;

  // "slots":
  void onDelete();
  void onProperties();

  // Serialization functions
  NodeSaveInfo saveInfo() const;

  friend QDataStream& operator<<(QDataStream& out, const NodeItem& config);
  friend QDataStream& operator>>(QDataStream& in, NodeItem& config);

protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
  QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
  QVector<PropertiesConfig> mFields;
  QMap<QString, QVariant> mProperties;
  QVector<std::shared_ptr<Connector>> mConnectors;

  NodeItem* mParentNode;
  QVector<NodeItem*> mChildrenNodes;

  bool mIsResizing{false};
  QSizeF mSize;

  void updatePosition(const QPointF& position);
};
