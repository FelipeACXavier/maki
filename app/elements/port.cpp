#include "port.h"

#include <QCursor>
#include <memory>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QSvgRenderer>

#include "app_paths.h"
#include "node.h"

namespace
{
QSvgRenderer* rendererForKind(PortItem::Kind kind)
{
  static std::unique_ptr<QSvgRenderer> inRenderer;
  static std::unique_ptr<QSvgRenderer> outRenderer;
  const QString path = AppPaths::icon(kind == PortItem::In ? QStringLiteral("port_in.svg") : QStringLiteral("port_out.svg"));
  std::unique_ptr<QSvgRenderer>& slot = (kind == PortItem::In) ? inRenderer : outRenderer;
  if (!slot || !slot->isValid())
    slot = std::make_unique<QSvgRenderer>(path);
  return (slot && slot->isValid()) ? slot.get() : nullptr;
}
}  // namespace

PortItem::PortItem(Kind kind, NodeItem* parentNode)
    : QGraphicsItem(parentNode)
    , mKind(kind)
{
  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::LeftButton);
  setFlag(QGraphicsItem::ItemStacksBehindParent, false);
  setZValue(10);
}

int PortItem::type() const
{
  return Types::PORT;
}

QRectF PortItem::boundingRect() const
{
  return QRectF(0, 0, kSize, kSize);
}

QPainterPath PortItem::shape() const
{
  QPainterPath p;
  p.addRect(boundingRect());
  return p;
}

void PortItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* /*option*/, QWidget* /*widget*/)
{
  QSvgRenderer* r = rendererForKind(mKind);
  if (!r)
    return;

  painter->setRenderHint(QPainter::Antialiasing, false);
  painter->setRenderHint(QPainter::SmoothPixmapTransform, false);
  r->render(painter, boundingRect());
}

NodeItem* PortItem::nodeItem() const
{
  return static_cast<NodeItem*>(parentItem());
}

QPointF PortItem::anchorScenePos() const
{
  return mapToScene(QPointF(kSize / 2.0, kSize / 2.0));
}

void PortItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  setCursor(Qt::CrossCursor);
  QGraphicsItem::hoverEnterEvent(event);
}

void PortItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  unsetCursor();
  QGraphicsItem::hoverLeaveEvent(event);
}
