#pragma once

#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <memory>

#include "elements/config.h"
#include "types.h"

class Connector;
class ConnectionItem;
class QGraphicsSceneMouseEvent;

class NodeItem : public QGraphicsItem
{
public:
  enum
  {
    Type = UserType + Types::NODE
  };

  NodeItem(const QPointF& initialPosition, const QPixmap& map, std::shared_ptr<NodeConfig> config, QGraphicsItem* parent = nullptr);
  virtual ~NodeItem();

  QString Id() const;

  int type() const override;
  QString nodeType() const;

  QRectF boundingRect() const override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override;

  QVector<std::shared_ptr<Connector>> connectors() const;
  QString behaviour() const;

protected:
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
  const QString mId;

  bool m_hovered{false};
  std::shared_ptr<NodeConfig> mConfig;

  std::shared_ptr<QGraphicsPixmapItem> mPixmapItem;
  QVector<std::shared_ptr<Connector>> mConnectors;

  void updateConnectors();
};
