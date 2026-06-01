#pragma once

#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QJsonValueConstRef>
#include <QLabel>
#include <QMimeData>
#include <QString>
#include <QWidget>
#include <memory>

#include "config.h"
#include "node_base.h"
#include "types.h"

/**
 * @brief A draggable item in a graphical scene.
 */
class DraggableItem : public NodeBase
{
public:
  enum
  {
    Type = Types::DRAGGABLE
  };

  /**
   * @brief Constructs a new DraggableItem with the given ID, configuration, and parent.
   *
   * @param id The unique identifier for this item.
   * @param config The configuration settings for this item.
   * @param parent The parent QGraphicsItem.
   */
  DraggableItem(const QString& id, std::shared_ptr<NodeConfig> config, QGraphicsItem* parent = nullptr);

  /**
   * @brief Destroys the DraggableItem.
   */
  virtual ~DraggableItem();

  /**
   * @brief Returns the type of this item.
   *
   * @return The type of this item.
   */
  int type() const override;

  /**
   * @brief Adjusts the width of the item.
   *
   * @param width The new width to set.
   */
  void adjustWidth(int width);

  /**
   * @brief Starts a drag operation based on the given mouse event.
   *
   * @param event The QGraphicsSceneMouseEvent that triggered the drag.
   */
  void startDrag(QGraphicsSceneMouseEvent* event);

  /**
   * @brief Handles a click event based on the given mouse event.
   *
   * @param event The QGraphicsSceneMouseEvent that triggered the click.
   */
  void handleClick(QGraphicsSceneMouseEvent* event);

  /**
   * @brief Returns the shape of this item for collision detection.
   *
   * @return The QPainterPath representing the shape of this item.
   */
  QPainterPath shape() const override;

  /**
   * @brief Paints this item using the given painter and style options.
   *
   * @param painter The QPainter to use for drawing.
   * @param style The QStyleOptionGraphicsItem containing style options.
   * @param widget The QWidget associated with the painting.
   */
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;

  /**
   * @brief Returns the bounding rectangle of this item.
   *
   * @return The QRectF representing the bounding rectangle of this item.
   */
  QRectF nodeRect() const override;
};
