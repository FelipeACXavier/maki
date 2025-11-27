#include "transition.h"

#include <QPainter>
#include <QPen>
#include <QUuid>

#include "app_configs.h"
#include "node.h"
#include "theme.h"

TransitionItem::TransitionItem(std::shared_ptr<TransitionSaveInfo> storage)
    : QGraphicsPathItem()
    , mId((!storage->id.isEmpty() && !storage->id.isNull()) ? storage->id : QUuid::createUuid().toString())
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

  mLabel = std::make_shared<QGraphicsTextItem>(this);
  mLabel->setFont(Fonts::Property);
  mLabel->setPlainText(mStorage->label);
  updateLabelPosition();

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
  if (id == mStorage->srcId)
    mStorage->srcPoint = mSource ? mSource->edgePointToward(mDestination->sceneBoundingRect().center()) : pos;
  else if (id == mStorage->dstId)
    mStorage->dstPoint = mDestination ? mDestination->edgePointToward(mSource->sceneBoundingRect().center()) : pos;
  else
    return;

  // Control points for Bézier curve
  QPainterPath path;
  path.moveTo(mStorage->srcPoint);
  path.lineTo(mStorage->dstPoint);

  setPath(path);
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

  double angle = std::atan2(-line.dy(), line.dx());
  const qreal arrowSize = 10;

  QPointF arrowP1 = line.p2() - QPointF(std::cos(angle + M_PI / 6) * arrowSize,
                                        -std::sin(angle + M_PI / 6) * arrowSize);
  QPointF arrowP2 = line.p2() - QPointF(std::cos(angle - M_PI / 6) * arrowSize,
                                        -std::sin(angle - M_PI / 6) * arrowSize);

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

  if (mEdge == Edge::NONE)
  {
    path.lineTo(end);
  }
  else
  {
    QPointF mid = (start + end) / 2;
    qreal distance = QLineF(start, end).length();
    qreal offset = qMin(80.0, distance * 0.5);  // cap max curve to avoid going crazy

    mid.setY(mid.y() + (mEdge == Edge::FORWARD ? offset : -offset));

    path.quadTo(mid, end);
  }

  setPath(path);
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
  if (!mLabel)
    return;

  mLabel->setPlainText(name);
  mStorage->label = name;
  updateLabelPosition();
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
  mLabel->setPos(labelPos.x() - labelSize.width() / 2,
                 labelPos.y() - labelSize.height() / 2);
}
QString TransitionItem::getEvent() const
{
  return mStorage->event;
}

void TransitionItem::setEvent(const QString& name)
{
  mStorage->event = name;
}
