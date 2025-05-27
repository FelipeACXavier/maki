#include "node.h"

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QMenu>
#include <QObject>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QTimer>
#include <QUuid>

#include "app_configs.h"
#include "flow.h"
#include "logging.h"
#include "style_helpers.h"
#include "system/canvas.h"

static const qreal FADE_THRESHOLD = 1.0;

static const qreal FADE_IN_BASE = 1000;
static const qreal FADE_IN_START = 0.01;
static const qreal FADE_IN_MULTIPLIER = 0.01;

static const qreal FADE_OUT_BASE = 4000;
static const qreal FADE_OUT_START = 2.0;
static const qreal FADE_OUT_MULTIPLIER = 0.3;

static const qreal LOG_TWO = log(2);
static const qreal MIN_OPACITY = 0.01;
static const qreal MAX_OPACITY = 1.0;

NodeItem::NodeItem(const QString& nodeId, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent)
    : NodeBase((!nodeId.isEmpty() && !nodeId.isNull()) ? nodeId : QUuid::createUuid().toString(), info->nodeId, nodeConfig, parent)
    , mStorage(info)
    , mChildrenNodes({})
    , mBaseScale(config()->libraryType == Types::LibraryTypes::STRUCTURAL ? mStorage->scale : 1.0)
    , mSize(mStorage->size) // / baseScale())
{
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  setCacheMode(DeviceCoordinateCache);
  setAcceptDrops(config()->libraryType == Types::LibraryTypes::STRUCTURAL);
  setAcceptHoverEvents(config()->libraryType == Types::LibraryTypes::STRUCTURAL);

  mStorage->id = this->id();
  mStorage->nodeId = this->nodeId();

  // Children are created by the canvas, so we must make sure that there is no trailing children information
  mStorage->children = {};

  if (parent && parent->type() == Types::NODE)
    mStorage->parentId = static_cast<NodeItem*>(parent)->id();

  for (const auto& property : config()->properties)
  {
    if (!mStorage->properties.contains(property.id))
      mStorage->properties[property.id] = property.defaultValue;
  }

  for (const auto& event : config()->events)
  {
    bool found = false;
    for (const auto& flow : mStorage->flows)
    {
      if (flow->name != event.name)
        continue;

      found = true;
      break;
    }

    if (found)
      continue;

    mStorage->flows.push_back(std::make_shared<FlowSaveInfo>(event));
  }

  // Add icon if it exists
  if (!mStorage->pixmap.isNull())
  {
    QSize newSize = mStorage->pixmap.size() / baseScale();
    setPixmap(mStorage->pixmap.scaled(newSize, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  }
  else
  {
    qreal labelSize = qMax(Fonts::BaseSize, mSize.width() / Fonts::BaseFactor);
    setLabel(getProperty("name").toString(), config()->body.textColor, labelSize);
  }

  updatePosition(snapToGrid(initialPosition - boundingRect().center(), Config::GRID_SIZE));
  // updatePosition(initialPosition);

  LOG_DEBUG("%s created at: (%f, %f) with size (%f, %f) and scale %f", qPrintable(id()), pos().x(), pos().y(), mSize.width(), mSize.height(), baseScale());
}

NodeItem::~NodeItem()
{
  // auto copy = transitions();
  // for (auto& item : copy)
  //   delete item;
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
  return nodeId();
}

qreal NodeItem::baseScale() const
{
  return mStorage->scale;
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

  // auto currentScale = static_cast<Canvas*>(scene())->getScale();
  // qreal zoomRatio = currentScale / baseScale();

  // qreal originalSize = mSize.width();
  // qreal fullOpacityThreshold = FADE_THRESHOLD;
  // qreal fadeInThreshold = FADE_IN_START + qMax((FADE_IN_BASE / originalSize) * FADE_IN_MULTIPLIER, MIN_OPACITY);
  // qreal fadeOutThreshold = FADE_OUT_START * (MAX_OPACITY + (FADE_OUT_BASE / originalSize) * FADE_OUT_MULTIPLIER);

  // // Make smaller objects fade faster when zooming out and slower when zooming in.
  // qreal opacity = MAX_OPACITY;
  // if (zoomRatio < fadeInThreshold)
  // {
  //   opacity = MIN_OPACITY;
  // }
  // else if (zoomRatio < fullOpacityThreshold)
  // {
  //   qreal progress = (zoomRatio - fadeInThreshold) / (fullOpacityThreshold - fadeInThreshold);
  //   opacity = progress;
  // }
  // else if (zoomRatio > fadeOutThreshold)
  // {
  //   qreal progress = MAX_OPACITY - (zoomRatio - fadeOutThreshold) / (fadeOutThreshold - fullOpacityThreshold);
  //   opacity = progress;
  // }

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

QVector<PropertiesConfig> NodeItem::configurationProperties() const
{
  return config()->properties;
}

QMap<QString, QVariant> NodeItem::properties() const
{
  return mStorage->properties;
}

QVector<PropertiesConfig> NodeItem::fields() const
{
  return mStorage->fields;
}

QVector<std::shared_ptr<FlowSaveInfo>> NodeItem::events() const
{
  return mStorage->flows;
}

QVector<ControlsConfig> NodeItem::controls() const
{
  return config()->controls;
}

QVariant NodeItem::getProperty(const QString& key) const
{
  if (!mStorage)
    return QVariant();

  if (mStorage->properties.find(key) == mStorage->properties.end())
    return QVariant();

  return mStorage->properties.value(key);
}

void NodeItem::setProperty(const QString& key, QVariant value)
{
  if (!mStorage)
    return;

  if (mStorage->properties.find(key) == mStorage->properties.end())
  {
    LOG_WARNING("Tried to update property %s but it does not exist", qPrintable(key));
    return;
  }

  mStorage->properties[key] = value;

  if (key == "name")
    setLabelName(value.toString());

  if (nodeModified)
    nodeModified(this);

  update();
}

VoidResult NodeItem::setField(const QString& key, const QJsonObject& value)
{
  if (!mStorage)
    return VoidResult::Failed("Storage is not set");

  // Check if key exists
  auto property = PropertiesConfig(value);
  if (!property.isValid())
    return VoidResult::Failed(property.errorMessage.toStdString());

  for (auto& field : fields())
  {
    if (field.id != key)
      continue;

    field = property;
    return VoidResult();
  }

  mStorage->fields.push_back(property);

  return VoidResult();
}

PropertiesConfig NodeItem::getField(const QString& key) const
{
  if (!mStorage)
    return PropertiesConfig();

  for (const auto& field : fields())
  {
    if (field.id == key)
      return field;
  }

  return PropertiesConfig();
}

void NodeItem::removeField(const QString& key)
{
  if (!mStorage)
    return;

  for (auto iter = mStorage->fields.begin(); iter < mStorage->fields.end(); ++iter)
  {
    if (iter->id != key)
      continue;

    mStorage->fields.erase(iter);
    return;
  }
}

void NodeItem::setEvent(int index, const FlowConfig& event)
{
  // if (!mStorage)
  //   return;

  // if (index < mStorage->events.size())
  //   mStorage->events[index] = event;
  // else
  //   mStorage->events.push_back(event);
}

QVector<INode*> NodeItem::children() const
{
  return mChildrenNodes;
}

void NodeItem::addChild(NodeItem* node, std::shared_ptr<NodeSaveInfo> info)
{
  if (info)
    mStorage->children.append(info);

  mChildrenNodes.push_back(node);
}

void NodeItem::childRemoved(NodeItem* child)
{
  mStorage->children.removeIf([child](std::shared_ptr<NodeSaveInfo> info) { return info->id == child->id(); });
  mChildrenNodes.removeAll(child);
}

INode* NodeItem::parentNode() const
{
  if (parentItem() && parentItem()->type() == Types::NODE)
    return static_cast<NodeItem*>(parentItem());

  return nullptr;
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
    QPointF delta = event->pos() - mResizeStartMousePos;
    qreal newWidth = qMax(Config::MINIMUM_NODE_SIZE, mResizeStartSize.width() + delta.x());
    qreal newHeight = qMax(Config::MINIMUM_NODE_SIZE, mResizeStartSize.height() + delta.y());

    // Clamp size to parent
    if (parentItem())
    {
      QRectF parentBounds = dynamic_cast<NodeItem*>(parentItem())->boundingRect();
      QPointF localTopLeft = pos();

      // Clamp width and height so the bottom-right stays inside parent
      qreal maxWidth = parentBounds.right() - localTopLeft.x();
      qreal maxHeight = parentBounds.bottom() - localTopLeft.y();

      newWidth = qMin(newWidth, maxWidth);
      newHeight = qMin(newHeight, maxHeight);
    }

    // Update the scale when the node is resized
    mStorage->scale = qMax(config()->body.width / newWidth, config()->body.height / newHeight);

    mSize.setWidth(newWidth);
    mSize.setHeight(newHeight);
    mStorage->size = mSize;

    qreal newFontSize = qMax(Fonts::BaseSize, mSize.width() / Fonts::BaseFactor);

    setLabelSize(newFontSize, mSize);
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
    mResizeStartMousePos = event->pos();
    mResizeStartSize = mSize;
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

  QGraphicsItem::mouseReleaseEvent(event);
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemPositionChange)
  {
    if (parentNode())
    {
      QPointF newPos = value.toPointF();
      auto parent = dynamic_cast<NodeItem*>(parentNode());
      QRectF bounds = parent->boundingRect();

      // Optional: subtract the child's size if you want full containment
      QRectF childRect = boundingRect();
      QSizeF childSize = childRect.size();

      // Clamp position so child stays inside parent
      newPos.setX(qBound(bounds.left(), newPos.x(), bounds.right() - childSize.width()));
      newPos.setY(qBound(bounds.top(), newPos.y(), bounds.bottom() - childSize.height()));

      return newPos;
    }
  }
  else if (change == QGraphicsItem::ItemPositionHasChanged)
  {
    updateExtrasPosition();
    mStorage->position = pos() + boundingRect().center();
  }

  return QGraphicsItem::itemChange(change, value);
}

void NodeItem::updatePosition(const QPointF& position)
{
  setPos(position);
  updateExtrasPosition();
  mStorage->position = pos() + boundingRect().center();
}

void NodeItem::updateExtrasPosition()
{
  for (auto& transition : transitions())
    transition->updatePath();

  updateLabelPosition();
}

// Slots
void NodeItem::deleteNode()
{
  // If the node has a parent, inform the parent about the deletion
  if (parentNode())
    dynamic_cast<NodeItem*>(parentNode())->childRemoved(this);

  auto toDelete = children();
  for (INode* child : toDelete)
    dynamic_cast<NodeItem*>(child)->deleteNode();

  auto transtionsToDelete = transitions();
  for (TransitionItem* transtion : transtionsToDelete)
    delete transtion;

  // Remove the item from the scene
  if (nodeDeleted)
    nodeDeleted(this);

  setSelected(false);

  QTimer::singleShot(0, qApp, [this]() { delete this; });
}

void NodeItem::onProperties()
{
  // Handle the properties action, e.g., show a dialog to edit properties
}

NodeSaveInfo NodeItem::saveInfo() const
{
  return *mStorage;
}

QVector<TransitionItem*> NodeItem::transitions() const
{
  return mTransitions;
}

void NodeItem::addTransition(TransitionItem* transition)
{
  // Make sure the source node holds the transition info
  if (transition->destination() && (id() != transition->destination()->id()))
    mStorage->transitions.push_back(transition->storage());

  mTransitions.push_back(transition);
}

void NodeItem::removeTransition(TransitionItem* transition)
{
  mStorage->transitions.removeIf([transition](std::shared_ptr<TransitionSaveInfo> item) {
    return transition->id() == item->id;
  });
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
  dir /= std::hypot(dir.x(), dir.y());
  qreal radius = boundingRect().width() / 2.0;
  return center + dir * radius;
}

bool NodeItem::canAddTransition() const
{
  int index = 0;
  for (const auto& t : transitions())
  {
    if (t->source()->id() == id())
      ++index;
  }

  return config()->transitions.isEmpty() || index < config()->transitions.size();
}

TransitionConfig NodeItem::nextTransition() const
{
  // Only count the transitions coming from this
  int index = 0;
  for (const auto& t : transitions())
  {
    if (t->source()->id() == id())
      ++index;
  }

  if (config()->transitions.isEmpty() || index >= config()->transitions.size())
    return TransitionConfig();

  return config()->transitions.at(index);
}

Flow* NodeItem::createFlow(const QString& flowName, std::shared_ptr<FlowSaveInfo> info)
{
  std::shared_ptr<FlowSaveInfo> flowConfig = info;
  if (info != nullptr)
  {
    // Clean up
    bool found = false;
    for (const auto& f : mStorage->flows)
    {
      if (f->id != info->id)
        continue;

      flowConfig = f;
      found = true;
      break;
    }

    if (!found)
      mStorage->flows.push_back(flowConfig);
  }
  else
  {
    flowConfig = std::make_shared<FlowSaveInfo>();
    mStorage->flows.push_back(flowConfig);
  }

  Flow* flow = new Flow(flowName, flowConfig);
  mFlows.push_back(flow);

  if (flowAdded)
    flowAdded(flow, this);

  return flow;
}

Flow* NodeItem::getFlow(const QString& flowId) const
{
  for (const auto& flow : mFlows)
  {
    if (flow->id() == flowId)
      return flow;
  }

  return nullptr;
}

void NodeItem::deleteFlow(const QString& flowId)
{
  mStorage->flows.removeIf([flowId](std::shared_ptr<FlowSaveInfo> item) {
    return flowId == item->id;
  });
  mFlows.removeIf([flowId](Flow* flow) { return flow->id() == flowId; });
}

void NodeItem::updateFlow()
{
}
