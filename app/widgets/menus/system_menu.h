#pragma once

#include <QList>
#include <QTreeWidget>

#include "result.h"
#include "settings_manager.h"
#include "style_helpers.h"

class NodeItem;
class Flow;
class QTreeWidgetItem;

/**
 * @class SystemMenu
 * @brief Tree widget used to display and manage flows and nodes in the system view.
 *
 * This widget maintains a hierarchical representation of system-level and component-level
 * flows, along with their nodes. It reacts to node and flow lifecycle events and exposes
 * signals for user interactions such as focusing, selecting, renaming, and removing items.
 */
class SystemMenu : public QTreeWidget
{
  Q_OBJECT
public:
  /**
   * @brief Constructs the system menu widget.
   * @param parent The parent widget.
   */
  SystemMenu(QWidget* parent);

  /**
   * @brief Handles the addition of a node to a flow.
   * @param flowId The identifier of the flow to which the node belongs.
   * @param node The node that was added.
   * @return A result indicating whether the operation succeeded.
   */
  VoidResult onNodeAdded(const QString& flowId, NodeItem* node);

  /**
   * @brief Handles the removal of a node from a flow.
   * @param flowId The identifier of the flow from which the node is removed.
   * @param nodeId The identifier of the node to remove.
   * @param parentId The identifier of the parent node, if applicable.
   * @return A result indicating whether the operation succeeded.
   */
  VoidResult onNodeRemoved(const QString& flowId, const QString& nodeId, const QString& parentId);

  /**
   * @brief Handles modifications to an existing node.
   * @param flowId The identifier of the flow containing the node.
   * @param node The modified node.
   * @return A result indicating whether the operation succeeded.
   */
  VoidResult onNodeModified(const QString& flowId, NodeItem* node);

  /**
   * @brief Handles selection state changes for a node.
   * @param flowId The identifier of the flow containing the node.
   * @param node The node whose selection state changed.
   * @param selected True if the node is selected, false otherwise.
   * @return A result indicating whether the operation succeeded.
   */
  VoidResult onNodeSelected(const QString& flowId, NodeItem* node, bool selected);

  /**
   * @brief Handles the addition of a flow to the menu.
   * @param flow The flow that was added.
   * @param node The node associated with the flow.
   * @return A result indicating whether the operation succeeded.
   */
  VoidResult onFlowAdded(Flow* flow, NodeItem* node);

  /**
   * @brief Handles the removal of a flow from the menu.
   * @param flowId The identifier of the flow to remove.
   * @param nodeId The identifier of the node associated with the flow.
   * @return A result indicating whether the operation succeeded.
   */
  VoidResult onFlowRemoved(const QString& flowId, const QString& nodeId);

signals:
  /**
   * @brief Emitted when a node should be focused in the editor.
   * @param flowId The identifier of the flow containing the node.
   * @param nodeId The identifier of the node to focus.
   */
  void nodeFocused(const QString& flowId, const QString& nodeId);

  /**
   * @brief Emitted when a node should be removed.
   * @param flowId The identifier of the flow containing the node.
   * @param nodeId The identifier of the node to remove.
   */
  void nodeRemoved(const QString& flowId, const QString& nodeId);

  /**
   * @brief Emitted when a flow is selected.
   * @param flowId The identifier of the selected flow.
   * @param nodeId The identifier of the node associated with the flow.
   */
  void flowSelected(const QString& flowId, const QString& nodeId);

  /**
   * @brief Emitted when a flow should be removed.
   * @param flowId The identifier of the flow to remove.
   * @param nodeId The identifier of the node associated with the flow.
   */
  void flowRemoved(const QString& flowId, const QString& nodeId);

  /**
   * @brief Emitted when a flow should be renamed.
   * @param flowId The identifier of the flow to rename.
   * @param nodeId The identifier of the node associated with the flow.
   */
  void flowRenamed(const QString& flowId, const QString& nodeId);

private slots:
  /**
   * @brief Shows the context menu at the given position.
   * @param pos The position where the context menu should appear.
   */
  void showContextMenu(const QPoint& pos);

  /**
   * @brief Handles clicks on tree items.
   * @param item The item that was clicked.
   * @param The clicked column index.
   */
  void onItemClicked(QTreeWidgetItem* item, int /* column */);

private:
  /**
   * @enum Roles
   * @brief Identifies the semantic role of a tree item.
   */
  enum Roles
  {
    ToplevelRole = 0,  ///< Item represents a top-level category node.
    Capabilities,
    SubTasks,
    Flows,
    ComponentRole,  ///< Item represents a component category or container.
    FlowRole,       ///< Item represents a flow.
    NodeRole        ///< Item represents a node within a flow.
  };

  /**
   * @brief Adds a root node to the menu.
   * @param node The node to add.
   * @return A result indicating whether the operation succeeded.
   */
  VoidResult addRootNode(NodeItem* node);

  /**
   * @brief Adds a leaf node to the menu.
   * @param node The node to add.
   * @return A result indicating whether the operation succeeded.
   */
  VoidResult addLeafNode(NodeItem* node);

  /**
   * @brief Populates a tree widget item using data from a node.
   * @param item The tree item to populate.
   * @param node The source node.
   */
  void populateItem(QTreeWidgetItem* item, const QIcon& icon,
                    const QString& name, const QString& type,
                    const QString& data, const Roles role, const QString& canvas = "");

  void populateTaskItem(QTreeWidgetItem* item, NodeItem* node);

  /**
   * @brief Finds a tree item by its identifier.
   * @param id The identifier to search for.
   * @return The matching tree item, or nullptr if none was found.
   */
  QTreeWidgetItem* getItemById(const QString& id) const;

  QTreeWidgetItem* findParentItemByRole(const QString& id, Roles role) const;

  QTreeWidgetItem* getOrCreateChildGroup(const QString& parentId, Roles role);

  /**
   * @brief Returns the root item containing system flows.
   * @return The tree item representing the system flows section.
   */
  QTreeWidgetItem* systemFlows();

  /**
   * @brief Returns the root item containing component flows.
   * @return The tree item representing the component flows section.
   */
  QTreeWidgetItem* componentFlows();

  /**
   * @brief Starts editing the given flow item.
   * @param item The flow item to edit.
   */
  void editFlow(QTreeWidgetItem* item);

  /**
   * @brief Removes the given flow item.
   * @param item The flow item to remove.
   */
  void removeFlow(QTreeWidgetItem* item);
};