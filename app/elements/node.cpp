#include "node.h"

#include <QGraphicsSceneMouseEvent>
#include <QGraphicsView>
#include <QJsonObject>
#include <QUuid>
#include <algorithm>

#include "app_configs.h"
#include "flow.h"
#include "flow_info.h"
#include "keys.h"
#include "logging.h"
#include "port.h"
#include "property_info.h"
#include "style_helpers.h"
#include "system/canvas.h"
#include "system/undo_commands/move_node.h"
#include "types.h"

namespace
{
std::optional<QColor> optionalColorProperty(const QVariant& value)
{
  if (!value.isValid())
    return std::nullopt;

  const QString name = value.toString().trimmed();
  if (name.isEmpty() || !QColor::isValidColorName(name))
    return std::nullopt;

  return QColor::fromString(name);
}

QPointF pushOutOfRect(const QRectF& nodeLocalRect, const QPointF& proposedPos, const QRectF& obstacleScene)
{
  QRectF proposed(proposedPos.x(), proposedPos.y(), nodeLocalRect.width(), nodeLocalRect.height());
  if (!proposed.intersects(obstacleScene))
    return proposedPos;

  const qreal overlapLeft = proposed.right() - obstacleScene.left();
  const qreal overlapRight = obstacleScene.right() - proposed.left();
  const qreal overlapTop = proposed.bottom() - obstacleScene.top();
  const qreal overlapBottom = obstacleScene.bottom() - proposed.top();

  const qreal minOverlapX = qMin(overlapLeft, overlapRight);
  const qreal minOverlapY = qMin(overlapTop, overlapBottom);

  QPointF adjusted = proposedPos;
  if (minOverlapX < minOverlapY)
  {
    if (overlapLeft < overlapRight)
      adjusted.rx() -= overlapLeft;
    else
      adjusted.rx() += overlapRight;
  }
  else
  {
    if (overlapTop < overlapBottom)
      adjusted.ry() -= overlapTop;
    else
      adjusted.ry() += overlapBottom;
  }

  return adjusted;
}
}  // namespace

NodeItem::NodeItem(const QString& nodeId, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent)
    : NodeBase((!nodeId.isEmpty() && !nodeId.isNull()) ? nodeId : QUuid::createUuid().toString(), info->getnodeId(), nodeConfig, parent)
    , mStorage(info)
    , mBaseScale(config()->libraryType == Types::LibraryTypes::STRUCTURAL ? mStorage->getScale() : 1.0)
    , mSize(mStorage->getSize())
{
  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsScenePositionChanges);
  setCacheMode(DeviceCoordinateCache);

  mStorage->setId(this->id());
  mStorage->setNodeId(this->nodeId());

  initializeNodeSize();

  mStorage->clearChildren();

  for (const auto& property : config()->properties)
  {
    if (!mStorage->getproperties().contains(property.id))
      mStorage->addProperty(property.id, property.defaultValue);
  }

  for (const auto& event : config()->events)
  {
    bool found = false;
    for (const auto& flow : mStorage->getevents())
    {
      if (flow->getname() != event.name)
        continue;

      found = true;
      break;
    }

    if (found)
      continue;

    mStorage->addEvent(std::make_shared<FlowSaveInfo>(event));
  }

  const qreal labelSize = qMax(Fonts::BaseSize, mSize.width() / Fonts::BaseFactor);
  setLabel(getProperty("name").toString(), labelSize);

  configurePorts();

  updatePosition(snapToGrid(initialPosition - boundingRect().center(), Config::GRID_SIZE));
  mLastPosition = pos();

  LOG_DEBUG("%s created at: (%f, %f) with size (%f, %f) and scale %f", qPrintable(id()), pos().x(), pos().y(), mSize.width(), mSize.height(), baseScale());
}

NodeItem::~NodeItem() = default;

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
  return getProperty("name").toString();
}

QString NodeItem::nodeType() const
{
  return nodeId();
}

qreal NodeItem::baseScale() const
{
  return mStorage->getScale();
}

VoidResult NodeItem::start()
{
  if (config()->body.nodeSvg.isEmpty() && !mStorage->getIcon().isEmpty())
    setIcon(mStorage->getIcon(), config()->body.iconColor);

  return NodeBase::start();
}

QRectF NodeItem::nodeRect() const
{
  return QRectF(0, 0, mSize.width(), mSize.height());
}

QRectF NodeItem::sceneNodeRect() const
{
  return mapRectToScene(nodeRect());
}

void NodeItem::configurePorts()
{
  if (config()->libraryType == Types::LibraryTypes::STRUCTURAL)
    return;

  if (config()->hasInPort)
    mInPort = new PortItem(PortItem::In, this);
  if (config()->hasOutPort)
    mOutPort = new PortItem(PortItem::Out, this);
  if (config()->hasAbortPort)
    mAbortPort = new PortItem(PortItem::Abort, this);
  if (config()->hasErrorPort)
    mErrorPort = new PortItem(PortItem::Error, this);
  updatePortPositions();
}

void NodeItem::paintDefaultNode(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  Q_UNUSED(style);
  Q_UNUSED(widget);

  const auto background = optionalColorProperty(getProperty(QStringLiteral("color"))).value_or(config()->body.backgroundColor);
  const QPen outlinePen = isSelected() ? QPen(Config::HIGHLIGHT, 4 / baseScale()) : QPen(Config::FOREGROUND, 1.0 / baseScale());

  NodeBase::paintNode(nodeRect(), background, outlinePen, painter);

  if (isSelected() && !config()->body.nodeSvg.isEmpty())
    paintSelectionOutline(painter, nodeRect());
}

void NodeItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  paintDefaultNode(painter, style, widget);
}

QPainterPath NodeItem::shape() const
{
  return NodeBase::nodeShape(nodeRect());
}

QVector<PropertyConfig> NodeItem::configurationProperties() const
{
  return config()->properties;
}

QMap<QString, QVariant> NodeItem::properties() const
{
  return mStorage->getproperties();
}

QVector<std::shared_ptr<IProperty>> NodeItem::fields() const
{
  return mStorage->getfields();
}

QVector<std::shared_ptr<IFlow>> NodeItem::events() const
{
  return mStorage->getflows();
}

QVector<ControlsConfig> NodeItem::controls() const
{
  return config()->controls;
}

QVariant NodeItem::getProperty(const QString& key) const
{
  if (!mStorage)
    return QVariant();

  return mStorage->getProperty(key);
}

void NodeItem::setProperty(const QString& key, QVariant value)
{
  LOG_DEBUG("[%s] Setting property %s of node: %s", qPrintable(id()), qPrintable(key), qPrintable(nodeId()));
  if (!mStorage)
    return;

  mStorage->addProperty(key, value);

  if (key == "name")
    setLabelName(value.toString());

  if (nodeModified)
    nodeModified(this);

  update();
}

void NodeItem::setEvent(int index, const FlowConfig& event)
{
  Q_UNUSED(index);
  Q_UNUSED(event);
}

VoidResult NodeItem::setField(const QString& key, const QJsonObject& property)
{
  if (!mStorage)
    return VoidResult::Failed("Storage is not set");

  mStorage->setField(key, std::dynamic_pointer_cast<IProperty>(std::make_shared<PropertyInfo>(property)));

  return VoidResult();
}

VoidResult NodeItem::setField(const QString& key, std::shared_ptr<PropertyInfo> property)
{
  if (!mStorage)
    return VoidResult::Failed("Storage is not set");

  mStorage->setField(key, std::dynamic_pointer_cast<IProperty>(property));

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

QVector<NodeItem*> NodeItem::children() const
{
  return mChildrenNodes;
}

void NodeItem::addParent(NodeItem* parent)
{
  if (!parent)
    return;

  mParentNode = parent;
  mStorage->setParentId(parent->id());
  setZValue(parent->zValue() + 2);
  if (!parent->isSubflowContainer())
    fitInsideParent(20);
}

void NodeItem::addChild(NodeItem* node, std::shared_ptr<NodeSaveInfo> info)
{
  if (!info)
    return;

  mStorage->addChild(info);
  mChildrenNodes.push_back(node);
}

void NodeItem::childRemoved(NodeItem* child)
{
  mStorage->removeChild(child->id());
  mChildrenNodes.removeAll(child);
}

void NodeItem::swapCapabilityOrder(NodeItem* a, NodeItem* b)
{
  Q_UNUSED(a);
  Q_UNUSED(b);
}

NodeItem* NodeItem::capabilityAtScenePos(const QPointF& scenePos, NodeItem* exclude) const
{
  Q_UNUSED(scenePos);
  Q_UNUSED(exclude);
  return nullptr;
}

bool NodeItem::placeholderSlotContainsScenePoint(const QPointF& scenePos) const
{
  Q_UNUSED(scenePos);
  return false;
}

void NodeItem::ensureSubtaskConnector(StructureCanvas* canvas)
{
  Q_UNUSED(canvas);
}

void NodeItem::destroySubtaskConnector() {}

void NodeItem::syncSubtaskConnector() {}

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

  QRectF r = parentNode()->mapRectToScene(parentNode()->nodeRect());
  return r.adjusted(padding, padding, -padding, -padding);
}

void NodeItem::applySize(const QSizeF& size)
{
  if (size == mSize)
    return;

  prepareGeometryChange();
  mSize = size;
  mStorage->setSize(mSize);
  mStorage->setScale(qMax(config()->body.width / mSize.width(), config()->body.height / mSize.height()));

  const qreal newFontSize = qMax(Fonts::BaseSize, mSize.width() / Fonts::BaseFactor);
  setLabelSize(newFontSize, mSize);
  updatePortPositions();
  update();
}

void NodeItem::setHoverPreview(const QString& iconPath, const QColor& color, bool active)
{
  Q_UNUSED(iconPath);
  Q_UNUSED(color);
  Q_UNUSED(active);
}

QPointF NodeItem::clampPosInside(const QRectF& inner, const QRectF& childSceneRect) const
{
  QPointF posScene = pos();
  QPointF offset = childSceneRect.topLeft() - posScene;

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

  QRectF childSceneRect = mapRectToScene(boundingRect());
  QPointF currentTopLeft = childSceneRect.topLeft();

  const qreal maxWidth = inner.right() - currentTopLeft.x();
  const qreal maxHeight = inner.bottom() - currentTopLeft.y();

  const qreal clampedW = qMin(width, qMax<qreal>(0.0, maxWidth));
  const qreal clampedH = qMin(height, qMax<qreal>(0.0, maxHeight));

  return QSizeF(clampedW, clampedH);
}

void NodeItem::fitInsideParent(qreal padding)
{
  QRectF inner = parentInnerSceneRect(padding);
  if (!inner.isValid())
    return;

  QSizeF currentSize = mSize;
  qreal maxW = inner.width();
  qreal maxH = inner.height();

  maxW = qMax(maxW, Config::MINIMUM_NODE_SIZE);
  maxH = qMax(maxH, Config::MINIMUM_NODE_SIZE);

  const qreal newW = qMin(currentSize.width(), maxW);
  const qreal newH = qMin(currentSize.height(), maxH);

  if (newW != currentSize.width() || newH != currentSize.height())
    applySize(QSizeF(newW, newH));

  QRectF childSceneRect = mapRectToScene(boundingRect());
  QPointF newPos = clampPosInside(inner, childSceneRect);
  updatePosition(newPos);
}

void NodeItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsItem::mouseMoveEvent(event);
}

void NodeItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  mDragStartPos = pos();
  QGraphicsItem::mousePressEvent(event);
}

void NodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  if (pos() != mDragStartPos)
  {
    if (auto* canvas = dynamic_cast<Canvas*>(scene()))
      canvas->undoStack()->push(new MoveNodeCommand(canvas, id(), mDragStartPos, pos()));
  }

  QGraphicsItem::mouseReleaseEvent(event);
}

void NodeItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  if (nodeHovered)
    nodeHovered(this, true);
  QGraphicsItem::hoverEnterEvent(event);
}

void NodeItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  if (nodeHovered)
    nodeHovered(this, false);
  QGraphicsItem::hoverLeaveEvent(event);
}

QVariant NodeItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemPositionChange)
  {
    QPointF newPos = value.toPointF();

    if (NodeItem* parent = parentNode())
    {
      QRectF parentRect = parent->nodeRect();
      parentRect = parentRect.adjusted(10, 10, -10, -10);
      parentRect.translate(parent->pos());

      if (parent->isSubflowContainer())
      {
        // The block only grows to the right and downwards, so children are kept
        // out of the left and top lanes but may move freely right/down.
        newPos.setX(std::max(newPos.x(), parentRect.left()));
        newPos.setY(std::max(newPos.y(), parentRect.top()));
        return newPos;
      }

      QRectF childLocalRect = nodeRect();

      const qreal minX = parentRect.left();
      const qreal maxX = parentRect.right() - childLocalRect.width();
      const qreal minY = parentRect.top();
      const qreal maxY = parentRect.bottom() - childLocalRect.height();

      newPos.setX(std::clamp(newPos.x(), minX, maxX));
      newPos.setY(std::clamp(newPos.y(), minY, maxY));

      return newPos;
    }

    if (!isSubflowContainer())
    {
      if (auto* canvas = dynamic_cast<Canvas*>(scene()))
      {
        if (canvas->type() == Types::LibraryTypes::BEHAVIOUR)
        {
          const QRectF localRect = nodeRect();
          for (QGraphicsItem* item : scene()->items())
          {
            if (!item || item->type() != NodeItem::Type || item == this)
              continue;

            auto* other = static_cast<NodeItem*>(item);
            if (!other->isSubflowContainer())
              continue;

            // Skip the mover's own attached blocks (Repeat/Within host).
            if (other->subflowHost() == this)
              continue;

            // Use the block body only — sceneBoundingRect also covers the
            // dashed connector gap, which must remain free for other nodes.
            newPos = pushOutOfRect(localRect, newPos, other->mapRectToScene(other->nodeRect()));
          }
        }
      }
    }

    return newPos;
  }
  else if (change == QGraphicsItem::ItemPositionHasChanged)
  {
    if (!mInUpdatePosition)
      updatePosition(value.toPointF());
  }

  return QGraphicsItem::itemChange(change, value);
}

void NodeItem::updatePosition(const QPointF& newPosition)
{
  if (newPosition == mLastPosition)
    return;

  const QPointF delta = newPosition - mLastPosition;

  mInUpdatePosition = true;
  prepareGeometryChange();
  setPos(newPosition);
  mInUpdatePosition = false;

  if (!mSuppressChildCascade)
  {
    for (auto* child : children())
    {
      auto* childNode = static_cast<NodeItem*>(child);
      childNode->updatePosition(childNode->pos() + delta);
    }
  }

  mLastPosition = newPosition;

  updateExtrasPosition();
  mStorage->setPosition(pos() + boundingRect().center());
}

void NodeItem::updateExtrasPosition()
{
  updatePortPositions();

  if (nodeMoved)
    nodeMoved(id());
}

void NodeItem::updatePortPositions()
{
  const QRectF portRect = nodeShapeContentRect(nodeRect());
  const qreal left = portRect.left();
  const qreal top = portRect.top();
  const qreal w = portRect.width();
  const qreal h = portRect.height();
  if (mInPort)
    mInPort->setPos(left - PortItem::kSize - PortItem::kGap, top + (h - PortItem::kSize) / 2.0);
  if (mOutPort)
    mOutPort->setPos(left + w + PortItem::kGap, top + (h - PortItem::kSize) / 2.0);

  const bool showAbort = mAbortPort && mAbortPort->isVisible();
  const bool showError = mErrorPort && mErrorPort->isVisible();
  if (!showAbort && !showError)
    return;

  const qreal topPortSize = PortItem::sizeForKind(PortItem::Abort);
  const qreal centerX = left + w * 0.5;
  const qreal shiftX = PortItem::kAbortErrorPortPositioning;
  // Abort and error both sit above the node, side by side.
  const qreal topY = top - topPortSize - PortItem::kGap;
  const qreal errorX = centerX + topPortSize * 0.5 + PortItem::kGap * 2.0 + shiftX;
  const qreal abortX = centerX - topPortSize - PortItem::kGap * 2.0 + shiftX;

  if (showAbort && showError)
  {
    mAbortPort->setPos(abortX, topY);
    mErrorPort->setPos(errorX, topY);
  }
  else if (showAbort)
  {
    mAbortPort->setPos(centerX - topPortSize * 0.5 + shiftX, topY);
  }
  else if (showError)
  {
    mErrorPort->setPos(centerX - PortItem::sizeForKind(PortItem::Error) * 0.5 + shiftX, topY);
  }
}

void NodeItem::onProperties() {}

NodeSaveInfo NodeItem::saveInfo() const
{
  return *mStorage;
}

void NodeItem::applyStructuralLayoutTopLeft(const QPointF& topLeftScene)
{
  Q_UNUSED(topLeftScene);
}

void NodeItem::finalizeStructuralPackedPositions(const QPointF& subtreeSceneTopLeft)
{
  Q_UNUSED(subtreeSceneTopLeft);
}

PortItem::Kind NodeItem::outgoingPortKindForEvent(const QString& event) const
{
  const QString e = event.trimmed();
  if (e.compare(QStringLiteral("on abort"), Qt::CaseInsensitive) == 0 && mAbortPort && mAbortPort->isVisible())
    return PortItem::Abort;
  if (e.compare(QStringLiteral("on error"), Qt::CaseInsensitive) == 0 && mErrorPort && mErrorPort->isVisible())
    return PortItem::Error;
  return PortItem::Out;
}

QPointF NodeItem::outgoingPortAnchorForEvent(const QString& event) const
{
  const PortItem::Kind kind = outgoingPortKindForEvent(event);
  if (kind == PortItem::Abort && mAbortPort && mAbortPort->isVisible())
    return mAbortPort->anchorScenePos();
  if (kind == PortItem::Error && mErrorPort && mErrorPort->isVisible())
    return mErrorPort->anchorScenePos();
  if (mOutPort)
    return mOutPort->anchorScenePos();
  return sceneBoundingRect().center();
}

QPointF NodeItem::incomingPortAnchor() const
{
  if (mInPort)
    return mInPort->anchorScenePos();
  return edgePointToward(sceneBoundingRect().center(), false);
}

QPointF NodeItem::edgePointToward(const QPointF& targetScenePos, bool fromOutgoingPort) const
{
  if (fromOutgoingPort && mOutPort)
    return mOutPort->anchorScenePos();
  if (!fromOutgoingPort && mInPort)
    return mInPort->anchorScenePos();

  QPointF center = sceneBoundingRect().center();
  QPointF dir = targetScenePos - center;

  if (dir.manhattanLength() < 0.001)
    return center;

  dir /= std::hypot(dir.x(), dir.y());
  const qreal radius = nodeRect().width() / 2.0;
  return center + dir * radius;
}

QVector<TransitionConfig> NodeItem::configTransitions() const
{
  return config()->transitions;
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
    for (const auto& f : mStorage->getflows())
    {
      if (f->getid() != info->getid())
        continue;

      flowConfig = std::dynamic_pointer_cast<FlowSaveInfo>(f);
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
    flowConfig->setOwner(id());
    mStorage->addFlow(flowConfig);
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
  mStorage->removeFlow(flowId);
  mFlows.removeIf([flowId](Flow* flow) { return flow->id() == flowId; });
}

void NodeItem::updateFlow() {}

void NodeItem::renameNode(const QString& name)
{
  setProperty("name", name);
}
