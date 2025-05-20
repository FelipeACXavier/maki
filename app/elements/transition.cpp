#include "transition.h"

#include <QPainter>
#include <QPen>
#include <QUuid>

#include "app_configs.h"
#include "node.h"

TransitionItem::TransitionItem(std::shared_ptr<TransitionSaveInfo> storage)
    : QGraphicsPathItem()
    , mId((!storage->id.isEmpty() && !storage->id.isNull()) ? storage->id : QUuid::createUuid().toString())
    , mComplete(false)
    , mSource(nullptr)
    , mDestination(nullptr)
    , mStorage(storage)
{
  // Make sure the transitions are behind the nodes
  setZValue(-1);
  setFlags(QGraphicsItem::ItemIsSelectable);

  // Set line color and width
  // TODO(felaze): make configurable
  setPen(QPen(Qt::white, 2));

  mStorage->id = id();
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
  mStorage->srcId = id;
  mStorage->srcPoint = point;
  mStorage->srcShift = controlShift;
}

void TransitionItem::setEnd(const QString& id, const QPointF& point, const QPointF& controlShift)
{
  mStorage->dstId = id;
  mStorage->dstPoint = point;
  mStorage->dstShift = controlShift;
}

void TransitionItem::done(NodeItem* source, NodeItem* destination)
{
  mComplete = true;
  mSource = source;
  mDestination = destination;

  mSource->addTransition(this);
  mDestination->addTransition(this);

  // Make sure line is update with new control points
  move(mStorage->srcId, mStorage->srcPoint);
  move(mStorage->dstId, mStorage->dstPoint);
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
  if (id == mStorage->srcId)
    mStorage->srcPoint = mSource ? mSource->edgePointToward(mDestination->sceneBoundingRect().center()) : pos;
  else if (id == mStorage->dstId)
    mStorage->dstPoint = mDestination ? mDestination->edgePointToward(mSource->sceneBoundingRect().center()) : pos;
  else
    return;

  // Control points for Bézier curve
  QPainterPath path;
  path.moveTo(mStorage->srcPoint);

  if (mComplete)
    // Control points for Bézier curve
    path.cubicTo(mStorage->srcPoint + mStorage->srcShift, mStorage->dstPoint + mStorage->dstShift, mStorage->dstPoint);
  else
    path.lineTo(mStorage->dstPoint);

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

std::shared_ptr<TransitionSaveInfo> TransitionItem::storage() const
{
  return mStorage;
}

TransitionSaveInfo TransitionItem::saveInfo() const
{
  return *mStorage;
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
