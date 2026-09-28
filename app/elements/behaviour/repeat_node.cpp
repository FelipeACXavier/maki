#include "repeat_node.h"

#include <QPainter>

#include "elements/behaviour/sub_flow.h"

static const qreal SUB_FLOW_SPACING = 30.0;
static const qreal SUB_FLOW_WIDTH = 400.0;
static const qreal SUB_FLOW_HEIGHT = 220.0;
static const qreal SUB_FLOW_TOP_LEFT_PADDING = 5.0;
static const int ICON_WIDTH = 50;

static const QString ITERATIONS = "iterations";
static const QString RATE = "rate";

RepeatNode::RepeatNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
                       QGraphicsItem* parent)
    : BehaviourNode(id, info, initialPosition, nodeConfig, parent)
{
  setAcceptDrops(true);
  mSubFlow = new SubFlow(Constants::MAIN_SUB_FLOW, this);
  mSubFlow->setCollapsed(true);
  mSubFlow->setTitle("Repeat");
  mSubFlow->setTitlePosition(Config::ControlPosition::Top, ICON_WIDTH + SUB_FLOW_TOP_LEFT_PADDING, ICON_WIDTH / 2 + SUB_FLOW_TOP_LEFT_PADDING);

  setProperty(ITERATIONS, maki::Value::createInt(0));
  setProperty(RATE, maki::Value::createInt(0));

  toggleCollapsed(mSubFlow, false);
}

SubFlow* RepeatNode::subFlow() const
{
  return mSubFlow;
}

void RepeatNode::createPorts()
{
  for (const auto& port : config()->ports)
    mPorts[{Constants::MAIN_SUB_FLOW, port.type}] = new PortItem(port.type, Constants::MAIN_SUB_FLOW, true, this);

  layoutSubFlow();
}

QRectF RepeatNode::extraBoundingRect() const
{
  const auto flow = subFlow();
  if (!flow || flow->isCollapsed())
    return NodeBase::extraBoundingRect();

  return mapRectFromItem(flow, flow->boundingRect());
}

QRectF RepeatNode::sceneAlignRect() const
{
  const auto flow = subFlow();
  if (!flow || flow->isCollapsed())
    return NodeItem::sceneAlignRect();

  return flow->sceneBoundingRect();
}

QRectF RepeatNode::nodeRect() const
{
  const auto flow = subFlow();
  if (!flow || flow->isCollapsed())
    return NodeItem::nodeRect();

  return QRectF(QPointF(0, 0), QSizeF{ICON_WIDTH, ICON_WIDTH});
}

QPainterPath RepeatNode::shape() const
{
  return NodeBase::nodeShape(nodeRect());
}

void RepeatNode::updateExtrasPosition(Config::NodeMove reason)
{
  BehaviourNode::updateExtrasPosition(reason);
  layoutSubFlow();
}

void RepeatNode::childPositionUpdated(NodeItem* child)
{
  if (!child)
    return;

  layoutSubFlow();
}

bool RepeatNode::constrainChildren() const
{
  return false;
}

QPointF RepeatNode::constrainChildPosition(const NodeItem* child, const QPointF& proposedPosition) const
{
  if (!child || !subFlow())
    return proposedPosition;

  constexpr qreal markerSpace = 60.0;
  const QRectF allowed = subFlow()->sceneBoundingRect().adjusted(markerSpace, SUB_FLOW_SPACING, -markerSpace, -SUB_FLOW_SPACING);

  const QRectF childSceneRect = child->sceneBoundingRect();
  const QPointF offset = childSceneRect.topLeft() - child->pos();

  QPointF result = proposedPosition;

  // Left/top fixed; right/bottom remain free so SubFlow can expand.
  result.setX(qMax(result.x(), allowed.left() - offset.x()));
  result.setY(qMax(result.y(), allowed.top() - offset.y()));

  return result;
}

QRectF RepeatNode::childAreaSceneRect(const QString& subflow) const
{
  if (auto* flow = subFlow())
    return flow->sceneBoundingRect();

  return BehaviourNode::childAreaSceneRect(subflow);
}

void RepeatNode::addChild(NodeItem* node, std::shared_ptr<NodeSaveInfo> info)
{
  NodeItem::addChild(node, info);
  layoutSubFlow();
}

void RepeatNode::childRemoved(NodeItem* child)
{
  NodeItem::childRemoved(child);
  layoutSubFlow();
}

void RepeatNode::setProperty(const QString& key, const maki::Value& value)
{
  NodeItem::setProperty(key, value);
  if (key == ITERATIONS || key == RATE)
  {
    const auto iterProp = getProperty(ITERATIONS);
    const auto rateProp = getProperty(RATE);
    if (const auto iter = maki::asValue(iterProp->getvalue()); const auto* rate = maki::asValue(rateProp->getvalue()))
    {
      const auto iString = iter->toIntValue() == 0 ? "forever " : iter->toStringValue() + " times ";
      const auto rString = rate->toIntValue() == 0 ? "" : "every " + rate->toStringValue() + " ms";
      mSubFlow->setTitle("Repeat " + iString + rString);
    }
    else
      mSubFlow->setTitle("Repeat");
  }
}

void RepeatNode::layoutSubFlow()
{
  auto* flow = subFlow();
  if (!flow)
    return;

  if (flow->isCollapsed())
  {
    layoutSubFlowMarkers();
    return;
  }

  const QRectF repeatRect = sceneNodeRect();
  const QRectF previousRect = sceneAlignRect();

  // Keep the Repeat icon inside the top-left of the subflow.
  const QPointF topLeft(repeatRect.left() - SUB_FLOW_TOP_LEFT_PADDING, repeatRect.top() - SUB_FLOW_TOP_LEFT_PADDING);

  QRectF sceneRect(topLeft, QSizeF(SUB_FLOW_WIDTH, SUB_FLOW_HEIGHT));

  QRectF childrenRect;
  bool first = true;

  for (auto* child : children())
  {
    if (!child)
      continue;

    const QRectF childRect = child->sceneBoundingRect();
    if (first)
    {
      childrenRect = childRect;
      first = false;
    }
    else
    {
      childrenRect = childrenRect.united(childRect);
    }
  }

  if (!first)
  {
    childrenRect.adjust(-SUB_FLOW_SPACING, -SUB_FLOW_SPACING, SUB_FLOW_SPACING, SUB_FLOW_SPACING);

    // Keep top-left fixed and grow only right/down.
    sceneRect.setRight(qMax(sceneRect.right(), childrenRect.right()));
    sceneRect.setBottom(qMax(sceneRect.bottom(), childrenRect.bottom()));
  }

  const QRectF localRect = mapRectFromScene(sceneRect);

  const bool changed = previousRect != sceneRect;
  if (changed)
  {
    prepareGeometryChange();
    flow->setRect(localRect);
  }

  layoutSubFlowMarkers();

  if (geometryChanged && changed)
    geometryChanged(this, previousRect);
}

void RepeatNode::layoutSubFlowMarkers()
{
  auto* flow = subFlow();
  if (!flow)
    return;

  if (flow->isCollapsed())
  {
    for (auto* port : mPorts)
      if (port)
        port->clearAnchorOverride();

    return;
  }

  const QRectF rect = flow->boundingRect();
  for (auto* port : mPorts)
  {
    if (!port)
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

    port->setAnchorOverride(flow->mapToScene(localAnchor));
  }
}

void RepeatNode::toggleCollapsed(SubFlow* flow, bool collapsed)
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
    if (child)
      child->setVisible(!collapsed);

  layoutSubFlow();

  if (subflowCollapsed)
    subflowCollapsed(this, flow->id(), collapsed);

  // The layout does not trigger when toggling since the rect never really changes
  // So we should trigger it here
  if (geometryChanged)
    geometryChanged(this, previousRect);
}

void RepeatNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->button() != Qt::LeftButton)
  {
    NodeItem::mouseDoubleClickEvent(event);
    return;
  }

  auto flow = subFlow();
  if (!flow)
  {
    NodeItem::mouseDoubleClickEvent(event);
    return;
  }

  toggleCollapsed(flow, !flow->isCollapsed());

  event->accept();
  return;
}

QVariant RepeatNode::itemChange(GraphicsItemChange change, const QVariant& value)
{
  if (change == QGraphicsItem::ItemSelectedHasChanged)
  {
    if (auto* flow = subFlow(); !flow->isCollapsed())
      flow->setHighlight(value.toBool());
  }

  return BehaviourNode::itemChange(change, value);
}

void RepeatNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  if (!subFlow() || subFlow()->isCollapsed())
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

  // Keep the icon exactly where the actual RepeatNode is.
  paintNodeBody(nodeRect(), painter);

  const QRectF flowRect = mapRectFromItem(subFlow(), subFlow()->boundingRect());
  const auto rect = QRectF(flowRect.left() - 8, flowRect.bottom() + 2, flowRect.width() + 2 * 8, 40);
  paintLabel(painter, rect, pen);
}
