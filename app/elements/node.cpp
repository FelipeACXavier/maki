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
#include "theme.h"

NodeItem::NodeItem(const QString& nodeId, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent)
    : NodeBase((!nodeId.isEmpty() && !nodeId.isNull()) ? nodeId : QUuid::createUuid().toString(), info->nodeId, nodeConfig, parent)
    , mStorage(info)
    , mBehaviour(nullptr)
    , mParentNode(nullptr)
    , mChildrenNodes({})
    , mBaseScale(config()->libraryType == Types::LibraryTypes::STRUCTURAL ? mStorage->scale : 1.0)
    , mSize(mStorage->size)
{
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  setCacheMode(DeviceCoordinateCache);
  setAcceptDrops(config()->libraryType == Types::LibraryTypes::STRUCTURAL);
  setAcceptHoverEvents(config()->libraryType == Types::LibraryTypes::STRUCTURAL);

  mStorage->id = this->id();
  mStorage->nodeId = this->nodeId();

  // Children are created by the canvas, so we must make sure that there is no trailing children information
  mStorage->children = {};

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
    setLabel(getProperty("name").toString(), labelSize);
  }

  updatePosition(snapToGrid(initialPosition - boundingRect().center(), Config::GRID_SIZE));
  mLastPosition = pos();

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

  NodeBase::paintNode(boundingRect(),
                      background,
                      isSelected() ? QPen(Config::HIGHLIGHT, 4 / baseScale()) : QPen(Config::FOREGROUND, 1.0 / baseScale()),
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
  LOG_INFO("[%s] Setting property of node: %s", qPrintable(id()), qPrintable(nodeId()));
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

VoidResult NodeItem::setField(const QString& key, const PropertiesConfig& property)
{
  if (!mStorage)
    return VoidResult::Failed("Storage is not set");

  // Check if key exists
  for (auto& field : mStorage->fields)
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

QVector<NodeItem*> NodeItem::children() const
{
  return mChildrenNodes;
}

void NodeItem::addParent(NodeItem* parent)
{
  if (!parent)
    return;

  mParentNode = parent;
  mStorage->parentId = parent->id();
  setZValue(parent->zValue() + 2);

  fitInsideParent(20);
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

NodeItem* NodeItem::parentNode() const
{
  return mParentNode;
}

QString NodeItem::behaviour() const
{
  return config()->behaviour.code;
}

HelpConfig NodeItem::help() const
{
  return config()->help;
}

QRectF NodeItem::parentInnerSceneRect(qreal padding) const
{
  if (!parentNode())
    return {};

  QRectF r = parentNode()->mapRectToScene(parentNode()->boundingRect());
  return r.adjusted(padding, padding, -padding, -padding);
}

// Apply a new logical size to this node in one place
void NodeItem::applySize(const QSizeF& size)
{
  if (size == mSize)
    return;

  mSize = size;
  mStorage->size = mSize;

  // Same scale logic as before
  mStorage->scale = qMax(config()->body.width / mSize.width(), config()->body.height / mSize.height());

  qreal newFontSize = qMax(Fonts::BaseSize, mSize.width() / Fonts::BaseFactor);

  setLabelSize(newFontSize, mSize);
  prepareGeometryChange();
  update();
}

// Clamp this node's position so its scene rect stays inside `inner`
QPointF NodeItem::clampPosInside(const QRectF& inner, const QRectF& childSceneRect) const
{
  QPointF posScene = pos();
  QPointF offset = childSceneRect.topLeft() - posScene;  // pos() → top-left

  QPointF newPos = posScene;

  const qreal minX = inner.left() - offset.x();
  const qreal maxX = inner.right() - (childSceneRect.width() + offset.x());
  const qreal minY = inner.top() - offset.y();
  const qreal maxY = inner.bottom() - (childSceneRect.height() + offset.y());

  newPos.setX(std::clamp(newPos.x(), minX, maxX));
  newPos.setY(std::clamp(newPos.y(), minY, maxY));

  return newPos;
}

QSizeF NodeItem::clampSize(qreal width, qreal height) const
{
  if (!parentNode())
    return QSizeF(width, height);

  QRectF inner = parentInnerSceneRect(10);
  if (!inner.isValid())
    return QSizeF(width, height);

  // Where is the child now?
  QRectF childSceneRect = mapRectToScene(boundingRect());
  QPointF currentTopLeft = childSceneRect.topLeft();

  // Max width/height keeping bottom-right within inner
  qreal maxWidth = inner.right() - currentTopLeft.x();
  qreal maxHeight = inner.bottom() - currentTopLeft.y();

  qreal clampedW = qMin(width, qMax<qreal>(0.0, maxWidth));
  qreal clampedH = qMin(height, qMax<qreal>(0.0, maxHeight));

  return QSizeF(clampedW, clampedH);
}

void NodeItem::fitInsideParent(qreal padding)
{
  QRectF inner = parentInnerSceneRect(padding);
  if (!inner.isValid())
    return;

  // 1) Clamp size so we're not bigger than the inner rect
  QSizeF currentSize = mSize;
  qreal maxW = inner.width();
  qreal maxH = inner.height();

  maxW = qMax(maxW, Config::MINIMUM_NODE_SIZE);
  maxH = qMax(maxH, Config::MINIMUM_NODE_SIZE);

  qreal newW = qMin(currentSize.width(), maxW);
  qreal newH = qMin(currentSize.height(), maxH);

  if (newW != currentSize.width() || newH != currentSize.height())
    applySize(QSizeF(newW, newH));

  // 2) Clamp position so we're fully inside `inner`
  QRectF childSceneRect = mapRectToScene(boundingRect());
  QPointF newPos = clampPosInside(inner, childSceneRect);
  setPos(newPos);
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (mIsResizing && (event->modifiers() & Qt::ShiftModifier))
  {
    QPointF delta = event->pos() - mResizeStartMousePos;

    qreal newWidth = qMax(Config::MINIMUM_NODE_SIZE, mResizeStartSize.width() + delta.x());
    qreal newHeight = qMax(Config::MINIMUM_NODE_SIZE, mResizeStartSize.height() + delta.y());

    QSizeF clampedSize = clampSize(newWidth, newHeight);

    applySize(clampedSize);

    // After the parent resizes, keep children inside
    for (auto* child : mChildrenNodes)
    {
      auto* nodeChild = static_cast<NodeItem*>(child);
      nodeChild->fitInsideParent(10);
    }
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
    if (NodeItem* parent = parentNode())
    {
      QPointF newPos = value.toPointF();  // proposed new pos in scene coords

      // auto parent = parentNode();
      // if (parent == nullptr)
      //   return QGraphicsItem::itemChange(change, value);

      QRectF parentRect = parent->boundingRect();
      parentRect = parentRect.adjusted(10, 10, -10, -10);
      parentRect.translate(parent->pos());

      // Child rect in its own coords
      QRectF childLocalRect = boundingRect();

      // Compute allowed range so childSceneRect stays inside parentRect
      const qreal minX = parentRect.left();
      const qreal maxX = parentRect.right() - childLocalRect.width();
      const qreal minY = parentRect.top();
      const qreal maxY = parentRect.bottom() - childLocalRect.height();

      // Clamp
      newPos.setX(std::clamp(newPos.x(), minX, maxX));
      newPos.setY(std::clamp(newPos.y(), minY, maxY));

      return newPos;  // this replaces the proposed position
    }
  }
  else if (change == QGraphicsItem::ItemPositionHasChanged)
  {
    updatePosition(value.toPointF());
  }

  return QGraphicsItem::itemChange(change, value);
}

void NodeItem::updatePosition(const QPointF& newPosition)
{
  setPos(newPosition);

  QPointF delta = newPosition - mLastPosition;
  for (auto* child : mChildrenNodes)
  {
    auto childNode = static_cast<NodeItem*>(child);
    childNode->updatePosition(childNode->pos() + delta);
  }

  mLastPosition = newPosition;

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
  {
    bool found = false;
    for (const auto& t : mStorage->transitions)
    {
      if (t->id == transition->id())
      {
        found = true;
        break;
      }
    }

    if (!found)
      mStorage->transitions.push_back(transition->storage());

    for (auto& t : transitions())
    {
      // If I am the source of this transition
      // Check whether we have another transition with me as destination
      auto src1 = transition->source()->id();
      auto dst1 = transition->destination()->id();
      auto src2 = t->source()->id();
      auto dst2 = t->destination()->id();
      if (((src1 == dst2) && (src2 == dst1)))
      {
        transition->setEdge(TransitionItem::Edge::FORWARD);
        t->setEdge(TransitionItem::Edge::BACKWARD);
      }
    }
  }

  bool found = false;
  for (const auto& t : transitions())
  {
    if (t->id() == transition->id())
    {
      found = true;
      break;
    }
  }

  if (!found)
    mTransitions.push_back(transition);

  updateExtrasPosition();
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

Flow* NodeItem::createBehaviour(std::shared_ptr<FlowSaveInfo> info)
{
  if (mBehaviour != nullptr)
    return mBehaviour;

  std::shared_ptr<FlowSaveInfo> flowConfig = info;
  if (flowConfig == nullptr)
    flowConfig = std::make_shared<FlowSaveInfo>();

  mStorage->behaviour = flowConfig;
  mBehaviour = new Flow("MainBehaviour", flowConfig);

  return mBehaviour;
}

QVector<Flow*> NodeItem::flows() const
{
  return mFlows;
}

Flow* NodeItem::createFlow(const QString& flowName, std::shared_ptr<FlowSaveInfo> info)
{
  std::shared_ptr<FlowSaveInfo> flowConfig = info;
  bool found = false;
  if (info != nullptr)
  {
    // Clean up
    for (const auto& f : mStorage->flows)
    {
      if (f->id != info->id)
        continue;

      flowConfig = f;
      found = true;
      break;
    }
  }
  else
  {
    flowConfig = std::make_shared<FlowSaveInfo>();
  }

  if (!found)
  {
    // Set this as owner of the flow
    flowConfig->owner = id();
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
