#include "draggable.h"

#include <QBuffer>
#include <QDrag>
#include <QDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QStyle>
#include <QStyleOptionGraphicsItem>

#include "app_configs.h"

DraggableItem::DraggableItem(const QString& id, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent)
    : NodeBase(nodeConfig, parent)
    , mNodeId(id)
{
  if (!config()->body.iconPath.isEmpty())
  {
    QPixmap icon(config()->body.iconPath);
    setPixmap(icon.scaled(scaledRect().size().toSize() * config()->body.iconScale, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  }
  else
    setLabel(nodeType(), config()->body.textColor);
}

DraggableItem::~DraggableItem()
{
}

QString DraggableItem::nodeId() const
{
  return mNodeId;
}

int DraggableItem::type() const
{
  return Type;
}

QRectF DraggableItem::boundingRect() const
{
  return scaledRect();
}

void DraggableItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  Q_UNUSED(widget);
  NodeBase::paintNode((style && style->state == QStyle::State_Active) ? boundingRect() : scaledRect(),
                      config()->body.backgroundColor,
                      QPen(config()->body.borderColor),
                      painter);
}

QPainterPath DraggableItem::shape() const
{
  return NodeBase::nodeShape(scaledRect());
}

void DraggableItem::adjustWidth(int width)
{
  int centerX = (width - boundingRect().width()) / 2;
  setPos(centerX, pos().y());

  updateLabelPosition();
}

void DraggableItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  // Draggable pixmap from the scale
  QPixmap pixmap(scaledRect().size().toSize());
  pixmap.fill(Qt::transparent);

  QPainter painter(&pixmap);
  painter.setRenderHint(QPainter::Antialiasing, false);
  painter.setRenderHint(QPainter::TextAntialiasing, false);

  QStyleOptionGraphicsItem opt;
  opt.state = QStyle::State_Active;
  paint(&painter, &opt, nullptr);
  paintLabel(&painter, pixmap.rect());

  // Save data
  QMimeData* mimeData = new QMimeData();
  mimeData->setData(Constants::TYPE_NODE, QByteArray());
  mimeData->setData(Constants::TYPE_NODE_ID, nodeId().toUtf8());

  QByteArray pixmapData;
  if (SerializeIcon(&pixmapData))
    mimeData->setData(Constants::TYPE_PIXMAP, pixmapData);

  // Create drag itself
  QDrag* drag = new QDrag(event->widget());
  drag->setMimeData(mimeData);
  drag->setPixmap(pixmap);
  drag->setHotSpot(QPoint(scaledRect().width() / 2, scaledRect().height() / 2));
  drag->exec(Qt::MoveAction);

  // Nicety, let's make the cursor show a drag action
  setCursor(Qt::ClosedHandCursor);
}

bool DraggableItem::SerializeIcon(QByteArray* pixmapData) const
{
  if (!mPixmapItem)
    return false;

  // Get pixmap from the parent, i.e., not scaled
  QPixmap iconPixmap(NodeBase::boundingRect().size().toSize());
  iconPixmap.fill(Qt::transparent);

  QPainter iconPainter(&iconPixmap);
  iconPainter.setRenderHint(QPainter::Antialiasing, false);
  iconPainter.setRenderHint(QPainter::TextAntialiasing, false);
  paintPixmap(&iconPainter);

  // Serialize it so it is copied
  QBuffer buffer(pixmapData);
  buffer.open(QIODevice::WriteOnly);
  iconPixmap.save(&buffer, "PNG");

  return true;
}
