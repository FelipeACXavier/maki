#include "canvas_view.h"

#define VIEW_CENTER viewport()->rect().center()
#define VIEW_WIDTH viewport()->rect().width()
#define VIEW_HEIGHT viewport()->rect().height()
#define DEFAULT_ZOOM 1.0

CanvasView::CanvasView(QWidget* parent)
{
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setAcceptDrops(true);

  setMaxSize();
  centerOn(0, 0);

  // TODO(felaze): make these configurable
  mZoomDelta = 0.1;
  mPanSpeed = 1;
  mDoMousePanning = false;
  mDoKeyZoom = false;
  mScale = DEFAULT_ZOOM;

  mPanButton = Qt::MiddleButton;
  mZoomKey = Qt::Key_Control;
}

qreal CanvasView::getScale() const
{
  return mScale;
}

void CanvasView::keyPressEvent(QKeyEvent* event)
{
  qint32 key = event->key();

  if (key == mZoomKey)
    mDoKeyZoom = true;

  if (mDoKeyZoom)
  {
    if (key == Qt::Key_Plus)
      zoomIn();
    else if (key == Qt::Key_Minus)
      zoomOut();
    else if (key == Qt::Key_Equal)
      resetZoom();
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
    pan(mapToScene(event->pos()) - mapToScene(mLastMousePos));

  QGraphicsView::mouseMoveEvent(event);
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

void CanvasView::setMaxSize()
{
  setSceneRect(INT_MIN / 2, INT_MIN / 2, INT_MAX, INT_MAX);
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
  resetTransform();
  mScale = DEFAULT_ZOOM;
}

void CanvasView::zoom(float scaleFactor)
{
  scale(scaleFactor, scaleFactor);
  mScale *= scaleFactor;
}

void CanvasView::pan(QPointF delta)
{
  // Scale the pan amount by the current zoom.
  delta *= mScale;
  delta *= mPanSpeed;

  // Have panning be anchored from the mouse.
  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  QPoint newCenter(VIEW_WIDTH / 2 - delta.x(), VIEW_HEIGHT / 2 - delta.y());
  centerOn(mapToScene(newCenter));

  // For zooming to anchor from the view center.
  setTransformationAnchor(QGraphicsView::AnchorViewCenter);
}
