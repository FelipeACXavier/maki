#include "library_container.h"

#include <QGraphicsScene>
#include <QVBoxLayout>

#include "config.h"
#include "draggable.h"
#include "library_scene.h"
#include "logging.h"
#include "widgets/section.h"

static constexpr qreal CELL_HEIGHT = 100.0;
static constexpr int PADDING = 15;

LibraryContainer::LibraryContainer(QWidget* parent)
    : QGraphicsView(parent)
    , mLastItemY(0)
{
  // We don't want to scroll our inner container, only the outer widget should be scrollable
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

LibraryContainer* LibraryContainer::create(const QString& name, QWidget* parent)
{
  auto container = new LibraryContainer();

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

  QRectF bounds = qobject_cast<LibraryScene*>(scene())->visibleItemsBounds();
  const int contentHeight = qCeil(bounds.height()) + 20;

  scene()->setSceneRect(0, 0, viewport()->width(), contentHeight);
  setFixedHeight(contentHeight + frameWidth() * 2);

  if (SectionWidget* parent = qobject_cast<SectionWidget*>(this->parent()->parent()))
    parent->updateContentHeight(contentHeight + frameWidth() * 2);
}

VoidResult LibraryContainer::addNode(const QString& id, std::shared_ptr<NodeConfig> config)
{
  auto* item = new DraggableItem(id, config);

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
  updateSceneSize();

  return hasMatches;
}

void LibraryContainer::relayoutItems()
{
  QList<DraggableItem*> items;
  for (QGraphicsItem* graphicsItem : scene()->items(Qt::AscendingOrder))
  {
    if (graphicsItem->type() != DraggableItem::Type)
      continue;

    items.append(dynamic_cast<DraggableItem*>(graphicsItem));
  }

  const qreal availableWidth = viewport()->width();
  const int columnCount = mColumnCount;

  const qreal columnWidth = availableWidth / columnCount;
  const qreal itemWidth = columnWidth - PADDING;

  for (int i = 0; i < items.size(); ++i)
  {
    const int column = i % columnCount;
    const int row = i / columnCount;

    items[i]->adjustWidth(itemWidth);

    QRectF itemBounds = items[i]->boundingRect();
    const qreal x = (column * columnWidth + columnWidth / 2.0) - (itemBounds.width() / 2);
    const qreal y = PADDING + row * (CELL_HEIGHT + PADDING);

    items[i]->setPos(x, y + CELL_HEIGHT - itemBounds.bottom());
  }

  updateSceneSize();
}