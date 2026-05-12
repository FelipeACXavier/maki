#pragma once

#include <QGraphicsView>
#include <QKeyEvent>
#include <QWheelEvent>

#include "widgets/settings_manager.h"

class Canvas;

/**
 * @brief Custom QGraphicsView for a canvas application.
 */
class CanvasView : public QGraphicsView
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a new CanvasView object.
   *
   * @param parent Pointer to the parent widget.
   */
  CanvasView(QWidget* parent = nullptr);

  /**
   * @brief Gets the current center point of the view.
   *
   * @return QPointF The center point.
   */
  QPointF getCenter() const;

  /**
   * @brief Gets the current scale factor of the view.
   *
   * @return qreal The scale factor.
   */
  qreal getScale() const;

  /**
   * @brief Sets the scale factor of the view.
   *
   * @param scale The new scale factor.
   */
  void setScale(qreal scale);

  /**
   * @brief Zooms in by a given scale factor.
   *
   * @param scaleFactor The zoom scale factor.
   */
  void zoom(float scaleFactor);

  /**
   * @brief Centers the view on a specific position.
   *
   * @param pos The position to center on.
   */
  void centerOn(const QPointF& pos);

  /**
   * @brief Centers the view on a specific QGraphicsItem.
   *
   * @param item The QGraphicsItem to center on.
   */
  void centerOn(const QGraphicsItem* item);

  /**
   * @brief Handles settings changes, specifically for appearance settings.
   *
   * @param settings The new appearance settings.
   */
  void onSettingsChanged(const AppearanceSettings& settings);

protected:
  /**
   * @brief Overrides the key press event to handle zoom and pan controls.
   *
   * @param event Pointer to the QKeyEvent object.
   */
  void keyPressEvent(QKeyEvent*) override;

  /**
   * @brief Overrides the key release event to handle zoom and pan controls.
   *
   * @param event Pointer to the QKeyEvent object.
   */
  void keyReleaseEvent(QKeyEvent*) override;

  /**
   * @brief Overrides the mouse move event to handle panning.
   *
   * @param event Pointer to the QMouseEvent object.
   */
  void mouseMoveEvent(QMouseEvent*) override;

  /**
   * @brief Overrides the mouse press event to start panning or zooming.
   *
   * @param event Pointer to the QMouseEvent object.
   */
  void mousePressEvent(QMouseEvent*) override;

  /**
   * @brief Overrides the mouse release event to stop panning.
   *
   * @param event Pointer to the QMouseEvent object.
   */
  void mouseReleaseEvent(QMouseEvent*) override;

  /**
   * @brief Overrides the wheel event to handle zooming.
   *
   * @param event Pointer to the QWheelEvent object.
   */
  void wheelEvent(QWheelEvent*) override;

  /**
   * @brief Draws the background of the view, including a grid if enabled.
   *
   * @param painter Pointer to the QPainter object.
   * @param rect The rectangle to draw the background in.
   */
  void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
  /// Flags to determine if zooming or panning should be done.
  bool mDoMousePanning;

  /// Flag to determine if key-based zooming is active.
  bool mDoKeyZoom;

  /// Flag to show the grid on the canvas.
  bool mShowGrid;

  /// Last mouse position for panning.
  QPoint mLastMousePos;

  /// Delta value for zooming.
  qreal mZoomDelta;

  /// Key used for zooming.
  qreal mZoomKey;

  /// Minimum allowed zoom level.
  qreal mMinZoom;

  /// Maximum allowed zoom level.
  qreal mMaxZoom;

  /// Mouse button used for panning.
  Qt::MouseButton mPanButton;

  /**
   * @brief Zooms in by applying a zoom factor of 1 + mZoomDelta.
   */
  void zoomIn();

  /**
   * @brief Zooms out by applying a zoom factor of 1 - mZoomDelta.
   */
  void zoomOut();

  /**
   * @brief Resets the view to its default scale.
   */
  void resetZoom();

  /**
   * @brief Quantises the proposed scale to ensure it is within valid limits and aligned with grid size.
   *
   * @param proposedScale The proposed scale factor.
   * @return qreal The quantised scale factor.
   */
  qreal quantisedScale(qreal proposedScale) const;

  /**
   * @brief Sets the maximum size of the view's scene rectangle.
   */
  void setMaxSize();
};
