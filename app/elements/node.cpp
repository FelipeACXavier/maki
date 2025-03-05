#include "node.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QObject>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QUuid>

#include "app_configs.h"
#include "connector.h"
#include "logging.h"
#include "style_helpers.h"

NodeItem::NodeItem(const QPointF& initialPosition, const QPixmap& pixmap, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent)
    : NodeBase(nodeConfig, parent)
    , mParentNode(nullptr)
    , mChildrenNodes({})
    , mSize(NodeBase::boundingRect().width(), NodeBase::boundingRect().height())
{
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  setCacheMode(DeviceCoordinateCache);
  setAcceptDrops(config()->libraryType == Types::LibraryTypes::STRUCTURAL);

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
  LOG_INFO("Calling destructor: %s", qPrintable(nodeType()));

  if (mParentNode)
  {
    LOG_INFO("Has parent: %s", qPrintable(mParentNode->nodeType()));
    mParentNode->childRemoved(this);
  }

  LOG_INFO("Destructor done: %s ", qPrintable(nodeType()));
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

QRectF NodeItem::boundingRect() const
{
  return QRectF(0, 0, mSize.width(), mSize.height());
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  auto color = getProperty("color");
  auto background = color ? QColor::fromString(color.Value().toString()) : config()->body.backgroundColor;

  NodeBase::paintNode(boundingRect(),
                      background,
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

QVector<PropertiesConfig> NodeItem::fields() const
{
  return mFields;
}

QVector<ControlsConfig> NodeItem::controls() const
{
  return config()->controls;
}

Result<QVariant> NodeItem::getProperty(const QString& key)
{
  if (mProperties.find(key) == mProperties.end())
    return Result<QVariant>::Failed("No property " + key.toStdString());

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

  update();
}

VoidResult NodeItem::setField(const QString& key, const QJsonObject& value)
{
  // Check if key exists
  auto property = PropertiesConfig(value);
  if (!property.isValid())
    return property.result();

  for (auto& field : mFields)
  {
    if (field.id != key)
      continue;

    field = property;
    return VoidResult();
  }

  mFields.push_back(property);

  return VoidResult();
}

Result<PropertiesConfig> NodeItem::getField(const QString& key)
{
  for (const auto& field : mFields)
  {
    if (field.id == key)
      return field;
  }

  return Result<PropertiesConfig>::Failed("Field does not exist");
}

QVector<NodeItem*> NodeItem::children() const
{
  return mChildrenNodes;
}

void NodeItem::addChild(NodeItem* child)
{
  mChildrenNodes.push_back(child);
}

void NodeItem::setParent(NodeItem* parent)
{
  mParentNode = parent;
}

void NodeItem::childRemoved(NodeItem* child)
{
  mChildrenNodes.removeAll(child);
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
  if (mIsResizing && (event->modifiers() & Qt::ShiftModifier))
  {
    qreal aspectRatio = mSize.width() / mSize.height();  // Store aspect ratio

    qreal newWidth = qMax(20.0, event->pos().x());  // Minimum width
    qreal newHeight = newWidth / aspectRatio;       // Maintain aspect ratio

    newWidth = qMax(20.0, newWidth);
    newHeight = qMax(20.0, newHeight);

    mSize.setWidth(qMax(20.0, newWidth));
    mSize.setHeight(qMax(20.0, newHeight));

    prepareGeometryChange();
    update();
  }
  else
  {
    QGraphicsItem::mouseMoveEvent(event);
  }

  updateConnectors();
}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if (config()->libraryType == Types::LibraryTypes::STRUCTURAL && event->modifiers() & Qt::ShiftModifier)
  {
    mIsResizing = true;
    event->accept();
  }
  else
  {
    QGraphicsItem::mousePressEvent(event);
  }
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  if (mIsResizing)
    mIsResizing = false;

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

void NodeItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
  // Create the menu
  QMenu menu;

  // Add actions to the menu
  QAction* action1 = menu.addAction("Delete");
  QAction* action2 = menu.addAction("Properties");

  // Connect actions to their slots
  QObject::connect(action1, &QAction::triggered, [this]() {
    onDelete();
  });
  QObject::connect(action2, &QAction::triggered, [this]() {
    onProperties();
  });

  // Execute the menu at the mouse cursor's position
  menu.exec(event->screenPos());
}

void NodeItem::onDelete()
{
  // Handle the delete action, e.g., remove the item from the scene
  scene()->removeItem(this);
  delete this;
}

void NodeItem::onProperties()
{
  // Handle the properties action, e.g., show a dialog to edit properties
}

void NodeItem::updateConnectors()
{
  for (auto& connector : mConnectors)
    connector->updateConnections();

  updateLabelPosition();
}
