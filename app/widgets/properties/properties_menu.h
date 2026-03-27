#pragma once

#include <qtmetamacros.h>

#include <QFrame>

#include "../menu_base.h"
#include "config.h"
#include "result.h"
#include "save_info.h"

class Flow;
class NodeItem;
class QLineEdit;
class QComboBox;
class QTableView;
class QHBoxLayout;
class TransitionItem;
class QStandardItemModel;
class QFormLayout;

class FlowSaveInfo;

class PropertiesMenu : public QFrame, public MenuBase
{
  Q_OBJECT
public:
  PropertiesMenu(QWidget* parent);

  VoidResult start(std::shared_ptr<SaveInfo> storage);

  VoidResult onNodeAdded(NodeItem* node) override;
  VoidResult onNodeRemoved(NodeItem* node) override;
  VoidResult onNodeModified(NodeItem* node) override;
  VoidResult onNodeSelected(NodeItem* node, bool selected) override;

  VoidResult onTransitionSelected(TransitionItem* transition);

  VoidResult onFlowAdded(Flow* flow, NodeItem* node);
  VoidResult onCreateEvent(NodeItem* node);
  VoidResult onFlowRemoved(const QString& flowId, NodeItem* node);

signals:
  void nodeFocused(const QString& nodeId);

  void flowSelected(const QString& flowId, const QString& nodeId);
  void flowRemoved(const QString& flowId, const QString& nodeId);

private:
  QString mCurrentNode;
  QDialog* mCurrentDialog;
  std::shared_ptr<SaveInfo> mStorage;

  // Property related actions
  VoidResult loadProperties(NodeItem* node);
  VoidResult loadPropertyInt(const PropertyInfo& property, NodeItem* node);
  VoidResult loadPropertyReal(const PropertyInfo& property, NodeItem* node);
  VoidResult loadPropertyColor(const PropertyInfo& property, NodeItem* node);
  VoidResult loadPropertySelect(const PropertyInfo& property, NodeItem* node);
  VoidResult loadPropertyString(const PropertyInfo& property, NodeItem* node);
  VoidResult loadPropertyBoolean(const PropertyInfo& property, NodeItem* node);
  VoidResult loadPropertyEventSelect(const PropertyInfo& property, NodeItem* node);
  QLineEdit* loadPropertyEventArguments(const PropertyInfo& property, NodeItem* node, const QString& propertyId, const QString& eventName, QComboBox* eventWidget);
  VoidResult loadPropertyComponentSelect(const PropertyInfo& property, NodeItem* node);

  // Control related actions
  VoidResult loadControls(NodeItem* node);
  VoidResult loadControlAddEvent(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout);
  VoidResult loadControlAddField(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout);
  VoidResult loadControlAddState(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout);

  void showContextMenu(QTableView* tableView, NodeItem* node, const QPoint& pos);
  void showEventContextMenu(QTableView* tableView, NodeItem* node, const QPoint& pos);

  void openEventDialog(QTableView* tableView, NodeItem* node, int row);
  void addEventToTable(QStandardItemModel* model, int row, std::shared_ptr<FlowSaveInfo> event);

  void openFieldDialog(QTableView* tableView, NodeItem* node, int row);
  void addStateToTable(QStandardItemModel* model, int row, std::shared_ptr<PropertyInfo> event);

  VoidResult loadEventArguments(const QString& nodeId, const QString& flowName, const PropertyInfo& property, NodeItem* node, Types::CallType callType, QFormLayout* formLayout);

  // Component select fields
  VoidResult loadFieldEventSelect(QComboBox* componentSelect, const QString& optionId, const PropertyInfo& property, NodeItem* node,
                                  std::function<void(const QString& nodeId, QComboBox* eventWidget)> populate);
  VoidResult loadFieldTriggerCall(QComboBox* componentSelect, const QString& optionId, const PropertyInfo& property, NodeItem* node);
};
