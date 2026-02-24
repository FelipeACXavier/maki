#pragma once

#include <QGraphicsView>
#include <QKeyEvent>
#include <QWheelEvent>

class PluginView : public QGraphicsView
{
  Q_OBJECT
public:
  PluginView(QWidget* parent = nullptr);

  QPointF getCenter() const;

  qreal getScale() const;
  void setScale(qreal scale);

  void zoom(float scaleFactor);
  void centerOn(const QPointF& pos);
  void centerOn(const QGraphicsItem* item);

protected:
  void keyPressEvent(QKeyEvent*) override;
  void keyReleaseEvent(QKeyEvent*) override;

  void mouseMoveEvent(QMouseEvent*) override;
  void mousePressEvent(QMouseEvent*) override;
  void mouseReleaseEvent(QMouseEvent*) override;
  void wheelEvent(QWheelEvent*) override;

private:
  // Flags to determine if zooming or panning should be done.
  bool mDoMousePanning;
  bool mDoKeyZoom;

  QPoint mLastMousePos;
  QPointF mCenterPoint;

  qreal mPanSpeed;
  qreal mZoomDelta;
  qreal mZoomKey;
  qreal mMinZoom;
  qreal mMaxZoom;
  Qt::MouseButton mPanButton;

  void pan(QPointF delta);

  void zoomIn();
  void zoomOut();
  void resetZoom();
  qreal quantisedScale(qreal proposedScale) const;

  void setMaxSize();
};
