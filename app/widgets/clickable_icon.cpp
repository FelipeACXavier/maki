#include "clickable_icon.h"

#include <QPainter>
#include <oclero/qlementine.hpp>

#include "app_configs.h"

ClickableIcon::ClickableIcon(const QIcon& icon, QWidget* parent)
    : ClickableIcon(icon, Config::SMALL_BUTTON_SIZE, parent)
{
}

ClickableIcon::ClickableIcon(const QIcon& icon, const QSize& size, QWidget* parent)
    : oclero::qlementine::IconWidget(icon, size, parent)
    , mChecked(false)
    , mCheckable(false)
    , mHovered(false)
{
  setIcon(icon);
  setIconSize(size);
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

  if (mCheckable)
    emit toggled(mChecked);
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

void ClickableIcon::enterEvent(QEnterEvent* event)
{
  mHovered = true;
  oclero::qlementine::IconWidget::enterEvent(event);
  update();
}

void ClickableIcon::leaveEvent(QEvent* event)
{
  mHovered = false;
  oclero::qlementine::IconWidget::leaveEvent(event);
  update();
}

void ClickableIcon::paintEvent(QPaintEvent* event)
{
  const auto* qlementineStyle = oclero::qlementine::appStyle();
  if (mHovered && qlementineStyle)
  {
    const auto theme = qlementineStyle->theme();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(theme.neutralColorHovered);

    const qreal radius = theme.borderRadius;
    painter.drawRoundedRect(rect(), radius, radius);
  }

  oclero::qlementine::IconWidget::paintEvent(event);
}
