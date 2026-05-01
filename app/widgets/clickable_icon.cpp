#include "clickable_icon.h"

ClickableIcon::ClickableIcon(const QIcon& icon, QWidget* parent)
    : ClickableIcon(icon, QSize(16, 16), parent)
{
}

ClickableIcon::ClickableIcon(const QIcon& icon, const QSize& size, QWidget* parent)
    : oclero::qlementine::IconWidget(icon, size, parent)
    , mChecked(false)
    , mCheckable(false)
{
  setIcon(icon);
  setCursor(Qt::PointingHandCursor);
}

bool ClickableIcon::isChecked()
{
  return mChecked;
}

void ClickableIcon::setCheckable(bool checkable)
{
  mCheckable = checkable;
}

void ClickableIcon::setChecked(bool check)
{
  mChecked = check;
}

void ClickableIcon::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    if (mCheckable)
    {
      mChecked = !mChecked;
      emit toggled(mChecked);
    }
    else
    {
      emit clicked();
    }
  }

  QWidget::mousePressEvent(event);
}