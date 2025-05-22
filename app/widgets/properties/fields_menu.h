#pragma once

#include <QFrame>

#include "../menu_base.h"
#include "config.h"
#include "result.h"

class Flow;
class NodeItem;
class QTableView;
class QHBoxLayout;
class EventDialog;
class QStandardItemModel;
class FlowSaveInfo;

class FieldsMenu : public QFrame, public MenuBase
{
  Q_OBJECT
public:
  FieldsMenu(QWidget* parent);

  VoidResult onNodeAdded(NodeItem* node) override;
  VoidResult onNodeRemoved(NodeItem* node) override;
  VoidResult onNodeModified(NodeItem* node) override;
  VoidResult onNodeSelected(NodeItem* node, bool selected) override;

  VoidResult onFlowAdded(Flow* flow, NodeItem* node);
  VoidResult onFlowRemoved(const QString& flowId, NodeItem* node);

signals:
  void nodeFocused(const QString& nodeId);

  void flowSelected(const QString& flowId, const QString& nodeId);
  void flowRemoved(const QString& flowId, const QString& nodeId);

private:
  void clear();
  QString mCurrentNode;
  EventDialog* mCurrentDialog;

  VoidResult loadControls(NodeItem* node);
  VoidResult loadControlAddField(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout);
  VoidResult loadControlAddEvent(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout);

  void showContextMenu(QTableView* tableView, NodeItem* node, const QPoint& pos);
  void showEventContextMenu(QTableView* tableView, NodeItem* node, const QPoint& pos);

  void openEventDialog(QTableView* tableView, NodeItem* node, int row);
  void addEventToTable(QStandardItemModel* model, int row, std::shared_ptr<FlowSaveInfo> event);
};
