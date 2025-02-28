#include "movable.h"

#include <QBuffer>
#include <QDrag>
#include <QDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>

#include "app_configs.h"

DraggableItem::DraggableItem(const QString& text, QGraphicsItem* parent)
    : QGraphicsRectItem(0, 0, 100, 50, parent)
{
  setAcceptDrops(false);
  setBrush(Qt::lightGray);

  // Create the text item (for the label inside the rectangle)
  mLabel = std::make_shared<QGraphicsTextItem>(text, this);
  mLabel->setDefaultTextColor(Qt::black);

  updateLabelPosition();
}

DraggableItem::~DraggableItem()
{
}

int DraggableItem::type() const
{
  return Type;
}

void DraggableItem::adjustWidth(int width)
{
  int centerX = (width - rect().width()) / 2;
  setPos(centerX, pos().y());

  updateLabelPosition();
}

void DraggableItem::updateLabelPosition()
{
  if (!mLabel)
    return;

  QRectF textBounds = mLabel->boundingRect();

  // Calculate centered position
  qreal x = rect().center().x() - textBounds.width() / 2;
  qreal y = rect().center().y() - textBounds.height() / 2;

  mLabel->setPos(x, y);
}

void DraggableItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  // Get pixmap from this
  QPixmap pixmap(boundingRect().size().toSize());
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing);

  QStyleOptionGraphicsItem opt;
  paint(&painter, &opt);

  painter.drawText(pixmap.rect(), Qt::AlignCenter, mLabel->toPlainText());

  // Serialize it so it is copied
  QByteArray pixmapData;
  QBuffer buffer(&pixmapData);
  buffer.open(QIODevice::WriteOnly);
  pixmap.save(&buffer, "PNG");

  // Save data
  QMimeData* mimeData = new QMimeData();
  mimeData->setData(Constants::TYPE_NODE, QByteArray());
  mimeData->setData(Constants::TYPE_PIXMAP, pixmapData);

  // Create drag itself
  QDrag* drag = new QDrag(event->widget());
  drag->setMimeData(mimeData);
  drag->setPixmap(pixmap);
  drag->setHotSpot(QPoint(boundingRect().width() / 2, boundingRect().height() / 2));
  drag->exec(Qt::MoveAction);

  // Nicety, let's make the cursor show a drag action
  setCursor(Qt::ClosedHandCursor);
}
