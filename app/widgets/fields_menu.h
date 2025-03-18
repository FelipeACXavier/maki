#pragma once

#include <QFrame>

#include "config.h"
#include "result.h"

class NodeItem;
class QTableView;
class QHBoxLayout;

class FieldsMenu : public QFrame
{
public:
  FieldsMenu(QWidget* parent);

  VoidResult onNodeSelected(NodeItem* node);
  VoidResult onNodeRemoved(NodeItem* node);

private:
  void clear();
  QString mCurrentNode;

  VoidResult loadControls(NodeItem* node);
  VoidResult loadControlAddField(const ControlsConfig& control, NodeItem* node, QWidget* parent, QHBoxLayout* controlLayout);

  void addDynamicWidget(QWidget* control, QWidget* parent);
  void showContextMenu(QTableView* tableView, NodeItem* node, const QPoint& pos);
};
