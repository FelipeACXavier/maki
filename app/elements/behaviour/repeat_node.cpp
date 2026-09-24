#include "repeat_node.h"

#include <QPainter>

#include "elements/behaviour/sub_flow.h"

static const qreal SUB_FLOW_SPACING = 30.0;
static const qreal SUB_FLOW_WIDTH = 400.0;
static const qreal SUB_FLOW_HEIGHT = 220.0;

RepeatNode::RepeatNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
                       QGraphicsItem* parent)
    : BehaviourNode(id, info, initialPosition, nodeConfig, parent)
{
  setAcceptDrops(true);
  mSubFlow = new SubFlow(this);
  toggleCollapsed(mSubFlow, false);
}

SubFlow* RepeatNode::subFlow() const
{
  return mSubFlow;
}

void RepeatNode::updatePosition(const QPointF& position)
{
  BehaviourNode::updatePosition(position);
  layoutSubFlow();
}

void RepeatNode::childPositionUpdated()
{
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

QRectF RepeatNode::childAreaSceneRect() const
{
  if (auto* flow = subFlow())
    return flow->sceneBoundingRect();

  return BehaviourNode::childAreaSceneRect();
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

  // This is always the minimum/default location of the sub-flow.
  const QSizeF minimumSize{SUB_FLOW_WIDTH, SUB_FLOW_HEIGHT};
  const QRectF repeatRect = sceneNodeRect();
  QRectF sceneRect(QPointF(repeatRect.center().x() - minimumSize.width() / 2.0, repeatRect.center().y()), minimumSize);
  QRectF childrenRect;
  bool first = true;
  for (auto* child : children())
  {
    if (!child)
      continue;

    if (first)
    {
      childrenRect = child->sceneBoundingRect();
      first = false;
    }
    else
    {
      childrenRect = childrenRect.united(child->sceneBoundingRect());
    }
  }

  if (!first)
  {
    childrenRect.adjust(-SUB_FLOW_SPACING, -SUB_FLOW_SPACING, SUB_FLOW_SPACING, SUB_FLOW_SPACING);
    sceneRect = sceneRect.united(childrenRect);
  }

  flow->setRect(mapRectFromScene(sceneRect));
  layoutSubFlowMarkers();
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

  const QRectF rect = flow->rect();
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

  flow->setCollapsed(collapsed);

  for (auto* port : mPorts)
    if (port)
      port->setMarker(!collapsed);

  // If collapsed, then they should not be visible
  for (auto& child : children())
    if (child)
      child->setVisible(!collapsed);

  layoutSubFlow();

  if (nodeMoved)
    nodeMoved(this, true);
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

  const QRectF flowRect = mapRectFromItem(subFlow(), subFlow()->rect());
  const auto rect = QRectF(flowRect.left() - 8, flowRect.bottom() + 2, flowRect.width() + 2 * 8, 40);
  paintLabel(painter, rect, pen);
}
