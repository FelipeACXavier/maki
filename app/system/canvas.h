#pragma once

#include <QApplication>
#include <QDropEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPainter>

#include "elements/node.h"
#include "elements/save_info.h"

class Connector;
class ConnectionItem;
class ConfigurationTable;

class Canvas : public QGraphicsScene
{
  Q_OBJECT
public:
  Canvas(std::shared_ptr<ConfigurationTable> configTable, QObject* parent = nullptr);

  void pasteCopiedItems();
  void copySelectedItems();

protected:
  void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
  void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
  void dropEvent(QGraphicsSceneDragDropEvent* event) override;

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  void drawBackground(QPainter* painter, const QRectF& rect) override;

  void keyPressEvent(QKeyEvent* event) override;

signals:
  void nodeSelected(NodeItem* node);

private slots:
  void onNodeSelected(NodeItem* node);

private:
  // TODO(felaze): Move connection behaviour to a separate class
  Connector* mConnector = nullptr;
  Connector* mHoveredConnector = nullptr;
  ConnectionItem* mConnection = nullptr;

  QList<SaveInfo> copiedNodes;
  std::shared_ptr<ConfigurationTable> mConfigTable;

  QGraphicsView* parentView() const;
  bool createNode(const SaveInfo& info, const QPointF& position, NodeItem* parent);
};
