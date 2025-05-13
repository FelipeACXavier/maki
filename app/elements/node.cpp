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
#include "system/canvas.h"

static const qreal FADE_THRESHOLD = 1.0;

static const qreal FADE_IN_BASE = 1000;
static const qreal FADE_IN_START = 0.01;
static const qreal FADE_IN_MULTIPLIER = 0.01;

static const qreal FADE_OUT_BASE = 2000;
static const qreal FADE_OUT_START = 2.0;
static const qreal FADE_OUT_MULTIPLIER = 0.3;

static const qreal LOG_TWO = log(2);
static const qreal MIN_OPACITY = 0.01;
static const qreal MAX_OPACITY = 1.0;

NodeItem::NodeItem(const QString& nodeId, const NodeSaveInfo& info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent)
    : NodeBase(nodeId, info.nodeId, nodeConfig, parent)
    , mChildrenNodes({})
    , mBaseScale(config()->libraryType == Types::LibraryTypes::STRUCTURAL ? info.scale : 1.0)
    , mSize(info.size / baseScale())
{
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  setCacheMode(DeviceCoordinateCache);
  setAcceptDrops(config()->libraryType == Types::LibraryTypes::STRUCTURAL);
  setAcceptHoverEvents(config()->libraryType == Types::LibraryTypes::STRUCTURAL);

  for (const auto& connector : config()->connectors)
  {
    QString connectorId = QUuid::createUuid().toString();
    for (const auto& cfg : info.connectors)
    {
      if (connector.id != cfg.configId)
        continue;

      connectorId = cfg.connectorId;
      break;
    }

    mConnectors.append(std::make_shared<Connector>(connector, connectorId, this));
  }

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

  if (info.events.isEmpty())
  {
    for (const auto& event : config()->events)
      mEvents.push_back(event);
  }
  else
  {
    mEvents = info.events;
  }

  LOG_INFO("Created node with %d events", mEvents.size());

  // Add icon if it exists
  if (!info.pixmap.isNull())
  {
    QSize newSize = info.pixmap.size() / baseScale();
    setPixmap(info.pixmap.scaled(newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  }
  else
  {
    qreal labelSize = qMax(Fonts::BaseSize, mSize.width() / Fonts::BaseFactor);
    setLabel(getProperty("name").toString(), config()->body.textColor, labelSize);
  }

  setParent(nullptr);
  updatePosition(snapToGrid(initialPosition - boundingRect().center(), Config::GRID_SIZE));

  LOG_DEBUG("%s created at: (%f, %f) with size (%f, %f) and scale %f", qPrintable(id()), pos().x(), pos().y(), mSize.width(), mSize.height(), baseScale());
}

NodeItem::~NodeItem()
{
  auto copy = mTransitions;
  for (auto& item : copy)
    delete item;
}

int NodeItem::type() const
{
  return Type;
}

Types::LibraryTypes NodeItem::function() const
{
  return config()->libraryType;
}

QString NodeItem::nodeName() const
{
  auto name = getProperty("name");
  return name.toString();
}

QString NodeItem::nodeType() const
{
  // This should also contain the library to make it unique
  return config()->type;
}

qreal NodeItem::baseScale() const
{
  return mBaseScale;
}

VoidResult NodeItem::start()
{
  return NodeBase::start();
}

QRectF NodeItem::boundingRect() const
{
  return QRectF(0, 0, mSize.width(), mSize.height());
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  auto color = getProperty("color");
  auto background = color.isValid() ? QColor::fromString(color.toString()) : config()->body.backgroundColor;

  auto currentScale = static_cast<Canvas*>(scene())->getScale();
  qreal zoomRatio = currentScale / baseScale();

  qreal originalSize = mSize.width();
  qreal fullOpacityThreshold = FADE_THRESHOLD;
  qreal fadeInThreshold = FADE_IN_START + qMax((FADE_IN_BASE / originalSize) * FADE_IN_MULTIPLIER, MIN_OPACITY);
  qreal fadeOutThreshold = FADE_OUT_START * (MAX_OPACITY + (FADE_OUT_BASE / originalSize) * FADE_OUT_MULTIPLIER);

  // Make smaller objects fade faster when zooming out and slower when zooming in.
  qreal opacity = MAX_OPACITY;
  if (zoomRatio < fadeInThreshold)
  {
    opacity = MIN_OPACITY;
    //  LOG_DEBUG("%s fade limit: %.2f | %.2f | %.2f | %.2f", qPrintable(nodeName()), opacity, zoomRatio, fadeInThreshold, fadeOutThreshold);
  }
  else if (zoomRatio < fullOpacityThreshold)
  {
    qreal progress = (zoomRatio - fadeInThreshold) / (fullOpacityThreshold - fadeInThreshold);
    // opacity = log(progress + 1) / LOG_TWO;
    opacity = progress;
    //  LOG_DEBUG("%s fade in: %.2f | %.2f | %.2f | %.2f", qPrintable(nodeName()), opacity, zoomRatio, fadeInThreshold, fadeOutThreshold);
  }
  else if (zoomRatio > fadeOutThreshold)
  {
    qreal progress = MAX_OPACITY - (zoomRatio - fadeOutThreshold) / (fadeOutThreshold - fullOpacityThreshold);
    //   // opacity = log(progress + 1) / LOG_TWO;
    opacity = progress;
    //   LOG_DEBUG("%s fade out: %.2f | %.2f | %.2f | %.2f", qPrintable(nodeName()), opacity, zoomRatio, fadeInThreshold, fadeOutThreshold);
  }

  // opacity = qBound(MIN_OPACITY, opacity, MAX_OPACITY);

  // setOpacity(opacity);

  // for (auto& connector : connectors())
  //   std::static_pointer_cast<Connector>(connector)->setOpacity(opacity);

  // setFlag(QGraphicsItem::ItemIsSelectable, opacity > Config::OPACITY_THRESHOLD);
  // setFlag(QGraphicsItem::ItemIsMovable, opacity > Config::OPACITY_THRESHOLD);

  NodeBase::paintNode(boundingRect(),
                      background,
                      isSelected() ? QPen(Config::Colours::ACCENT, 4 / baseScale()) : QPen(config()->body.borderColor, 1.0 / baseScale()),
                      painter);
}

QPainterPath NodeItem::shape() const
{
  return NodeBase::nodeShape(boundingRect());
}

QVector<std::shared_ptr<IConnector>> NodeItem::connectors() const
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

QVector<EventConfig> NodeItem::events() const
{
  return mEvents;
}

QVector<ControlsConfig> NodeItem::controls() const
{
  return config()->controls;
}

QVariant NodeItem::getProperty(const QString& key) const
{
  if (mProperties.find(key) == mProperties.end())
    return QVariant();

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

  if (key == "name")
    setLabelName(value.toString());

  if (nodeModified)
    nodeModified(this);

  update();
}

VoidResult NodeItem::setField(const QString& key, const QJsonObject& value)
{
  // Check if key exists
  auto property = PropertiesConfig(value);
  if (!property.isValid())
    return VoidResult::Failed(property.errorMessage.toStdString());

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

PropertiesConfig NodeItem::getField(const QString& key) const
{
  for (const auto& field : mFields)
  {
    if (field.id == key)
      return field;
  }

  return PropertiesConfig();
}

void NodeItem::removeField(const QString& key)
{
  for (auto iter = mFields.begin(); iter < mFields.end(); ++iter)
  {
    if (iter->id != key)
      continue;

    mFields.erase(iter);
    return;
  }
}

void NodeItem::setEvent(int index, const EventConfig& event)
{
  if (index < mEvents.size())
    mEvents[index] = event;
  else
    mEvents.push_back(event);
}

QVector<INode*> NodeItem::children() const
{
  return mChildrenNodes;
}

void NodeItem::addChild(NodeItem* child)
{
  mChildrenNodes.push_back(child);
}

INode* NodeItem::parentNode() const
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
    // qreal aspectRatio = mSize.width() / mSize.height();

    qreal newWidth = qMax(Config::MINIMUM_NODE_SIZE, event->pos().x());
    qreal newHeight = qMax(Config::MINIMUM_NODE_SIZE, event->pos().y());  // newWidth / aspectRatio;

    // Update the scale when the node is resized
    mBaseScale = qMax(config()->body.width / newWidth, config()->body.height / newHeight);

    mSize.setWidth(newWidth);
    mSize.setHeight(newHeight);

    qreal newFontSize = qMax(Fonts::BaseSize, mSize.width() / Fonts::BaseFactor);

    setLabelSize(newFontSize);
    prepareGeometryChange();
    update();
  }
  else
  {
    QGraphicsItem::mouseMoveEvent(event);
  }
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

  updatePosition(snapToGrid(scenePos(), Config::GRID_SIZE / static_cast<Canvas*>(scene())->getScale()));

  QGraphicsItem::mouseReleaseEvent(event);
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemPositionChange)
  {
    QPointF newPos = value.toPointF();

    if (parentNode() != nullptr)
    {
      auto parent = dynamic_cast<NodeItem*>(parentNode());

      // Ensure child stays inside parent's bounding box
      // Clamp position inside parent
      QRectF parentRect = parent->mapRectToScene(parent->boundingRect());
      QRectF childRect = boundingRect();

      newPos.setX(qBound(parentRect.left(), newPos.x(), parentRect.right() - childRect.width()));
      newPos.setY(qBound(parentRect.top(), newPos.y(), parentRect.bottom() - childRect.height()));
    }

    // Move all children relatively
    for (INode* child : children())
    {
      if (!child)
        continue;

      auto nodeChild = dynamic_cast<NodeItem*>(child);

      QPointF relativeOffset = nodeChild->pos() - pos();  // Maintain offset
      nodeChild->updatePosition(newPos + relativeOffset);
    }

    updateExtrasPosition();

    if (parentNode())
      return newPos;
  }

  return QGraphicsItem::itemChange(change, value);
}

void NodeItem::updatePosition(const QPointF& position)
{
  setPos(position);
  updateExtrasPosition();
}

void NodeItem::updateExtrasPosition()
{
  for (auto& connector : connectors())
    std::dynamic_pointer_cast<Connector>(connector)->updateConnections();

  for (auto& transition : mTransitions)
    transition->updatePath();

  updateLabelPosition();
}

// Slots
void NodeItem::deleteNode()
{
  // If the node has a parent, inform the parent about the deletion
  if (parentNode())
    dynamic_cast<NodeItem*>(parentNode())->childRemoved(this);

  for (INode* child : children())
    dynamic_cast<NodeItem*>(child)->deleteNode();

  // Remove the item from the scene
  if (nodeDeleted)
    nodeDeleted(this);

  setSelected(false);

  delete this;
}

void NodeItem::onProperties()
{
  // Handle the properties action, e.g., show a dialog to edit properties
}

NodeSaveInfo NodeItem::saveInfo() const
{
  NodeSaveInfo info;
  info.id = id();
  info.position = scenePos();
  info.nodeId = nodeId();
  info.fields = fields();
  info.events = events();
  info.pixmap = nodePixmap();
  info.properties = properties();
  info.size = QSizeF{mSize.width() * mBaseScale, mSize.height() * mBaseScale};
  info.scale = baseScale();

  for (const auto& connector : connectors())
    info.connectors.push_back(std::dynamic_pointer_cast<Connector>(connector)->saveInfo());

  if (parentNode())
    info.parentId = dynamic_cast<NodeItem*>(parentNode())->id();

  for (const auto& child : children())
  {
    // Save position relative to parent
    auto childInfo = static_cast<NodeItem*>(child)->saveInfo();
    childInfo.position = scenePos() - childInfo.position;
    info.children.push_back(childInfo);
  }

  return info;
}

// TODO(Felaze): these should be in a separate class
QVector<TransitionItem*> NodeItem::transitions() const
{
  return mTransitions;
}

void NodeItem::addTransition(TransitionItem* transition)
{
  mTransitions.push_back(transition);
}

void NodeItem::removeTransition(TransitionItem* transition)
{
  mTransitions.removeIf([transition](TransitionItem* item) {
    return item->id() == transition->id();
  });
}

QPointF NodeItem::edgePointToward(const QPointF& targetScenePos) const
{
  QPointF center = sceneBoundingRect().center();
  QPointF dir = targetScenePos - center;

  if (dir.manhattanLength() < 0.001)
    return center;  // avoid divide by zero

  // Normalise and scale
  dir /= std::hypot(dir.x(), dir.y());  // same as dir.normalized() in QVector2D
  qreal radius = boundingRect().width() / 2.0;
  return center + dir * radius;
}
