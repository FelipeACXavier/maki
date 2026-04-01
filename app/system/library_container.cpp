#include "library_container.h"

#include <QGraphicsScene>
#include <QVBoxLayout>

#include "config.h"
#include "library_scene.h"
#include "widgets/section.h"

static const int PADDING = 15;

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
  DraggableItem* item = new DraggableItem(id, config);

  // Center the item in the sidebar and make sure it is below the last item added
  item->setPos(static_cast<int>(viewport()->width() / 2), mLastItemY + PADDING);

  QRectF itemBounds = item->boundingRect();
  QRectF labelBounds = item->labelBoundingRect();
  mLastItemY = item->mapToScene(itemBounds.bottomLeft() + labelBounds.bottomLeft()).y();

  // Add item to scene
  scene()->addItem(item);

  updateSceneSize();

  return VoidResult();
}

void LibraryContainer::resizeEvent(QResizeEvent* event)
{
  QGraphicsView::resizeEvent(event);
  adjustNodePositions();  // Adjust positions on resize
}

void LibraryContainer::adjustNodePositions()
{
  // Reposition each node in the scene to the center based on the new width
  for (QGraphicsItem* item : scene()->items())
  {
    if (item->type() != DraggableItem::Type)
      continue;

    dynamic_cast<DraggableItem*>(item)->adjustWidth(viewport()->width());
  }
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
