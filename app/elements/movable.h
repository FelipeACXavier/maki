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
#include "types.h"

class DraggableItem : public QGraphicsItem
{
public:
  enum
  {
    Type = UserType + Types::DRAGGABLE
  };

  DraggableItem(const QString& id, std::shared_ptr<NodeConfig> config, QGraphicsItem* parent = nullptr);
  virtual ~DraggableItem();

  QString id() const;
  int type() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
  QPainterPath shape() const override;
  QRectF boundingRect() const override;

  void adjustWidth(int width);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
  const QString mId;
  std::shared_ptr<NodeConfig> mConfig;

  const QRectF mBounds;
  QRectF mScaledBounds;

  // std::shared_ptr<QGraphicsItem> mShape;
  std::shared_ptr<QPixmap> mPixmap;
  std::shared_ptr<QGraphicsTextItem> mLabel;

  void updateLabelPosition();
  QRectF drawingBorders(const QRectF& input) const;
  qreal computeScaleFactor() const;
};
