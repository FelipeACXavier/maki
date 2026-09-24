#pragma once

#include <QGraphicsItem>
#include <QList>

#include "ids.h"

class Flow;
class NodeItem;

class SubFlow : public QGraphicsItem
{
public:
  enum
  {
    Type = Types::SUB_FLOW
  };

  SubFlow(QGraphicsItem* parent = nullptr);
  ~SubFlow() override = default;

  int type() const override;

  QRectF boundingRect() const override;

  void setRect(const QRectF& rect);

  void setCollapsed(bool collapsed);
  bool isCollapsed() const;

  void setHighlight(bool highlight);

  std::function<void(NodeItem* node)> nodeDropped;
  std::function<void(bool collapsed)> collapsedChanged;

protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
  QRectF mRect{0.0, 0.0, 400.0, 220.0};
  bool mIsCollapsed{false};
  bool mIsHighlighted{false};
};
