#include "connection.h"

#include <QPen>

ConnectionItem::ConnectionItem()
    : QGraphicsLineItem()
{
  setPen(QPen(Qt::white, 2));  // Set line color and width
}

void ConnectionItem::setStart(QString id, QPointF point)
{
  mSrcId = id;
  mSrcPoint = point;
}

void ConnectionItem::setEnd(QString id, QPointF point)
{
  mDstId = id;
  mDstPoint = point;
}

void ConnectionItem::move(QString id, QPointF pos)
{
  if (id == mSrcId)
  {
    mSrcPoint = pos;
    // LOG_INFO("Moving start to: (%f %f), (%f, %f)", mStartPoint.x(), mStartPoint.y(), mEndPoint.x(), mEndPoint.y());
    setLine(QLineF(mSrcPoint, mDstPoint));
  }
  else if (id == mDstId)
  {
    mDstPoint = pos;
    // LOG_INFO("Moving end to: (%f %f) (%f, %f)", mStartPoint.x(), mStartPoint.y(), mEndPoint.x(), mEndPoint.y());
    setLine(QLineF(mSrcPoint, mDstPoint));
  }
}
