#include "sub_flow_marker.h"

#include <QPainter>

#include "style_helpers.h"

static const qreal MARKER_WIDTH = 30;
static const qreal MARKER_HEIGHT = 25;

SubFlowMarker::SubFlowMarker(Types::Port kind, QGraphicsItem* parent)
    : PortItem(kind, true, parent)
{
  setAcceptedMouseButtons(Qt::NoButton);
  setFlag(ItemIsSelectable, false);
}

QRectF SubFlowMarker::boundingRect() const
{
  return QRectF(0.0, 0.0, MARKER_WIDTH, MARKER_HEIGHT);
}

QPointF SubFlowMarker::inputAnchor() const
{
  return mapToScene(QPointF(0.0, MARKER_HEIGHT / 2.0));
}

QPointF SubFlowMarker::outputAnchor() const
{
  return mapToScene(QPointF(MARKER_WIDTH, MARKER_HEIGHT / 2.0));
}

void SubFlowMarker::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

  if (!mRenderer)
    return;

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing);
  const QRectF rect = boundingRect();
  paintSvg(mRenderer, painter, rect.center(), rect.width(), rect.height());
  painter->restore();
}
