#include "node.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QMenu>
#include <QObject>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QUuid>

#include "app_configs.h"
#include "connector.h"
#include "logging.h"
#include "style_helpers.h"

NodeItem::NodeItem(const SaveInfo& info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent)
    : NodeBase(info.nodeId, nodeConfig, parent)
    , mChildrenNodes({})
    , mSize(info.size)
{
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  setCacheMode(DeviceCoordinateCache);
  setAcceptDrops(config()->libraryType == Types::LibraryTypes::STRUCTURAL);

  // Add icon if it exists
  if (!info.pixmap.isNull())
    setPixmap(info.pixmap);
  else
    setLabel(nodeType(), config()->body.textColor);

  for (const auto& connector : config()->connectors)
    mConnectors.append(std::make_shared<Connector>(connector, this));

  if (info.properties.isEmpty())
  {
    for (const auto& property : config()->properties)
      mProperties[property.id] = property.defaultValue;
  }
  else
  {
    mProperties = info.properties;
  }

  if (!info.fields.isEmpty())
    mFields = info.fields;

  setParent(nullptr);
  updatePosition(snapToGrid(initialPosition - boundingRect().center(), Config::GRID_SIZE));
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
  auto ret = getProperty("type");
  return ret ? ret.Value().toString() : config()->type;
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

QVector<PropertiesConfig> NodeItem::configurationProperties() const
{
  return config()->properties;
}

QMap<QString, QVariant> NodeItem::properties() const
{
  return mProperties;
}

QVector<PropertiesConfig> NodeItem::fields() const
{
  return mFields;
}

QVector<ControlsConfig> NodeItem::controls() const
{
  return config()->controls;
}

Result<QVariant> NodeItem::getProperty(const QString& key) const
{
  if (mProperties.find(key) == mProperties.end())
    return Result<QVariant>::Failed("No property " + key.toStdString());

  return mProperties.value(key);
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

Result<PropertiesConfig> NodeItem::getField(const QString& key) const
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

NodeItem* NodeItem::parentNode() const
{
  return mParentNode;
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
    qreal aspectRatio = mSize.width() / mSize.height();

    qreal newWidth = qMax(Config::MINIMUM_NODE_SIZE, event->pos().x());
    qreal newHeight = newWidth / aspectRatio;

    mSize.setWidth(qMax(Config::MINIMUM_NODE_SIZE, qMax(Config::MINIMUM_NODE_SIZE, newWidth)));
    mSize.setHeight(qMax(Config::MINIMUM_NODE_SIZE, qMax(Config::MINIMUM_NODE_SIZE, newHeight)));

    prepareGeometryChange();
    update();
  }
  else
  {
    QGraphicsItem::mouseMoveEvent(event);
  }

  updatePosition(scenePos());
}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if (config()->libraryType == Types::LibraryTypes::STRUCTURAL && event->modifiers() & Qt::ShiftModifier)
  {
    mIsResizing = true;
    dynamic_cast<QGraphicsView*>(scene()->parent())->setCursor(Qt::SizeFDiagCursor);
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
  {
    mIsResizing = false;
    dynamic_cast<QGraphicsView*>(scene()->parent())->setCursor(Qt::ArrowCursor);
  }

  // Display information in the help menu
  if (isSelected())
  {
    if (nodeSeletected)
      nodeSeletected(this);
  }

  updatePosition(snapToGrid(scenePos(), Config::GRID_SIZE));

  QGraphicsItem::mouseReleaseEvent(event);
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemPositionChange)
  {
    QPointF newPos = value.toPointF();

    if (parentNode() != nullptr)
    {
      // Ensure child stays inside parent's bounding box
      // Clamp position inside parent
      QRectF parentRect = parentNode()->mapRectToScene(parentNode()->boundingRect());
      QRectF childRect = boundingRect();

      newPos.setX(qBound(parentRect.left(), newPos.x(), parentRect.right() - childRect.width()));
      newPos.setY(qBound(parentRect.top(), newPos.y(), parentRect.bottom() - childRect.height()));
    }

    // Move all children relatively
    for (NodeItem* child : mChildrenNodes)
    {
      if (!child)
        continue;

      QPointF relativeOffset = child->pos() - pos();  // Maintain offset
      child->updatePosition(newPos + relativeOffset);
    }

    if (parentNode())
      return newPos;
  }

  return QGraphicsItem::itemChange(change, value);
}

void NodeItem::contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
  // Create the menu
  QMenu menu;

  // Add actions to the menu
  QAction* deleteAction = menu.addAction("Delete");
  QAction* propertiesAction = menu.addAction("Properties");
  QAction* copyAction = menu.addAction("Copy");

  // Connect actions to their slots
  QObject::connect(deleteAction, &QAction::triggered, [this]() { onDelete(); });
  QObject::connect(propertiesAction, &QAction::triggered, [this]() {
    onProperties();
  });
  QObject::connect(copyAction, &QAction::triggered, [this]() {
    if (nodeCopied)
      nodeCopied(this);
  });

  // Execute the menu at the mouse cursor's position
  menu.exec(event->screenPos());
}

void NodeItem::updatePosition(const QPointF& position)
{
  setPos(position);

  for (auto& connector : mConnectors)
    connector->updateConnections();

  updateLabelPosition();
}

// Slots
void NodeItem::onDelete()
{
  // If the node has a parent, inform the parent about the deletion
  if (parentNode())
    parentNode()->childRemoved(this);

  for (NodeItem* child : children())
    child->onDelete();

  // Remove the item from the scene
  if (nodeDeleted)
    nodeDeleted(this);

  // Optionally delete the item
  delete this;
}

void NodeItem::onProperties()
{
  // Handle the properties action, e.g., show a dialog to edit properties
}

SaveInfo NodeItem::saveInfo() const
{
  SaveInfo info;
  info.position = pos();
  info.fields = fields();
  info.nodeId = nodeId();
  info.pixmap = nodePixmap();
  info.properties = properties();
  info.size = mSize;

  return info;
}
