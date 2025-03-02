#include "connection.h"

#include <QPen>

ConnectionItem::ConnectionItem()
    : QGraphicsPathItem()
    , mComplete(false)
{
  // Make sure the connections are behind the nodes
  setZValue(-1);

  // Set line color and width
  // TODO(felaze): make configurable
  setPen(QPen(Qt::white, 2));
}

void ConnectionItem::setStart(const QString& id, const QPointF& point, const QPointF& controlShift)
{
  mSrcId = id;
  mSrcPoint = point;
  mSrcShift = controlShift;
}

void ConnectionItem::setEnd(const QString& id, const QPointF& point, const QPointF& controlShift)
{
  mDstId = id;
  mDstPoint = point;
  mDstShift = controlShift;
}

void ConnectionItem::done()
{
  mComplete = true;

  // Make sure line is update with new control points
  move(mSrcId, mSrcPoint);
  move(mDstId, mDstPoint);
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
  QPainterPath path;
  path.moveTo(mSrcPoint);
  if (mComplete)
    // Control points for Bézier curve
    path.cubicTo(mSrcPoint + mSrcShift, mDstPoint + mDstShift, mDstPoint);
  else
    path.lineTo(mDstPoint);

  setPath(path);
}
