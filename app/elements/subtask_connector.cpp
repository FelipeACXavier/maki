#include "subtask_connector.h"

#include <algorithm>

#include <QPainter>
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <cmath>

#include "app_configs.h"
#include "node.h"

namespace
{
constexpr qreal kStrokeWidth = 3.0;
constexpr qreal kArrowSize = 10.0;
/** Extra pixels below caption so elbows do not collide with multi-line titles. */
constexpr qreal kOutletBelowCaption = 6.0;

QPointF parentConnectorOutletScene(const NodeItem* task)
{
  if (!task)
    return {};
  const QRectF bodyScene = task->mapRectToScene(task->boundingRect());
  const QRectF withLabelScene = task->mapRectToScene(task->itemRectIncludingLabel());
  return QPointF(bodyScene.center().x(), withLabelScene.bottom() + kOutletBelowCaption);
}

void drawArrowHead(QPainter* painter, const QPointF& tipLocal, qreal angleRad)
{
  QPointF p1 =
      tipLocal - QPointF(std::cos(angleRad + M_PI / 6) * kArrowSize, -std::sin(angleRad + M_PI / 6) * kArrowSize);
  QPointF p2 =
      tipLocal - QPointF(std::cos(angleRad - M_PI / 6) * kArrowSize, -std::sin(angleRad - M_PI / 6) * kArrowSize);
  QPolygonF head;
  head << tipLocal << p1 << p2;
  painter->setBrush(QBrush(Config::FOREGROUND));
  painter->drawPolygon(head);
}

QPainterPath elbowConnectorPathScene(const QPointF& parentOutletScene, const QRectF& childBodyScene)
{
  QPointF pb = parentOutletScene;
  QPointF ct(childBodyScene.center().x(), childBodyScene.top());
  qreal midY = (pb.y() + ct.y()) * 0.5;
  midY = std::clamp(midY, std::min(pb.y(), ct.y()), std::max(pb.y(), ct.y()));

  QPainterPath path;
  path.moveTo(pb);
  path.lineTo(pb.x(), midY);
  path.lineTo(ct.x(), midY);
  path.lineTo(ct);
  return path;
}
}  // namespace

SubtaskConnector::SubtaskConnector(NodeItem* task)
    : mTask(task)
{
  setAcceptedMouseButtons(Qt::NoButton);
  setFlag(QGraphicsItem::ItemIsSelectable, false);
  setZValue(task ? task->zValue() - 1 : -1);
  syncGeometry();
}

SubtaskConnector::~SubtaskConnector() = default;

void SubtaskConnector::syncGeometry()
{
  prepareGeometryChange();

  mStrokePath = QPainterPath();
  mBounds = QRectF();

  if (!mTask || !mTask->scene())
    return;

  const QRectF parentUnion = mTask->mapRectToScene(mTask->itemRectIncludingLabel())
                                 .adjusted(0, 0, 0, kOutletBelowCaption);
  mBounds |= parentUnion;

  const QPointF parentOutlet = parentConnectorOutletScene(mTask);

  QVector<NodeItem*> subs = mTask->structuralSubtaskChildren();
  for (NodeItem* st : subs)
  {
    QRectF childBody = st->mapRectToScene(st->boundingRect());
    mBounds |= childBody;
    QPainterPath seg = elbowConnectorPathScene(parentOutlet, childBody);
    mBounds |= seg.boundingRect();
    mStrokePath.addPath(seg);
  }

  mBounds = mBounds.adjusted(-24, -12, 24, 20);
  setPos(mBounds.topLeft());

  const QPointF origin = pos();
  mStrokePath.translate(-origin);

  if (mTask)
    setZValue(mTask->zValue() - 1);
}

QRectF SubtaskConnector::boundingRect() const
{
  if (mBounds.isEmpty())
    return QRectF();
  return QRectF(0, 0, mBounds.width(), mBounds.height());
}

void SubtaskConnector::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

  if (!mTask || mBounds.isEmpty())
    return;

  painter->save();

  QPen pen(Config::FOREGROUND, kStrokeWidth, Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
  painter->setPen(pen);
  painter->setBrush(Qt::NoBrush);
  painter->drawPath(mStrokePath);

  const QPointF parentOutletScene = parentConnectorOutletScene(mTask);
  const QPointF origin = mBounds.topLeft();

  for (NodeItem* st : mTask->structuralSubtaskChildren())
  {
    const QRectF childBody = st->mapRectToScene(st->boundingRect());
    qreal midY = (parentOutletScene.y() + childBody.top()) * 0.5;
    midY = std::clamp(midY, std::min(parentOutletScene.y(), childBody.top()),
                      std::max(parentOutletScene.y(), childBody.top()));
    QPointF tipLocal(childBody.center().x() - origin.x(), childBody.top() - origin.y());
    QPointF jointLocal(childBody.center().x() - origin.x(), midY - origin.y());
    const qreal angle = std::atan2(-(tipLocal.y() - jointLocal.y()), tipLocal.x() - jointLocal.x());
    drawArrowHead(painter, tipLocal, angle);
  }

  painter->restore();
}

QPainterPath SubtaskConnector::shape() const
{
  QPainterPathStroker stroker;
  stroker.setWidth(14);
  return stroker.createStroke(mStrokePath);
}
