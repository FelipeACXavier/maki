#include "sub_flow.h"

#include <QPainter>

#include "app_configs.h"

SubFlow::SubFlow(QGraphicsItem* parent)
    : QGraphicsItem(parent)
{
  setFlag(QGraphicsItem::ItemStacksBehindParent, true);
  setZValue(-1.0);
}

int SubFlow::type() const
{
  return Type;
}

QRectF SubFlow::boundingRect() const
{
  return mRect;
}

void SubFlow::setHighlight(bool highlight)
{
  mIsHighlighted = highlight;
}

void SubFlow::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

  painter->save();

  const QPen pen = mIsHighlighted ? QPen(Config::HIGHLIGHT, 3.0) : QPen(Qt::gray, 1.2);
  painter->setPen(pen);

  QColor background = Qt::white;
  background.setAlpha(40);

  painter->setBrush(background);
  painter->drawRoundedRect(mRect, 12.0, 12.0);

  painter->restore();
}

void SubFlow::setRect(const QRectF& rect)
{
  if (mRect == rect)
    return;

  prepareGeometryChange();
  mRect = rect;
  update();
}

void SubFlow::setCollapsed(bool collapsed)
{
  if (mIsCollapsed == collapsed)
    return;

  mIsCollapsed = collapsed;
  setVisible(!collapsed);

  if (collapsedChanged)
    collapsedChanged(collapsed);

  update();
}

bool SubFlow::isCollapsed() const
{
  return mIsCollapsed;
}
