#include "plugin_view.h"

#include <QShortcut>

#include "app_configs.h"
#include "style_helpers.h"

static constexpr qreal DEFAULT_ZOOM = 1.0;

PluginView::PluginView(QWidget* parent)
    // TODO(felaze): this class should inherit from CanvasView
    : mDoMousePanning(false)
    , mDoKeyZoom(false)
    , mPanSpeed(1)
    , mZoomDelta(0.2)
    , mZoomKey(Qt::Key_Control)
    , mMinZoom(0.2)
    , mMaxZoom(10)
    , mPanButton(Qt::MiddleButton)
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
}

qreal PluginView::getScale() const
{
  return transform().m11();
}

QPointF PluginView::getCenter() const
{
  return mCenterPoint;
}

void PluginView::centerOn(const QGraphicsItem* item)
{
  QGraphicsView::centerOn(item);
}

void PluginView::centerOn(const QPointF& pos)
{
  mCenterPoint = pos;
  QGraphicsView::centerOn(mCenterPoint);
}

void PluginView::setScale(qreal scale)
{
  QTransform t;
  const double snapped = quantisedScale(scale);
  t.scale(snapped, snapped);
  setTransform(t);
  update();
}

void PluginView::setMaxSize()
{
  setSceneRect(INT_MIN / 2, INT_MIN / 2, INT_MAX, INT_MAX);
}

void PluginView::keyPressEvent(QKeyEvent* event)
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

void PluginView::keyReleaseEvent(QKeyEvent* event)
{
  if (event->key() == mZoomKey)
    mDoKeyZoom = false;

  QGraphicsView::keyReleaseEvent(event);
}

void PluginView::mouseMoveEvent(QMouseEvent* event)
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

void PluginView::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == mPanButton)
  {
    setCursor(Qt::ClosedHandCursor);
    mLastMousePos = event->pos();
    mDoMousePanning = true;
  }

  QGraphicsView::mousePressEvent(event);
}

void PluginView::mouseReleaseEvent(QMouseEvent* event)
{
  if (event->button() == mPanButton)
  {
    setCursor(Qt::ArrowCursor);
    mDoMousePanning = false;
  }

  QGraphicsView::mouseReleaseEvent(event);
}

void PluginView::wheelEvent(QWheelEvent* event)
{
  QPoint scrollAmount = event->angleDelta();

  // Apply zoom.
  scrollAmount.y() > 0 ? zoomIn() : zoomOut();
}

void PluginView::zoomIn()
{
  zoom(1 + mZoomDelta);
}

void PluginView::zoomOut()
{
  zoom(1 - mZoomDelta);
}

void PluginView::resetZoom()
{
  QTransform t;
  const double snapped = quantisedScale(DEFAULT_ZOOM);
  t.scale(snapped, snapped);
  setTransform(t);
}

void PluginView::zoom(float scaleFactor)
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

qreal PluginView::quantisedScale(qreal proposedScale) const
{
  const qreal dpr = devicePixelRatioF();
  const qreal S = Config::GRID_SIZE * proposedScale * dpr;

  const qreal k = std::max(1.0, std::round(S));  // avoid 0
  const qreal snapped = (k / (Config::GRID_SIZE * dpr));

  // Clamp to sane limits
  return std::clamp(snapped, mMinZoom, mMaxZoom);
}

void PluginView::pan(QPointF delta)
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
