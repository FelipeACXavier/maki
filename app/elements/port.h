#pragma once

#include <QGraphicsItem>
#include <QGraphicsSceneHoverEvent>
#include <QStyleOptionGraphicsItem>

#include "ids.h"

class NodeItem;

class PortItem : public QGraphicsItem
{
public:
  enum Kind
  {
    In,
    Out
  };

  static constexpr qreal kSize = 8.0;
  static constexpr qreal kGap = - 3.0;

  PortItem(Kind kind, NodeItem* parentNode);

  int type() const override;
  QRectF boundingRect() const override;
  QPainterPath shape() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

  Kind kind() const { return mKind; }
  NodeItem* nodeItem() const;

  /** Scene position for transition endpoints (center of the port square). */
  QPointF anchorScenePos() const;

protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
  Kind mKind;
};
