#pragma once

#include <QApplication>
#include <QDropEvent>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QJsonObject>
#include <QMouseEvent>
#include <QPainter>
#include <QTimer>

#include "elements/node.h"
#include "json.h"
#include "node_info.h"
#include "save_info.h"

class QUndoStack;
class CanvasView;
class TransitionItem;
class ConfigurationTable;
class EdgeRouter;

/**
 * @brief The Canvas class represents the main drawing area for nodes and transitions.
 */
class Canvas : public QGraphicsScene
{
  Q_OBJECT
public:
  struct CopiedNode
  {
    NodeSaveInfo info;           /// Save information of the copied node.
    QPointF posRelativeToMouse;  /// Position relative to the mouse when copying.

    QJsonObject toJson() const
    {
      QJsonObject object;
      object["info"] = info.toJson();
      object["position"] = JSON::fromPointF(posRelativeToMouse);
      return object;
    }

    static CopiedNode fromJson(const QJsonObject& object)
    {
      CopiedNode copied;
      copied.info = NodeSaveInfo::fromJson(object["info"].toObject());
      copied.posRelativeToMouse = JSON::toPointF(object["position"].toObject());
      return copied;
    }
  };

  /**
   * @brief Constructs a new Canvas object.
   *
   * @param canvasId The unique identifier for this canvas.
   * @param storage Shared pointer to the save information.
   * @param configTable Shared pointer to the configuration table.
   * @param parent Pointer to the parent object.
   */
  Canvas(const QString& canvasId, std::shared_ptr<ConfigurationTable> configTable, std::shared_ptr<EdgeRouter> router, QObject* parent = nullptr);

  ~Canvas();
  /**
   * @brief Returns the unique identifier for this canvas.
   *
   * @return The canvas ID.
   */
  QString id() const;

  /**
   * @brief Pastes copied items onto the canvas.
   */
  void pasteCopiedItems();

  /**
   * @brief Copies selected items to the clipboard.
   *
   * @param node Pointer to the clicked node, if any.
   */
  void copySelectedItems(NodeItem* node);

  /**
   * @brief Deletes selected items from the canvas.
   */
  void deleteSelectedItems();

  void autoRoute();

  /**
   * @brief Returns the current scale of the canvas.
   *
   * @return The scale value.
   */
  qreal getScale() const;

  /**
   * @brief Returns the center position of the canvas.
   *
   * @return The center position as a QPointF.
   */
  QPointF getCenter() const;

  /**
   * @brief Loads the canvas from save information.
   *
   * @param info The save information to load.
   * @return VoidResult indicating success or failure.
   */
  VoidResult loadFromSave(const SaveInfo& info);

  /**
   * @brief Returns a list of available node items.
   *
   * @return List of NodeItem pointers.
   */
  QList<NodeItem*> availableNodes();

  /**
   * @brief Returns the type of library this canvas belongs to.
   *
   * @return The library type as Types::LibraryTypes.
   */
  virtual Types::LibraryTypes type() const;

  /**
   * @brief Returns a pointer to the undo stack associated with this canvas.
   *
   * @return Pointer to QUndoStack.
   */
  QUndoStack* undoStack() const;

  /**
   * @brief Handles node click events.
   *
   * @param node Pointer to the clicked node.
   */
  void nodeClicked(NodeItem* node);

  /**
   * @brief Populates the canvas with a given flow.
   *
   * @param flow Pointer to the flow to populate.
   */
  void populate(const FlowSaveInfo& flow);

  /**
   * @brief Handles theme change events.
   */
  void themeChanged();

  // Used for undo commands
  /**
   * @brief Creates a node based on save information.
   *
   * @param info The save information for the node.
   */
  void createNode(const NodeSaveInfo info);

  /**
   * @brief Removes a node based on save information.
   *
   * @param info The save information for the node.
   */
  void removeNode(const NodeSaveInfo info);

  /**
   * @brief Triggers the removal of a node.
   *
   * @param node Pointer to the node to be removed.
   */
  void triggerNodeRemoval(const NodeSaveInfo& nodeInfo);

  /**
   * @brief Moves a node to a new position.
   *
   * @param nodeId The ID of the node to move.
   * @param position The new position for the node.
   */
  void moveNodeTo(const QString& nodeId, const QPointF& position);

  /**
   * @brief Sets the size of a node.
   *
   * @param nodeId The ID of the node to resize.
   * @param size The new size for the node.
   */
  void setNodeSize(const QString& nodeId, const QSizeF& size);

  /**
   * @brief Creates a transition based on save information.
   *
   * @param info The save information for the transition.
   */
  void createTransition(const TransitionSaveInfo& info);

  /**
   * @brief Removes a transition based on save information.
   *
   * @param info The save information for the transition.
   */
  void removeTransition(const TransitionSaveInfo& info);

  /**
   * @brief Requests alignment of nodes.
   *
   * @param items List of alignment nodes.
   * @param mode The alignment mode.
   * @param direction The alignment direction.
   */
  void requestAlignNodes(const QList<Types::AlignmentNode>& items, Types::AlignmentMode mode, Types::AlignmentDirection direction);

  /**
   * @brief Aligns nodes based on the given parameters.
   *
   * @param items List of alignment nodes.
   * @param mode The alignment mode.
   * @param direction The alignment direction.
   * @param useGiven Whether to use the given positions for alignment.
   */
  void alignNodes(const QList<Types::AlignmentNode>& items, Types::AlignmentMode mode, Types::AlignmentDirection direction, bool useGiven);

  std::shared_ptr<EdgeRouter> router() const;

protected:
  /**
   * @brief Handles drag enter events.
   *
   * @param event Pointer to the QGraphicsSceneDragDropEvent.
   */
  void
  dragEnterEvent(QGraphicsSceneDragDropEvent* event) override;

  /**
   * @brief Handles drag move events.
   *
   * @param event Pointer to the QGraphicsSceneDragDropEvent.
   */
  void dragMoveEvent(QGraphicsSceneDragDropEvent* event) override;

  /**
   * @brief Handles drop events.
   *
   * @param event Pointer to the QGraphicsSceneDragDropEvent.
   */
  void dropEvent(QGraphicsSceneDragDropEvent* event) override;

  /**
   * @brief Handles mouse press events.
   *
   * @param event Pointer to the QGraphicsSceneMouseEvent.
   */
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  /**
   * @brief Handles mouse move events.
   *
   * @param event Pointer to the QGraphicsSceneMouseEvent.
   */
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

  /**
   * @brief Handles mouse release events.
   *
   * @param event Pointer to the QGraphicsSceneMouseEvent.
   */
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  /**
   * @brief Handles context menu events.
   *
   * @param event Pointer to the QGraphicsSceneContextMenuEvent.
   */
  void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

  /**
   * @brief Updates the parent of a node.
   *
   * @param node Pointer to the node.
   * @param storage Shared pointer to the save information.
   * @param adding Whether this is an addition or removal.
   */
  virtual void updateParent(NodeItem* node, std::shared_ptr<NodeSaveInfo> storage, bool adding);

signals:
  /**
   * @brief Emitted when a node is selected.
   *
   * @param node Pointer to the selected node.
   * @param selected Whether the node is selected or not.
   */
  void nodeSelected(NodeItem* node, bool selected);

  /**
   * @brief Emitted when a node is added.
   *
   * @param node Pointer to the added node.
   */
  void nodeAdded(NodeItem* node);

  /**
   * @brief Emitted when a node is removed.
   *
   * @param nodeId The ID of the removed node.
   * @param parentId The ID of the parent node.
   */
  void nodeRemoved(const QString& nodeId, const QString& parentId);

  /**
   * @brief Emitted when a node is modified.
   *
   * @param node Pointer to the modified node.
   */
  void nodeModified(NodeItem* node);

  /**
   * @brief Emitted when a transition is selected.
   *
   * @param transtion Pointer to the selected transition.
   */
  void transitionSelected(TransitionItem* transtion);

  /**
   * @brief Emitted when an event is created.
   *
   * @param node Pointer to the node where the event is created.
   */
  void createEvent(NodeItem* node);

  /**
   * @brief Emitted when a flow is opened.
   *
   * @param flow Pointer to the opened flow.
   * @param node Pointer to the node associated with the flow.
   */
  void openFlow(Flow* flow, const QString& nodeId);

  /**
   * @brief Emitted when a flow is added.
   *
   * @param flow Pointer to the added flow.
   * @param node Pointer to the node associated with the flow.
   */
  void flowAdded(Flow* flow, NodeItem* node);

  /**
   * @brief Emitted when a flow is removed.
   *
   * @param flowId The ID of the removed flow.
   * @param nodeId The ID of the node associated with the flow.
   */
  void flowRemoved(const QString& flowId, const QString& nodeId);

public slots:
  /**
   * @brief Handles focus on a node.
   *
   * @param flowId The ID of the flow that contains the given node.
   * @param nodeId The ID of the focused node.
   */
  void onFocusNode(const QString& flowId, const QString& nodeId);

  /**
   * @brief Handles removal of a node.
   *
   * @param nodeId The ID of the node to remove.
   */
  void onRemoveNode(const QString& flowId, const QString& nodeId);

  /**
   * @brief Handles selection of a flow.
   *
   * @param flowId The ID of the selected flow.
   * @param nodeId The ID of the node associated with the flow.
   */
  void onFlowSelected(const QString& flowId, const QString& nodeId);

  /**
   * @brief Handles removal of a flow.
   *
   * @param flowId The ID of the removed flow.
   * @param nodeId The ID of the node associated with the flow.
   */
  void onFlowRemoved(const QString& flowId, const QString& nodeId);

protected:
  std::shared_ptr<ConfigurationTable> mConfigTable;  /// Pointer to the configuration table.
  std::shared_ptr<EdgeRouter> mRouter;               /// Pointer to the system edge router.

  virtual void addedItemNode(NodeItem* node, std::shared_ptr<NodeSaveInfo> info);
  virtual void addedItemFlow(Flow* flow, NodeItem* node);
  virtual void addTransition(TransitionItem* transition);
  virtual void removeTransition(TransitionItem* transition);
  virtual bool canAddTransition(NodeItem* node) const;
  virtual TransitionConfig nextTransition(NodeItem* node) const;
  virtual QVector<QGraphicsItem*> cleanTransitionsOfNode(const QString& nodeId);
  virtual void onNodeMoved(const QString& nodeId);

  /**
   * @brief Returns the parent view of this canvas.
   *
   * @return Pointer to CanvasView.
   */
  CanvasView* parentView() const;

private:
  enum class NodeCreation
  {
    Dropping,
    Pasting,
    Loading,
    Populating
  };

  NodeItem* mHoveredNode = nullptr;       /// Pointer to the hovered node.
  TransitionItem* mTransition = nullptr;  /// Pointer to the current transition being created.
  NodeItem* mNode = nullptr;              /// Pointer to the currently clicked node.
  QPointF mStartDragPosition;             /// The starting position of a drag operation.
  bool mDragging = false;                 /// Whether a drag operation is in progress.
  bool mMouseDown = false;                /// Whether the mouse button is pressed.

  QPointF mSelectionStart;
  QGraphicsRectItem* mSelectionRect = nullptr;

  QTimer* mHoverTimer = nullptr;     /// Timer for handling hover events.
  QUndoStack* mUndoStack = nullptr;  /// Pointer to the undo stack.

  const QString mId;  /// Unique identifier for this canvas.

  QList<CopiedNode> mCopiedNodes;   /// List of copied nodes.
  QList<NodeItem*> mSelectedNodes;  /// List of currently selected nodes.

  /**
   * @brief Clears all items from the canvas.
   */
  void clearCanvas();

  /**
   * @brief Selects a node and updates its selection state.
   *
   * @param node Pointer to the node to select.
   * @param select Whether to select or deselect the node.
   */
  void selectNode(NodeItem* node, bool select);

  /**
   * @brief Creates a new node based on save information and other parameters.
   *
   * @param creation The type of node creation (e.g., dropping, pasting).
   * @param info Shared pointer to the save information for the node.
   * @param position The initial position of the node.
   * @param parent Pointer to the parent node.
   * @return Pointer to the created NodeItem.
   */
  NodeItem* createNode(NodeCreation creation, std::shared_ptr<NodeSaveInfo> info, const QPointF& position, NodeItem* parent);

  /**
   * @brief Finds a node by its ID.
   *
   * @param id The ID of the node to find.
   * @return Pointer to the found NodeItem, or nullptr if not found.
   */
  NodeItem* findNodeWithId(const QString& id) const;

  /**
   * @brief Returns a list of currently selected nodes.
   *
   * @return List of NodeItem pointers.
   */
  QList<NodeItem*> selectedNodes() const;

  /**
   * @brief Aligns nodes horizontally based on the given parameters.
   *
   * @param items List of alignment nodes.
   * @param direction The alignment direction.
   */
  void alignNodesHorizontally(const QList<Types::AlignmentNode>& items, Types::AlignmentDirection direction);

  /**
   * @brief Aligns nodes vertically based on the given parameters.
   *
   * @param items List of alignment nodes.
   * @param direction The alignment direction.
   */
  void alignNodesVertically(const QList<Types::AlignmentNode>& items, Types::AlignmentDirection direction);

  // Context menu
  void createAlignMenu(QMenu* alignMenu, const QList<Types::AlignmentNode>& items);  /// Creates a context menu for alignment options.

  QVector<QGraphicsItem*> removeNode(NodeItem* node);  /// Removes a node and its associated connections.

  void clearSelectedNodes();                                                           /// Clears all selected nodes.
  bool isModifierSet(QGraphicsSceneMouseEvent* event, Qt::KeyboardModifier modifier);  /// Checks if a specific keyboard modifier is set.

  bool nodeClickHandler(QGraphicsSceneMouseEvent* event, QGraphicsItem* item);        /// Handles node click events.
  bool transitionClickHandler(QGraphicsSceneMouseEvent* event, QGraphicsItem* item);  /// Handles transition click events.

  void createNodeContextMenu(QMenu& menu);        /// Creates a context menu for nodes.
  void createTransitionContextMenu(QMenu& menu);  /// Creates a context menu for transitions.

  bool isParentSelected(NodeItem* node);                                                                              /// Checks if the parent of a node is selected.
  void pasteCopiedItems(const QPointF& mousePosition, NodeItem* parentNode, QList<CopiedNode> nodes, bool relative);  /// Pastes copied items at a specified position.

  VoidResult loadFromSave(const QVector<std::shared_ptr<INode>>& nodes, NodeItem* parent);  /// Loads nodes and their children from save information.

  void onSelectionChanged();
};

inline QDataStream& operator<<(QDataStream& out, const Canvas::CopiedNode& node)
{
  out << node.info;
  out << node.posRelativeToMouse;
  return out;
}

inline QDataStream& operator>>(QDataStream& in, Canvas::CopiedNode& node)
{
  in >> node.info;
  in >> node.posRelativeToMouse;
  return in;
}
