#include "transition.h"

#include <QPainter>
#include <QPen>
#include <QUuid>
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
      mStorage->setDstPoint(mDestination->incomingPortAnchorForEvent(mStorage->getevent()));
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
    const QPointF end = mDestination->incomingPortAnchorForEvent(mStorage->getevent());

    setPath(router->route(start, end, {}));
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

  // Abort / error: park labels on opposite sides and at different path fractions so they
  // stay readable when both edges share a destination.
  qreal pathPercent = 0.5;
  qreal side = 1.0;
  const QString event = getEvent();
  if (event.compare(QStringLiteral("on abort"), Qt::CaseInsensitive) == 0)
  {
    pathPercent = 0.40;
    side = 1.0;
  }
  else if (event.compare(QStringLiteral("on error"), Qt::CaseInsensitive) == 0)
  {
    pathPercent = 0.60;
    side = -1.0;
  }

  const QPointF midPoint = p.pointAtPercent(pathPercent);
  const qreal angleDeg = p.angleAtPercent(pathPercent);
  const qreal angleRad = qDegreesToRadians(angleDeg);

  const qreal offsetDistance = 12.0 * side;
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
