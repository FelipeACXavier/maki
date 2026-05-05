#pragma once

#include <QTreeWidget>

#include "../menu_base.h"
#include "result.h"

class Flow;
class NodeItem;

/**
 * @brief A custom QTreeWidget for managing flow and node items.
 */
class FlowMenu : public QTreeWidget
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a new FlowMenu instance.
   *
   * @param parent The parent widget.
   */
  FlowMenu(QWidget* parent);

  /**
   * @brief Adds a system flow to the menu.
   *
   * @param flowName The name of the flow to add.
   * @return VoidResult indicating success or failure.
   */
  VoidResult addSystemFlow(const QString& flowName);

  /**
   * @brief Handles when a flow is added.
   *
   * @param flow The flow that was added.
   * @param node The node associated with the flow.
   * @return VoidResult indicating success or failure.
   */
  VoidResult onFlowAdded(Flow* flow, NodeItem* node);

  /**
   * @brief Handles when a flow is removed.
   *
   * @param flowId The ID of the flow that was removed.
   * @param nodeId The ID of the node associated with the flow.
   * @return VoidResult indicating success or failure.
   */
  VoidResult onFlowRemoved(const QString& flowId, const QString& nodeId);

  /**
   * @brief Handles when a node is added.
   *
   * @param flowId The ID of the flow that contains the node.
   * @param node The node that was added.
   * @return VoidResult indicating success or failure.
   */
  VoidResult onNodeAdded(const QString& flowId, NodeItem* node);

  /**
   * @brief Handles when a node is removed.
   *
   * @param flowId The ID of the flow that contains the node.
   * @param nodeId The ID of the node that was removed.
   * @return VoidResult indicating success or failure.
   */
  VoidResult onNodeRemoved(const QString& flowId, const QString& nodeId);

  /**
   * @brief Handles when a node is modified.
   *
   * @param flowId The ID of the flow that contains the node.
   * @param node The node that was modified.
   * @return VoidResult indicating success or failure.
   */
  VoidResult onNodeModified(const QString& flowId, NodeItem* node);

  /**
   * @brief Handles when a node is selected.
   *
   * @param flowId The ID of the flow that contains the node.
   * @param node The node that was selected.
   * @param selected True if the node is selected, false otherwise.
   * @return VoidResult indicating success or failure.
   */
  VoidResult onNodeSelected(const QString& flowId, NodeItem* node, bool selected);

signals:
  /**
   * @brief Emitted when a node is focused.
   *
   * @param nodeId The ID of the focused node.
   */
  void nodeFocused(const QString& nodeId);

  /**
   * @brief Emitted when a flow is selected.
   *
   * @param flowId The ID of the selected flow.
   * @param nodeId The ID of the node associated with the flow.
   */
  void flowSelected(const QString& flowId, const QString& nodeId);

  /**
   * @brief Emitted when a flow is removed.
   *
   * @param flowId The ID of the removed flow.
   * @param nodeId The ID of the node associated with the flow.
   */
  void flowRemoved(const QString& flowId, const QString& nodeId);

  /**
   * @brief Emitted when a flow is renamed.
   *
   * @param flowId The ID of the renamed flow.
   * @param nodeId The ID of the node associated with the flow.
   */
  void flowRenamed(const QString& flowId, const QString& nodeId);

private slots:
  /**
   * @brief Shows the context menu at the specified position.
   *
   * @param pos The position where the context menu should be shown.
   */
  void showContextMenu(const QPoint& pos);

  /**
   * @brief Handles when an item is clicked.
   *
   * @param item The item that was clicked.
   * @param column The column of the item that was clicked (not used).
   */
  void onItemClicked(QTreeWidgetItem* item, int /* column */);

private:
  enum Roles
  {
    ToplevelRole = 0,
    ComponentRole,
    FlowRole,
    NodeRole
  };

  /**
   * @brief Retrieves the system flows item.
   *
   * @return QTreeWidgetItem* The system flows item.
   */
  QTreeWidgetItem* systemFlows();

  /**
   * @brief Retrieves the component flows item.
   *
   * @return QTreeWidgetItem* The component flows item.
   */
  QTreeWidgetItem* componentFlows();

  /**
   * @brief Retrieves an item by its ID.
   *
   * @param id The ID of the item to retrieve.
   * @return QTreeWidgetItem* The retrieved item, or nullptr if not found.
   */
  QTreeWidgetItem* getItemById(const QString& id);

  /**
   * @brief Edits a flow.
   *
   * @param item The item representing the flow to edit.
   */
  void editFlow(QTreeWidgetItem* item);

  /**
   * @brief Removes a flow.
   *
   * @param item The item representing the flow to remove.
   */
  void removeFlow(QTreeWidgetItem* item);
};
