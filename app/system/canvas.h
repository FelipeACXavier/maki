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

class CanvasView;
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

  qreal getScale() const;
  VoidResult loadFromSave(const SaveInfo& info);

protected:
  void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
  void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
  void dropEvent(QGraphicsSceneDragDropEvent* event) override;

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  void keyPressEvent(QKeyEvent* event) override;

signals:
  void nodeSelected(NodeItem* node);

private:
  // TODO(felaze): Move connection behaviour to a separate class
  Connector* mConnector = nullptr;
  Connector* mHoveredConnector = nullptr;
  ConnectionItem* mConnection = nullptr;

  QList<NodeSaveInfo> copiedNodes;
  std::shared_ptr<ConfigurationTable> mConfigTable;

  CanvasView* parentView() const;
  NodeItem* createNode(const NodeSaveInfo& info, const QPointF& position, NodeItem* parent);

  NodeItem* findNodeWithId(const QString& id) const;
  Connector* findConnectorWithId(const QString& id) const;
};
