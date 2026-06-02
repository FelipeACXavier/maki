#include "transition.h"

#include <QPainter>
#include <QPen>
#include <QUuid>

#include <cmath>

#include "app_configs.h"
#include "node.h"
#include "port.h"

#include "system/canvas.h"
#include "system/edge_router.h"

namespace
{
constexpr qreal kStub = 14.0;
constexpr qreal kChannelMargin = 18.0;
constexpr qreal kTrimStart = PortItem::kSize * 0.5 + 1.0;
constexpr qreal kTrimTopStart = PortItem::kTopPortSize * 0.5 + 1.0;
constexpr qreal kTrimEnd = PortItem::kSize * 0.35 + 1.0;

bool usesOrthogonalRouting(PortItem::Kind srcPortKind)
{
  return srcPortKind == PortItem::Abort || srcPortKind == PortItem::Error;
}

void trimStraightEndpoints(QPointF& start, QPointF& end, bool shortenEnd)
{
  const QPointF origStart = start;
  const QPointF origEnd = end;
  QLineF seg(origStart, origEnd);
  const qreal len = seg.length();
  if (len < 1.0)
    return;

  const qreal dx = (origEnd.x() - origStart.x()) / len;
  const qreal dy = (origEnd.y() - origStart.y()) / len;

  const qreal trimFromOut = qMin(kTrimStart, len * 0.35);
  if (trimFromOut < len - 0.5)
    start += QPointF(dx * trimFromOut, dy * trimFromOut);

  if (shortenEnd)
  {
    const qreal trimBeforeIn = qMin(kTrimEnd, len * 0.2);
    if (trimBeforeIn < len - 0.5 && trimFromOut + trimBeforeIn < len - 0.5)
      end -= QPointF(dx * trimBeforeIn, dy * trimBeforeIn);
  }
}

QPainterPath buildStraightTransitionPath(QPointF start, QPointF end, bool trimEnds)
{
  if (trimEnds)
    trimStraightEndpoints(start, end, true);
  QPainterPath path;
  path.moveTo(start);
  path.lineTo(end);
  return path;
}

qreal laneOffsetForEvent(const QString& event)
{
  if (event.compare(QStringLiteral("on abort"), Qt::CaseInsensitive) == 0)
    return -14.0;
  if (event.compare(QStringLiteral("on error"), Qt::CaseInsensitive) == 0)
    return 14.0;
  return 0.0;
}

qreal sideRoutingChannelY(const NodeItem* source,
                          const NodeItem* destination,
                          const QPointF& start,
                          qreal lane,
                          bool exitDownward)
{
  const QRectF srcRect = source->sceneBoundingRect();
  const QRectF dstRect = destination ? destination->sceneBoundingRect() : QRectF();

  qreal y = exitDownward ? start.y() + kStub : start.y() - kStub;
  if (exitDownward)
  {
    y = qMax(y, srcRect.bottom() + kChannelMargin);
    if (destination)
      y = qMax(y, dstRect.bottom() + kChannelMargin);
    return y + lane;
  }

  y = qMin(y, srcRect.top() - kChannelMargin);
  if (destination)
    y = qMin(y, dstRect.top() - kChannelMargin);
  return y - lane;
}

QVector<QPointF> buildOrthogonalPoints(const QPointF& start,
                                       const QPointF& end,
                                       PortItem::Kind srcPortKind,
                                       const NodeItem* source,
                                       const NodeItem* destination,
                                       qreal lane)
{
  QVector<QPointF> pts;
  pts.reserve(6);
  pts.append(start);

  const qreal approachX = end.x() - kStub;
  const bool exitDownward =
      srcPortKind == PortItem::Abort && source && start.y() > source->sceneBoundingRect().center().y();
  const qreal channelY = sideRoutingChannelY(source, destination, start, lane, exitDownward);
  pts.append(QPointF(start.x(), channelY));
  pts.append(QPointF(approachX, channelY));
  pts.append(QPointF(approachX, end.y()));

  pts.append(end);
  return pts;
}

void trimPolylineEndpoints(QVector<QPointF>& pts, qreal trimStart, qreal trimEnd)
{
  if (pts.size() < 2)
    return;

  if (trimStart > 0.0)
  {
    QLineF seg(pts[0], pts[1]);
    const qreal len = seg.length();
    if (len > trimStart)
      pts[0] = seg.pointAt(trimStart / len);
  }

  if (trimEnd > 0.0)
  {
    const int n = pts.size();
    QLineF seg(pts[n - 2], pts[n - 1]);
    const qreal len = seg.length();
    if (len > trimEnd)
      pts[n - 1] = seg.pointAt((len - trimEnd) / len);
  }
}

QPainterPath pathFromPoints(const QVector<QPointF>& pts)
{
  QPainterPath path;
  if (pts.isEmpty())
    return path;

  path.moveTo(pts.front());
  for (int i = 1; i < pts.size(); ++i)
    path.lineTo(pts[i]);
  return path;
}

QPainterPath buildOrthogonalTransitionPath(const QPointF& start,
                                           const QPointF& end,
                                           PortItem::Kind srcPortKind,
                                           const NodeItem* source,
                                           const NodeItem* destination,
                                           const QString& event,
                                           bool trimEnds)
{
  const qreal lane = laneOffsetForEvent(event);
  QVector<QPointF> pts = buildOrthogonalPoints(start, end, srcPortKind, source, destination, lane);
  if (trimEnds)
    trimPolylineEndpoints(pts, kTrimTopStart, kTrimEnd);
  return pathFromPoints(pts);
}
}  // namespace


TransitionItem::TransitionItem(std::shared_ptr<TransitionSaveInfo> storage)
    : QGraphicsPathItem()
    , mId((!storage->getid().isEmpty() && !storage->getid().isNull()) ? storage->getid() : QUuid::createUuid().toString())
    , mComplete(false)
    , mEdge(Edge::NONE)
    , mSource(nullptr)
    , mDestination(nullptr)
    , mStorage(storage)
{
  setFlags(QGraphicsItem::ItemIsSelectable);

  setPen(QPen(Qt::white, 2));

  mLabel = new QGraphicsTextItem(this);
  mLabel->setFont(Fonts::Property);
  mLabel->setPlainText(mStorage->getevent());
  updateLabelPosition();

  mStorage->setId(id());
}

TransitionItem::~TransitionItem()
{
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
  mStorage->setSrcId(id);
  mStorage->setSrcPoint(point);
  mStorage->setSrcShift(controlShift);
}

void TransitionItem::setEnd(const QString& id, const QPointF& point, const QPointF& controlShift)
{
  mStorage->setDstId(id);
  mStorage->setDstPoint(point);
  mStorage->setDstShift(controlShift);
}

void TransitionItem::done(NodeItem* source, NodeItem* destination)
{
  mComplete = true;
  mSource = source;
  mDestination = destination;

  // Make sure line is update with new control points
  move(mStorage->getsrcId(), mStorage->srcPoint());
  move(mStorage->getdstId(), mStorage->dstPoint());
}

void TransitionItem::setEdge(Edge edge)
{
  mEdge = edge;
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
  Q_UNUSED(pos);

  if (id == mStorage->getsrcId())
  {
    if (!mSource)
      mStorage->setSrcPoint(pos);
    else
      mStorage->setSrcPoint(mSource->outgoingPortAnchorForEvent(mStorage->getevent()));
  }
  else if (id == mStorage->getdstId())
  {
    if (!mDestination)
      mStorage->setDstPoint(pos);
    else if (mSource)
      mStorage->setDstPoint(mDestination->incomingPortAnchor());
    else
      mStorage->setDstPoint(pos);
  }
  else
  {
    return;
  }

  const QPointF start = mStorage->srcPoint();
  const QPointF end = mStorage->dstPoint();

  if (mEdge != Edge::NONE)
  {
    QPainterPath path(start);
    QPointF mid = (start + end) * 0.5;
    const qreal distance = QLineF(start, end).length();
    const qreal offset = qMin(80.0, distance * 0.5);
    mid.setY(mid.y() + (mEdge == Edge::FORWARD ? offset : -offset));
    path.quadTo(mid, end);
    setPath(path);
  }
  else
  {
    const PortItem::Kind srcKind =
        mSource ? mSource->outgoingPortKindForEvent(mStorage->getevent()) : PortItem::Out;
    if (usesOrthogonalRouting(srcKind))
    {
      setPath(buildOrthogonalTransitionPath(start, end, srcKind, mSource, mDestination, mStorage->getevent(),
                                            mDestination != nullptr));
    }
    else
    {
      setPath(buildStraightTransitionPath(start, end, mDestination != nullptr));
    }
  }

  updateLabelPosition();
  prepareGeometryChange();
}

void TransitionItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  QGraphicsPathItem::paint(painter, option, widget);

  setPen(isSelected() ? QPen(Config::HIGHLIGHT, 2) : QPen(Config::FOREGROUND, 2));

  if (mLabel)
    mLabel->setDefaultTextColor(Config::FOREGROUND);

  QLineF line = path().currentPosition() == path().pointAtPercent(1.0)
                    ? QLineF(path().pointAtPercent(0.99), path().pointAtPercent(1.0))
                    : QLineF(path().pointAtPercent(0.98), path().pointAtPercent(1.0));

  const double angle = std::atan2(-line.dy(), line.dx());
  const qreal arrowSize = 10;

  const QPointF arrowP1 = line.p2() - QPointF(std::cos(angle + M_PI / 6) * arrowSize,
                                               -std::sin(angle + M_PI / 6) * arrowSize);
  const QPointF arrowP2 = line.p2() - QPointF(std::cos(angle - M_PI / 6) * arrowSize,
                                               -std::sin(angle - M_PI / 6) * arrowSize);

  QPolygonF arrowHead;
  arrowHead << line.p2() << arrowP1 << arrowP2;

  painter->setBrush(isSelected() ? QBrush(Config::HIGHLIGHT) : QBrush(Config::FOREGROUND));
  painter->drawPolygon(arrowHead);
}

QPainterPath TransitionItem::shape() const
{
  QPainterPathStroker stroker;
  stroker.setWidth(10);
  return stroker.createStroke(path());
}

std::shared_ptr<TransitionSaveInfo> TransitionItem::storage() const
{
  return mStorage;
}

TransitionSaveInfo TransitionItem::saveInfo() const
{
  return *mStorage;
}

void TransitionItem::updatePath(QPainterPath painterPath)
{
  if (!mSource || !mDestination)
    return;

  const QPointF start = mSource->outgoingPortAnchorForEvent(mStorage->getevent());
  const QPointF end = mDestination->incomingPortAnchor();

  if (mEdge != Edge::NONE)
  {
    QPainterPath path(start);
    QPointF mid = (start + end) * 0.5;
    const qreal distance = QLineF(start, end).length();
    const qreal offset = qMin(80.0, distance * 0.5);
    mid.setY(mid.y() + (mEdge == Edge::FORWARD ? offset : -offset));
    path.quadTo(mid, end);
    setPath(path);
  }
  else
  {
    const PortItem::Kind srcKind = mSource->outgoingPortKindForEvent(mStorage->getevent());
    if (usesOrthogonalRouting(srcKind))
      setPath(buildOrthogonalTransitionPath(start, end, srcKind, mSource, mDestination, mStorage->getevent(), true));
    else
      setPath(buildStraightTransitionPath(start, end, true));
  }

  updateLabelPosition();
  prepareGeometryChange();
}

QString TransitionItem::getName() const
{
  if (!mLabel)
    return QString();

  return mLabel->toPlainText();
}

void TransitionItem::setName(const QString& name)
{
  mStorage->setLabel(name);
}

void TransitionItem::updateLabelPosition()
{
  if (!mLabel)
    return;

  const QPainterPath& p = path();
  if (p.length() == 0.0)
    return;

  const QPointF midPoint = p.pointAtPercent(0.5);
  const qreal angleDeg = p.angleAtPercent(0.5);

  const qreal angleRad = qDegreesToRadians(angleDeg);

  const qreal offsetDistance = 10.0;
  const qreal dx = -std::sin(angleRad);
  const qreal dy = -std::cos(angleRad);

  const QPointF offset(dx * offsetDistance, dy * offsetDistance);
  const QPointF labelPos = midPoint + offset;

  const QSizeF labelSize = mLabel->boundingRect().size();
  mLabel->setPos(labelPos.x() - labelSize.width() / 2,
                 labelPos.y() - labelSize.height() / 2);
}

QString TransitionItem::getEvent() const
{
  return mStorage->getevent();
}

void TransitionItem::setEvent(const QString& name)
{
  mStorage->setEvent(name);
  if (!mLabel)
    return;

  mLabel->setPlainText(name);
  updateLabelPosition();
}
