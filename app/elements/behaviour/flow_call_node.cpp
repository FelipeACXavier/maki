#include "elements/behaviour/flow_call_node.h"

#include <QFontMetricsF>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QJsonArray>
#include <QJsonObject>
#include <QObject>
#include <QPainter>
#include <QSvgRenderer>

#include "mission_parameter.h"

FlowCallNode::FlowCallNode(const QString& id, std::shared_ptr<NodeSaveInfo> info, const QPointF& initialPosition, std::shared_ptr<NodeConfig> nodeConfig,
                           QGraphicsItem* parent)
    : BehaviourNode(id, info, initialPosition, nodeConfig, parent)
{
  setAcceptHoverEvents(true);
}

std::pair<QString, QString> FlowCallNode::calledFlowId() const
{
  const auto propValue = getProperty("task");
  if (!propValue || !propValue->getvalue())
    return std::make_pair(QString(), QString());

  auto prop = maki::asValue(propValue->getvalue());
  if (!prop->isRecord())
    return std::make_pair(QString(), QString());

  auto name = maki::recordString(prop->toRecord(), "componentId");
  auto id = maki::recordString(prop->toRecord(), "callId");
  LOG_DEBUG("calledFlowName: {} {}", name, id);
  return std::make_pair(name, id);
}

void FlowCallNode::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
  // Show information about the called flow
  NodeItem::hoverMoveEvent(event);
}

void FlowCallNode::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  NodeItem::hoverLeaveEvent(event);
}

void FlowCallNode::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->button() != Qt::LeftButton)
  {
    NodeItem::mouseDoubleClickEvent(event);
    return;
  }

  const auto [componentId, flowId] = calledFlowId();
  if (componentId.isEmpty() || flowId.isEmpty())
  {
    NodeItem::mouseDoubleClickEvent(event);
    return;
  }

  if (focusOn)
    focusOn(this, componentId, flowId, Types::FLOW);

  event->accept();
  return;
}