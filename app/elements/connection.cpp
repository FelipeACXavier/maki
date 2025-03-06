#include "connection.h"

#include <QPen>
#include <QUuid>

#include "app_configs.h"
#include "connector.h"

ConnectionItem::ConnectionItem()
    : QGraphicsPathItem()
    , mId(QUuid::createUuid().toString())
    , mComplete(false)
    , mSource(nullptr)
    , mDestination(nullptr)
{
  // Make sure the connections are behind the nodes
  setZValue(-1);
  setFlags(QGraphicsItem::ItemIsSelectable);

  // Set line color and width
  // TODO(felaze): make configurable
  setPen(QPen(Qt::white, 2));
}

ConnectionItem::~ConnectionItem()
{
  if (mSource != nullptr)
    mSource->removeConnection(this);
  if (mDestination != nullptr)
    mDestination->removeConnection(this);
}

QString ConnectionItem::id() const
{
  return mId;
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

void ConnectionItem::done(Connector* source, Connector* destination)
{
  mComplete = true;
  mSource = source;
  mDestination = destination;

  // Make sure line is update with new control points
  move(mSrcId, mSrcPoint);
  move(mDstId, mDstPoint);
}

Connector* ConnectionItem::source() const
{
  return mSource;
}

Connector* ConnectionItem::destination() const
{
  return mDestination;
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

void ConnectionItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  QGraphicsPathItem::paint(painter, option, widget);

  if (isSelected())
    setPen(QPen(Config::Colours::ACCENT, 2));
  else
    setPen(QPen(Qt::white, 2));
}

ConnectionSaveInfo ConnectionItem::saveInfo() const
{
  ConnectionSaveInfo info;
  info.srcId = mSrcId;
  info.srcPoint = mSrcPoint;
  info.srcShift = mSrcShift;

  info.dstId = mDstId;
  info.dstPoint = mDstPoint;
  info.dstShift = mDstShift;

  return info;
}
