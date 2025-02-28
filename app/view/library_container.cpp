#include "library_container.h"

#include <QGraphicsScene>
#include <QJsonObject>
#include <QVBoxLayout>

#include "elements/movable.h"

static const int PADDING = 15;

LibraryContainer::LibraryContainer(QGraphicsScene* scene)
    : QGraphicsView(scene)
{
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

VoidResult LibraryContainer::addNode(const QJsonValueConstRef& config)
{
  if (!config.isObject())
    return VoidResult::Failed("Invalid node format");

  QJsonObject node = config.toObject();
  if (!node.contains("name"))
    return VoidResult::Failed("Nodes must contain a name");

  // Create Draggable Items
  DraggableItem* item1 = new DraggableItem(node["name"].toString());

  // Center the item in the sidebar and make sure it is below the last item added
  int x_center = 100;
  item1->setPos(x_center, getYOfLastItem() + PADDING);

  // Add item to scene
  scene()->addItem(item1);

  return VoidResult();
}

void LibraryContainer::resizeEvent(QResizeEvent* event)
{
  QGraphicsView::resizeEvent(event);
  adjustNodePositions();  // Adjust positions on resize
}

void LibraryContainer::adjustNodePositions()
{
  // Get the updated width of the sidebar
  int viewWidth = viewport()->width();

  // Reposition each node in the scene to the center based on the new width
  for (QGraphicsItem* item : scene()->items())
  {
    if (item->type() != DraggableItem::Type)
      continue;

    dynamic_cast<DraggableItem*>(item)->adjustWidth(viewWidth);
  }
}

int LibraryContainer::getYOfLastItem() const
{
  QList<QGraphicsItem*> items = scene()->items();
  for (const auto& item : items)
  {
    if (item->type() == DraggableItem::Type)
      return item->mapToScene(item->boundingRect().bottomLeft()).y();
  }

  return 0;
}
