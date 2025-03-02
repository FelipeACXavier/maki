#pragma once

#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <memory>

#include "elements/config.h"
#include "types.h"

class ConnectionItem;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneHoverEvent;

class Connector : public QGraphicsEllipseItem
{
public:
  enum
  {
    Type = UserType + Types::CONNECTOR
  };

  Connector(const ConnectorConfig& config, QGraphicsItem* parent);

  QString Id() const;
  int type() const override;
  QPointF center() const;
  QPair<QPointF, QPointF> shift() const;

  Types::ConnectorType connectorType() const;

  void updateConnections();
  void addConnection(std::shared_ptr<ConnectionItem> connection);

protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
  const QString mId;

  std::shared_ptr<ConnectorConfig> mConfig;
  QVector<std::shared_ptr<ConnectionItem>> mConnections;

  Qt::GlobalColor typeToColor(Types::ConnectorType type) const;
};

class NodeItem : public QGraphicsItem
{
public:
  enum
  {
    Type = UserType + Types::NODE
  };

  NodeItem(const QPointF& initialPosition, const QPixmap& map, std::shared_ptr<NodeConfig> config, QGraphicsItem* parent = nullptr);

  QString Id() const;

  int type() const override;

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
  const QString mId;

  bool m_hovered{false};
  std::shared_ptr<NodeConfig> mConfig;

  std::shared_ptr<QGraphicsPixmapItem> mPixmapItem;
  QVector<std::shared_ptr<Connector>> mConnectors;

  void updateConnectors();
};
