#include "node.h"

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QUuid>

#include "app_configs.h"
#include "connector.h"
#include "logging.h"
#include "style_helpers.h"

NodeItem::NodeItem(const QPointF& initialPosition, const QPixmap& pixmap, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent)
    : NodeBase(nodeConfig, parent)
{
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  setCacheMode(DeviceCoordinateCache);

  // Add icon if it exists
  if (!pixmap.isNull())
    setPixmap(pixmap);
  else
    setLabel(config()->name, config()->body.textColor);

  for (const auto& connector : config()->connectors)
    mConnectors.append(std::make_shared<Connector>(connector, this));

  for (const auto& property : config()->properties)
    mProperties[property.id] = property.defaultValue;

  setPos(snapToGrid(initialPosition - boundingRect().center(), Config::GRID_SIZE));

  updateConnectors();
}

NodeItem::~NodeItem()
{
}

int NodeItem::type() const
{
  return Type;
}

QString NodeItem::nodeType() const
{
  // This should also contain the library to make it unique
  return config()->name;
}

VoidResult NodeItem::start()
{
  if (nodeSeletected)
    nodeSeletected(this);

  return NodeBase::start();
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  NodeBase::paintNode(boundingRect(),
                      config()->body.backgroundColor,
                      isSelected() ? QPen(Config::Colours::ACCENT, 4) : QPen(config()->body.borderColor),
                      painter);
}

QPainterPath NodeItem::shape() const
{
  return NodeBase::nodeShape(boundingRect());
}

QVector<std::shared_ptr<Connector>> NodeItem::connectors() const
{
  return mConnectors;
}

QVector<PropertiesConfig> NodeItem::properties() const
{
  return config()->properties;
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
  return config()->behaviour.code;
}

HelpConfig NodeItem::help() const
{
  return config()->help;
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

  updateLabelPosition();
}
