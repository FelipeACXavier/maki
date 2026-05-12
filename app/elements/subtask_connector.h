#pragma once

#include <QGraphicsItem>
#include <QPainterPath>

class NodeItem;
class StructureCanvas;
class QGraphicsSceneMouseEvent;

/**
 * Draws the vertical trunk, horizontal branches with arrowheads from a Task to its subtasks,
 * and the "+" placeholder to add another subtask. Scene-level item (no QGraphics parent).
 */
class SubtaskConnector : public QGraphicsItem
{
public:
  SubtaskConnector(NodeItem* task, StructureCanvas* canvas);
  ~SubtaskConnector() override;

  NodeItem* task() const
  {
    return mTask;
  }

  void syncGeometry();

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
  QPainterPath shape() const override;

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
  NodeItem* mTask;
  StructureCanvas* mCanvas;

  QRectF mBounds;
  QPainterPath mStrokePath;
  QRectF mPlaceholderRect;
};
