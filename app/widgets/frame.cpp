#include "frame.h"

StyledFrame::StyledFrame(QWidget* parent)
    : QFrame(parent)
{
  setFrameShape(QFrame::NoFrame);
  setAttribute(Qt::WA_StyledBackground, false);
  setAutoFillBackground(false);
}

void StyledFrame::setBackgroundRole(BackgroundRole role)
{
  mBackgroundRole = role;
  update();
}

void StyledFrame::setBorderRole(BorderRole role)
{
  mBorderRole = role;
  update();
}

void StyledFrame::setCustomBackgroundColor(const QColor& color)
{
  mCustomBackgroundColor = color;
  mBackgroundRole = BackgroundRole::Custom;
  update();
}

void StyledFrame::setCustomBorderColor(const QColor& color)
{
  mCustomBorderColor = color;
  mBorderRole = BorderRole::Custom;
  update();
}

void StyledFrame::setBorderWidth(int width)
{
  mBorderWidth = width;
  update();
}

void StyledFrame::setRadius(qreal radius)
{
  mRadius = radius;
  update();
}

void StyledFrame::paintEvent(QPaintEvent* event)
{
  Q_UNUSED(event);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  const qreal halfPen = mBorderWidth > 0 ? mBorderWidth / 2.0 : 0.0;
  const QRectF r = rect().adjusted(
      halfPen,
      halfPen,
      -halfPen,
      -halfPen);

  QPainterPath path;
  path.addRoundedRect(r, mRadius, mRadius);

  painter.fillPath(path, backgroundColor());

  if (mBorderRole != BorderRole::None && mBorderWidth > 0)
  {
    painter.setPen(QPen(borderColor(), mBorderWidth));
    painter.drawPath(path);
  }
}

QColor StyledFrame::backgroundColor() const
{
  const auto pal = palette();

  switch (mBackgroundRole)
  {
    case BackgroundRole::Window:
      return pal.color(QPalette::Window);
    case BackgroundRole::Base:
      return pal.color(QPalette::Base);
    case BackgroundRole::Button:
      return pal.color(QPalette::Button);
    case BackgroundRole::Midlight:
      return pal.color(QPalette::Midlight);
    case BackgroundRole::Custom:
      return mCustomBackgroundColor;
  }

  return pal.color(QPalette::Window);
}

QColor StyledFrame::borderColor() const
{
  const auto pal = palette();

  switch (mBorderRole)
  {
    case BorderRole::None:
      return Qt::transparent;
    case BorderRole::Mid:
      return pal.color(QPalette::Mid);
    case BorderRole::Highlight:
      return pal.color(QPalette::Highlight);
    case BorderRole::Custom:
      return mCustomBorderColor;
  }

  return pal.color(QPalette::Mid);
}