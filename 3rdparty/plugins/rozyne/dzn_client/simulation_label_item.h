#pragma once

#include <QGraphicsObject>
#include <QJsonObject>
#include <QPainter>

class TraceLabelItem final : public QGraphicsObject
{
  Q_OBJECT
public:
  struct Payload
  {
    QString text;      // Full label
    QString instance;  // e.g. api in "api.abort"
    QString call;      // e.g. abort in api.abort
    bool illegal = false;

    // Optional: store a raw JSON snippet or whatever your simulator expects
    QJsonObject meta;
  };

  struct Style
  {
    QColor valid;
    QColor hover;
    QColor invalid;
    QPen labelPen;
    QPen borderPen;
    QFont font;
  };

  TraceLabelItem(const QRectF& rect, const Style* const style, Payload payload, QGraphicsItem* parent = nullptr);

  QRectF boundingRect() const override
  {
    return mRect.adjusted(-1, -1, 1, 1);
  }
  void paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) override;

  const Payload& payload() const
  {
    return mPayload;
  }

  std::function<void(TraceLabelItem::Payload payload)> clicked;
  std::function<void(TraceLabelItem::Payload payload)> rightClicked;

protected:
  void hoverEnterEvent(QGraphicsSceneHoverEvent*) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent*) override;
  void mousePressEvent(QGraphicsSceneMouseEvent* e) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* e) override;

private:
  const QRectF mRect;
  const Payload mPayload;

  const Style* const mStyle;

  bool mHovered = false;
  bool mPressed = false;
};