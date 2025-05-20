#include "transition.h"

#include <QPainter>
#include <QPen>
#include <QUuid>

#include "app_configs.h"
#include "node.h"

TransitionItem::TransitionItem()
    : QGraphicsPathItem()
    , mId(QUuid::createUuid().toString())
    , mComplete(false)
    , mSource(nullptr)
    , mDestination(nullptr)
{
  // Make sure the transitions are behind the nodes
  setZValue(-1);
  setFlags(QGraphicsItem::ItemIsSelectable);

  // Set line color and width
  // TODO(felaze): make configurable
  setPen(QPen(Qt::white, 2));
}

TransitionItem::~TransitionItem()
{
  if (mSource != nullptr)
    mSource->removeTransition(this);
  if (mDestination != nullptr)
    mDestination->removeTransition(this);
}

QString TransitionItem::id() const
{
  return mId;
}

int TransitionItem::type() const
{
  return Type;
}

void TransitionItem::setStart(const QString& id, const QPointF& point, const QPointF& controlShift)
{
  mSrcId = id;
  mSrcPoint = point;
  mSrcShift = controlShift;
}

void TransitionItem::setEnd(const QString& id, const QPointF& point, const QPointF& controlShift)
{
  mDstId = id;
  mDstPoint = point;
  mDstShift = controlShift;
}

void TransitionItem::done(NodeItem* source, NodeItem* destination)
{
  mComplete = true;
  mSource = source;
  mDestination = destination;

  // Make sure line is update with new control points
  move(mSrcId, mSrcPoint);
  move(mDstId, mDstPoint);
}

NodeItem* TransitionItem::source() const
{
  return mSource;
}

NodeItem* TransitionItem::destination() const
{
  return mDestination;
}

void TransitionItem::move(const QString& id, QPointF pos)
{
  if (id == mSrcId)
    mSrcPoint = mSource ? mSource->edgePointToward(mDestination->sceneBoundingRect().center()) : pos;
  else if (id == mDstId)
    mDstPoint = mDestination ? mDestination->edgePointToward(mSource->sceneBoundingRect().center()) : pos;
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

void TransitionItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  QGraphicsPathItem::paint(painter, option, widget);

  setPen(isSelected() ? QPen(Config::Colours::ACCENT, 2) : QPen(Qt::white, 2));

  QLineF line = path().currentPosition() == path().pointAtPercent(1.0)
                    ? QLineF(path().pointAtPercent(0.99), path().pointAtPercent(1.0))
                    : QLineF(path().pointAtPercent(0.98), path().pointAtPercent(1.0));

  double angle = std::atan2(-line.dy(), line.dx());
  const qreal arrowSize = 10;

  QPointF arrowP1 = line.p2() - QPointF(std::cos(angle + M_PI / 6) * arrowSize,
                                        -std::sin(angle + M_PI / 6) * arrowSize);
  QPointF arrowP2 = line.p2() - QPointF(std::cos(angle - M_PI / 6) * arrowSize,
                                        -std::sin(angle - M_PI / 6) * arrowSize);

  QPolygonF arrowHead;
  arrowHead << line.p2() << arrowP1 << arrowP2;

  painter->setBrush(isSelected() ? QBrush(Config::Colours::ACCENT) : QBrush(Qt::white));
  painter->drawPolygon(arrowHead);
}

TransitionSaveInfo TransitionItem::saveInfo() const
{
  TransitionSaveInfo info;
  info.srcPoint = mSrcPoint;
  info.srcShift = mSrcShift;

  info.dstId = mDstId;
  info.dstPoint = mDstPoint;
  info.dstShift = mDstShift;

  return info;
}

void TransitionItem::updatePath()
{
  if (!mSource || !mDestination)
    return;

  // Get center positions in scene coordinates
  QPointF fromCenter = mSource->sceneBoundingRect().center();
  QPointF toCenter = mDestination->sceneBoundingRect().center();

  // Compute edge points toward the other node
  QPointF start = mSource->edgePointToward(toCenter);
  QPointF end = mDestination->edgePointToward(fromCenter);

  // Draw the curve or line
  QPainterPath path(start);

  path.lineTo(end);

  setPath(path);
  prepareGeometryChange();  // if needed for boundingRect
}
