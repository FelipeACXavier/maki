#pragma once

#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QMimeData>
#include <QString>
#include <QWidget>
#include <memory>

#include "types.h"

class DraggableItem : public QGraphicsRectItem
{
public:
  enum
  {
    Type = UserType + Type::DRAGGABLE
  };

  DraggableItem(const QString& text, QGraphicsItem* parent = nullptr);
  virtual ~DraggableItem();

  int type() const override;
  void adjustWidth(int width);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
  std::shared_ptr<QGraphicsTextItem> mLabel;
  std::shared_ptr<QGraphicsPixmapItem> mPixmap;

  void updateLabelPosition();
};
