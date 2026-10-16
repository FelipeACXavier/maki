#include "editor_focus_border.h"

#include <QPainter>
#include <oclero/qlementine.hpp>

namespace maki
{
EditorFocusBorder::EditorFocusBorder(QWidget* parent)
    : QWidget(parent)
{
  setAttribute(Qt::WA_TransparentForMouseEvents);
  setAttribute(Qt::WA_NoSystemBackground);
  setFocusPolicy(Qt::NoFocus);
  hide();
}

void EditorFocusBorder::setActive(bool active)
{
  if (mActive == active)
    return;

  mActive = active;
  setVisible(active);
  update();
}

void EditorFocusBorder::paintEvent(QPaintEvent* event)
{
  Q_UNUSED(event);

  if (!mActive)
    return;

  if (const auto* style = oclero::qlementine::appStyle())
  {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const auto theme = style->theme();
    painter.fillRect(QRect(0, 0, width(), 2), theme.primaryColor);
  }
}
}  // namespace maki