#include "svg_click_button.h"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QSvgWidget>

SvgClickButton::SvgClickButton(const QString& svgPath, const QSize& size, QWidget* parent)
    : QWidget(parent)
{
  setFixedSize(size);
  setCursor(Qt::PointingHandCursor);
  setFocusPolicy(Qt::NoFocus);

  auto* layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);

  mSvg = new QSvgWidget(svgPath, this);
  mSvg->setFixedSize(size);
  layout->addWidget(mSvg);
}

void SvgClickButton::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton && isEnabled())
    emit clicked();

  QWidget::mousePressEvent(event);
}
