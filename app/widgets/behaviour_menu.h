#pragma once

#include <QFrame>
#include <QListWidget>

#include "config.h"
#include "result.h"

class Canvas;
class NodeItem;
class BehaviourDialog;

/**
 * @brief A menu for managing behaviours in a graphical interface.
 */
class BehaviourMenu : public QFrame
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a new BehaviourMenu instance.
   *
   * @param parent The parent widget of this menu.
   */
  BehaviourMenu(QWidget* parent);

  /**
   * @brief Handles the selection or deselection of a node.
   *
   * @param node The NodeItem that was selected or deselected.
   * @param selected True if the node is now selected, false otherwise.
   * @return VoidResult A result indicating success or failure.
   */
  VoidResult onNodeSelected(NodeItem* node, bool selected);

  /**
   * @brief Handles the removal of a node.
   *
   * @param nodeId The ID of the node that was removed.
   * @return VoidResult A result indicating success or failure.
   */
  VoidResult onNodeRemoved(const QString& nodeId);

  /// Lambda used to retrieve the current nodes in a flow
  std::function<QList<NodeItem*>()> mGetAvailableNodes;

private slots:
  /**
   * @brief Adds a new behaviour to the menu.
   */
  void addBehaviour();

  /**
   * @brief Edits an existing behaviour in the menu.
   *
   * @param item The QListWidgetItem representing the behaviour to edit.
   */
  void editBehaviour(QListWidgetItem* item);

  /**
   * @brief Shows the context menu at a specified position.
   *
   * @param pos The position where the context menu should be shown.
   */
  void showContextMenu(const QPoint& pos);

  /**
   * @brief Handles the acceptance of a dialog.
   */
  void onDialogAccepted();

  /**
   * @brief Handles the rejection of a dialog.
   */
  void onDialogRejected();

private:
  QString mCurrentNode;  /// The ID of the currently selected node.

  QListWidget* mBehaviourList;  /// The list widget displaying available behaviours.

  BehaviourDialog* mCurrentDialog;  /// The current dialog for adding or editing a behaviour.

  /**
   * @brief Clears the menu.
   */
  void clear();

  /**
   * @brief Sets up the global view of the menu.
   */
  void setGlobalView();

  /**
   * @brief Adds a dynamic widget to the menu.
   *
   * @param dynamicWidget The widget to add.
   * @param parent The parent widget for the new widget.
   */
  void addDynamicWidget(QWidget* dynamicWidget, QWidget* parent);
};
