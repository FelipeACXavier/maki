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
}  // namespace

PortItem::PortItem(Types::Port kind, QGraphicsItem* parentNode)
    : PortItem(kind, false, parentNode)
{
}

PortItem::PortItem(Types::Port kind, bool isMarker, QGraphicsItem* parentNode)
    : QGraphicsItem(parentNode)
    , mIsMarker(isMarker)
    , mKind(kind)
    , mCurrentIconPath("")
{
  setAcceptHoverEvents(true);
  setAcceptedMouseButtons(Qt::LeftButton);
  setFlag(QGraphicsItem::ItemStacksBehindParent, false);
  setZValue(10);
  setToolTip(tooltipForKind(kind));

  updateRenderer();
  updatePosition();
}

Types::Port PortItem::kind() const
{
  return mKind;
}

void PortItem::setMarker(bool marker)
{
  if (isMarker() == marker)
    return;

  prepareGeometryChange();

  mIsMarker = marker;

  updateRenderer();
  updatePosition();
  update();
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

bool PortItem::isMarker() const
{
  return mIsMarker;
}

bool PortItem::canStartTransition() const
{
  return isOutgoing() || (isMarker() && isIncoming());
}

bool PortItem::canEndTransition() const
{
  return isIncoming() || (isMarker() && isOutgoing());
}

qreal PortItem::getSize() const
{
  if (isMarker())
    return 30;
  else if (isAbort())
    return kAbortPortSize;
  else if (isError())
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
  if (!mRenderer || !mRenderer->isValid())
    return;

  painter->setRenderHint(QPainter::Antialiasing, false);
  painter->setRenderHint(QPainter::SmoothPixmapTransform, false);

  const QRectF target = boundingRect();
  mRenderer->render(painter, target);
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

void PortItem::setAnchorOverride(const QPointF& scenePos)
{
  mAnchorOverride = scenePos;
  updatePosition();
}

void PortItem::clearAnchorOverride()
{
  mAnchorOverride.reset();
  updatePosition();
}

void PortItem::updatePosition()
{
  if (isMarker() && mAnchorOverride)
  {
    const QPointF localCenter = parentItem()->mapFromScene(*mAnchorOverride);
    setPos(localCenter - boundingRect().center());
    return;
  }

  setPos(defaultPosition());
}

QPointF PortItem::defaultPosition() const
{
  auto* node = nodeItem();
  if (!node)
    return {};

  const QRectF portRect = node->drawingRect(node->nodeRect());

  const qreal left = portRect.left();
  const qreal top = portRect.top();
  const qreal w = portRect.width();
  const qreal h = portRect.height();

  const qreal shift = (h / 2.0 * qTan(qDegreesToRadians(30.0))) - 5.0;

  if (isIncoming())
    return {left - kSize - kGap, top + (h - kSize) / 2.0};

  if (isOut())
    return {left + w + kGap, top + (h - kSize) / 2.0};

  if (isAbort())
    return {left + shift - kAbortPortSize / 2.0, top + kGap - kAbortPortSize / 2.0};

  return {left + w / 2.0 + shift - kErrorPortSize / 2.0, top + kGap - kErrorPortSize / 2.0};
}

QString PortItem::iconPath() const
{
  switch (mKind)
  {
    case Types::Port::OUT:
      return isMarker() ? "node_success.svg" : "port_out.svg";
    case Types::Port::ABORT:
      return isMarker() ? "node_failure.svg" : "port_abort.svg";
    case Types::Port::ERROR:
      return isMarker() ? "node_failure.svg" : "port_error.svg";
    case Types::Port::IN:
    default:
      return isMarker() ? "node_start.svg" : "port_in.svg";
  }
}

void PortItem::updateRenderer()
{
  const QString path = iconPathFromTheme(iconPath());
  if (path == mCurrentIconPath)
    return;

  mCurrentIconPath = path;

  if (!mRenderer)
    mRenderer = std::make_unique<QSvgRenderer>();

  mRenderer->load(path);
  update();
}
