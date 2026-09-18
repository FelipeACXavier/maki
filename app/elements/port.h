#pragma once

#include <QGraphicsItem>
#include <QGraphicsSceneHoverEvent>
#include <QStyleOptionGraphicsItem>

#include "ids.h"
#include "types.h"

class NodeItem;

class PortItem : public QGraphicsItem
{
public:
  enum
  {
    Type = Types::PORT
  };

  static constexpr qreal kSize = 8.0;
  static constexpr qreal kAbortPortSize = 12.0;
  static constexpr qreal kErrorPortSize = 10.0;
  static constexpr qreal kGap = 1.0;
  static constexpr qreal kHitPadding = 30.0;  // area around port where it's still possible to initiate/drop a transition

  PortItem(Types::Port kind, QGraphicsItem* parentNode);

  int type() const override;
  QRectF boundingRect() const override;
  QPainterPath shape() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

  Types::Port kind() const;
  bool isIncoming() const;
  bool isOutgoing() const;
  bool isOut() const;
  bool isAbort() const;
  bool isError() const;

  QString defaultTransitionEvent() const;
  QString defaultTransitionLabel() const;
  NodeItem* nodeItem() const;

  /** Scene position for transition endpoints (center of the port square). */
  QPointF anchorScenePos() const;

protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
  Types::Port mKind;

  qreal getSize() const;
};