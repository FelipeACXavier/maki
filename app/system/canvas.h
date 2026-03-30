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
#include "save_info.h"

class QUndoStack;
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
  void copySelectedItems(NodeItem* node);
  void deleteSelectedItems();

  qreal getScale() const;
  QPointF getCenter() const;
  VoidResult loadFromSave(const SaveInfo& info);

  QList<NodeItem*> availableNodes();

  virtual Types::LibraryTypes type() const;
  QUndoStack* undoStack() const;

  void nodeClicked(NodeItem* node);
  void populate(Flow* flow);

  void themeChanged();

  // Used for undo commands
  void createNode(const NodeSaveInfo info);
  void removeNode(const NodeSaveInfo info);
  void triggerNodeRemoval(const NodeItem* node);

  void moveNodeTo(const QString& nodeId, const QPointF& position);
  void setNodeSize(const QString& nodeId, const QSizeF& size);

  void createTransition(const TransitionSaveInfo& info);
  void removeTransition(const TransitionSaveInfo& info);

  void requestAlignNodes(const QList<Types::AlignmentNode>& items, Types::AlignmentMode mode, Types::AlignmentDirection direction);
  void alignNodes(const QList<Types::AlignmentNode>& items, Types::AlignmentMode mode, Types::AlignmentDirection direction, bool useGiven);

protected:
  void
  dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;
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
  void nodeRemoved(const QString& nodeId, const QString& parentId);
  void nodeModified(NodeItem* node);

  void transitionSelected(TransitionItem* transtion);

  void createEvent(NodeItem* node);
  void openFlow(Flow* flow, NodeItem* node);
  void flowAdded(Flow* flow, NodeItem* node);
  void flowRemoved(const QString& flowId, const QString& nodeId);

public slots:
  void onFocusNode(const QString& nodeId);
  void onRemoveNode(const QString& nodeId);

  void onFlowSelected(const QString& flowId, const QString& nodeId);
  void onFlowRemoved(const QString& flowId, const QString& nodeId);

private:
  enum class NodeCreation
  {
    Dropping,
    Pasting,
    Loading,
    Populating
  };
  // TODO(felaze): Move connection behaviour to a separate class
  NodeItem* mHoveredNode = nullptr;
  TransitionItem* mTransition = nullptr;
  NodeItem* mNode = nullptr;
  QPointF mStartDragPosition;
  bool mDragging = false;
  bool mMouseDown = false;

  QTimer* mHoverTimer = nullptr;
  QUndoStack* mUndoStack = nullptr;

  struct CopiedNode
  {
    NodeSaveInfo info;
    QPointF posRelativeToMouse;
  };

  const QString mId;

  QList<CopiedNode> mCopiedNodes;
  QList<NodeItem*> mSelectedNodes;
  std::shared_ptr<ConfigurationTable> mConfigTable;
  std::shared_ptr<SaveInfo> mStorage;

  void clearCanvas();
  void selectNode(NodeItem* node, bool select);

  CanvasView* parentView() const;
  NodeItem* createNode(NodeCreation creation, std::shared_ptr<NodeSaveInfo> info, const QPointF& position, NodeItem* parent);

  NodeItem* findNodeWithId(const QString& id) const;
  QList<NodeItem*> selectedNodes() const;

  void alignNodesHorizontally(const QList<Types::AlignmentNode>& items, Types::AlignmentDirection direction);
  void alignNodesVertically(const QList<Types::AlignmentNode>& items, Types::AlignmentDirection direction);

  // Context menu
  // TODO(felaze): Make this a separate class
  QMenu* createAlignMenu(const QList<Types::AlignmentNode>& items);

  QVector<QGraphicsItem*> removeNode(NodeItem* node);

  void clearSelectedNodes();
  bool isModifierSet(QGraphicsSceneMouseEvent* event, Qt::KeyboardModifier modifier);

  bool nodeClickHandler(QGraphicsSceneMouseEvent* event, QGraphicsItem* item);
  bool transitionClickHandler(QGraphicsSceneMouseEvent* event, QGraphicsItem* item);

  void createNodeContextMenu(QMenu& menu);
  void createTransitionContextMenu(QMenu& menu);

  bool isParentSelected(NodeItem* node);
  void pasteCopiedItems(const QPointF& mousePosition, NodeItem* parentNode, QList<CopiedNode> nodes, bool relative);

  VoidResult loadFromSave(const QVector<std::shared_ptr<INode>>& nodes, NodeItem* parent);
};
