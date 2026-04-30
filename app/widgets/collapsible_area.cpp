#include "collapsible_area.h"

CollapsibleAreaWidth::CollapsibleAreaWidth(QWidget* parent)
    : QWidget(parent)
    , mContentWidth(0)
{
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
}

int CollapsibleAreaWidth::contentWidth() const
{
  return mContentWidth;
}

void CollapsibleAreaWidth::setContentWidth(int width)
{
  if (mContentWidth == width)
    return;

  mContentWidth = width;
  updateGeometry();
  resize(mContentWidth, height());
}

QSize CollapsibleAreaWidth::sizeHint() const
{
  QSize s = QWidget::sizeHint();
  s.setWidth(mContentWidth);
  return s;
}

QSize CollapsibleAreaWidth::minimumSizeHint() const
{
  QSize s = QWidget::minimumSizeHint();
  s.setWidth(mContentWidth);
  return s;
}

// ===========================================================================================================
// Height
CollapsibleAreaHeight::CollapsibleAreaHeight(QWidget* parent)
    : QWidget(parent)
    , mContentHeight(0)
{
  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

int CollapsibleAreaHeight::contentHeight() const
{
  return mContentHeight;
}

void CollapsibleAreaHeight::setContentHeight(int height)
{
  if (mContentHeight == height)
    return;

  mContentHeight = height;
  updateGeometry();
  resize(width(), mContentHeight);
}

QSize CollapsibleAreaHeight::sizeHint() const
{
  QSize s = QWidget::sizeHint();
  s.setHeight(mContentHeight);
  return s;
}

QSize CollapsibleAreaHeight::minimumSizeHint() const
{
  QSize s = QWidget::minimumSizeHint();
  s.setHeight(mContentHeight);
  return s;
}
