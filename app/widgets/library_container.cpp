#include "widgets/library_container.h"

#include <QGraphicsScene>
#include <QMouseEvent>
#include <QVBoxLayout>

#include "canvas/elements/draggable.h"
#include "config.h"
#include "logging.h"
#include "widgets/library_scene.h"
#include "widgets/section.h"

static constexpr int PADDING = 16;

LibraryContainer::LibraryContainer(bool draggableItems, QWidget* parent)
    : QGraphicsView(parent)
    , mLastItemY(0)
    , mColumnCount(2)
    , mDraggableItems(draggableItems)
{
  // We don't want to scroll our inner container, only the outer widget should be scrollable
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  setFocusPolicy(Qt::StrongFocus);
  viewport()->setFocusPolicy(Qt::StrongFocus);
}

LibraryContainer* LibraryContainer::create(const QString& name, bool draggableItems, QWidget* parent)
{
  auto container = new LibraryContainer(draggableItems);

  // Not sure why, but setting a small width makes sure that the nodes are centered
  LibraryScene* scene = new LibraryScene(container);
  scene->setSceneRect(0, 0, 50, 500);

  container->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  container->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  container->setScene(scene);

  // Set the layout so the view covers the entire toolbox
  container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

  // A QGraphicsScene must be contained in a QWidget before it can be added to the toolbox
  auto sidebarContainer = new SectionWidget(parent);
  sidebarContainer->addItem(container, name);

  parent->layout()->addWidget(sidebarContainer);

  return container;
}

void LibraryContainer::updateSceneSize()
{
  if (!scene())
    return;

  const QRectF bounds = qobject_cast<LibraryScene*>(scene())->visibleItemsBounds();

  const int contentHeight = qCeil(bounds.bottom()) + PADDING;
  const int widgetHeight = contentHeight + frameWidth() * 2;

  scene()->setSceneRect(0, 0, viewport()->width(), contentHeight);

  setFixedHeight(widgetHeight);

  if (auto* parent = qobject_cast<SectionWidget*>(this->parent()->parent()))
    parent->updateContentHeight(widgetHeight);
}

VoidResult LibraryContainer::addNode(const QString& nodeId, std::shared_ptr<NodeConfig> config, const QString& instanceId)
{
  auto* item = new DraggableItem(instanceId, nodeId, config, mDraggableItems);

  // Add item to scene
  scene()->addItem(item);
  relayoutItems();

  return VoidResult();
}

void LibraryContainer::setColumnCount(int count)
{
  mColumnCount = std::max(1, count);
  relayoutItems();
}

void LibraryContainer::setDraggable(bool draggable)
{
  if (auto* scene = qobject_cast<LibraryScene*>(this->scene()))
  {
    scene->setDraggable(draggable);
    relayoutItems();
  }
}

void LibraryContainer::resizeEvent(QResizeEvent* event)
{
  QGraphicsView::resizeEvent(event);
  relayoutItems();  // Adjust positions on resize
}

bool LibraryContainer::filterNodes(const QString& query)
{
  auto* libraryScene = qobject_cast<LibraryScene*>(scene());
  if (!libraryScene)
    return false;

  const bool hasMatches = libraryScene->filterNodes(query);
  relayoutItems();

  return hasMatches;
}

void LibraryContainer::relayoutItems()
{
  // We calculate the cellHeight before based on the tallest item
  int cellHeight = 0;

  const qreal availableWidth = viewport()->width();
  const int columnCount = mColumnCount;

  const qreal columnWidth = availableWidth / columnCount;
  const qreal itemWidth = columnWidth - PADDING;

  QList<DraggableItem*> items;
  for (QGraphicsItem* graphicsItem : scene()->items(Qt::AscendingOrder))
  {
    if (graphicsItem->type() != DraggableItem::Type)
      continue;
    if (!graphicsItem->isVisible())
      continue;

    if (auto item = qgraphicsitem_cast<DraggableItem*>(graphicsItem))
    {
      item->adjustWidth(itemWidth);
      item->update();
      if (item->boundingRect().height() > cellHeight)
        cellHeight = item->boundingRect().height();

      items.append(item);
    }
  }

  const auto halfPadding = PADDING / 2;
  for (int i = 0; i < items.size(); ++i)
  {
    const int column = i % columnCount;
    const int row = i / columnCount;
    const QRectF itemBounds = items[i]->boundingRect();

    const qreal x = column * columnWidth + columnWidth / 2.0 - itemBounds.center().x();
    const qreal y = halfPadding + row * (cellHeight + halfPadding) - itemBounds.top();

    items[i]->setPos(x, y);
  }
  updateSceneSize();
}

void LibraryContainer::mousePressEvent(QMouseEvent* event)
{
  setFocus(Qt::MouseFocusReason);
  QGraphicsView::mousePressEvent(event);
}
