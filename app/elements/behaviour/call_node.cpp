#include "elements/behaviour/call_node.h"

#include <QPainter>

#include "elements/behaviour/call_capability.h"
#include "elements/behaviour/component_overlay.h"
#include "save_info.h"

CallNode::CallNode(const QString& id,
                   std::shared_ptr<NodeSaveInfo> info,
                   const QPointF& initialPosition,
                   std::shared_ptr<NodeConfig> nodeConfig,
                   QGraphicsItem* parent)
    // Clone config so Sync/Async/unset SVG swaps do not mutate the shared library entry.
    : BehaviourNode(id, info, initialPosition, std::make_shared<NodeConfig>(*nodeConfig), parent)
{
  syncNodeSvgFromState();
}

void CallNode::setProperty(const QString& key, QVariant value)
{
  BehaviourNode::setProperty(key, value);
  if (key == call_capability::kModeProperty || key == call_capability::kCapabilityProperty)
    syncNodeSvgFromState();
}

void CallNode::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  applyNodeSvg(call_capability::nodeSvgForState(*this));
  BehaviourNode::paint(painter, style, widget);
}

bool CallNode::hasCapabilitySelected() const
{
  return call_capability::hasCapabilitySelected(*this);
}

qreal CallNode::capabilitySlotDiameter() const
{
  const QRectF drawingBounds = drawingRect(nodeRect());
  return qMin(drawingBounds.width(), drawingBounds.height()) * config()->body.iconScale
         * behaviour::kComponentOverlayDiameterFactor;
}

QRectF CallNode::capabilitySlotSceneRect() const
{
  const QRectF drawingBounds = drawingRect(nodeRect());
  const qreal diameter = capabilitySlotDiameter();
  const qreal radius = diameter * 0.5;
  const QPointF localCenter =
      behaviour::callCapabilityIconCenter(drawingBounds, diameter, hasCapabilitySelected());
  const QPointF center = mapToScene(localCenter);
  return QRectF(center.x() - radius, center.y() - radius, diameter, diameter);
}

bool CallNode::capabilitySlotContainsScenePoint(const QPointF& scenePos) const
{
  const QRectF r = capabilitySlotSceneRect();
  if (r.isEmpty())
    return false;
  const QPointF d = scenePos - r.center();
  const qreal rr = r.width() * 0.5;
  return (d.x() * d.x() + d.y() * d.y()) <= rr * rr;
}

void CallNode::assignCapability(const QString& capabilityName, const QString& capabilityId, SaveInfo* storage)
{
  call_capability::applyCapabilitySelection(*this, capabilityName, capabilityId, storage);
  syncNodeSvgFromState();
}

void CallNode::syncNodeSvgFromState()
{
  setNodeSvg(call_capability::nodeSvgForState(*this));
}

void CallNode::paintBehaviourExtras(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  Q_UNUSED(style);
  Q_UNUSED(widget);

  if (hasCapabilitySelected())
  {
    behaviour::paintSelectedComponentOverlay(this, painter);
    return;
  }

  behaviour::paintEmptySlotSvg(painter, nodeRect().center(), capabilitySlotDiameter());
}
