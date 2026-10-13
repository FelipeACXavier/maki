#include "control_widget.h"

namespace maki
{
ControlWidget::ControlWidget(QWidget* parent)
    : QWidget(parent)
{
}

void ControlWidget::start()
{
}

void ControlWidget::activate()
{
  emit activated();
}

void ControlWidget::setTouring()
{
}

}  // namespace maki