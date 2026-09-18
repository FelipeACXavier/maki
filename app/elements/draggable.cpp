#include "draggable.h"

#include <QBuffer>
#include <QDrag>
#include <QDropEvent>
#include <QGraphicsSceneMouseEvent>
#include <QStyle>
#include <QStyleOptionGraphicsItem>
#include <QUuid>

#include "app_configs.h"
#include "keys.h"
#include "save_info.h"
#include "style_helpers.h"

DraggableItem::DraggableItem(const QString& nodeId, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent)
    : NodeBase(QUuid::createUuid().toString(), nodeId, nodeConfig, parent)
{
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

    NodeBase::paintLabel(painter, rect, pen);
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
  paintLabel(&painter, pixmap.rect(), QPen(Config::FOREGROUND));

  NodeSaveInfo info;
  info.setNodeId(nodeId());
  info.setIcon(nodeIcon());
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
