#include "clickable_label.h"

#include <QMouseEvent>

namespace maki
{

ClickableLabel::ClickableLabel(QWidget* parent)
    : oclero::qlementine::Label(parent)
{
  setCursor(Qt::PointingHandCursor);
}

void ClickableLabel::mouseDoubleClickEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
    emit clicked();

  oclero::qlementine::Label::mousePressEvent(event);
}

}  // namespace maki