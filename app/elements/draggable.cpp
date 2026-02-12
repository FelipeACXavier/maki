#include "draggable.h"

#include <QBuffer>
#include <QDrag>
#include <QDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include <QUuid>

#include "app_configs.h"
#include "logging.h"
#include "save_info.h"
#include "theme.h"

DraggableItem::DraggableItem(const QString& nodeId, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent)
    : NodeBase(QUuid::createUuid().toString(), nodeId, nodeConfig, parent)
{
  setFlag(QGraphicsItem::ItemIsSelectable, true);

  if (!config()->body.iconPath.isEmpty())
  {
    QPixmap icon(config()->body.iconPath);
    setPixmap(icon.scaled(scaledRect().size().toSize() * config()->body.iconScale, Qt::KeepAspectRatio, Qt::SmoothTransformation));
  }
  else
  {
    setLabel(config()->type, Fonts::BaseSize);
  }
}

DraggableItem::~DraggableItem()
{
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
                      isSelected() ? QPen(Config::HIGHLIGHT, 2.0) : QPen(Config::FOREGROUND, 1.0),
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

void DraggableItem::startDrag(QGraphicsSceneMouseEvent* event)
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

  NodeSaveInfo info;
  info.setNodeId(nodeId());
  info.setPixmap(nodePixmap());
  info.setSize(QSize(config()->body.width, config()->body.height));

  QByteArray data;
  QDataStream stream(&data, QIODevice::WriteOnly);
  stream << info;

  QMimeData* mimeData = new QMimeData();
  mimeData->setData(Constants::TYPE_NODE, data);

  // Create drag itself
  QDrag* drag = new QDrag(event->widget());
  drag->setMimeData(mimeData);
  drag->setPixmap(pixmap);
  drag->setHotSpot(QPoint(scaledRect().width() / 2, scaledRect().height() / 2));
  drag->exec(Qt::MoveAction);

  // Nicety, let's make the cursor show a drag action
  setCursor(Qt::ClosedHandCursor);
}

void DraggableItem::handleClick(QGraphicsSceneMouseEvent* event)
{
  setSelected(true);
  update();
}
