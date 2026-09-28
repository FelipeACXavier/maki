#include "sub_flow.h"

#include <QPainter>

#include "app_configs.h"

SubFlow::SubFlow(const QString& id, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , mId(id)
    , mTitle("")
{
  setFlag(QGraphicsItem::ItemStacksBehindParent, true);
  setZValue(-1.0);
}

QString SubFlow::id() const
{
  return mId;
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

  if (!title().isEmpty())
  {
    QFont titleFont = painter->font();

    titleFont.setBold(true);
    painter->setFont(titleFont);
    painter->setPen(Config::FOREGROUND);

    painter->setBrush(Qt::NoBrush);
    painter->drawText(mTextRect, Qt::AlignLeft | Qt::AlignVCenter, mTitle);
  }

  painter->restore();
}

void SubFlow::setRect(const QRectF& rect)
{
  if (mRect == rect)
    return;

  prepareGeometryChange();
  mRect = rect;
  if (mPosition == Config::ControlPosition::Bottom)
    mTextRect = QRectF(mRect.left() + mLeftPadding, mRect.bottom() - mTopPadding - 12, qMax(0.0, mRect.width() - mLeftPadding), 12);
  else
    mTextRect = QRectF(mRect.left() + mLeftPadding, mRect.top() + mTopPadding, qMax(0.0, mRect.width() - mLeftPadding), 12);
  update();
}

void SubFlow::setTitlePosition(const Config::ControlPosition pos, int leftPadding, int topPadding)
{
  mPosition = pos;
  mLeftPadding = leftPadding;
  mTopPadding = topPadding;
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

void SubFlow::setTitle(const QString& title)
{
  if (mTitle == title)
    return;

  mTitle = title;
  update();
}

QString SubFlow::title() const
{
  return mTitle;
}
