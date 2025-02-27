#include "connection.h"

#include <QPen>

static const qreal CONTROL_POINT_SHIFT = 100;

ConnectionItem::ConnectionItem()
    : QGraphicsPathItem()
{
  // Make sure the connections are behind the nodes
  setZValue(-1);

  // Set line color and width
  // TODO(felaze): make conigurable
  setPen(QPen(Qt::white, 2));
}

void ConnectionItem::setStart(const QString& id, QPointF point)
{
  mSrcId = id;
  mSrcPoint = point;
}

void ConnectionItem::setEnd(const QString& id, QPointF point)
{
  mDstId = id;
  mDstPoint = point;
}

void ConnectionItem::move(const QString& id, QPointF pos)
{
  if (id == mSrcId)
    mSrcPoint = pos;
  else if (id == mDstId)
    mDstPoint = pos;
  else
    return;

  // Control points for Bézier curve
  QPointF control1(mSrcPoint.x() + CONTROL_POINT_SHIFT, mSrcPoint.y());
  QPointF control2(mDstPoint.x() - CONTROL_POINT_SHIFT, mDstPoint.y());

  QPainterPath path;
  path.moveTo(mSrcPoint);
  path.cubicTo(control1, control2, mDstPoint);

  setPath(path);
}
