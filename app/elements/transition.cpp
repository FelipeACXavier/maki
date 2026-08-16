#include "transition.h"

#include <QLineF>
#include <QPainter>
#include <QPen>
#include <QUuid>
#include <QVector>
#include <cmath>

#include "app_configs.h"
#include "node.h"
#include "system/canvas.h"
#include "system/edge_router.h"

namespace
{
/** Combo placeholder "-" means no event; never paint it on the canvas. */
QString normalizedEventLabel(const QString& event)
{
  return (event == Constants::EMPTY_COMBO) ? QString() : event;
}

const QColor kOnErrorColor(0xd1, 0x61, 0x5d);
const QColor kOnAbortColor(0x96, 0x76, 0x62);

QColor transitionStrokeColor(const TransitionItem& transition)
{
  if (transition.isSelected())
    return Config::HIGHLIGHT;

  const QString event = transition.getEvent();
  if (event.compare(QStringLiteral("on error"), Qt::CaseInsensitive) == 0)
    return kOnErrorColor;
  if (event.compare(QStringLiteral("on abort"), Qt::CaseInsensitive) == 0)
    return kOnAbortColor;
  return Config::FOREGROUND;
}

/** Abort/error: up then right into the in-port. Abort hits a bit higher, error a bit lower. */
constexpr qreal kAbortErrorPortFan = 3.0;

void appendOrthoPoint(QVector<QPointF>& points, const QPointF& point)
{
  if (points.isEmpty())
  {
    points << point;
    return;
  }
  if (QLineF(points.last(), point).length() < 0.5)
    return;
  points << point;
}

QPainterPath manhattanAbortErrorPath(const QPointF& start, const QPointF& end, const QString& event)
{
  const qreal fan = event.compare(QStringLiteral("on abort"), Qt::CaseInsensitive) == 0 ? -kAbortErrorPortFan
                                                                                         : kAbortErrorPortFan;
  const QPointF dest(end.x(), end.y() + fan);

  QVector<QPointF> points;
  appendOrthoPoint(points, start);
  appendOrthoPoint(points, QPointF(start.x(), dest.y()));
  appendOrthoPoint(points, dest);

  QPainterPath path;
  if (points.isEmpty())
    return path;
  path.moveTo(points.first());
  for (int i = 1; i < points.size(); ++i)
    path.lineTo(points.at(i));
  return path;
}

constexpr qreal kLabelClearancePad = 6.0;

QPointF unitPerpLeft(const QPointF& dir)
{
  const qreal len = std::hypot(dir.x(), dir.y());
  if (len < 1e-6)
    return QPointF(0.0, -1.0);
  const QPointF unit(dir.x() / len, dir.y() / len);
  return QPointF(-unit.y(), unit.x());
}

bool longestHorizontalSegment(const QPainterPath& path, QLineF& best)
{
  QPointF prev;
  qreal bestLen = 0.0;
  bool found = false;
  for (int i = 0; i < path.elementCount(); ++i)
  {
    const auto element = path.elementAt(i);
    const QPointF cur(element.x, element.y);
    if (element.type != QPainterPath::MoveToElement)
    {
      const QLineF seg(prev, cur);
      if (std::abs(seg.dx()) >= std::abs(seg.dy()) && seg.length() > bestLen)
      {
        best = seg;
        bestLen = seg.length();
        found = true;
      }
    }
    prev = cur;
  }
  return found;
}

QPointF labelCenterBesidePath(const QPointF& sample, const QPointF& perp, const QSizeF& labelSize)
{
  const qreal hw = labelSize.width() / 2.0;
  const qreal hh = labelSize.height() / 2.0;
  const qreal dist = std::abs(perp.x()) * hw + std::abs(perp.y()) * hh + kLabelClearancePad;
  return sample + perp * dist;
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
  QFont labelFont = Fonts::Main;
  labelFont.setPointSizeF(Fonts::BaseSize);
  mLabel->setFont(labelFont);
  setEvent(mStorage->getevent());

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

QString TransitionItem::sourceId() const
{
  return mStorage->getsrcId();
}

QString TransitionItem::destinationId() const
{
  return mStorage->getdstId();
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

  const auto canvas = static_cast<Canvas*>(scene());
  if (!canvas)
    return;
  const auto router = canvas->router();
  if (!router)
    return;

  if (isPortBoundEvent())
    setPath(manhattanAbortErrorPath(mStorage->srcPoint(), mStorage->dstPoint(), getEvent()));
  else
    setPath(router->route(mStorage->srcPoint(), mStorage->dstPoint(), {}));
  updateLabelPosition();
  prepareGeometryChange();
}

void TransitionItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  const QColor color = transitionStrokeColor(*this);
  setPen(QPen(color, 2));

  if (mLabel)
    mLabel->setDefaultTextColor(color);

  QGraphicsPathItem::paint(painter, option, widget);

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

  painter->setPen(Qt::NoPen);
  painter->setBrush(QBrush(color));
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

  if (painterPath.isEmpty())
  {
    const auto canvas = static_cast<Canvas*>(scene());
    if (!canvas)
      return;

    const auto router = canvas->router();
    if (!router)
      return;

    // Compute edge points toward the other node
    const QPointF start = mSource->outgoingPortAnchorForEvent(mStorage->getevent());
    const QPointF end = mDestination->incomingPortAnchor();

    if (isPortBoundEvent())
      setPath(manhattanAbortErrorPath(start, end, getEvent()));
    else
      setPath(router->route(start, end, {}));
  }
  else if (isPortBoundEvent())
  {
    const QPointF start = mSource->outgoingPortAnchorForEvent(mStorage->getevent());
    const QPointF end = mDestination->incomingPortAnchor();
    setPath(manhattanAbortErrorPath(start, end, getEvent()));
  }
  else
  {
    setPath(painterPath);
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

  const QSizeF labelSize = mLabel->boundingRect().size();
  QPointF sample;
  QPointF perp;

  const bool abort = getEvent().compare(QStringLiteral("on abort"), Qt::CaseInsensitive) == 0;
  const bool error = getEvent().compare(QStringLiteral("on error"), Qt::CaseInsensitive) == 0;

  QLineF horiz;
  if ((abort || error) && longestHorizontalSegment(p, horiz) && horiz.length() > 8.0)
  {
    sample = horiz.pointAt(abort ? 0.40 : 0.65);
    perp = unitPerpLeft(horiz.p2() - horiz.p1());
    // Abort stays above the line, error below, so the two labels never share a corridor.
    if (abort && perp.y() > 0.0)
      perp = QPointF(-perp.x(), -perp.y());
    if (error && perp.y() < 0.0)
      perp = QPointF(-perp.x(), -perp.y());
  }
  else
  {
    qreal pathPercent = 0.5;
    qreal side = 1.0;
    if (abort)
    {
      pathPercent = 0.45;
      side = 1.0;
    }
    else if (error)
    {
      pathPercent = 0.70;
      side = -1.0;
    }

    sample = p.pointAtPercent(pathPercent);
    const qreal angleRad = qDegreesToRadians(p.angleAtPercent(pathPercent));
    perp = QPointF(-std::sin(angleRad) * side, -std::cos(angleRad) * side);
    const qreal len = std::hypot(perp.x(), perp.y());
    perp = (len < 1e-6) ? QPointF(0.0, -1.0) : QPointF(perp.x() / len, perp.y() / len);
  }

  const QPointF center = labelCenterBesidePath(sample, perp, labelSize);
  mLabel->setPos(center.x() - labelSize.width() / 2.0, center.y() - labelSize.height() / 2.0);
}

QString TransitionItem::getEvent() const
{
  return normalizedEventLabel(mStorage->getevent());
}

void TransitionItem::setEvent(const QString& name)
{
  const QString normalized = normalizedEventLabel(name);
  mStorage->setEvent(normalized);
  if (!mLabel)
    return;

  mLabel->setPlainText(normalized);
  updateLabelPosition();
  update();
}

bool TransitionItem::isPortBoundEvent() const
{
  const QString event = getEvent();
  return event.compare(QStringLiteral("on abort"), Qt::CaseInsensitive) == 0
         || event.compare(QStringLiteral("on error"), Qt::CaseInsensitive) == 0;
}
