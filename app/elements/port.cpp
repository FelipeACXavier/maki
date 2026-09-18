#include "port.h"

#include <QCursor>
#include <QGraphicsSceneHoverEvent>
#include <QObject>
#include <QPainter>
#include <QSvgRenderer>
#include <memory>

#include "app_paths.h"
#include "node.h"
#include "style_helpers.h"

namespace
{
QString iconPathForKind(Types::Port kind)
{
  switch (kind)
  {
    case Types::Port::OUT:
      return QStringLiteral("port_out.svg");
    case Types::Port::ABORT:
      return QStringLiteral("port_abort.svg");
    case Types::Port::ERROR:
      return QStringLiteral("port_error.svg");
    case Types::Port::IN:
    default:
      return QStringLiteral("port_in.svg");
  }
  return QString();
}

QString tooltipForKind(Types::Port kind)
{
  switch (kind)
  {
    case Types::Port::ABORT:
      return QObject::tr("Define how to handle an abort command.");
    case Types::Port::ERROR:
      return QObject::tr("Define how to handle an error.");
    default:
      return {};
  }
}

QSvgRenderer* rendererForKind(Types::Port kind)
{
  static std::map<Types::Port, std::unique_ptr<QSvgRenderer>> renderers;
  if (!renderers.contains(kind))
  {
    const QString path = iconPathFromTheme(iconPathForKind(kind));
    renderers[kind] = std::make_unique<QSvgRenderer>(path);
  }

  return renderers[kind].get();
}
}  // namespace

PortItem::PortItem(Types::Port kind, QGraphicsItem* parentNode)
    : QGraphicsItem(parentNode)
    , mKind(kind)
{
  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::LeftButton);
  setFlag(QGraphicsItem::ItemStacksBehindParent, false);
  setZValue(10);
  setToolTip(tooltipForKind(kind));

  if (auto* node = qgraphicsitem_cast<NodeItem*>(parentNode))
  {
    const QRectF portRect = node->drawingRect(node->nodeRect());
    const qreal left = portRect.left();
    const qreal top = portRect.top();
    const qreal w = portRect.width();
    const qreal h = portRect.height();
    const qreal shift = (h / 2 * qTan(qDegreesToRadians(30))) - 5;
    qreal x = 0;
    qreal y = 0;
    if (isIncoming())
    {
      x = left - PortItem::kSize - PortItem::kGap;
      y = top + (h - PortItem::kSize) / 2.0;
    }
    else if (isOut())
    {
      x = left + w + PortItem::kGap;
      y = top + (h - PortItem::kSize) / 2.0;
    }
    else if (isAbort())
    {
      x = left + (shift - PortItem::kAbortPortSize / 2);
      y = top + PortItem::kGap - (PortItem::kAbortPortSize / 2);
    }
    else
    {
      x = left + w / 2 + (shift - PortItem::kErrorPortSize / 2);
      y = top + PortItem::kGap - (PortItem::kErrorPortSize / 2);
    }

    setPos(x, y);
  }
}

Types::Port PortItem::kind() const
{
  return mKind;
}

bool PortItem::isIncoming() const
{
  return mKind == Types::Port::IN;
}

bool PortItem::isOutgoing() const
{
  return mKind != Types::Port::IN;
}

bool PortItem::isOut() const
{
  return mKind == Types::Port::OUT;
}

bool PortItem::isAbort() const
{
  return mKind == Types::Port::ABORT;
}

bool PortItem::isError() const
{
  return mKind == Types::Port::ERROR;
}

qreal PortItem::getSize() const
{
  if (isAbort())
    return kAbortPortSize;
  if (isError())
    return kErrorPortSize;

  return kSize;
}

QString PortItem::defaultTransitionEvent() const
{
  switch (mKind)
  {
    case Types::Port::ABORT:
      return QString("on abort");
    case Types::Port::ERROR:
      return QString("on error");
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
  const qreal s = getSize();
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

  const QRectF target = boundingRect();
  r->render(painter, target);
}

NodeItem* PortItem::nodeItem() const
{
  return static_cast<NodeItem*>(parentItem());
}

QPointF PortItem::anchorScenePos() const
{
  return mapToScene(boundingRect().center());
}

void PortItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  if (isOutgoing())
    setCursor(Qt::CrossCursor);
  QGraphicsItem::hoverEnterEvent(event);
}

void PortItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  unsetCursor();
  QGraphicsItem::hoverLeaveEvent(event);
}