#pragma once

#include <QApplication>
#include <QDropEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPainter>
#include <QSplitter>

// 🎨 Node Item (Represents a single node with connection points)
class NodeItem : public QGraphicsItem
{
public:
  NodeItem(QGraphicsItem* parent = nullptr);
  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

  // Get the connection points
  QPointF leftConnectionPoint() const;
  QPointF rightConnectionPoint() const;

  QRectF leftConnectionArea() const;
  QRectF rightConnectionArea() const;

  QPointF topCorner() const;

protected:
  // void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  // void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
  // void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
  // void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
  const float mWidth = 100;
  const float mHeight = 50;
  const float mLeft = 0;
  const float mTop = 0;
  const float mRadius = 5;

  bool m_hovered{false};

  const QPointF mLeftPoint{mLeft, mHeight / 2};
  const QPointF mRightPoint{mWidth, mHeight / 2};
};

class ConnectionItem : public QGraphicsLineItem
{
public:
  ConnectionItem(NodeItem* startNode, NodeItem* endNode);

  void updateLine();

private:
  NodeItem* m_startNode;
  NodeItem* m_endNode;
};

class Canvas : public QGraphicsScene
{
public:
  Canvas(QObject* parent = nullptr);

protected:
  void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
  void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
  void dropEvent(QGraphicsSceneDragDropEvent* event) override;

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
  NodeItem* m_startNode = nullptr;            // The node from which the connection starts
  QGraphicsLineItem* m_connection = nullptr;  // Temporary line being drawn
};
