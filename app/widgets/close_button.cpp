#include "close_button.h"

#include <QPainter>
#include <oclero/qlementine.hpp>

CloseButton::CloseButton(QWidget* parent)
    : QToolButton(parent)
{
  setAutoRaise(true);
  setCursor(Qt::PointingHandCursor);
}

void CloseButton::paintEvent(QPaintEvent* event)
{
  // Let Qlementine paint the normal QToolButton:
  // hover, pressed state, background, etc.
  QToolButton::paintEvent(event);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  QFont font = this->font();
  font.setPixelSize(12);
  font.setWeight(QFont::Medium);
  painter.setFont(font);

  // Use the palette so this still follows the active Qlementine theme.
  if (const auto* style = oclero::qlementine::appStyle())
    painter.setPen(style->theme().secondaryColor);
  else
    painter.setPen(palette().color(QPalette::ButtonText));

  const QString text = "×";
  const QFontMetrics metrics(font);
  const QRect textBounds = metrics.boundingRect(text);

  const qreal x = (width() - textBounds.width()) / 2.0 - textBounds.left();
  const qreal y = (height() - textBounds.height()) / 2.0 - textBounds.top() - 1;

  painter.drawText(QPointF(x, y), text);
}