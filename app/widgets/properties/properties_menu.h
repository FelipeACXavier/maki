#pragma once

#include <qtmetamacros.h>

#include <QFrame>

#include "../menu_base.h"
#include "config.h"
#include "result.h"

class Flow;
class NodeItem;
class SaveInfo;
class QTableView;
class QHBoxLayout;
class FlowSaveInfo;
class TransitionItem;
class QStandardItemModel;

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

  void clear();

  // Property related actions
  VoidResult loadProperties(NodeItem* node);
  VoidResult loadPropertyInt(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyReal(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyColor(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertySelect(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyString(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyBoolean(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyStateSelect(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertySetState(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyEventSelect(const PropertiesConfig& property, NodeItem* node);
  VoidResult loadPropertyComponentSelect(const PropertiesConfig& property, NodeItem* node);

  void addStateAssignment(const PropertiesConfig& property, int index, NodeItem* node, QWidget* parent);

  // Control related actions
  VoidResult loadControls(NodeItem* node);
  VoidResult loadControlAddField(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout);
  VoidResult loadControlAddEvent(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout);
  VoidResult loadControlAddState(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout);

  void showContextMenu(QTableView* tableView, NodeItem* node, const QPoint& pos);
  void showEventContextMenu(QTableView* tableView, NodeItem* node, const QPoint& pos);

  void openEventDialog(QTableView* tableView, NodeItem* node, int row);
  void addEventToTable(QStandardItemModel* model, int row, std::shared_ptr<FlowSaveInfo> event);

  void openFieldDialog(QTableView* tableView, NodeItem* node, int row);
  void addStateToTable(QStandardItemModel* model, int row, const PropertiesConfig& event);
};
