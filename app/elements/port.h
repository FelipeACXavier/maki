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
    In = 0,
    Out = 1,
    Abort = 2,
    Error = 3
  };

  static constexpr qreal kSize = 8.0;
  static constexpr qreal kTopPortSize = 12.0;
  static constexpr qreal kGap = 1.0;
  static constexpr qreal kHitPadding = 30.0; // area around port where it's still possible to initiate/drop a transition

  static qreal sizeForKind(Kind kind);

  PortItem(Kind kind, NodeItem* parentNode);

  int type() const override;
  QRectF boundingRect() const override;
  QPainterPath shape() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

  Kind kind() const { return mKind; }
  bool isIncoming() const { return mKind == In; }
  bool isOutgoing() const { return mKind != In; }
  QString defaultTransitionEvent() const;
  QString defaultTransitionLabel() const;
  NodeItem* nodeItem() const;

  /** Scene position for transition endpoints (center of the port square). */
  QPointF anchorScenePos() const;

protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
  Kind mKind;
};
