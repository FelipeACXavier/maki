#include "library_scene.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSvgItem>

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
    if (item)
    {
      if (item->type() == DraggableItem::Type)
        mClickedItem = static_cast<DraggableItem*>(item);
      else if (item->type() == QGraphicsSvgItem::Type)
        mClickedItem = static_cast<DraggableItem*>(item->parentItem());

      if (mClickedItem)
      {
        // mPressed = true;
        mDragging = false;
        mPressScenePos = event->scenePos();

        dynamic_cast<QGraphicsView*>(parent())->setCursor(Qt::OpenHandCursor);

        event->accept();
        return;
      }
    }
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
  for (QGraphicsItem* item : selectedItems())
  {
    if (item->type() == DraggableItem::Type)
      item->setSelected(false);
  }

  clearSelection();
  mClickedItem = nullptr;
}

bool LibraryScene::filterNodes(const QString& query)
{
  const QString q = query.trimmed().toLower();
  bool anyVisible = false;

  for (QGraphicsItem* graphicsItem : items())
  {
    auto* item = dynamic_cast<DraggableItem*>(graphicsItem);
    if (!item)
      continue;

    const QString fixed = item->nodeId().trimmed().toLower();
    const bool matches = q.isEmpty() || fixed.contains(q);
    item->setVisible(matches);

    if (matches)
      anyVisible = true;
  }

  relayoutVisibleItems();

  return anyVisible;
}

void LibraryScene::relayoutVisibleItems()
{
  int y = 0;
  QList<QGraphicsItem*> allItems = items(Qt::AscendingOrder);
  const auto width = qobject_cast<QGraphicsView*>(parent())->viewport()->width();
  for (QGraphicsItem* graphicsItem : allItems)
  {
    auto* item = dynamic_cast<DraggableItem*>(graphicsItem);
    if (!item || !item->isVisible())
      continue;

    item->setPos(static_cast<int>((width - item->boundingRect().width()) / 2), y + 15);

    QRectF itemBounds = item->boundingRect();
    // QRectF labelBounds = item->labelBoundingRect();
    y = item->mapToScene(itemBounds.bottomLeft()).y();
  }
}

QRectF LibraryScene::visibleItemsBounds() const
{
  QRectF bounds;
  QList<QGraphicsItem*> allItems = items(Qt::AscendingOrder);

  for (QGraphicsItem* item : allItems)
  {
    if (!item || !item->isVisible())
      continue;

    QRectF itemRect = item->sceneBoundingRect();
    bounds = bounds.united(itemRect);
  }

  return bounds;
}

void LibraryScene::themeChanged()
{
  for (QGraphicsItem* item : items())
    item->update();
}
