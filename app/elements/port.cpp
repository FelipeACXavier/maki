#include "port.h"

#include <QCursor>
#include <QGraphicsSceneHoverEvent>
#include <QPainter>
#include <QSvgRenderer>

#include <memory>

#include "app_paths.h"
#include "node.h"

namespace
{
QString iconPathForKind(PortItem::Kind kind)
{
  switch (kind)
  {
    case PortItem::In:
      return QStringLiteral("port_in.svg");
    case PortItem::Out:
      return QStringLiteral("port_out.svg");
    case PortItem::Abort:
      return QStringLiteral("port_abort.svg");
    case PortItem::Error:
      return QStringLiteral("port_error.svg");
  }
  return QString();
}

QSvgRenderer* rendererForKind(PortItem::Kind kind)
{
  static std::unique_ptr<QSvgRenderer> renderers[4];
  const int index = static_cast<int>(kind);
  if (index < 0 || index >= 4)
    return nullptr;

  const QString path = AppPaths::icon(iconPathForKind(kind));
  if (!renderers[index] || !renderers[index]->isValid())
    renderers[index] = std::make_unique<QSvgRenderer>(path);

  return (renderers[index] && renderers[index]->isValid()) ? renderers[index].get() : nullptr;
}
}  // namespace

qreal PortItem::sizeForKind(Kind kind)
{
  if (kind == Abort)
    return kAbortPortSize;
  if (kind == Error)
    return kErrorPortSize;
  return kSize;
}

PortItem::PortItem(Kind kind, NodeItem* parentNode)
    : QGraphicsItem(parentNode)
    , mKind(kind)
{
  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::LeftButton);
  setFlag(QGraphicsItem::ItemStacksBehindParent, false);
  setZValue(10);
}

QString PortItem::defaultTransitionEvent() const
{
  switch (mKind)
  {
    case Abort:
      return QStringLiteral("on abort");
    case Error:
      return QStringLiteral("on error");
    default:
      return QString();
  }
}

QString PortItem::defaultTransitionLabel() const
{
  return defaultTransitionEvent();
}

int PortItem::type() const
{
  return Types::PORT;
}

QRectF PortItem::boundingRect() const
{
  const qreal s = sizeForKind(mKind);
  return QRectF(0, 0, s, s);
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
  const qreal s = sizeForKind(mKind);
  return mapToScene(QPointF(s / 2.0, s / 2.0));
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
