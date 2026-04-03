#include "simulation_label_item.h"

#include <QCursor>
#include <QGraphicsSceneMouseEvent>

TraceLabelItem::TraceLabelItem(const QRectF& rect, const Style* const style, Payload payload, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , mRect(rect)
    , mPayload(std::move(payload))
    , mStyle(style)
{
  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
  setCursor(Qt::PointingHandCursor);
}

void TraceLabelItem::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*)
{
  p->setRenderHint(QPainter::Antialiasing, true);

  QColor fill = mPayload.illegal ? mStyle->invalid : mStyle->valid;

  if (mHovered && !mPayload.illegal)
    fill = mStyle->hover;

  p->setPen(mStyle->borderPen);
  p->setBrush(fill);
  p->drawRoundedRect(mRect, 6, 6);

  // Text
  p->setPen(mStyle->labelPen);
  p->setFont(mStyle->font);
  p->drawText(mRect.adjusted(6, 0, -6, 0), Qt::AlignCenter, mPayload.call);
}

void TraceLabelItem::hoverEnterEvent(QGraphicsSceneHoverEvent*)
{
  mHovered = true;
  update();
}

void TraceLabelItem::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
  mHovered = false;
  mPressed = false;
  update();
}

void TraceLabelItem::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
  if (e->button() == Qt::LeftButton || e->button() == Qt::RightButton)
  {
    mPressed = true;
    update();
    e->accept();
    return;
  }
  e->ignore();
}

void TraceLabelItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{
  const bool inside = mRect.contains(e->pos());

  if (mPressed && inside)
  {
    if (e->button() == Qt::LeftButton && clicked)
      clicked(mPayload);
    if (e->button() == Qt::RightButton && rightClicked)
      rightClicked(mPayload);
  }

  mPressed = false;
  update();
  e->accept();
}