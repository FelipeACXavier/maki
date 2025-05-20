#pragma once

#include <QTreeWidget>

#include "../menu_base.h"
#include "result.h"

class Flow;
class NodeItem;

class FlowMenu : public QTreeWidget
{
  Q_OBJECT
public:
  FlowMenu(QWidget* parent);

  VoidResult addSystemFlow(const QString& flowName);
  Result<Flow*> addComponentFlow(NodeItem* node, const QString& flowName);

  VoidResult onFlowAdded(Flow* flow, NodeItem* node);
  VoidResult onFlowRemoved(const QString& flowId, NodeItem* node);

  VoidResult onNodeAdded(const QString& flowId, NodeItem* node);
  VoidResult onNodeRemoved(const QString& flowId, NodeItem* node);
  VoidResult onNodeModified(const QString& flowId, NodeItem* node);
  VoidResult onNodeSelected(const QString& flowId, NodeItem* node, bool selected);

signals:
  void nodeFocused(const QString& nodeId);

  void flowSelected(const QString& flowId, const QString& nodeId);
  void flowRemoved(const QString& flowId, const QString& nodeId);
  void flowRenamed(const QString& flowId, const QString& nodeId);

private slots:
  void showContextMenu(const QPoint& pos);
  void onItemClicked(QTreeWidgetItem* item, int /* column */);

private:
  enum Roles
  {
    ToplevelRole = 0,
    ComponentRole,
    FlowRole,
    NodeRole
  };

  QTreeWidgetItem* systemFlows();
  QTreeWidgetItem* componentFlows();

  QTreeWidgetItem* getItemById(const QString& id);

  void editFlow(QTreeWidgetItem* item);
  void removeFlow(QTreeWidgetItem* item);
};
