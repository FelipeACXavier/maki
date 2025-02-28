#pragma once

#include <QApplication>
#include <QDropEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPainter>

class NodeItem;
class Connector;
class ConnectionItem;
class ConfigurationTable;

class Canvas : public QGraphicsScene
{
public:
  Canvas(std::shared_ptr<ConfigurationTable> configTable, QObject* parent = nullptr);

protected:
  void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
  void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
  void dropEvent(QGraphicsSceneDragDropEvent* event) override;

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
  // TODO(felaze): Move connection behaviour to a separate class
  Connector* mConnector = nullptr;
  Connector* mHoveredConnector = nullptr;
  std::shared_ptr<ConnectionItem> mConnection = nullptr;

  std::shared_ptr<ConfigurationTable> mConfigTable;
};
