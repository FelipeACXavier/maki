#pragma once

#include <QGraphicsItem>
#include <QPainterPath>

class NodeItem;

/**
 * Draws orthogonal parent→child connectors with arrowheads from a Task to its nested subtasks.
 * Scene-level item (no QGraphics parent).
 */
class SubtaskConnector : public QGraphicsItem
{
public:
  explicit SubtaskConnector(NodeItem* task);
  ~SubtaskConnector() override;

  NodeItem* task() const { return mTask; }

  void syncGeometry();

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
  QPainterPath shape() const override;

private:
  NodeItem* mTask = nullptr;

  QRectF mBounds;
  QPainterPath mStrokePath;
};
