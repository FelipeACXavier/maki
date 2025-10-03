#include "library_container.h"

#include <QGraphicsScene>
#include <QVBoxLayout>

#include "config.h"
#include "logging.h"
#include "elements/draggable.h"

static const int PADDING = 15;

LibraryContainer::LibraryContainer(QGraphicsScene* scene)
    : QGraphicsView(scene)
    , mLastItemY(0)
{
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
}

LibraryContainer* LibraryContainer::create(const QString& name, QToolBox* parent)
{
  // A QGraphicsScene must be contained in a QWidget before it can be added to the toolbox
  QWidget* sidebarContainer = new QWidget;
  QVBoxLayout* layout = new QVBoxLayout(sidebarContainer);
  layout->setContentsMargins(0, 0, 0, 0);

  QGraphicsScene* scene = new QGraphicsScene();
  // Not sure why, but setting a small width makes sure that the nodes are centered
  scene->setSceneRect(0, 0, 50, 500);

  auto container = new LibraryContainer(scene);
  container->setScene(scene);
  container->setAlignment(Qt::AlignLeft | Qt::AlignTop);

  // Set the layout so the view covers the entire toolbox
  container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  layout->addWidget(container);
  sidebarContainer->setLayout(layout);

  parent->addItem(sidebarContainer, name);

  return container;
}

void LibraryContainer::updateSceneSize()
{
  QRectF bounds = scene()->itemsBoundingRect();
  scene()->setSceneRect(0, 0, width(), bounds.height() + 20);
}

VoidResult LibraryContainer::addNode(const QString& id, std::shared_ptr<NodeConfig> config)
{
  // Create Draggable Items
  DraggableItem* item = new DraggableItem(id, config);

  // Center the item in the sidebar and make sure it is below the last item added
  item->setPos(static_cast<int>(viewport()->width() / 2), mLastItemY + PADDING);
  mLastItemY = item->mapToScene(item->boundingRect().bottomLeft()).y();

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

