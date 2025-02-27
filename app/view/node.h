#pragma once

#include <QGraphicsItem>
#include <memory>

class ConnectionItem;

class NodeItem : public QGraphicsItem
{
public:
  NodeItem(QGraphicsItem* parent = nullptr);

  QString Id() const;

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

  // Get the connection points
  QPointF leftConnectionPoint() const;
  QPointF rightConnectionPoint() const;

  QRectF leftConnectionArea() const;
  QRectF rightConnectionArea() const;

  QPointF topCorner() const;

  void addConnection(std::shared_ptr<ConnectionItem> connection);
  std::shared_ptr<ConnectionItem> startConnection(QPointF startPoint, QPointF endPoint);
  void endConnection(std::shared_ptr<ConnectionItem> connection);

  // QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
  const float mWidth = 100;
  const float mHeight = 50;
  const float mLeft = 0;
  const float mTop = 0;
  const float mRadius = 5;

  bool m_hovered{false};

  QVector<std::shared_ptr<ConnectionItem>> mInConnections;
  QVector<std::shared_ptr<ConnectionItem>> mOutConnections;

  const QPointF mLeftPoint{mLeft, mHeight / 2};
  const QPointF mRightPoint{mWidth, mHeight / 2};

  const QString mId;
};
