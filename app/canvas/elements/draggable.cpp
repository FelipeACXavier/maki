#include "draggable.h"

#include <QBuffer>
#include <QDrag>
#include <QDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include <QUuid>

#include "app_configs.h"
#include "document/node_info.h"
#include "keys.h"
#include "style_helpers.h"

DraggableItem::DraggableItem(const QString& instanceId, const QString& nodeId, std::shared_ptr<NodeConfig> nodeConfig, bool draggable, QGraphicsItem* parent)
    : NodeBase(instanceId.isEmpty() ? QUuid::createUuid().toString() : instanceId, nodeId, nodeConfig, parent)
    , mDraggable(draggable)
{
  setAcceptHoverEvents(true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setLabel(nameFromNodeId(config()->type), Fonts::BaseSize);
}

DraggableItem::~DraggableItem()
{
}

int DraggableItem::type() const
{
  return Type;
}

QRectF DraggableItem::nodeRect() const
{
  return scaledRect();
}

void DraggableItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget)
{
  Q_UNUSED(widget);
  if (config()->libraryType != Types::LibraryTypes::STRUCTURAL || config()->body.iconPath.isEmpty())
  {
    NodeBase::paintNode(nodeRect(), config()->body.backgroundColor, isSelected() ? QPen(Config::HIGHLIGHT, 2.0) : QPen(Config::FOREGROUND, 1.0), painter);
  }
  else
  {
    // I think this can become its own function
    oclero::qlementine::Theme theme;
    if (auto qlementinestyle = oclero::qlementine::appStyle())
      theme = qlementinestyle->theme();

    const auto background = theme.neutralColor;
    const QPen pen = QPen(Config::FOREGROUND, 1.0);

    painter->setPen(pen);
    painter->setBrush(QBrush(background));

    const auto rect = scaledRect();
    if (config()->type != ConfigKeys::TASK_NODE)
    {
      const QRectF r = rect.adjusted(2, 2, -2, -2);
      painter->drawEllipse(r);
    }
    paintSvg(mRenderer, painter, nodeRect().center(), rect.width(), rect.height());

    NodeBase::paintLabel(painter, labelBoundingRect(), pen);
  }
}

QPainterPath DraggableItem::shape() const
{
  return NodeBase::nodeShape(scaledRect());
}

void DraggableItem::adjustWidth(int width)
{
  int centerX = (width - boundingRect().width()) / 2;
  setPos(centerX, pos().y());
}

Qt::DropAction DraggableItem::startDrag(QGraphicsSceneMouseEvent* event)
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

  NodeSaveInfo info(*config());
  info.setNodeId(nodeId());
  info.setIcon(nodeIcon());

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

  // Nicety, let's make the cursor show a drag action
  setCursor(Qt::DragMoveCursor);
  const Qt::DropAction result = drag->exec(Qt::MoveAction);
  setCursor(Qt::ArrowCursor);

  return drag->target() ? Qt::MoveAction : Qt::IgnoreAction;
}

void DraggableItem::handleClick(QGraphicsSceneMouseEvent* event)
{
  setSelected(true);
  update();
}

void DraggableItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  setCursor(mDraggable ? Qt::OpenHandCursor : Qt::PointingHandCursor);
}

void DraggableItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  setCursor(Qt::ArrowCursor);
}
