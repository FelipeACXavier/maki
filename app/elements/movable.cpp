#include "movable.h"

#include <QBuffer>
#include <QDrag>
#include <QDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>

#include "app_configs.h"

DraggableItem::DraggableItem(const QString& id, std::shared_ptr<NodeConfig> config, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , mId(id)
    , mConfig(config)
    , mBounds(0, 0, mConfig->body.width, mConfig->body.height)
{
  // Create the text item (for the label inside the rectangle)
  mLabel = std::make_shared<QGraphicsTextItem>(mConfig->name, this);
  mLabel->setDefaultTextColor(mConfig->body.textColor);

  updateLabelPosition();
}

DraggableItem::~DraggableItem()
{
}

QString DraggableItem::Id() const
{
  return mId;
}

int DraggableItem::type() const
{
  return Type;
}

void DraggableItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  // Set rounded corners by using a QRectF with the desired corner radius
  painter->setPen(mConfig->body.borderColor);
  painter->setBrush(mConfig->body.backgroundColor);

  painter->setRenderHint(QPainter::Antialiasing, false);

  if (mConfig->body.shape == Type::Shape::RECTANGLE)
  {
    painter->drawRect(drawingBoarders());
  }
  else if (mConfig->body.shape == Type::Shape::ELLIPSE)
  {
    painter->drawEllipse(drawingBoarders());
  }
  else if (mConfig->body.shape == Type::Shape::DIAMOND)
  {
    QPolygonF diamond;
    diamond << QPointF(drawingBoarders().center().x(), drawingBoarders().top())     // Top
            << QPointF(drawingBoarders().right(), drawingBoarders().center().y())   // Right
            << QPointF(drawingBoarders().center().x(), drawingBoarders().bottom())  // Bottom
            << QPointF(drawingBoarders().left(), drawingBoarders().center().y());   // Left

    painter->drawPolygon(diamond);
  }
  else
  {
    painter->drawRoundedRect(drawingBoarders(), 5, 5);  // 10 is the radius of the corners
  }
}

// Override shape() to define the item's collision shape with rounded corners
QPainterPath DraggableItem::shape() const
{
  QPainterPath path;
  if (mConfig->body.shape == Type::Shape::RECTANGLE)
  {
    path.addRect(boundingRect());
  }
  else if (mConfig->body.shape == Type::Shape::ELLIPSE)
  {
    path.addEllipse(boundingRect());
  }
  else if (mConfig->body.shape == Type::Shape::DIAMOND)
  {
    QPolygonF diamond;
    diamond << QPointF(boundingRect().center().x(), boundingRect().top())     // Top
            << QPointF(boundingRect().right(), boundingRect().center().y())   // Right
            << QPointF(boundingRect().center().x(), boundingRect().bottom())  // Bottom
            << QPointF(boundingRect().left(), boundingRect().center().y());   // Left

    path.addPolygon(diamond);
  }
  else
  {
    path.addRoundedRect(boundingRect(), 10, 10);  // 10 is the corner radius
  }

  return path;
}

QRectF DraggableItem::boundingRect() const
{
  return mBounds;
}

QRectF DraggableItem::drawingBoarders() const
{
  return boundingRect().adjusted(1, 1, -1, -1);
}

void DraggableItem::adjustWidth(int width)
{
  int centerX = (width - boundingRect().width()) / 2;
  setPos(centerX, pos().y());

  updateLabelPosition();
}

void DraggableItem::updateLabelPosition()
{
  if (!mLabel)
    return;

  QRectF textBounds = mLabel->boundingRect();

  // Calculate centered position
  qreal x = boundingRect().center().x() - textBounds.width() / 2;
  qreal y = boundingRect().center().y() - textBounds.height() / 2;

  mLabel->setPos(x, y);
}

void DraggableItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  // Get pixmap from this
  QPixmap pixmap(boundingRect().size().toSize());
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing, false);
  painter.setRenderHint(QPainter::TextAntialiasing, false);

  QStyleOptionGraphicsItem opt;
  paint(&painter, &opt);

  painter.setPen(mLabel->defaultTextColor());
  painter.drawText(pixmap.rect(), Qt::AlignCenter, mLabel->toPlainText());

  // Serialize it so it is copied
  QByteArray pixmapData;
  QBuffer buffer(&pixmapData);
  buffer.open(QIODevice::WriteOnly);
  pixmap.save(&buffer, "PNG");

  // Save data
  QMimeData* mimeData = new QMimeData();
  mimeData->setData(Constants::TYPE_NODE, QByteArray());
  mimeData->setData(Constants::TYPE_NODE_ID, Id().toUtf8());
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
