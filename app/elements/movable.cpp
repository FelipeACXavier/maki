#include "movable.h"

#include <QBuffer>
#include <QDrag>
#include <QDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QStyle>
#include <QStyleOptionGraphicsItem>

#include "app_configs.h"

const qreal MAX_SIDEBAR_WIDTH = 100.0;
const qreal MAX_SIDEBAR_HEIGHT = 100.0;

DraggableItem::DraggableItem(const QString& id, std::shared_ptr<NodeConfig> config, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , mId(id)
    , mConfig(config)
    , mBounds(0, 0, mConfig->body.width, mConfig->body.height)
{
  qreal scaleFactor = computeScaleFactor();
  mScaledBounds = QRectF(0, 0, mConfig->body.width * scaleFactor, mConfig->body.height * scaleFactor);

  if (!mConfig->body.iconPath.isEmpty())
  {
    // Load icon
    QPixmap icon(mConfig->body.iconPath);
    mPixmap = std::make_shared<QPixmap>(icon.scaled(boundingRect().size().toSize() * mConfig->body.iconScale, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  }
  else
  {
    // Create the text item (for the label inside the rectangle)
    mLabel = std::make_shared<QGraphicsTextItem>(mConfig->name, this);
    mLabel->setDefaultTextColor(mConfig->body.textColor);
  }

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
  QRectF bounds = drawingBorders((option && option->state == QStyle::State_Active) ? mBounds : boundingRect());

  painter->setPen(mConfig->body.borderColor);
  painter->setBrush(mConfig->body.backgroundColor);

  painter->setRenderHint(QPainter::Antialiasing, false);

  if (mConfig->body.shape == Types::Shape::RECTANGLE)
  {
    painter->drawRect(bounds);
  }
  else if (mConfig->body.shape == Types::Shape::ELLIPSE)
  {
    painter->drawEllipse(bounds);
  }
  else if (mConfig->body.shape == Types::Shape::DIAMOND)
  {
    QPolygonF diamond;
    diamond << QPointF(bounds.center().x(), bounds.top())     // Top
            << QPointF(bounds.right(), bounds.center().y())   // Right
            << QPointF(bounds.center().x(), bounds.bottom())  // Bottom
            << QPointF(bounds.left(), bounds.center().y());   // Left

    painter->drawPolygon(diamond);
  }
  else
  {
    painter->drawRoundedRect(bounds, 5, 5);  // 10 is the radius of the corners
  }

  if (mPixmap)
  {
    QPointF center = bounds.center();
    QPointF topLeft = center - QPointF(mPixmap->width() / 2, mPixmap->height() / 2);
    painter->drawPixmap(topLeft, *mPixmap);
  }
}

// Override shape() to define the item's collision shape with rounded corners
QPainterPath DraggableItem::shape() const
{
  QPainterPath path;
  if (mConfig->body.shape == Types::Shape::RECTANGLE)
  {
    path.addRect(boundingRect());
  }
  else if (mConfig->body.shape == Types::Shape::ELLIPSE)
  {
    path.addEllipse(boundingRect());
  }
  else if (mConfig->body.shape == Types::Shape::DIAMOND)
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
  return mScaledBounds;
}

QRectF DraggableItem::drawingBorders(const QRectF& input) const
{
  return input.adjusted(1, 1, -1, -1);
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
  QPixmap pixmap(mBounds.size().toSize());
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing, false);
  painter.setRenderHint(QPainter::TextAntialiasing, false);

  QStyleOptionGraphicsItem opt;
  opt.state = QStyle::State_Active;
  paint(&painter, &opt, nullptr);

  if (mLabel)
  {
    painter.setPen(mLabel->defaultTextColor());
    painter.drawText(pixmap.rect(), Qt::AlignCenter, mLabel->toPlainText());
  }

  if (mPixmap)
  {
    QRectF rect = mBounds;
    QPointF center = rect.center();
    QPointF topLeft = center - QPointF(mPixmap->width() / 2, mPixmap->height() / 2);
    painter.drawPixmap(topLeft, *mPixmap);
  }

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
  drag->setHotSpot(QPoint(mBounds.width() / 2, mBounds.height() / 2));
  drag->exec(Qt::MoveAction);

  // Nicety, let's make the cursor show a drag action
  setCursor(Qt::ClosedHandCursor);
}

qreal DraggableItem::computeScaleFactor() const
{
  qreal widthScale = (mConfig->body.width > MAX_SIDEBAR_WIDTH)
                         ? MAX_SIDEBAR_WIDTH / mConfig->body.width
                         : 1.0;

  qreal heightScale = (mConfig->body.height > MAX_SIDEBAR_HEIGHT)
                          ? MAX_SIDEBAR_HEIGHT / mConfig->body.height
                          : 1.0;

  return qMin(widthScale, heightScale);  // Use the smallest scale to maintain aspect ratio
}
