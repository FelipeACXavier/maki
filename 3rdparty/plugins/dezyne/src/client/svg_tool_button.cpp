#include "svg_tool_button.h"

#include <QPainter>
#include <QStyleOptionToolButton>

SvgToolButton::SvgToolButton(const QString& svgPath, QWidget* parent)
    : QToolButton(parent)
    , mRenderer(svgPath)
{
}

void SvgToolButton::paintEvent(QPaintEvent*)
{
  QStyleOptionToolButton option;
  initStyleOption(&option);

  QPainter painter(this);

  // Let Qlementine draw the actual button.
  style()->drawComplexControl(QStyle::CC_ToolButton, &option, &painter, this);

  constexpr int iconSize = 20;

  const QRectF iconRect((width() - iconSize) / 2.0, (height() - iconSize) / 2.0, iconSize, iconSize);

  mRenderer.render(&painter, iconRect);
}