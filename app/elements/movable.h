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

class DraggableItem : public QGraphicsRectItem
{
public:
  enum
  {
    Type = UserType + Type::DRAGGABLE
  };

  DraggableItem(const QString& id, std::shared_ptr<NodeConfig> config, QGraphicsItem* parent = nullptr);
  virtual ~DraggableItem();

  QString Id() const;
  int type() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = nullptr) override;
  QPainterPath shape() const override;

  void adjustWidth(int width);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

private:
  const QString mId;

  std::shared_ptr<QGraphicsTextItem> mLabel;
  std::shared_ptr<QGraphicsPixmapItem> mPixmap;

  std::shared_ptr<NodeConfig> mConfig;

  void updateLabelPosition();
};
