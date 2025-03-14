#pragma once

#include <QGraphicsView>
#include <QKeyEvent>
#include <QWheelEvent>

class CanvasView : public QGraphicsView
{
  Q_OBJECT
public:
  CanvasView(QWidget* parent = nullptr);
  qreal getScale() const;

protected:
  void keyPressEvent(QKeyEvent*) override;
  void keyReleaseEvent(QKeyEvent*) override;

  void mouseMoveEvent(QMouseEvent*) override;
  void mousePressEvent(QMouseEvent*) override;
  void mouseReleaseEvent(QMouseEvent*) override;
  void wheelEvent(QWheelEvent*) override;

  void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
  // Flags to determine if zooming or panning should be done.
  bool mDoMousePanning;
  bool mDoKeyZoom;

  QPoint mLastMousePos;
  qreal mScale;

  qreal mPanSpeed;
  qreal mZoomDelta;
  qreal mZoomKey;
  Qt::MouseButton mPanButton;

  void pan(QPointF delta);

  void zoom(float scaleFactor);
  void zoomIn();
  void zoomOut();
  void resetZoom();

  void setMaxSize();
};
