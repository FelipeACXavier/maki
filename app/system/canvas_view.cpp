#include "canvas_view.h"

#include <qgraphicsview.h>

#include <QShortcut>

#include "app_configs.h"
#include "canvas.h"

static constexpr qreal DEFAULT_ZOOM = 1.0;

CanvasView::CanvasView(QWidget* parent)
{
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // TODO(felaze): Evaluate the peformance impact of this
  setViewportUpdateMode(ViewportUpdateMode::FullViewportUpdate);

  setAttribute(Qt::WA_DeleteOnClose);

  // TODO(felaze): Maybe we can make this custom
  setDragMode(QGraphicsView::RubberBandDrag);
  setAcceptDrops(true);

  setMaxSize();
  centerOn({0, 0});

  // TODO(felaze): make these configurable
  mZoomDelta = 0.2;
  mPanSpeed = 1;
  mDoMousePanning = false;
  mDoKeyZoom = false;

  mMinZoom = 0.2;
  mMaxZoom = 10;

  mPanButton = Qt::MiddleButton;
  mZoomKey = Qt::Key_Control;
}

qreal CanvasView::getScale() const
{
  return transform().m11();
}

QPointF CanvasView::getCenter() const
{
  return mCenterPoint;
}

void CanvasView::centerOn(const QGraphicsItem* item)
{
  QGraphicsView::centerOn(item);
}

void CanvasView::centerOn(const QPointF& pos)
{
  mCenterPoint = pos;
  QGraphicsView::centerOn(mCenterPoint);
}

void CanvasView::setScale(qreal scale)
{
  QTransform t;
  const double snapped = quantisedScale(scale);
  t.scale(snapped, snapped);
  setTransform(t);
  update();
}

void CanvasView::setMaxSize()
{
  setSceneRect(INT_MIN / 2, INT_MIN / 2, INT_MAX, INT_MAX);
}

void CanvasView::keyPressEvent(QKeyEvent* event)
{
  qint32 key = event->key();

  if (key == mZoomKey)
    mDoKeyZoom = true;

  if (mDoKeyZoom)
  {
    switch (key)
    {
      case Qt::Key_Plus:
        zoomIn();
        break;
      case Qt::Key_Minus:
        zoomOut();
        break;
      case Qt::Key_Equal:
        resetZoom();
        break;
    }
  }
  else
  {
    QGraphicsView::keyPressEvent(event);
  }
}

void CanvasView::keyReleaseEvent(QKeyEvent* event)
{
  if (event->key() == mZoomKey)
    mDoKeyZoom = false;

  QGraphicsView::keyReleaseEvent(event);
}

void CanvasView::mouseMoveEvent(QMouseEvent* event)
{
  if (mDoMousePanning)
  {
    pan(mapToScene(event->pos()) - mapToScene(mLastMousePos));
    event->accept();
  }
  else
  {
    QGraphicsView::mouseMoveEvent(event);
  }

  mLastMousePos = event->pos();
}

void CanvasView::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == mPanButton)
  {
    setCursor(Qt::ClosedHandCursor);
    mLastMousePos = event->pos();
    mDoMousePanning = true;
  }

  QGraphicsView::mousePressEvent(event);
}

void CanvasView::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() == mPanButton)
  {
    setCursor(Qt::ArrowCursor);
    mDoMousePanning = false;
  }

  QGraphicsView::mouseReleaseEvent(event);
}

void CanvasView::wheelEvent(QWheelEvent* event)
{
  QPoint scrollAmount = event->angleDelta();

  // Apply zoom.
  scrollAmount.y() > 0 ? zoomIn() : zoomOut();
}

void CanvasView::zoomIn()
{
  zoom(1 + mZoomDelta);
}

void CanvasView::zoomOut()
{
  zoom(1 - mZoomDelta);
}

void CanvasView::resetZoom()
{
  QTransform t;
  const double snapped = quantisedScale(DEFAULT_ZOOM);
  t.scale(snapped, snapped);
  setTransform(t);
}

void CanvasView::zoom(float scaleFactor)
{
  const qreal cur = getScale();
  const qreal target = cur * static_cast<qreal>(scaleFactor);
  const qreal snapped = quantisedScale(target);

  // Compute relative factor to apply on top of current transform
  const double rel = snapped / cur;
  if (qFuzzyCompare(rel, 1.0))
    return;

  scale(rel, rel);
  update();
}

qreal CanvasView::quantisedScale(qreal proposedScale) const
{
  const qreal dpr = devicePixelRatioF();
  const qreal S = Config::GRID_SIZE * proposedScale * dpr;

  const qreal k = std::max(1.0, std::round(S));  // avoid 0
  const qreal snapped = (k / (Config::GRID_SIZE * dpr));

  // Clamp to sane limits
  return std::clamp(snapped, mMinZoom, mMaxZoom);
}

void CanvasView::pan(QPointF delta)
{
  // Scale the pan amount by the current zoom.
  delta *= getScale();
  delta *= mPanSpeed;

  // Have panning be anchored from the mouse.
  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

  QPoint newCenter(qreal(viewport()->rect().width()) / 2 - delta.x(), qreal(viewport()->rect().height()) / 2 - delta.y());
  centerOn(mapToScene(newCenter));

  // For zooming to anchor from the view center.
  setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}

void CanvasView::drawBackground(QPainter* p, const QRectF& /* rect */)
{
  // Save so we dont affect the following calls
  p->save();
  p->setWorldTransform(QTransform());
  p->setRenderHint(QPainter::Antialiasing, false);

  QPen pen(Qt::gray, 0.5, Qt::DotLine);
  pen.setCapStyle(Qt::SquareCap);
  pen.setCosmetic(true);  // Keep line thickness fixed

  p->setPen(pen);

  // Viewport rectangle in device pixels (HiDPI aware)
  const qreal dpr = this->devicePixelRatioF();
  const QRect vp = viewport()->rect();

  const int stepDev = std::max(1, static_cast<int>(std::lround(Config::GRID_SIZE * dpr)));

  for (int xDev = (vp.left() * dpr); xDev <= (vp.right() * dpr); xDev += stepDev)
  {
    const qreal x = xDev / dpr;
    p->drawLine(QPointF(x, vp.top()), QPointF(x, vp.bottom()));
  }

  for (int yDev = (vp.top() * dpr); yDev <= (vp.bottom() * dpr); yDev += stepDev)
  {
    const qreal y = yDev / dpr;
    p->drawLine(QPointF(vp.left(), y), QPointF(vp.right(), y));
  }

  p->restore();
}