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
#include "node_base.h"
#include "types.h"

class DraggableItem : public NodeBase
{
public:
  enum
  {
    Type = UserType + Types::DRAGGABLE
  };

  DraggableItem(const QString& id, std::shared_ptr<NodeConfig> config, QGraphicsItem* parent = nullptr);
  virtual ~DraggableItem();

  int type() const override;
  void adjustWidth(int width);

  QPainterPath shape() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* style, QWidget* widget) override;

  QRectF boundingRect() const override;

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
  std::shared_ptr<QPixmap> mPixmap;
  std::shared_ptr<QGraphicsTextItem> mLabel;
};
