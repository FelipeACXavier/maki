#include "node.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QUuid>

#include "app_configs.h"
#include "connector.h"
#include "style_helpers.h"

NodeItem::NodeItem(const QPointF& initialPosition, const QPixmap& pixmap, std::shared_ptr<NodeConfig> config, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , mId(QUuid::createUuid().toString())
    , mConfig(config)
{
  setZValue(mConfig->body.zIndex);
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  setCacheMode(DeviceCoordinateCache);

  mPixmapItem = std::make_shared<QGraphicsPixmapItem>(pixmap, this);

  for (const auto& connector : mConfig->connectors)
    mConnectors.append(std::make_shared<Connector>(connector, this));

  // Create connection points as ellipses
  // mConnectors.append(std::make_shared<Connector>(mRightPoint, Config::CONNECTOR_RADIUS, this));

  setPos(snapToGrid(initialPosition - boundingRect().center(), Config::GRID_SIZE));
}

NodeItem::~NodeItem()
{
}

QString NodeItem::Id() const
{
  return mId;
}

int NodeItem::type() const
{
  return Type;
}

QString NodeItem::nodeType() const
{
  // This should also contain the library to make it unique
  return mConfig->name;
}

QRectF NodeItem::boundingRect() const
{
  return mPixmapItem->boundingRect();
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  Q_UNUSED(style);
  Q_UNUSED(widget);

  painter->drawPixmap(0, 0, mPixmapItem->pixmap());

  // If selected, draw an extra outline
  if (isSelected())
  {
    painter->setPen(QPen(Config::Colours::ACCENT, 4));
    painter->drawRect(mPixmapItem->boundingRect());
  }
}

QVector<std::shared_ptr<Connector>> NodeItem::connectors() const
{
  return mConnectors;
}

QString NodeItem::behaviour() const
{
  return mConfig->behaviour.code;
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsItem::mouseMoveEvent(event);
  updateConnectors();
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  setPos(snapToGrid(scenePos(), Config::GRID_SIZE));
  updateConnectors();
  QGraphicsItem::mouseReleaseEvent(event);
}

void NodeItem::updateConnectors()
{
  for (auto& connector : mConnectors)
    connector->updateConnections();
}
