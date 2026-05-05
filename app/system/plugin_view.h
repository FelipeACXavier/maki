#pragma once

#include <QGraphicsView>
#include <QKeyEvent>
#include <QWheelEvent>

/**
 * @brief Custom QGraphicsView for plugin functionality.
 */
class PluginView : public QGraphicsView
{
  Q_OBJECT

public:
  /**
   * @brief Constructs a new PluginView object.
   *
   * @param parent Pointer to the parent widget.
   */
  PluginView(QWidget* parent = nullptr);

  /**
   * @brief Gets the current center point of the view.
   *
   * @return QPointF The current center point.
   */
  QPointF getCenter() const;

  /**
   * @brief Gets the current scale factor of the view.
   *
   * @return qreal The current scale factor.
   */
  qreal getScale() const;

  /**
   * @brief Sets the scale factor of the view.
   *
   * @param scale The new scale factor.
   */
  void setScale(qreal scale);

  /**
   * @brief Zooms in by a specified scale factor.
   *
   * @param scaleFactor The zoom scale factor.
   */
  void zoom(float scaleFactor);

  /**
   * @brief Centers the view on a given position.
   *
   * @param pos The position to center on.
   */
  void centerOn(const QPointF& pos);

  /**
   * @brief Centers the view on a given QGraphicsItem.
   *
   * @param item The QGraphicsItem to center on.
   */
  void centerOn(const QGraphicsItem* item);

protected:
  /**
   * @brief Handles key press events.
   *
   * @param event Pointer to the QKeyEvent object.
   */
  void keyPressEvent(QKeyEvent*) override;

  /**
   * @brief Handles key release events.
   *
   * @param event Pointer to the QKeyEvent object.
   */
  void keyReleaseEvent(QKeyEvent*) override;

  /**
   * @brief Handles mouse move events.
   *
   * @param event Pointer to the QMouseEvent object.
   */
  void mouseMoveEvent(QMouseEvent*) override;

  /**
   * @brief Handles mouse press events.
   *
   * @param event Pointer to the QMouseEvent object.
   */
  void mousePressEvent(QMouseEvent*) override;

  /**
   * @brief Handles mouse release events.
   *
   * @param event Pointer to the QMouseEvent object.
   */
  void mouseReleaseEvent(QMouseEvent*) override;

  /**
   * @brief Handles wheel events.
   *
   * @param event Pointer to the QWheelEvent object.
   */
  void wheelEvent(QWheelEvent*) override;

private:
  /// Flag to determine if mouse panning should be done.
  bool mDoMousePanning;  /// Determines whether mouse panning is enabled.

  /// Flag to determine if key zooming should be done.
  bool mDoKeyZoom;  /// Determines whether key-based zooming is enabled.

  /// Last known mouse position during panning.
  QPoint mLastMousePos;  /// Stores the last mouse position for panning.

  /// Current center point of the view.
  QPointF mCenterPoint;  /// Keeps track of the current center point of the view.

  /// Speed at which the view pans.
  qreal mPanSpeed;  /// Controls the speed of the panning action.

  /// Delta value for zooming.
  qreal mZoomDelta;  /// Defines the increment or decrement in zoom level.

  /// Key used for zooming.
  Qt::Key mZoomKey;  /// Specifies the key that triggers zoom actions.

  /// Minimum allowed zoom level.
  qreal mMinZoom;  /// Sets the lower limit for the zoom factor.

  /// Maximum allowed zoom level.
  qreal mMaxZoom;  /// Sets the upper limit for the zoom factor.

  /// Mouse button used for panning.
  Qt::MouseButton mPanButton;  /// Determines which mouse button is used for panning.

  /**
   * @brief Pans the view by a given delta.
   *
   * @param delta The delta to pan by.
   */
  void pan(QPointF delta);

  /**
   * @brief Zooms in.
   */
  void zoomIn();

  /**
   * @brief Zooms out.
   */
  void zoomOut();

  /**
   * @brief Resets the view to its default scale.
   */
  void resetZoom();

  /**
   * @brief Quantises a proposed scale factor to ensure it is within valid limits.
   *
   * @param proposedScale The proposed scale factor.
   * @return qreal The quantised scale factor.
   */
  qreal quantisedScale(qreal proposedScale) const;

  /**
   * @brief Sets the maximum size of the view's scene rect.
   */
  void setMaxSize();
};
