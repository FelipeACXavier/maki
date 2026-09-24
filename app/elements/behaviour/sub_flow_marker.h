#pragma once

#include <QSvgRenderer>

#include "elements/port.h"
#include "types.h"

class SubFlowMarker : public PortItem
{
public:
  SubFlowMarker(Types::Port kind, QGraphicsItem* parent = nullptr);

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

  QPointF inputAnchor() const;
  QPointF outputAnchor() const;

private:
  Types::Port mKind;
  QSvgRenderer* mRenderer = nullptr;
};
