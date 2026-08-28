#include "svg_button.h"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QSvgWidget>

SvgButton::SvgButton(const QString& svgPath, const QSize& size, QWidget* parent)
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

void SvgButton::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton && isEnabled())
    emit clicked();

  QWidget::mousePressEvent(event);
}