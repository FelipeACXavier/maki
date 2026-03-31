#pragma once

#include <QApplication>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QMouseEvent>

#include "elements/draggable.h"

class LibraryScene : public QGraphicsScene
{
  Q_OBJECT
public:
  LibraryScene(QObject* parent = nullptr);

  QRectF visibleItemsBounds() const;
  bool filterNodes(const QString& query);
  void relayoutVisibleItems();

signals:
  void libraryNodeSelected(const QString& nodeType);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

private:
  QPointF mPressScenePos;
  bool mPressed = false;
  bool mDragging = false;
  DraggableItem* mClickedItem;

  void clearSelectedNodes();
};
