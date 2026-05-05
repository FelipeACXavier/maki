#pragma once

#include <QApplication>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QMouseEvent>

#include "elements/draggable.h"

/**
 * @brief A custom QGraphicsScene for a library application.
 */
class LibraryScene : public QGraphicsScene
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a new LibraryScene object.
   *
   * @param parent The parent QObject, if any.
   */
  LibraryScene(QObject* parent = nullptr);

  /**
   * @brief Returns the bounding rectangle of all visible items in the scene.
   *
   * @return QRectF The bounding rectangle.
   */
  QRectF visibleItemsBounds() const;

  /**
   * @brief Filters nodes based on a query string.
   *
   * @param query The query string to filter nodes by.
   * @return bool True if any nodes were filtered, false otherwise.
   */
  bool filterNodes(const QString& query);

  /**
   * @brief Relayouts the visible items in the scene.
   */
  void relayoutVisibleItems();

signals:
  /**
   * @brief Emitted when a library node is selected.
   *
   * @param nodeType The type of the selected node.
   */
  void libraryNodeSelected(const QString& nodeType);

protected:
  /**
   * @brief Handles mouse press events in the scene.
   *
   * @param event The QGraphicsSceneMouseEvent object.
   */
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  /**
   * @brief Handles mouse move events in the scene.
   *
   * @param event The QGraphicsSceneMouseEvent object.
   */
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

  /**
   * @brief Handles mouse release events in the scene.
   *
   * @param event The QGraphicsSceneMouseEvent object.
   */
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  /**
   * @brief Handles context menu events in the scene.
   *
   * @param event The QGraphicsSceneContextMenuEvent object.
   */
  void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

private:
  QPointF mPressScenePos; /// Scene position where mouse was pressed.
  bool mPressed = false; /// Flag indicating if mouse is currently pressed.
  bool mDragging = false; /// Flag indicating if dragging is in progress.
  DraggableItem* mClickedItem; /// Pointer to the item that was clicked.

  /**
   * @brief Clears all selected nodes from the scene.
   */
  void clearSelectedNodes();
};
