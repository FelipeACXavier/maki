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
#include "style_helpers.h"
#include "theme.h"
#include "app_paths.h"

namespace
{
constexpr qreal kTaskCornerRadius = 28.0;
constexpr qreal kTaskInnerPadding = 6.0;
constexpr qreal kTaskSlotDiameterFactor = 0.30;
constexpr qreal kTaskSlotTopY = 0.30;
constexpr qreal kTaskSlotBottomY = 0.70;
constexpr qreal kTaskSlotLeftX = 0.30;
constexpr qreal kTaskSlotRightX = 0.70;
const QColor kTaskSlotColor = QColor("#d9d9d9");
}  // namespace

DraggableItem::DraggableItem(const QString& nodeId, std::shared_ptr<NodeConfig> nodeConfig, QGraphicsItem* parent)
    : NodeBase(QUuid::createUuid().toString(), nodeId, nodeConfig, parent)
{
  setFlag(QGraphicsItem::ItemIsSelectable, true);

  if (config()->body.nodeSvg.isEmpty() && !config()->body.iconPath.isEmpty())
  {
    setIcon(AppPaths::icon(config()->body.iconPath), config()->body.iconColor);
  }

  setLabel(config()->type, Fonts::BaseSize);
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
  const QRectF rect = (style && style->state == QStyle::State_Active) ? boundingRect() : scaledRect();

  if (config()->libraryType == Types::LibraryTypes::STRUCTURAL && config()->type == QStringLiteral("Task"))
  {
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(isSelected() ? QPen(Config::HIGHLIGHT, 2.0) : QPen(Config::FOREGROUND, 1.0));
    painter->setBrush(Qt::NoBrush);

    const QRectF bodyRect = rect.adjusted(kTaskInnerPadding, kTaskInnerPadding, -kTaskInnerPadding, -kTaskInnerPadding);
    painter->drawRoundedRect(bodyRect, kTaskCornerRadius, kTaskCornerRadius);

    const qreal slotDiameter = qMin(bodyRect.width(), bodyRect.height()) * kTaskSlotDiameterFactor;
    const qreal slotRadius = slotDiameter * 0.5;
    painter->setPen(Qt::NoPen);
    painter->setBrush(kTaskSlotColor);

    const QVector<QPointF> centers = {
        QPointF(bodyRect.left() + bodyRect.width() * kTaskSlotLeftX, bodyRect.top() + bodyRect.height() * kTaskSlotTopY),
        QPointF(bodyRect.left() + bodyRect.width() * kTaskSlotRightX, bodyRect.top() + bodyRect.height() * kTaskSlotTopY),
        QPointF(bodyRect.left() + bodyRect.width() * kTaskSlotLeftX, bodyRect.top() + bodyRect.height() * kTaskSlotBottomY),
        QPointF(bodyRect.left() + bodyRect.width() * kTaskSlotRightX, bodyRect.top() + bodyRect.height() * kTaskSlotBottomY),
    };
    for (const QPointF& c : centers)
      painter->drawEllipse(c, slotRadius, slotRadius);
    return;
  }

  NodeBase::paintNode(rect,
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
