#pragma once

#include <QApplication>
#include <QDropEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPainter>

class NodeItem;
class ConnectionItem;

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

  void drawBackground(QPainter *painter, const QRectF &rect) override;

private:
  NodeItem* m_startNode = nullptr;
  std::shared_ptr<ConnectionItem> m_connection = nullptr;
};
