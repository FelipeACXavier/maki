#include "node.h"

#include <QGraphicsDropShadowEffect>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QMenu>
#include <QObject>
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QSvgRenderer>
#include <QTimer>
#include <QUndoStack>
#include <QUuid>

#include "app_configs.h"
#include "app_paths.h"
#include "canvas/canvas.h"
#include "canvas/canvas_message.h"
#include "document/flow_info.h"
#include "document/node_info.h"
#include "document/property_info.h"
#include "keys.h"
#include "logging.h"
#include "macros.h"
#include "system/undo_commands/move_node.h"
#include "system/undo_commands/rename_node.h"
#include "system/undo_commands/resize_node.h"
#include "widgets/controls/label_editor.h"

NodeItem::NodeItem(const QString& nodeId, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
                   QGraphicsItem* parent)
    : NodeBase((!nodeId.isEmpty() && !nodeId.isNull()) ? nodeId : QUuid::createUuid().toString(), info->getnodeId(), nodeConfig, parent)
    , mStorage(info)
    , mSize(mStorage->getSize())
    , mParentNode(nullptr)
    , mChildrenNodes({})
    , mBaseScale(config()->libraryType == Types::LibraryTypes::STRUCTURAL ? mStorage->getScale() : 1.0)
{
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  setCacheMode(DeviceCoordinateCache);
  setAcceptDrops(config()->libraryType == Types::LibraryTypes::STRUCTURAL);
  setAcceptHoverEvents(config()->libraryType == Types::LibraryTypes::STRUCTURAL);

  // Add icon if it exists
  if (!mStorage->getIcon().isEmpty())
    setIcon(AppPaths::icon(config()->body.iconPath), config()->body.iconColor);

  qreal labelSize = qMax(Fonts::BaseSize, mSize.width() / Fonts::BaseFactor);
  if (const auto* name = getProperty(ConfigKeys::NAME))
    setLabel(name->getvalue()->toStringValue(), labelSize);

  updatePosition(initialPosition - nodeRect().center(), Config::NodeMove::Start);
  LOG_DEBUG("{} created at: ({}, {}) with size ({}, {}), scale {} and {} ports", id(), pos().x(), pos().y(), mSize.width(), mSize.height(), baseScale(),
            config()->ports.size());
}

NodeItem::~NodeItem()
{
  if (mControlWidget)
  {
    delete mControlWidget.data();
    mControlWidget = nullptr;
  }
}

int NodeItem::type() const
{
  return Type;
}

Types::LibraryTypes NodeItem::function() const
{
  return config()->libraryType;
}

void NodeItem::createPorts()
{
  for (const auto& port : config()->ports)
    mPorts[{Constants::MAIN_SUB_FLOW, port.type}] = new PortItem(port.type, this);
}

QString NodeItem::nodeName() const
{
  if (const auto* name = getProperty(ConfigKeys::NAME))
    return name->getvalue()->toStringValue();

  return QString();
}

QString NodeItem::nodeType() const
{
  // This should also contain the library to make it unique
  return nodeId();
}

qreal NodeItem::baseScale() const
{
  return mStorage->getScale();
}

VoidResult NodeItem::start()
{
  // updatePosition(snapToGrid(mLastPosition - boundingRect().center(), Config::GRID_SIZE));
  createPorts();
  return VoidResult();
}

QRectF NodeItem::nodeRect() const
{
  return QRectF(0, 0, mSize.width(), mSize.height());
}

QRectF NodeItem::sceneNodeRect() const
{
  return mapRectToScene(nodeRect());
}

QRectF NodeItem::sceneAlignRect() const
{
  return sceneNodeRect();
}

PortItem* NodeItem::getPort(Types::Port type, const QString& subFlowId) const
{
  if (!mPorts.contains({subFlowId, type}))
    return nullptr;

  return mPorts[{subFlowId, type}];
}

void NodeItem::highlight(const QColor& color, const QString& message, int durationMs)
{
  auto* glow = new QGraphicsDropShadowEffect();
  glow->setOffset(0, 0);
  glow->setBlurRadius(16);
  glow->setColor(color);

  setGraphicsEffect(glow);

  // Persistent message
  if (!message.isEmpty())
  {
    if (!mHighlightMessage)
    {
      mHighlightMessage = new CanvasMessage(message, [this]() { dismissHighlight(); }, this);
      if (!mHighlightMessage)
        return;

      // Centered on top
      constexpr qreal spacing = 10.0;
      const QRectF messageRect = mHighlightMessage->boundingRect();
      const qreal x = nodeRect().center().x() - messageRect.width() / 2.0;
      const qreal y = nodeRect().top() - messageRect.height() - spacing;
      mHighlightMessage->setPos(x, y);
    }
    else
    {
      mHighlightMessage->setMessage(message);
      mHighlightMessage->show();
    }
  }
  else
  {
    QTimer::singleShot(durationMs, [this]() {
      setGraphicsEffect(nullptr);
      update();
    });
  }
}

void NodeItem::dismissHighlight()
{
  if (mHighlightMessage)
    mHighlightMessage->hide();

  setGraphicsEffect(nullptr);
  update();
}

void NodeItem::showSimulationControls(maki::ControlWidget* controls, const QColor& highlightColor)
{
  showControls(controls, ControlProperties{
                           .fadingMode = ControlProperties::FadingMode::NoFade,
                           .highlightColor = highlightColor,
                           .position = Config::ControlPosition::Top,
                         });
}

void NodeItem::placeControl(const QPointF& localPos)
{
  mControlLocalPos = localPos;
  updateControlPosition();
}

void NodeItem::updateControlPosition()
{
  if (!mControlWidget || mControlWidget->scene() != scene())
    return;

  // Convert from NodeItem coordinates into scene coordinates.
  mControlWidget->setPos(mapToScene(mControlLocalPos));
}

void NodeItem::showControls(maki::ControlWidget* controls, const ControlProperties& properties)
{
  if (!controls)
  {
    dismissControl();
    return;
  }

  if (!scene())
    return;

  if (properties.highlightColor.isValid())
  {
    auto* glow = new QGraphicsDropShadowEffect();
    glow->setOffset(0, 0);
    glow->setBlurRadius(16);
    glow->setColor(properties.highlightColor);

    setGraphicsEffect(glow);
  }
  else
  {
    setGraphicsEffect(nullptr);
  }

  if (!mControlWidget)
  {
    mControlWidget = new CanvasControlWidget([this]() {
      if (mControlWidget)
        QMetaObject::invokeMethod(mControlWidget.data(), [this]() { dismissControl(); }, Qt::QueuedConnection);
    });

    // Add directly to the same scene as the NodeItem.
    scene()->addItem(mControlWidget);

    // Above normal canvas items.
    mControlWidget->setZValue(10000);
    mControlWidget->hide();
  }

  mControlActive = true;
  mControlWidget->setControlWidget(controls, properties);

  // Use the proxy as the timer context so the callback is cancelled
  // automatically if the proxy is destroyed.
  QTimer::singleShot(0, mControlWidget.data(), [this, position = properties.position, fixedPosition = properties.fixedPosition]() {
    if (!mControlWidget)
      return;

    constexpr qreal spacing = 10.0;
    const QRectF node = nodeRect();
    const QRectF control = mControlWidget->boundingRect();

    QPointF pos;
    switch (position)
    {
      case Config::ControlPosition::Top:
        pos = {node.center().x() - control.width() / 2.0, node.top() - control.height() - spacing};
        break;

      case Config::ControlPosition::Bottom:
        pos = {node.center().x() - control.width() / 2.0, node.bottom() + spacing};
        break;

      case Config::ControlPosition::Left:
        pos = {node.left() - control.width() - spacing, node.center().y() - control.height() / 2.0};
        break;

      case Config::ControlPosition::Right:
        pos = {node.right() + spacing, node.center().y() - control.height() / 2.0};
        break;

      case Config::ControlPosition::Fixed:
        pos = fixedPosition - control.center();
        break;
    }

    // Store the local anchor and convert to scene coordinates.
    placeControl(pos);

    mControlWidget->show();
    mControlWidget->activate();
  });
}

void NodeItem::dismissControl()
{
  if (mControlWidget)
  {
    if (auto control = qobject_cast<LabelEditor*>(mControlWidget->controlWidget()); control)
      toggleLabelVisibility(true);

    mControlWidget->hide();
    mControlWidget->deleteLater();
    mControlWidget = nullptr;
  }

  mControlActive = false;

  setGraphicsEffect(nullptr);
  update();
}

void NodeItem::hideControl()
{
  if (!hasControl())
    return;

  const auto hiding = (mControlWidget && mControlWidget->hideControl());
  // If we are already hiding, i.e., the control has started to fade, we don't need to dismiss the control
  if (hiding)
    return;

  // Controls that do not fade must be forcefully dismissed
  dismissControl();
}

bool NodeItem::hasControl()
{
  return mControlActive;
}

void NodeItem::paintDefaultNode(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  Q_UNUSED(style);
  Q_UNUSED(widget);

  auto color = getProperty("color");
  auto background = color ? QColor::fromString(color->getvalue()->toStringValue()) : config()->body.backgroundColor;
  const QPen outlinePen = isSelected() ? QPen(Config::HIGHLIGHT, 3.0 / baseScale()) : QPen(Config::FOREGROUND, 1.0 / baseScale());

  NodeBase::paintNode(nodeRect(), background, outlinePen, painter);
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  paintDefaultNode(painter, style, widget);
}

QPainterPath NodeItem::shape() const
{
  QPainterPath path = NodeBase::nodeShape(nodeRect());

  if (!labelBoundingRect().isEmpty())
    path.addRect(labelBoundingRect());

  return path;
}

QVector<PropertyConfig> NodeItem::configurationProperties() const
{
  return config()->properties;
}

QVector<std::shared_ptr<IParameter>> NodeItem::properties() const
{
  return mStorage->getproperties();
}

QVector<std::shared_ptr<IParameter>> NodeItem::fields() const
{
  return mStorage->getfields();
}

QVector<std::shared_ptr<IFlow>> NodeItem::eventConfigs() const
{
  return mStorage->getevents();
}

QVector<std::shared_ptr<IFlow>> NodeItem::flowConfigs() const
{
  return mStorage->getflows();
}

QVector<std::shared_ptr<IFlow>> NodeItem::allEventFlowConfigs() const
{
  return eventConfigs() + flowConfigs();
}

QVector<ControlsConfig> NodeItem::controls() const
{
  return config()->controls;
}

const PropertyInfo* NodeItem::getProperty(const QString& key) const
{
  if (!mStorage)
    return nullptr;

  return mStorage->getProperty(key);
}

void NodeItem::setProperty(const QString& key, const maki::Value& value)
{
  if (!mStorage)
    return;

  LOG_TRACE("Setting property {} of node: {} to {}", key, nodeId(), value.toReadable());
  if (key == ConfigKeys::NAME)
    setLabelName(value.toString());

  if (nodeModified)
    nodeModified(this);

  update();
}

void NodeItem::renameNode()
{
  const QRectF bounds = labelBoundingRect();
  if (bounds.isEmpty())
    return;

  auto* editor = LabelEditor::create(nodeName(), labelFont());
  editor->setFixedWidth(bounds.width());
  editor->finished = [this](const QString& name) {
    if (name.isEmpty())
      return;

    if (auto canvas = qobject_cast<Canvas*>(scene()))
      canvas->undoStack()->push(new RenameNodeCommand(id(), nodeName(), name));
  };

  constexpr QPointF labelOffset{0.5, -0.5};
  showControls(editor, ControlProperties{
                         .fixedPosition = bounds.center() + labelOffset,
                         .position = Config::ControlPosition::Fixed,
                       });

  toggleLabelVisibility();
  editor->setFocus(Qt::MouseFocusReason);
}

void NodeItem::setEvent(int index, const FlowConfig& event)
{
}

VoidResult NodeItem::setField(const QString& key, const QJsonObject& property)
{
  if (!mStorage)
    return VoidResult::Failed("Storage is not set");

  return VoidResult();
}

VoidResult NodeItem::setField(const QString& key, std::shared_ptr<PropertyInfo> property)
{
  if (!mStorage)
    return VoidResult::Failed("Storage is not set");

  return VoidResult();
}

PropertyInfo NodeItem::getField(const QString& key) const
{
  if (!mStorage)
    return PropertyInfo();

  return mStorage->getField(key);
}

void NodeItem::removeField(const QString& key)
{
  if (!mStorage)
    return;

  mStorage->removeField(key);
}

QVector<NodeItem*> NodeItem::children(const QString& subflow) const
{
  return mChildrenNodes;
}

void NodeItem::addParent(NodeItem* parent, const QString& subflow)
{
  if (!parent)
    return;

  LOG_DEBUG("Setting parent {} of node {}", parent->nodeId(), nodeId());
  mParentNode = parent;
  setZValue(parent->zValue() + 2);

  fitInsideParent(20);
}

void NodeItem::removeParent()
{
  if (!parentNode())
    return;

  LOG_DEBUG("Removing parent {} of node {}", parentNode()->nodeId(), nodeId());
  mParentNode = nullptr;
  setZValue(config()->body.zIndex);
}

void NodeItem::addChild(NodeItem* node, std::shared_ptr<NodeSaveInfo> info)
{
  if (!info)
    return;

  LOG_INFO("{} adding child: {}", id(), info->getid());
  mChildrenNodes.push_back(node);
}

void NodeItem::childRemoved(NodeItem* child)
{
  mChildrenNodes.removeAll(child);
}

NodeItem* NodeItem::parentNode() const
{
  return mParentNode;
}

QString NodeItem::parentSubFlow() const
{
  return mStorage->getParentSubFlow();
}

QString NodeItem::behaviour() const
{
  return config()->behaviour.code;
}

HelpConfig NodeItem::help() const
{
  return config()->help;
}

bool NodeItem::constrainChildren() const
{
  return true;
}

QRectF NodeItem::childAreaSceneRect(const QString& subflow) const
{
  return mapRectToScene(nodeRect());
}

QRectF NodeItem::parentInnerSceneRect(qreal padding) const
{
  if (!parentNode())
    return {};

  return parentNode()->childAreaSceneRect().adjusted(padding, padding, -padding, -padding);
}

// Apply a new logical size to this node in one place
void NodeItem::applySize(const QSizeF& size)
{
  if (size == mSize)
    return;

  prepareGeometryChange();
  mSize = size;
  SaveInfo::instance().setSizeOfNode(id(), size);

  qreal newFontSize = qMax(Fonts::BaseSize, mSize.width() / Fonts::BaseFactor);
  setLabelSize(newFontSize, mSize);

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
  const auto* parent = parentNode();
  if (!parent)
    return;

  if (parent->constrainChildren())
  {
    QRectF inner = parentInnerSceneRect(padding);
    if (!inner.isValid())
      return;

    QSizeF currentSize = mSize;
    const qreal maxW = qMax(inner.width(), Config::MINIMUM_NODE_SIZE);
    const qreal maxH = qMax(inner.height(), Config::MINIMUM_NODE_SIZE);

    const qreal newW = qMin(currentSize.width(), maxW);
    const qreal newH = qMin(currentSize.height(), maxH);

    if (newW != currentSize.width() || newH != currentSize.height())
      applySize(QSizeF(newW, newH));
  }

  const QPointF constrained = parent->constrainChildPosition(this, pos());

  if (constrained != pos())
    updatePosition(constrained);
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
    for (auto* child : children())
      if (auto* nodeChild = qgraphicsitem_cast<NodeItem*>(child))
        nodeChild->fitInsideParent(10);
  }
  else
  {
    QGraphicsItem::mouseMoveEvent(event);
  }
}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  // if (config()->libraryType == Types::LibraryTypes::STRUCTURAL && event->modifiers() & Qt::ShiftModifier)
  // {
  //   mIsResizing = true;
  //   mResizeStartMousePos = event->pos();
  //   mResizeStartSize = mSize;
  //   dynamic_cast<QGraphicsView*>(scene()->parent())->setCursor(Qt::SizeFDiagCursor);
  //   event->accept();
  // }
  // else
  // {
  mDragStartPos = pos();
  QGraphicsItem::mousePressEvent(event);
  // }
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  if (mIsResizing)
  {
    mIsResizing = false;
    if (auto canvas = qobject_cast<Canvas*>(scene()))
      canvas->undoStack()->push(new ResizeNodeCommand(canvas, id(), mResizeStartSize, mSize));
  }
  else if (pos() != mDragStartPos)
  {
    if (auto canvas = qobject_cast<Canvas*>(scene()))
      canvas->undoStack()->push(new MoveNodeCommand(canvas, id(), mDragStartPos, pos()));

    if (nodeMoved)
      nodeMoved(this, true);
  }

  QGraphicsItem::mouseReleaseEvent(event);
}

void NodeItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->button() == Qt::LeftButton && labelBoundingRect().contains(event->pos()) && isLabelVisible())
  {
    renameNode();
    event->accept();
    return;
  }

  QGraphicsItem::mouseDoubleClickEvent(event);
}

void NodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  setCursor(Qt::OpenHandCursor);
  mHovered = true;
  NodeBase::hoverEnterEvent(event);
  if (nodeHovered)
    nodeHovered(this, true);
}

void NodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  setCursor(Qt::ArrowCursor);
  mHovered = false;
  NodeBase::hoverLeaveEvent(event);
  if (nodeHovered)
    nodeHovered(this, false);
}

bool NodeItem::isHovered() const
{
  return mHovered;
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemPositionChange)
  {
    if (const auto* parent = parentNode())
      return parent->constrainChildPosition(this, value.toPointF());
  }
  else if (change == QGraphicsItem::ItemPositionHasChanged)
  {
    updatePosition(value.toPointF());
  }

  return QGraphicsItem::itemChange(change, value);
}

void NodeItem::updatePosition(const QPointF& newPosition, Config::NodeMove reason)
{
  setPos(newPosition);

  QPointF delta = newPosition - mLastPosition;
  for (auto* child : children())
    if (child)
      child->updatePosition(child->pos() + delta, reason);

  mLastPosition = newPosition;
  if (reason != Config::NodeMove::Start)
    updateExtrasPosition(reason);
}

QPointF NodeItem::centerPosition() const
{
  return pos() + nodeRect().center();
}

void NodeItem::setCenterPosition(const QPointF& center)
{
  setPos(center - nodeRect().center());

  mLastPosition = pos();
  updateExtrasPosition(Config::NodeMove::Relayout);
}

void NodeItem::childPositionUpdated(NodeItem* /* child */)
{
}

QPointF NodeItem::constrainChildPosition(const NodeItem* child, const QPointF& proposedPosition) const
{
  if (!child)
    return proposedPosition;

  constexpr qreal padding = 10.0;
  const QRectF allowed = childAreaSceneRect().adjusted(padding, padding, -padding, -padding);

  if (!allowed.isValid())
    return proposedPosition;

  const QRectF childSceneRect = child->sceneBoundingRect();

  // pos() is the scene position for your semantic children because they are
  // not QGraphicsItem children of the parent.
  const QPointF offset = childSceneRect.topLeft() - child->pos();

  QPointF result = proposedPosition;

  const qreal minX = allowed.left() - offset.x();
  const qreal maxX = allowed.right() - childSceneRect.width() - offset.x();
  const qreal minY = allowed.top() - offset.y();
  const qreal maxY = allowed.bottom() - childSceneRect.height() - offset.y();

  // Avoid calling std::clamp with min > max.
  if (minX <= maxX)
    result.setX(std::clamp(result.x(), minX, maxX));

  if (minY <= maxY)
    result.setY(std::clamp(result.y(), minY, maxY));

  return result;
}

void NodeItem::updateExtrasPosition(Config::NodeMove reason)
{
  SaveInfo::instance().setPositionOfNode(id(), mLastPosition + boundingRect().center());

  if (auto* parent = parentNode())
    parent->childPositionUpdated(this);

  updateControlPosition();

  if (nodeMoved && reason == Config::NodeMove::User)
    nodeMoved(this, false);
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

QPointF NodeItem::edgePointToward(const QPointF& targetScenePos) const
{
  QPointF center = sceneNodeRect().center();
  QPointF dir = targetScenePos - center;

  if (dir.manhattanLength() < 0.001)
    return center;  // avoid divide by zero

  // Normalise and scale
  dir /= std::hypot(dir.x(), dir.y());
  qreal radius = nodeRect().width() / 2.0;
  return center + dir * radius;
}

QVector<TransitionConfig> NodeItem::configTransitions() const
{
  return config()->transitions;
}

std::vector<std::shared_ptr<Flow>> NodeItem::flows() const
{
  return mFlows;
}

PFlow NodeItem::createFlow(std::shared_ptr<FlowSaveInfo> info)
{
  auto exists = maki::findIf(mFlows, [id = info->getid()](const auto& f) { return f->id() == id; });
  if (exists)
    return *exists;

  auto flow = std::make_shared<Flow>(info->getname(), info);
  mFlows.push_back(flow);

  if (flowAdded)
    flowAdded(flow, this);

  return flow;
}

PFlow NodeItem::getFlow(const QString& flowId) const
{
  auto flow = maki::findBy(flows(), &Flow::id, flowId);
  if (flow)
    return *flow;

  return nullptr;
}

void NodeItem::deleteFlow(const QString& flowId)
{
  std::erase_if(mFlows, [&flowId](const std::shared_ptr<Flow>& flow) { return flow->id() == flowId; });
}
