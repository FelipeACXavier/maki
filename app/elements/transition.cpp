#include "transition.h"

#include <QPainter>
#include <QPen>
#include <QUuid>

#include "app_configs.h"
#include "node.h"
#include "system/canvas.h"
#include "system/edge_router.h"

TransitionItem::TransitionItem(std::shared_ptr<TransitionSaveInfo> storage)
    : QGraphicsPathItem()
    , mId((!storage->getid().isEmpty() && !storage->getid().isNull()) ? storage->getid() : QUuid::createUuid().toString())
    , mComplete(false)
    , mEdge(Edge::NONE)
    , mSource(nullptr)
    , mDestination(nullptr)
    , mStorage(storage)
{
  // Make sure the transitions are behind the nodes
  // setZValue(-1);
  setFlags(QGraphicsItem::ItemIsSelectable);

  // Set line color and width
  // TODO(felaze): make configurable
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

  updatePath();
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
  if (id == mStorage->getsrcId())
    mStorage->setSrcPoint(mSource ? mSource->outgoingPortAnchorForEvent(getEvent()) : pos);
  else if (id == mStorage->getdstId())
    mStorage->setDstPoint(mDestination ? mSource->incomingPortAnchor() : pos);
  else
    return;

  const auto canvas = qobject_cast<Canvas*>(scene());
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
  QGraphicsPathItem::paint(painter, option, widget);

  setPen(isSelected() ? QPen(Config::HIGHLIGHT, 2) : QPen(Config::FOREGROUND, 2));

  if (mLabel)
    mLabel->setDefaultTextColor(Config::FOREGROUND);

  QLineF line = path().currentPosition() == path().pointAtPercent(1.0) ? QLineF(path().pointAtPercent(0.99), path().pointAtPercent(1.0))
                                                                       : QLineF(path().pointAtPercent(0.98), path().pointAtPercent(1.0));

  double angle = std::atan2(-line.dy(), line.dx());
  const qreal arrowSize = 10;

  QPointF arrowP1 = line.p2() - QPointF(std::cos(angle + M_PI / 6) * arrowSize, -std::sin(angle + M_PI / 6) * arrowSize);
  QPointF arrowP2 = line.p2() - QPointF(std::cos(angle - M_PI / 6) * arrowSize, -std::sin(angle - M_PI / 6) * arrowSize);

  QPolygonF arrowHead;
  arrowHead << line.p2() << arrowP1 << arrowP2;

  painter->setBrush(isSelected() ? QBrush(Config::HIGHLIGHT) : QBrush(Config::FOREGROUND));
  painter->drawPolygon(arrowHead);
}

QPainterPath TransitionItem::shape() const
{
  QPainterPathStroker stroker;
  stroker.setWidth(10);  // Wider clickable area
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
    const auto canvas = qobject_cast<Canvas*>(scene());
    if (!canvas)
      return;

    const auto router = canvas->router();
    if (!router)
      return;

    // Compute edge points toward the other node
    const QPointF start = mSource->outgoingPortAnchorForEvent(getEvent());
    const QPointF end = mDestination->incomingPortAnchor();

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

  QPointF midPoint = p.pointAtPercent(0.5);
  qreal angleDeg = p.angleAtPercent(0.5);

  // Convert angle to radians for vector math
  qreal angleRad = qDegreesToRadians(angleDeg);

  // Compute the unit perpendicular vector
  qreal offsetDistance = 10.0;  // adjust this as needed
  qreal dx = -std::sin(angleRad);
  qreal dy = -std::cos(angleRad);

  QPointF offset(dx * offsetDistance, dy * offsetDistance);
  QPointF labelPos = midPoint + offset;

  QSizeF labelSize = mLabel->boundingRect().size();
  mLabel->setPos(labelPos.x() - labelSize.width() / 2, labelPos.y() - labelSize.height() / 2);
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
