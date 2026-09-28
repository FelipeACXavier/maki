#include "within_node.h"

#include <QPainter>

#include "elements/behaviour/sub_flow.h"

static const qreal SUB_FLOW_SPACING = 30.0;
static const qreal SUB_FLOW_WIDTH = 400.0;
static const qreal SUB_FLOW_HEIGHT = 220.0;
static const int ICON_WIDTH = 50;
static const int PADDING = 8;

static const QString DO_FLOW_ID = "do";
static const QString ELSE_FLOW_ID = "else";
static const QString TIMEOUT = "timeout";

WithinNode::WithinNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
                       QGraphicsItem* parent)
    : BehaviourNode(id, info, initialPosition, nodeConfig, parent)
{
  setAcceptDrops(true);
  mDoFlow = new SubFlow(DO_FLOW_ID, this);
  mElseFlow = new SubFlow(ELSE_FLOW_ID, this);
  mDoFlow->setCollapsed(true);
  mElseFlow->setCollapsed(true);
  mDoFlow->setTitle("Try to do");
  mElseFlow->setTitle("On timeout");
  mDoFlow->setTitlePosition(Config::ControlPosition::Top, PADDING, PADDING);
  mElseFlow->setTitlePosition(Config::ControlPosition::Top, PADDING, PADDING);

  setProperty(TIMEOUT, maki::Value::createInt(0));
  toggleCollapsed(mDoFlow, false);
  toggleCollapsed(mElseFlow, false);
}

SubFlow* WithinNode::subFlow(const QString& subflow) const
{
  if (subflow == DO_FLOW_ID)
    return mDoFlow;
  else if (subflow == ELSE_FLOW_ID)
    return mElseFlow;

  return nullptr;
}

QList<SubFlow*> WithinNode::subFlows() const
{
  return {mDoFlow, mElseFlow};
}

void WithinNode::createPorts()
{
  // Add the placeholder ports for the sub-flows
  mPorts[{DO_FLOW_ID, Types::Port::IN}] = new PortItem(Types::Port::IN, DO_FLOW_ID, true, this);
  mPorts[{ELSE_FLOW_ID, Types::Port::IN}] = new PortItem(Types::Port::IN, ELSE_FLOW_ID, true, this);

  // Create the normal ports
  for (const auto& port : config()->ports)
    mPorts[{Constants::MAIN_SUB_FLOW, port.type}] = new PortItem(port.type, Constants::MAIN_SUB_FLOW, true, this);

  layoutSubFlows();
}

bool WithinNode::hasExpandedSubFlow() const
{
  return (mDoFlow && !mDoFlow->isCollapsed()) || (mElseFlow && !mElseFlow->isCollapsed());
}

QRectF WithinNode::expandedSubFlowsRect() const
{
  QRectF result;
  bool first = true;

  for (const auto* flow : subFlows())
  {
    if (!flow || flow->isCollapsed())
      continue;

    const QRectF rect = mapRectFromItem(flow, flow->boundingRect());

    if (first)
    {
      result = rect;
      first = false;
    }
    else
    {
      result = result.united(rect);
    }
  }

  return result;
}

QRectF WithinNode::expandedSubFlowsSceneRect() const
{
  QRectF result;
  bool first = true;

  for (const auto* flow : subFlows())
  {
    if (!flow || flow->isCollapsed())
      continue;

    const QRectF rect = flow->sceneBoundingRect();
    if (first)
    {
      result = rect;
      first = false;
    }
    else
    {
      result = result.united(rect);
    }
  }

  return result;
}

QRectF WithinNode::extraBoundingRect() const
{
  if (!hasExpandedSubFlow())
    return NodeBase::extraBoundingRect();

  QRectF result = NodeBase::extraBoundingRect();
  const QRectF flowsRect = expandedSubFlowsRect();
  if (result.isEmpty())
    return flowsRect;

  return result.united(flowsRect);
}

QRectF WithinNode::sceneAlignRect() const
{
  if (!hasExpandedSubFlow())
    return NodeItem::sceneAlignRect();

  QRectF rect = expandedSubFlowsSceneRect().united(sceneNodeRect());

  const qreal centerY = sceneNodeRect().center().y();

  const qreal extent = qMax(centerY - rect.top(), rect.bottom() - centerY);

  rect.setTop(centerY - extent);
  rect.setBottom(centerY + extent);

  return rect;
}

QRectF WithinNode::nodeRect() const
{
  if (!hasExpandedSubFlow())
    return NodeItem::nodeRect();

  return QRectF(QPointF(0.0, 0.0), QSizeF{ICON_WIDTH, ICON_WIDTH});
}

QPainterPath WithinNode::shape() const
{
  return NodeBase::nodeShape(nodeRect());
}

void WithinNode::updateExtrasPosition(Config::NodeMove reason)
{
  BehaviourNode::updateExtrasPosition(reason);
  if (reason == Config::NodeMove::User)
    layoutSubFlows();
}

void WithinNode::childPositionUpdated(NodeItem* child)
{
  if (!child)
    return;

  layoutSubFlows();
}

bool WithinNode::constrainChildren() const
{
  return false;
}

QPointF WithinNode::constrainChildPosition(const NodeItem* child, const QPointF& proposedPosition) const
{
  if (!child)
    return proposedPosition;

  auto* flow = subFlow(child->parentSubFlow());
  if (!flow)
    return proposedPosition;

  constexpr qreal markerSpace = 60.0;
  const QRectF flowRect = flow->sceneBoundingRect();
  const QRectF childRect = child->sceneBoundingRect();

  // Difference between the item's position and the top-left of its actual
  // visual footprint.
  const QPointF offset = childRect.topLeft() - child->pos();

  QPointF result = proposedPosition;

  // --------------------------------------------------------------------------
  // Horizontal:
  //
  // Both subflows have a fixed left edge and are free to grow to the right.
  // --------------------------------------------------------------------------
  const qreal minimumLeft = flowRect.left() + markerSpace;
  result.setX(qMax(result.x(), minimumLeft - offset.x()));

  // --------------------------------------------------------------------------
  // Vertical:
  //
  // DO:
  //   - bottom is fixed at the centre line
  //   - may grow upward
  //
  // TIMEOUT:
  //   - top is fixed at the centre line
  //   - may grow downward
  // --------------------------------------------------------------------------
  if (child->parentSubFlow() == DO_FLOW_ID)
  {
    const qreal maximumBottom = flowRect.bottom() - SUB_FLOW_SPACING;

    // proposed scene top + child height <= maximumBottom
    const qreal maximumY = maximumBottom - childRect.height() - offset.y();
    result.setY(qMin(result.y(), maximumY));
  }
  else if (child->parentSubFlow() == ELSE_FLOW_ID)
  {
    const qreal minimumTop = flowRect.top() + SUB_FLOW_SPACING;
    result.setY(qMax(result.y(), minimumTop - offset.y()));
  }

  return result;
}

QRectF WithinNode::childAreaSceneRect(const QString& subflow) const
{
  if (auto* flow = subFlow(subflow))
    return flow->sceneBoundingRect();

  return BehaviourNode::childAreaSceneRect(subflow);
}

QRectF WithinNode::childrenSceneRect(const QString& subFlowId) const
{
  QRectF result;
  bool first = true;

  for (auto* child : children())
  {
    if (!child)
      continue;

    if (child->parentSubFlow() != subFlowId)
      continue;

    const QRectF rect = child->sceneBoundingRect();
    if (first)
    {
      result = rect;
      first = false;
    }
    else
    {
      result = result.united(rect);
    }
  }

  return result;
}

void WithinNode::addChild(NodeItem* node, std::shared_ptr<NodeSaveInfo> info)
{
  NodeItem::addChild(node, info);
  if (node)
    layoutSubFlows();
}

void WithinNode::childRemoved(NodeItem* child)
{
  NodeItem::childRemoved(child);
  if (child)
    layoutSubFlows();
}

void WithinNode::setProperty(const QString& key, const maki::Value& value)
{
  NodeItem::setProperty(key, value);
  if (key == TIMEOUT)
  {
    const auto iterProp = getProperty(TIMEOUT);
    if (const auto iter = maki::asValue(iterProp->getvalue()))
      mDoFlow->setTitle("Try to do within " + iter->toStringValue() + " ms");
    else
      mDoFlow->setTitle("Try to do");
  }
}

void WithinNode::layoutSubFlowMarkers()
{
  const auto doFlow = subFlow(DO_FLOW_ID);
  const auto elseFlow = subFlow(ELSE_FLOW_ID);
  if (!doFlow || !elseFlow)
    return;

  if (doFlow->isCollapsed() && elseFlow->isCollapsed())
  {
    for (auto* port : mPorts)
      if (port)
        port->clearAnchorOverride();

    return;
  }

  const QRectF rect = boundingRect();
  for (auto* port : mPorts)
  {
    if (!port || port->subFlow() != Constants::MAIN_SUB_FLOW)
      continue;

    QPointF localAnchor;
    switch (port->kind())
    {
      case Types::Port::IN:
        localAnchor = {rect.left(), rect.center().y()};
        break;

      case Types::Port::OUT:
        localAnchor = {rect.right(), rect.top() + rect.height() / 2.0};
        break;

      case Types::Port::ERROR:
        localAnchor = {rect.right(), rect.top() + 3.0 * rect.height() / 4.0};
        break;

      case Types::Port::ABORT:
        localAnchor = {rect.right(), rect.top() + rect.height() / 4.0};
        break;

      default:
        continue;
    }
    port->setAnchorOverride(this->mapToScene(localAnchor));
  }
  const QRectF doRect = doFlow->boundingRect();
  const QRectF elseRect = elseFlow->boundingRect();
  auto doPort = getPort(Types::Port::IN, DO_FLOW_ID);
  auto elsePort = getPort(Types::Port::IN, ELSE_FLOW_ID);
  if (!doPort || !elsePort)
    return;

  doPort->setAnchorOverride(doFlow->mapToScene(QPointF{doRect.left(), doRect.center().y()}));
  elsePort->setAnchorOverride(elseFlow->mapToScene(QPointF{elseRect.left(), elseRect.center().y()}));
}

void WithinNode::toggleCollapsed(SubFlow* flow, bool collapsed)
{
  if (!flow)
    return;

  if (flow->isCollapsed() == collapsed)
    return;

  const auto previousRect = sceneAlignRect();

  prepareGeometryChange();
  flow->setCollapsed(collapsed);

  for (auto* port : mPorts)
    if (port)
      port->setMarker(!collapsed);

  // If collapsed, then they should not be visible
  for (auto& child : children())
    if (child && child->parentSubFlow() == flow->id())
      child->setVisible(!collapsed);

  if (subflowCollapsed)
    subflowCollapsed(this, flow->id(), collapsed);

  // The layout does not trigger when toggling since the rect never really changes
  // So we should trigger it here
  if (geometryChanged)
    geometryChanged(this, previousRect);
}

void WithinNode::layoutSubFlows()
{
  if (!mDoFlow || !mElseFlow)
    return;

  if (mDoFlow->isCollapsed() && mElseFlow->isCollapsed())
  {
    layoutSubFlowMarkers();
    return;
  }

  const QRectF previousRect = sceneAlignRect();

  // The icon is the fixed anchor.
  const QRectF iconRect = sceneNodeRect();
  const QRectF doChildren = childrenSceneRect(DO_FLOW_ID);
  const QRectF timeoutChildren = childrenSceneRect(ELSE_FLOW_ID);

  const qreal left = iconRect.center().x() - SUB_FLOW_WIDTH / 2.0;
  qreal right = iconRect.center().x() + SUB_FLOW_WIDTH / 2.0;

  if (!doChildren.isEmpty())
    right = qMax(right, doChildren.right() + SUB_FLOW_SPACING);

  if (!timeoutChildren.isEmpty())
    right = qMax(right, timeoutChildren.right() + SUB_FLOW_SPACING);

  const qreal separatorY = iconRect.center().y();
  qreal doTop = separatorY - SUB_FLOW_HEIGHT;

  if (!doChildren.isEmpty())
    doTop = qMin(doTop, doChildren.top() - SUB_FLOW_SPACING);

  qreal timeoutBottom = separatorY + SUB_FLOW_HEIGHT;

  if (!timeoutChildren.isEmpty())
    timeoutBottom = qMax(timeoutBottom, timeoutChildren.bottom() + SUB_FLOW_SPACING);

  QRectF doSceneRect(QPointF(left, doTop), QPointF(right, separatorY));
  QRectF timeoutSceneRect(QPointF(left, separatorY), QPointF(right, timeoutBottom));

  prepareGeometryChange();

  if (!mDoFlow->isCollapsed())
    mDoFlow->setRect(mapRectFromScene(doSceneRect));

  if (!mElseFlow->isCollapsed())
    mElseFlow->setRect(mapRectFromScene(timeoutSceneRect));

  layoutSubFlowMarkers();

  const QRectF currentRect = sceneAlignRect();

  if (geometryChanged && currentRect != previousRect)
    geometryChanged(this, previousRect);

  update();
}

void WithinNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->button() != Qt::LeftButton)
  {
    NodeItem::mouseDoubleClickEvent(event);
    return;
  }

  auto doFlow = subFlow(DO_FLOW_ID);
  auto elseFlow = subFlow(ELSE_FLOW_ID);
  if (!doFlow || !elseFlow)
  {
    NodeItem::mouseDoubleClickEvent(event);
    return;
  }

  toggleCollapsed(doFlow, !doFlow->isCollapsed());
  toggleCollapsed(elseFlow, !elseFlow->isCollapsed());
  layoutSubFlows();

  event->accept();
  return;
}

QVariant WithinNode::itemChange(GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemSelectedHasChanged)
  {
    const bool selected = value.toBool();
    for (auto* flow : subFlows())
    {
      if (!flow || flow->isCollapsed())
        continue;

      flow->setHighlight(selected);
    }
  }

  return BehaviourNode::itemChange(change, value);
}

void WithinNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  if (!hasExpandedSubFlow())
  {
    BehaviourNode::paint(painter, style, widget);
    return;
  }

  Q_UNUSED(style);
  Q_UNUSED(widget);

  auto color = getProperty("color");
  const QColor background = color ? QColor::fromString(color->getvalue()->toStringValue()) : config()->body.backgroundColor;
  const QPen pen = isSelected() ? QPen(Config::HIGHLIGHT, 3.0 / baseScale()) : QPen(Config::FOREGROUND, 1.0 / baseScale());

  painter->setPen(pen);
  painter->setBrush(background);
  painter->setRenderHint(QPainter::Antialiasing, false);

  paintNodeBody(nodeRect(), painter);

  // const QRectF flowsRect = expandedSubFlowsRect();
  // const QRectF labelRect(flowsRect.left() - 8.0, flowsRect.bottom() + 2.0, flowsRect.width() + 16.0, 40.0);
  // paintLabel(painter, labelRect, pen);
}
