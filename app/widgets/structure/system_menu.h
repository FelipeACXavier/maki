#pragma once

#include <QList>
#include <QTreeWidget>

#include "result.h"
#include "style_helpers.h"
#include "widgets/settings_manager.h"

class NodeItem;
class Flow;
class QTreeWidgetItem;

class SystemMenu : public QTreeWidget
{
  Q_OBJECT
public:
  SystemMenu(QWidget* parent);

  VoidResult onNodeAdded(const QString& flowId, NodeItem* node);
  VoidResult onNodeRemoved(const QString& flowId, NodeItem* node);
  VoidResult onNodeModified(const QString& flowId, NodeItem* node);
  VoidResult onNodeSelected(const QString& flowId, NodeItem* node, bool selected);

  VoidResult onFlowAdded(Flow* flow, NodeItem* node);
  VoidResult onFlowRemoved(const QString& flowId, const QString& nodeId);

  void onThemeChanged(const AppearanceSettings& settings);

signals:
  void nodeFocused(const QString& flowId, const QString& nodeId);
  void nodeRemoved(const QString& flowId, const QString& nodeId);

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

  QList<TreeWidgetWithIcon> mIcons;

  VoidResult addRootNode(NodeItem* node);
  VoidResult addLeafNode(NodeItem* node);

  void populateItem(QTreeWidgetItem* item, NodeItem* node);
  QTreeWidgetItem* getItemById(const QString& id);

  QTreeWidgetItem* systemFlows();
  QTreeWidgetItem* componentFlows();

  void editFlow(QTreeWidgetItem* item);
  void removeFlow(QTreeWidgetItem* item);
};
