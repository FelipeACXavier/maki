#include "node_base.h"

#include <QPainter>
#include <QStyleOptionGraphicsItem>

const qreal MAX_WIDTH = 100.0;
const qreal MAX_HEIGHT = 100.0;

NodeBase::NodeBase(const QString& id, const QString& nodeId, std::shared_ptr<NodeConfig> config, QGraphicsItem* parent)
    : QGraphicsItem(parent)
    , mConfig(config)
    , mId(id)
    , mBounds(0, 0, mConfig->body.width, mConfig->body.height)
    , mNodeId(nodeId)
{
  setZValue(mConfig->body.zIndex);

  qreal scaleFactor = computeScaleFactor();
  mScaledBounds = QRectF(0, 0, mConfig->body.width * scaleFactor, mConfig->body.height * scaleFactor);
}

QString NodeBase::id() const
{
  return mId;
}

int NodeBase::type() const
{
  return Type;
}

QString NodeBase::nodeId() const
{
  return mNodeId;
}

QString NodeBase::nodeType() const
{
  return config()->type;
}

VoidResult NodeBase::start()
{
  return VoidResult();
}

std::shared_ptr<NodeConfig> NodeBase::config() const
{
  return mConfig;
}

QRectF NodeBase::boundingRect() const
{
  return mBounds;
}

QRectF NodeBase::scaledRect() const
{
  return mScaledBounds;
}

QRectF NodeBase::drawingRect(const QRectF& input) const
{
  return input.adjusted(2, 2, -2, -2);
}

void NodeBase::paintNode(const QRectF& bounds, const QColor& background, const QPen& text, QPainter* painter)
{
  painter->setPen(text);
  painter->setBrush(background);
  painter->setRenderHint(QPainter::Antialiasing, false);

  const auto drawingBounds = drawingRect(bounds);
  if (mConfig->body.shape == Types::Shape::RECTANGLE)
  {
    painter->drawRect(drawingBounds);
  }
  else if (mConfig->body.shape == Types::Shape::ELLIPSE)
  {
    painter->drawEllipse(drawingBounds);
  }
  else if (mConfig->body.shape == Types::Shape::DIAMOND)
  {
    QPolygonF diamond;
    diamond << QPointF(drawingBounds.center().x(), drawingBounds.top())     // Top
            << QPointF(drawingBounds.right(), drawingBounds.center().y())   // Right
            << QPointF(drawingBounds.center().x(), drawingBounds.bottom())  // Bottom
            << QPointF(drawingBounds.left(), drawingBounds.center().y());   // Left

    painter->drawPolygon(diamond);
  }
  else
  {
    painter->drawRoundedRect(drawingBounds, 5, 5);  // 10 is the radius of the corners
  }

  paintPixmap(painter);
}

QPainterPath NodeBase::nodeShape(const QRectF& bounds) const
{
  QPainterPath path;
  if (mConfig->body.shape == Types::Shape::RECTANGLE)
  {
    path.addRect(bounds);
  }
  else if (mConfig->body.shape == Types::Shape::ELLIPSE)
  {
    path.addEllipse(bounds);
  }
  else if (mConfig->body.shape == Types::Shape::DIAMOND)
  {
    QPolygonF diamond;
    diamond << QPointF(bounds.center().x(), bounds.top())     // Top
            << QPointF(bounds.right(), bounds.center().y())   // Right
            << QPointF(bounds.center().x(), bounds.bottom())  // Bottom
            << QPointF(bounds.left(), bounds.center().y());   // Left

    path.addPolygon(diamond);
  }
  else
  {
    path.addRoundedRect(bounds, config()->body.borderRadius, config()->body.borderRadius);  // 10 is the corner radius
  }

  return path;
}

void NodeBase::paintLabel(QPainter* painter, const QRectF& area) const
{
  if (!mLabel)
    return;

  painter->setPen(mLabel->defaultTextColor());
  painter->drawText(area, Qt::AlignCenter, mLabel->toPlainText());
}

void NodeBase::paintPixmap(QPainter* painter) const
{
  if (!mPixmapItem)
    return;

  QRectF rect = boundingRect();
  QPointF center = rect.center();
  QPointF topLeft = center - QPointF((qreal)mPixmapItem->pixmap().width() / 2, (qreal)mPixmapItem->pixmap().height() / 2);
  painter->drawPixmap(topLeft, mPixmapItem->pixmap());
}

void NodeBase::setLabel(const QString& name, const QColor& color)
{
  mLabel = std::make_shared<QGraphicsTextItem>(name, this);
  mLabel->setDefaultTextColor(color);

  updateLabelPosition();
}

void NodeBase::setPixmap(const QPixmap& pixmap)
{
  mPixmapItem = std::make_shared<QGraphicsPixmapItem>(pixmap);
}

void NodeBase::updateLabelPosition()
{
  if (!mLabel)
    return;

  QRectF textBounds = mLabel->boundingRect();

  // Calculate centered position
  qreal x = boundingRect().center().x() - textBounds.width() / 2;
  qreal y = boundingRect().center().y() - textBounds.height() / 2;

  mLabel->setPos(x, y);
}

qreal NodeBase::computeScaleFactor() const
{
  qreal widthScale = (mConfig->body.width > MAX_WIDTH)
                         ? MAX_WIDTH / mConfig->body.width
                         : 1.0;

  qreal heightScale = (mConfig->body.height > MAX_HEIGHT)
                          ? MAX_HEIGHT / mConfig->body.height
                          : 1.0;

  return qMin(widthScale, heightScale);  // Use the smallest scale to maintain aspect ratio
}

QPixmap NodeBase::nodePixmap() const
{
  if (mPixmapItem)
    return mPixmapItem->pixmap();

  return QPixmap();
}
