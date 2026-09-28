#pragma once

#include <QGraphicsItem>
#include <QList>

#include "app_configs.h"
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

  SubFlow(const QString& id, QGraphicsItem* parent = nullptr);
  ~SubFlow() override = default;

  QString id() const;
  int type() const override;

  QRectF boundingRect() const override;

  void setRect(const QRectF& rect);

  void setCollapsed(bool collapsed);
  bool isCollapsed() const;

  void setHighlight(bool highlight);

  void setTitle(const QString& title);
  QString title() const;
  void setTitlePosition(const Config::ControlPosition pos, int leftPadding, int topPadding);

  std::function<void(NodeItem* node)> nodeDropped;
  std::function<void(bool collapsed)> collapsedChanged;

protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
  const QString mId;
  QRectF mRect{0.0, 0.0, 400.0, 220.0};
  bool mIsCollapsed{false};
  bool mIsHighlighted{false};

  QString mTitle;
  QRectF mTextRect{0.0, 0.0, 100.0, 12.0};
  Config::ControlPosition mPosition = Config::ControlPosition::Top;
  int mLeftPadding = 0;
  int mTopPadding = 0;
};
