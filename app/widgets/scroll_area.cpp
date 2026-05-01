#include "scroll_area.h"

#include <QEvent>
#include <oclero/qlementine.hpp>

StyledScrollArea::StyledScrollArea(QWidget* parent)
    : QScrollArea(parent)
{
  setFrameShape(QFrame::NoFrame);
  setWidgetResizable(true);

  viewport()->setAutoFillBackground(false);
  viewport()->setAttribute(Qt::WA_StyledBackground, false);
}

void StyledScrollArea::setBackgroundRole(BackgroundRole role)
{
  mBackgroundRole = role;
  viewport()->update();
}

void StyledScrollArea::setCustomBackgroundColor(const QColor& color)
{
  mCustomBackgroundColor = color;
  mBackgroundRole = BackgroundRole::Custom;
  viewport()->update();
}

bool StyledScrollArea::viewportEvent(QEvent* event)
{
  if (event->type() == QEvent::Paint)
  {
    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(viewport()->rect(), backgroundColor());
  }

  return QScrollArea::viewportEvent(event);
}

QColor StyledScrollArea::backgroundColor() const
{
  if (mBackgroundRole == BackgroundRole::Custom)
    return mCustomBackgroundColor;

  const auto* style = oclero::qlementine::appStyle();

  if (!style)
    return palette().color(QPalette::Base);

  const auto theme = style->theme();

  switch (mBackgroundRole)
  {
    case BackgroundRole::Window:
      return theme.backgroundColorMain1;

    case BackgroundRole::Base:
      return theme.backgroundColorMain2;

    case BackgroundRole::Button:
      return theme.backgroundColorMain3;

    case BackgroundRole::Midlight:
      return theme.backgroundColorMain4;

    case BackgroundRole::Custom:
      return mCustomBackgroundColor;
  }

  return palette().color(QPalette::Base);
}