#pragma once

#include <QFrame>

#include "../menu_base.h"
#include "config.h"
#include "result.h"

class NodeItem;
class QTableView;
class QHBoxLayout;
class EventDialog;
class QStandardItemModel;

class FieldsMenu : public QFrame, public MenuBase
{
public:
  FieldsMenu(QWidget* parent);

  VoidResult onNodeAdded(NodeItem* node) override;
  VoidResult onNodeRemoved(NodeItem* node) override;
  VoidResult onNodeModified(NodeItem* node) override;
  VoidResult onNodeSelected(NodeItem* node, bool selected) override;

private:
  void clear();
  QString mCurrentNode;
  EventDialog* mCurrentDialog;

  VoidResult loadControls(NodeItem* node);
  VoidResult loadControlAddField(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout);
  VoidResult loadControlAddEvent(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout);

  void showContextMenu(QTableView* tableView, NodeItem* node, const QPoint& pos);
  void showEventContextMenu(QTableView* tableView, NodeItem* node, const QPoint& pos);

  void editEvent(QTableView* tableView, NodeItem* node, const QModelIndex& index);
  void addEventToTable(QStandardItemModel* model, int row, const EventConfig& event);
};
