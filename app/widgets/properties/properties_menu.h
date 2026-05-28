#pragma once

#include <QFrame>

#include "../menu_base.h"
#include "config.h"
#include "result.h"
#include "save_info.h"
#include "types.h"
#include "widgets/frame.h"
#include "widgets/widget_factory.h"

class Flow;
class NodeItem;
class QLineEdit;
class QComboBox;
class QTableView;
class QHBoxLayout;
class TransitionItem;
class QStandardItemModel;
class QFormLayout;
class QLayout;

class FlowSaveInfo;

/**
 * @brief PropertiesMenu class that extends QFrame and MenuBase.
 */
class PropertiesMenu : public QFrame, public MenuBase
{
  Q_OBJECT

public:
  /**
   * @brief Constructs a new PropertiesMenu object.
   *
   * @param parent Pointer to the parent widget.
   */
  PropertiesMenu(QWidget* parent);

  /**
   * @brief Starts the menu with the given storage.
   *
   * @param storage Shared pointer to the save info storage.
   * @return VoidResult The result of the operation.
   */
  VoidResult start(std::shared_ptr<SaveInfo> storage);

  /**
   * @brief Handles when a node is added.
   *
   * @param node Pointer to the added node item.
   * @return VoidResult The result of the operation.
   */
  VoidResult onNodeAdded(NodeItem* node) override;

  /**
   * @brief Handles when a node is removed.
   *
   * @param nodeId ID of the removed node.
   * @return VoidResult The result of the operation.
   */
  VoidResult onNodeRemoved(const QString& nodeId) override;

  /**
   * @brief Handles when a node is modified.
   *
   * @param node Pointer to the modified node item.
   * @return VoidResult The result of the operation.
   */
  VoidResult onNodeModified(NodeItem* node) override;

  /**
   * @brief Handles when a node is selected.
   *
   * @param node Pointer to the selected node item.
   * @param selected True if the node is selected, false otherwise.
   * @return VoidResult The result of the operation.
   */
  VoidResult onNodeSelected(NodeItem* node, bool selected) override;

  /**
   * @brief Handles when a transition is selected.
   *
   * @param transition Pointer to the selected transition item.
   * @return VoidResult The result of the operation.
   */
  VoidResult onTransitionSelected(TransitionItem* transition);

  /**
   * @brief Handles when a flow is added.
   *
   * @param flow Pointer to the added flow.
   * @param node Pointer to the associated node item.
   * @return VoidResult The result of the operation.
   */
  VoidResult onFlowAdded(Flow* flow, NodeItem* node);

  /**
   * @brief Handles when an event is created for a node.
   *
   * @param node Pointer to the node item.
   * @return VoidResult The result of the operation.
   */
  VoidResult onCreateEvent(NodeItem* node);

  /**
   * @brief Handles when a flow is removed.
   *
   * @param flowId ID of the removed flow.
   * @param node Pointer to the associated node item.
   * @return VoidResult The result of the operation.
   */
  VoidResult onFlowRemoved(const QString& flowId, NodeItem* node);

  QLayout* layout() const;

signals:
  /**
   * @brief Emitted when a node is focused.
   *
   * @param nodeId ID of the focused node.
   */
  void nodeFocused(const QString& nodeId);

  /**
   * @brief Emitted when a flow is selected.
   *
   * @param flowId ID of the selected flow.
   * @param nodeId ID of the associated node.
   */
  void flowSelected(const QString& flowId, const QString& nodeId);

  /**
   * @brief Emitted when a flow is removed.
   *
   * @param flowId ID of the removed flow.
   * @param nodeId ID of the associated node.
   */
  void flowRemoved(const QString& flowId, const QString& nodeId);

private:
  QString mCurrentNode;                /// Current node ID.
  QDialog* mCurrentDialog;             /// Pointer to the current dialog.
  std::shared_ptr<SaveInfo> mStorage;  /// Shared pointer to the save info storage.
  StyledFrame* mFrame;

  // Property related actions
  VoidResult loadProperties(NodeItem* node);                                                                                                                         /// Loads properties for a given node.
  VoidResult loadPropertyInt(const PropertyInfo& property, NodeItem* node);                                                                                          /// Loads integer property.
  VoidResult loadPropertyReal(const PropertyInfo& property, NodeItem* node);                                                                                         /// Loads real number property.
  VoidResult loadPropertyColor(const PropertyInfo& property, NodeItem* node);                                                                                        /// Loads color property.
  VoidResult loadPropertySelect(const PropertyInfo& property, NodeItem* node);                                                                                       /// Loads select property.
  VoidResult loadPropertyString(const PropertyInfo& property, NodeItem* node);                                                                                       /// Loads string property.
  VoidResult loadPropertyBoolean(const PropertyInfo& property, NodeItem* node);                                                                                      /// Loads boolean property.
  VoidResult loadPropertyEventSelect(const PropertyInfo& property, NodeItem* node);                                                                                  /// Loads event select property.
  QLineEdit* loadPropertyEventArguments(const PropertyInfo& property, NodeItem* node, const QString& propertyId, const QString& eventName, QComboBox* eventWidget);  /// Loads event arguments.
  VoidResult loadPropertyComponentSelect(const PropertyInfo& property, NodeItem* node);                                                                              /// Loads component select property.

  // Control related actions
  VoidResult loadControls(NodeItem* node);                                                                                     /// Loads controls for a given node.
  VoidResult loadControlAddEvent(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout);  /// Adds an event control.
  VoidResult loadControlAddField(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout);  /// Adds a field control.
  VoidResult loadControlAddState(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout);  /// Adds a state control.

  void showContextMenu(QTableView* tableView, NodeItem* node, const QPoint& pos);       /// Shows context menu for a table view.
  void showEventContextMenu(QTableView* tableView, NodeItem* node, const QPoint& pos);  /// Shows event context menu for a table view.

  void openEventDialog(QTableView* tableView, NodeItem* node, int row);                           /// Opens an event dialog.
  void addEventToTable(QStandardItemModel* model, int row, std::shared_ptr<FlowSaveInfo> event);  /// Adds an event to the table.

  void openFieldDialog(QTableView* tableView, NodeItem* node, int row);                           /// Opens a field dialog.
  void addStateToTable(QStandardItemModel* model, int row, std::shared_ptr<PropertyInfo> event);  /// Adds a state to the table.

  VoidResult loadEventArguments(const QString& nodeId, const QString& flowName, const PropertyInfo& property, NodeItem* node, Types::CallType callType, QFormLayout* formLayout);  /// Loads event arguments for a given property.

  // Component select fields
  VoidResult loadFieldEventSelect(maki::SelectorWidget* componentSelect, const QString& optionId, const PropertyInfo& property, NodeItem* node,
                                  std::function<void(const QString& nodeId, QComboBox* eventWidget)> populate);                                   /// Loads field event select.
  VoidResult loadFieldTriggerCall(maki::SelectorWidget* componentSelect, const QString& optionId, const PropertyInfo& property, NodeItem* node);  /// Loads field trigger call.

  void addCompleter(maki::StringWidget* field, const QString& nodeId, const Types::PropertyTypes dataType, QStringList variables = {});
};
