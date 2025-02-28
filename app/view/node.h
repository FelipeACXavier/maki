#pragma once

#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QPixmap>
#include <memory>

#include "types.h"

class ConnectionItem;
class QGraphicsSceneMouseEvent;
class QGraphicsSceneHoverEvent;

class Connector : public QGraphicsEllipseItem
{
public:
  enum
  {
    Type = UserType + Type::CONNECTOR
  };

  Connector(const QPointF& center, int radius, QGraphicsItem* parent = nullptr);

  QString Id() const;
  int type() const override;
  QPointF center() const;

  void updateConnections();
  void addConnection(std::shared_ptr<ConnectionItem> connection);

protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
  const QString mId;
  QPointF mCenter;

  QVector<std::shared_ptr<ConnectionItem>> mConnections;
};

class NodeItem : public QGraphicsItem
{
public:
  enum
  {
    Type = UserType + Type::NODE
  };

  NodeItem(const QPointF& initialPosition, const QPixmap& map, QGraphicsItem* parent = nullptr);

  QString Id() const;

  int type() const override;

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

  // Get the connection points
  QPointF leftConnectionPoint() const;
  QPointF rightConnectionPoint() const;

  QRectF leftConnectionArea() const;
  QRectF rightConnectionArea() const;

protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
  const QString mId;

  const float mWidth = 100;
  const float mHeight = 50;
  const float mLeft = 0;
  const float mTop = 0;
  const float mRadius = 5;

  bool m_hovered{false};

  QPixmap mPixmap;
  std::shared_ptr<QGraphicsPixmapItem> mPixmapItem;
  QVector<std::shared_ptr<Connector>> mConnectors;

  const QPointF mLeftPoint{mLeft, mHeight / 2};
  const QPointF mRightPoint{mWidth, mHeight / 2};

  void updateConnectors();
};
