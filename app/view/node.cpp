#include "node.h"

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QUuid>

#include "app_configs.h"
#include "connection.h"
#include "style_helpers.h"

Connector::Connector(const ConnectorConfig& config, QGraphicsItem* parent)
    : QGraphicsEllipseItem(parent)
    , mId(QUuid::createUuid().toString())
    , mConfig(std::make_shared<ConnectorConfig>(config))
{
  setZValue(1);
  setBrush(typeToColor(mConfig->type));
  setAcceptHoverEvents(true);

  const QPointF center = config.getPosition(parent->boundingRect());
  const QPointF shift = QPointF(Config::CONNECTOR_RADIUS, Config::CONNECTOR_RADIUS);

  setRect(QRectF(center - shift, center + shift));
}

QString Connector::Id() const
{
  return mId;
}

int Connector::type() const
{
  return Type;
}

Types::ConnectorType Connector::connectorType() const
{
  return mConfig->type;
}

QPointF Connector::center() const
{
  return scenePos() + mConfig->getPosition(parentItem()->boundingRect());
}

QPair<QPointF, QPointF> Connector::shift() const
{
  return QPair<QPointF, QPointF>(mConfig->getShift(), mConfig->getMirrorShift());
}

void Connector::updateConnections()
{
  for (auto& conn : mConnections)
    conn->move(Id(), center());
}

void Connector::addConnection(std::shared_ptr<ConnectionItem> connection)
{
  mConnections.push_back(connection);
  updateConnections();
}

void Connector::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  // Check if hovering over a connection point
  if (contains(event->pos()))
    updateColor(true);

  QGraphicsEllipseItem::hoverEnterEvent(event);
}

void Connector::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  // Reset color when leaving the connection point
  updateColor(false);

  QGraphicsEllipseItem::hoverLeaveEvent(event);
}

void Connector::updateColor(bool accent)
{
  setBrush(accent ? Config::Colours::ACCENT : typeToColor(mConfig->type));
}

Qt::GlobalColor Connector::typeToColor(Types::ConnectorType type) const
{
  switch (type)
  {
    case Types::ConnectorType::IN:
      return Qt::blue;
    case Types::ConnectorType::OUT:
      return Qt::yellow;
    case Types::ConnectorType::IN_AND_OUT:
      return Qt::green;
    default:
    case Types::ConnectorType::UNKNOWN:
      return Qt::red;
  }
}

NodeItem::NodeItem(const QPointF& initialPosition, const QPixmap& pixmap, std::shared_ptr<NodeConfig> config, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , mId(QUuid::createUuid().toString())
    , mConfig(config)
{
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  setCacheMode(DeviceCoordinateCache);

  mPixmapItem = std::make_shared<QGraphicsPixmapItem>(pixmap, this);

  for (const auto& connector : mConfig->connectors)
    mConnectors.append(std::make_shared<Connector>(connector, this));

  // Create connection points as ellipses
  // mConnectors.append(std::make_shared<Connector>(mRightPoint, Config::CONNECTOR_RADIUS, this));

  setPos(snapToGrid(initialPosition - boundingRect().center(), Config::GRID_SIZE));
}

QString NodeItem::Id() const
{
  return mId;
}

int NodeItem::type() const
{
  return Type;
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
