#pragma once

#include <QGraphicsItem>
#include <QGraphicsSceneHoverEvent>
#include <QStyleOptionGraphicsItem>
#include <QSvgRenderer>

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
  PortItem(Types::Port kind, const QString& subflowId, bool isMarker, QGraphicsItem* parentNode);

  int type() const override;
  QRectF boundingRect() const override;
  QPainterPath shape() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

  QString subFlow() const;

  Types::Port kind() const;
  bool isIncoming() const;
  bool isOutgoing() const;
  bool isOut() const;
  bool isAbort() const;
  bool isError() const;

  bool canStartTransition() const;
  bool canEndTransition() const;

  QString defaultTransitionEvent() const;
  QString defaultTransitionLabel() const;
  NodeItem* nodeItem() const;

  /** Scene position for transition endpoints (center of the port square). */
  QPointF anchorScenePos() const;
  void setAnchorOverride(const QPointF& scenePos);
  void clearAnchorOverride();

  bool isMarker() const;
  void setMarker(bool isMarker);

protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
  bool mIsMarker;
  Types::Port mKind;
  const QString mSubFlow;
  std::optional<QPointF> mAnchorOverride;

  std::unique_ptr<QSvgRenderer> mRenderer;
  QString mCurrentIconPath;

  QSizeF getSize() const;
  QPointF defaultPosition() const;
  void updatePosition();
  void updateRenderer();
  QString iconPath() const;
};
