#pragma once

#include <QApplication>
#include <QDropEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>

#include "elements/node.h"
#include "elements/save_info.h"

class CanvasView;
class TransitionItem;
class ConfigurationTable;

class Canvas : public QGraphicsScene
{
  Q_OBJECT
public:
  Canvas(const QString& canvasId, std::shared_ptr<SaveInfo> storage, std::shared_ptr<ConfigurationTable> configTable, QObject* parent = nullptr);

  QString id() const;
  void pasteCopiedItems();
  void copySelectedItems();
  void deleteSelectedItems();

  qreal getScale() const;
  QPointF getCenter() const;
  VoidResult loadFromSave(const SaveInfo& info);

  QList<NodeItem*> availableNodes();

  virtual Types::LibraryTypes type() const;

  void populate(Flow* flow);

protected:
  void dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
  void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;
  void dropEvent(QGraphicsSceneDragDropEvent* event) override;

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

  virtual void updateParent(NodeItem* node, std::shared_ptr<NodeSaveInfo> storage, bool adding);

signals:
  void nodeSelected(NodeItem* node, bool selected);
  void nodeAdded(NodeItem* node);
  void nodeRemoved(NodeItem* node);
  void nodeModified(NodeItem* node);

  void openFlow(Flow* flow, NodeItem* node);
  void closeFlow(Flow* flow, NodeItem* node);

public slots:
  void onFocusNode(const QString& nodeId);
  void onRemoveNode(const QString& nodeId);
  void onSelectNode(const QList<QString>& nodeIds);
  void onRenameNode(const QString& nodeId, const QString& name);

  void onFlowSelected(const QString& flowId, const QString& nodeId);
  void onFlowRemoved(const QString& flowId, const QString& nodeId);

private:
  enum class NodeCreation
  {
    Dropping,
    Pasting
  };
  // TODO(felaze): Move connection behaviour to a separate class
  NodeItem* mHoveredNode = nullptr;
  TransitionItem* mTransition = nullptr;
  NodeItem* mNode = nullptr;
  QPointF mStartDragPosition;

  int mFrontZValue = 5;
  int mBackZValue = -5;

  QTimer* mHoverTimer;

  QList<NodeSaveInfo> copiedNodes;
  std::shared_ptr<ConfigurationTable> mConfigTable;
  std::shared_ptr<SaveInfo> mStorage;

  const QString mId;

  void clearCanvas();
  void selectNode(NodeItem* node, bool select);

  CanvasView* parentView() const;
  NodeItem* createNode(NodeCreation creation, const NodeSaveInfo& info, const QPointF& position, NodeItem* parent);

  NodeItem* findNodeWithId(const QString& id) const;

  // Context menu
  // TODO(felaze): Make this a separate class
  QMenu* createAlignMenu(const QList<QGraphicsItem*>& items);

  void clearSelectedNodes();
  bool isModifierSet(QGraphicsSceneMouseEvent* event, Qt::KeyboardModifier modifier);
};
