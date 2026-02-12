#include "library_scene.h"

#include <QGraphicsSceneMouseEvent>

#include "elements/draggable.h"
#include "logging.h"

static const int MIN_DRAG_DISTANCE = 200;

LibraryScene::LibraryScene(QObject* parent)
    : QGraphicsScene(parent)
    , mClickedItem(nullptr)
{
  setBackgroundBrush(Qt::transparent);
}

void LibraryScene::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    clearSelectedNodes();

    QGraphicsItem* item = itemAt(event->scenePos(), QTransform());
    if (item && item->type() == DraggableItem::Type)
    {
      mClickedItem = static_cast<DraggableItem*>(item);

      // mPressed = true;
      mDragging = false;
      mPressScenePos = event->scenePos();

      dynamic_cast<QGraphicsView*>(parent())->setCursor(Qt::OpenHandCursor);

      event->accept();
      return;
    }
  }
  else
  {
  }

  QGraphicsScene::mousePressEvent(event);
}

void LibraryScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (!mClickedItem || !(event->buttons() & Qt::LeftButton))
  {
    QGraphicsScene::mouseMoveEvent(event);
    return;
  }

  // Use screenPos so it feels right across view transforms/zoom; scenePos is also ok.
  const int dist = (event->scenePos() - event->buttonDownScreenPos(Qt::LeftButton)).manhattanLength();
  LOG_INFO("Dragged distance: %d", dist);
  if (mClickedItem && !mDragging && dist >= MIN_DRAG_DISTANCE)
  {
    mDragging = true;
    mClickedItem->startDrag(event);
  }

  event->accept();

  QGraphicsScene::mouseMoveEvent(event);
}

void LibraryScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->button() != Qt::LeftButton)
  {
    QGraphicsScene::mouseReleaseEvent(event);
    return;
  }

  // If we never exceeded drag threshold, treat it as a click
  if (mClickedItem && !mDragging)
  {
    mClickedItem->handleClick(event);
    emit libraryNodeSelected(mClickedItem->nodeId());
  }

  mDragging = false;
  mClickedItem = nullptr;

  dynamic_cast<QGraphicsView*>(parent())->setCursor(Qt::ArrowCursor);
  dynamic_cast<QGraphicsView*>(parent())->update();
  event->accept();
}

void LibraryScene::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
  QGraphicsScene::contextMenuEvent(event);
}

void LibraryScene::clearSelectedNodes()
{
  LOG_INFO("Clearing all nodes");
  for (QGraphicsItem* item : selectedItems())
  {
    if (item->type() == DraggableItem::Type)
      item->setSelected(false);
  }

  clearSelection();
}