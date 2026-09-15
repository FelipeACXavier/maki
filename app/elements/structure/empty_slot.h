#pragma once

#include <QGraphicsObject>

#include "ids.h"

class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

class EmptySlot : public QGraphicsObject
{
public:
  enum
  {
    Type = Types::EMPTY_SLOT
  };

  EmptySlot(qreal diameter, QGraphicsItem* parent = nullptr);

  /**
   * @brief Returns the type of this item.
   *
   * @return The type of the item.
   */
  int type() const override;

  QRectF boundingRect() const override;

  void setDiameter(qreal diameter);
  qreal diameter() const;

  std::function<void()> clicked;

protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
  qreal mDiameter = 0.0;
  bool mHovered = false;
  bool mPressed = false;
};