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

// ===========================================================================================================
// TraceCollapseItem
// ===========================================================================================================
TraceCollapseItem::TraceCollapseItem(const QRectF& rect, QString text, QGraphicsItem* parent)
    : QGraphicsObject(parent)
    , mRect(rect)
    , mText(std::move(text))
{
  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::LeftButton);
  setCursor(Qt::PointingHandCursor);
}

void TraceCollapseItem::paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*)
{
  p->setRenderHint(QPainter::Antialiasing, true);

  QColor fill = mHovered ? QColor("#dddddd") : QColor("#f6f6f6");

  p->setPen(QPen(Qt::black, 1));
  p->setBrush(fill);
  p->drawRoundedRect(mRect, 3, 3);

  p->setPen(Qt::black);
  p->drawText(mRect, Qt::AlignCenter, mText);
}

void TraceCollapseItem::hoverEnterEvent(QGraphicsSceneHoverEvent*)
{
  mHovered = true;
  update();
}

void TraceCollapseItem::hoverLeaveEvent(QGraphicsSceneHoverEvent*)
{
  mHovered = false;
  mPressed = false;
  update();
}

void TraceCollapseItem::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
  if (e->button() == Qt::LeftButton)
  {
    mPressed = true;
    e->accept();
    return;
  }

  e->ignore();
}

void TraceCollapseItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{
  if (mPressed && mRect.contains(e->pos()) && clicked)
    clicked();

  mPressed = false;
  update();
  e->accept();
}