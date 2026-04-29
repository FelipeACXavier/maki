#include "subtask_connector.h"

#include <cmath>

#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPen>
#include <QSvgRenderer>
#include <QStyleOptionGraphicsItem>

#include "app_paths.h"
#include "node.h"
#include "system/structure_canvas.h"
#include "theme.h"

namespace
{
constexpr qreal kStrokeWidth = 3.0;
constexpr qreal kTrunkDrop = 24.0;
constexpr qreal kRowGap = 48.0;
constexpr qreal kPlaceholderSide = 48.0;
constexpr qreal kArrowSize = 10.0;
constexpr qreal kPlaceholderBranchLength = 86.0;
/** Slightly-left anchor, but still attached to task body. */
constexpr qreal kTrunkMinInset = 28.0;
constexpr qreal kTrunkWidthFraction = 0.30;

/** Anchor on the visible task body (not full scene bounds with label). */
QPointF trunkTopFromTaskRect(const NodeItem& task)
{
  const QRectF taskBodySceneRect = task.mapRectToScene(task.boundingRect());
  const qreal trunkX = taskBodySceneRect.left() + qMax(kTrunkMinInset, taskBodySceneRect.width() * kTrunkWidthFraction);
  return QPointF(trunkX, taskBodySceneRect.bottom());
}

void drawArrowHead(QPainter* painter, const QPointF& tip, qreal angleRad)
{
  QPointF p1 = tip - QPointF(std::cos(angleRad + M_PI / 6) * kArrowSize, -std::sin(angleRad + M_PI / 6) * kArrowSize);
  QPointF p2 = tip - QPointF(std::cos(angleRad - M_PI / 6) * kArrowSize, -std::sin(angleRad - M_PI / 6) * kArrowSize);
  QPolygonF head;
  head << tip << p1 << p2;
  painter->setBrush(QBrush(Config::FOREGROUND));
  painter->drawPolygon(head);
}
}  // namespace

SubtaskConnector::SubtaskConnector(NodeItem* task, StructureCanvas* canvas)
    : mTask(task)
    , mCanvas(canvas)
{
  setAcceptedMouseButtons(Qt::LeftButton);
  setFlag(QGraphicsItem::ItemIsSelectable, false);
  setZValue(task ? task->zValue() - 1 : -1);
  syncGeometry();
}

SubtaskConnector::~SubtaskConnector() = default;

void SubtaskConnector::syncGeometry()
{
  prepareGeometryChange();

  mStrokePath = QPainterPath();
  mPlaceholderRect = QRectF();
  mBounds = QRectF();

  if (!mTask || !mTask->scene())
    return;

  const QRectF taskR = mTask->sceneBoundingRect();
  const QPointF trunkTop = trunkTopFromTaskRect(*mTask);
  const qreal trunkX = trunkTop.x();

  QVector<NodeItem*> subs = mTask->structuralSubtaskChildren();

  qreal lowestBranchY = trunkTop.y() + kTrunkDrop;
  qreal lowestSubtaskBottom = lowestBranchY;
  for (NodeItem* st : subs)
  {
    QRectF sr = st->sceneBoundingRect();
    lowestBranchY = qMax(lowestBranchY, sr.center().y());
    lowestSubtaskBottom = qMax(lowestSubtaskBottom, sr.bottom());
  }

  // keep placeholder fully below last subtask to avoid overlap
  const qreal placeholderTop = qMax(lowestBranchY + kRowGap, lowestSubtaskBottom + kRowGap);
  const qreal placeholderBranchY = placeholderTop + (kPlaceholderSide * 0.5);
  const qreal placeholderCenterX = trunkX + kPlaceholderBranchLength;
  const QRectF placeholderScene(placeholderCenterX - kPlaceholderSide / 2.0,
                                placeholderTop,
                                kPlaceholderSide,
                                kPlaceholderSide);

  const qreal trunkBottomY = placeholderBranchY;

  QRectF br = taskR;
  br |= placeholderScene;
  br |= QRectF(trunkTop, QSizeF(1, 1));
  br |= QRectF(QPointF(trunkX, trunkTop.y()), QPointF(trunkX, trunkBottomY));

  for (NodeItem* st : subs)
  {
    QRectF sr = st->sceneBoundingRect();
    const qreal branchY = sr.center().y();
    br |= QRectF(QPointF(qMin(trunkX, sr.left()), branchY), QPointF(qMax(trunkX, sr.left()), branchY));
    br |= sr;
  }
  br |= QRectF(QPointF(qMin(trunkX, placeholderScene.left()), placeholderBranchY),
               QPointF(qMax(trunkX, placeholderScene.left()), placeholderBranchY));

  br = br.adjusted(-20, -10, 20, 20);
  mBounds = br;
  const QPointF origin = br.topLeft();
  setPos(origin);

  auto toLocal = [&](const QPointF& scenePt) -> QPointF {
    return scenePt - origin;
  };

  mStrokePath.moveTo(toLocal(trunkTop));
  mStrokePath.lineTo(toLocal(QPointF(trunkX, trunkBottomY)));

  for (NodeItem* st : subs)
  {
    QRectF sr = st->sceneBoundingRect();
    const qreal branchY = sr.center().y();
    mStrokePath.moveTo(toLocal(QPointF(trunkX, branchY)));
    mStrokePath.lineTo(toLocal(QPointF(sr.left(), branchY)));
  }
  mStrokePath.moveTo(toLocal(QPointF(trunkX, placeholderBranchY)));
  mStrokePath.lineTo(toLocal(QPointF(placeholderScene.left(), placeholderBranchY)));

  mPlaceholderRect = placeholderScene.translated(-origin);

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

  QVector<NodeItem*> subs = mTask->structuralSubtaskChildren();
  const qreal trunkX = trunkTopFromTaskRect(*mTask).x();

  for (NodeItem* st : subs)
  {
    QRectF sr = st->sceneBoundingRect();
    const qreal branchY = sr.center().y();
    QLineF branch(QPointF(trunkX, branchY), QPointF(sr.left(), branchY));
    const qreal angle = std::atan2(-branch.dy(), branch.dx());
    drawArrowHead(painter, branch.p2() - mBounds.topLeft(), angle);
  }
  QLineF placeholderBranch(QPointF(trunkX, mPlaceholderRect.center().y() + mBounds.topLeft().y()),
                           QPointF(mPlaceholderRect.left() + mBounds.topLeft().x(), mPlaceholderRect.center().y() + mBounds.topLeft().y()));
  const qreal placeholderAngle = std::atan2(-placeholderBranch.dy(), placeholderBranch.dx());
  drawArrowHead(painter, placeholderBranch.p2() - mBounds.topLeft(), placeholderAngle);

  const QString svgPath = AppPaths::icon(QStringLiteral("node_subtask_placeholder.svg"));
  QSvgRenderer renderer(svgPath);
  if (renderer.isValid())
  {
    painter->setPen(Qt::NoPen);
    renderer.render(painter, mPlaceholderRect);
  }
  else
  {
    painter->setPen(pen);
    painter->drawEllipse(mPlaceholderRect);
  }

  painter->restore();
}

QPainterPath SubtaskConnector::shape() const
{
  QPainterPathStroker stroker;
  stroker.setWidth(12);
  QPainterPath p = stroker.createStroke(mStrokePath);
  p.addEllipse(mPlaceholderRect.adjusted(-4, -4, 4, 4));
  return p;
}

void SubtaskConnector::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if (!mCanvas || !mTask || !event)
  {
    QGraphicsItem::mousePressEvent(event);
    return;
  }

  const QPointF local = event->pos();
  if (mPlaceholderRect.contains(local))
  {
    mCanvas->createSubtask(mTask);
    event->accept();
    return;
  }

  QGraphicsItem::mousePressEvent(event);
}
