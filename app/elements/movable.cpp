#include "movable.h"

#include <QDrag>
#include <QDropEvent>

#include "style_helpers.h"

MovableLabel::MovableLabel(const QString& text, QWidget* parent)
    : QLabel(text, parent)
{
  setAlignment(Qt::AlignCenter);
  setAttribute(Qt::WA_DeleteOnClose);
}

void MovableLabel::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    QDrag* drag = new QDrag(this);
    QMimeData* mimeData = new QMimeData;
    mimeData->setText(QString::number(reinterpret_cast<quintptr>(this)));
    drag->setMimeData(mimeData);
    drag->setPixmap(grab());
    drag->exec(Qt::MoveAction);
  }
}

DraggableLabel::DraggableLabel(const QString& text, QWidget* parent)
    : MovableLabel(text, parent)
{
}

void DraggableLabel::mousePressEvent(QMouseEvent* event)
{
  if (parentWidget() == nullptr)
  {
    // Only drag from outside
    QDrag* drag = new QDrag(this);
    QMimeData* mimeData = new QMimeData;
    mimeData->setText(text());
    drag->setMimeData(mimeData);
    drag->setPixmap(grab());
    drag->exec(Qt::MoveAction);
  }
  else
  {
    MovableLabel::mousePressEvent(event);  // Normal move behavior inside DropArea
  }
}
