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
  void deleteSelectedItems();

  qreal getScale() const;
  QPointF getCenter() const;
  VoidResult loadFromSave(const SaveInfo& info);

protected:
  void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
  void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
  void dropEvent(QGraphicsSceneDragDropEvent* event) override;

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

signals:
  void nodeSelected(NodeItem* node);
  void nodeAdded(NodeItem* node);
  void nodeRemoved(NodeItem* node);
  void nodeModified(NodeItem* node);

public slots:
  void onFocusNode(const QString& nodeId);
  void onRemoveNode(const QString& nodeId);
  void onSelectNode(const QString& nodeId);
  void onRenameNode(const QString& nodeId, const QString& name);

private:
  // TODO(felaze): Move connection behaviour to a separate class
  Connector* mConnector = nullptr;
  Connector* mHoveredConnector = nullptr;
  ConnectionItem* mConnection = nullptr;

  QList<NodeSaveInfo> copiedNodes;
  std::shared_ptr<ConfigurationTable> mConfigTable;

  void clearCanvas();
  void selectNode(NodeItem* node, bool select);

  CanvasView* parentView() const;
  NodeItem* createNode(const NodeSaveInfo& info, const QPointF& position, NodeItem* parent);

  NodeItem* findNodeWithId(const QString& id) const;
  Connector* findConnectorWithId(const QString& id) const;

  // Context menu
  // TODO(felaze): Make this a separate class
  QMenu* createAlignMenu(const QList<QGraphicsItem*>& items);
  QMenu* createConnectionMenu(const QList<QGraphicsItem*>& items);
};
