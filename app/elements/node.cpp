#include "node.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QUuid>

#include "app_configs.h"
#include "connector.h"
#include "logging.h"
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

  for (const auto& property : mConfig->properties)
    mProperties[property.id] = property.defaultValue;

  setPos(snapToGrid(initialPosition - boundingRect().center(), Config::GRID_SIZE));
}

NodeItem::~NodeItem()
{
}

QString NodeItem::id() const
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

VoidResult NodeItem::start()
{
  if (nodeSeletected)
    nodeSeletected(this);

  return VoidResult();
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

QVector<PropertiesConfig> NodeItem::properties() const
{
  return mConfig->properties;
}

Result<QVariant> NodeItem::getProperty(const QString& key)
{
  if (mProperties.find(key) == mProperties.end())
  {
    LOG_WARNING("Tried to update property %s but it does not exist", qPrintable(key));
    return Result<QVariant>::Failed("No property " + key.toStdString());
  }

  LOG_DEBUG("Getting property: %s", qPrintable(key));
  return mProperties[key];
}

void NodeItem::setProperty(const QString& key, QVariant value)
{
  if (mProperties.find(key) == mProperties.end())
  {
    LOG_WARNING("Tried to update property %s but it does not exist", qPrintable(key));
    return;
  }

  mProperties[key] = value;
}

QString NodeItem::behaviour() const
{
  return mConfig->behaviour.code;
}

HelpConfig NodeItem::help() const
{
  return mConfig->help;
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsItem::mouseMoveEvent(event);
  updateConnectors();
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  // Display information in the help menu
  if (isSelected())
  {
    if (nodeSeletected)
      nodeSeletected(this);
  }

  setPos(snapToGrid(scenePos(), Config::GRID_SIZE));
  updateConnectors();
  QGraphicsItem::mouseReleaseEvent(event);
}

void NodeItem::updateConnectors()
{
  for (auto& connector : mConnectors)
    connector->updateConnections();
}
