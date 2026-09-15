#include "empty_slot.h"

#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPen>
#include <oclero/qlementine.hpp>

EmptySlot::EmptySlot(qreal diameter, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , mDiameter(diameter)
{
  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::LeftButton);

  /*
   * This makes setPos() refer to the center of the slot instead of its
   * top-left corner.
   */
  setTransformOriginPoint(0.0, 0.0);
}

int EmptySlot::type() const
{
  return Type;
}

QRectF EmptySlot::boundingRect() const
{
  const qreal radius = diameter() * 0.5;
  return QRectF(-radius, -radius, diameter(), diameter());
}

void EmptySlot::setDiameter(qreal diameter)
{
  if (qFuzzyCompare(mDiameter, diameter))
    return;

  prepareGeometryChange();

  mDiameter = diameter;

  update();
}

qreal EmptySlot::diameter() const
{
  return mDiameter;
}

void EmptySlot::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
  if (!painter)
    return;

  const auto* style = oclero::qlementine::appStyle();
  const auto& theme = style->theme();

  painter->save();
  painter->setRenderHint(QPainter::Antialiasing, true);

  const qreal radius = mDiameter * 0.5;
  const QRectF circleRect(-radius, -radius, mDiameter, mDiameter);

  QColor background = mHovered ? theme.neutralColorHovered : theme.neutralColor;
  if (mPressed)
    background = background.darker(104);

  painter->setBrush(background);

  QPen outlinePen(mHovered ? theme.primaryColorHovered : theme.secondaryColor);
  outlinePen.setWidthF(1);
  outlinePen.setStyle(mHovered ? Qt::SolidLine : Qt::DashLine);

  painter->setPen(outlinePen);
  painter->drawEllipse(circleRect);

  constexpr qreal innerDiameterFactor = 0.42;
  const qreal innerDiameter = mDiameter * innerDiameterFactor;
  const qreal innerRadius = innerDiameter * 0.5;
  const QRectF innerCircle(-innerRadius, -innerRadius, innerDiameter, innerDiameter);

  outlinePen.setWidthF(2.5);
  outlinePen.setStyle(Qt::SolidLine);
  painter->setPen(outlinePen);
  painter->setBrush(Qt::NoBrush);
  // painter->drawEllipse(innerCircle);

  constexpr qreal plusLengthFactor = 0.8;
  const qreal plusHalfLength = innerRadius * plusLengthFactor;
  painter->drawLine(QPointF(-plusHalfLength, 0.0), QPointF(+plusHalfLength, 0.0));
  painter->drawLine(QPointF(0.0, -plusHalfLength), QPointF(0.0, +plusHalfLength));
  painter->restore();
}

void EmptySlot::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  mHovered = true;
  update();
  QGraphicsObject::hoverEnterEvent(event);
}

void EmptySlot::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  mHovered = false;
  mPressed = false;
  update();
  QGraphicsObject::hoverLeaveEvent(event);
}

void EmptySlot::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->button() != Qt::LeftButton)
  {
    QGraphicsObject::mousePressEvent(event);
    return;
  }

  mPressed = true;
  update();
  event->accept();
}

void EmptySlot::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->button() != Qt::LeftButton)
  {
    QGraphicsObject::mouseReleaseEvent(event);
    return;
  }

  const bool wasPressed = mPressed;
  mPressed = false;
  update();

  /*
   * Emit only when the release still happens inside the item.
   */
  if (wasPressed && clicked && contains(event->pos()))
    clicked();

  event->accept();
}