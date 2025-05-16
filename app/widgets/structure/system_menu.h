#pragma once

#include <QTreeWidget>

#include "../menu_base.h"
#include "result.h"

class NodeItem;
class QTreeWidgetItem;

class SystemMenu : public QTreeWidget, public MenuBase
{
  Q_OBJECT
public:
  SystemMenu(QWidget* parent);

  VoidResult onNodeAdded(NodeItem* node) override;
  VoidResult onNodeRemoved(NodeItem* node) override;
  VoidResult onNodeModified(NodeItem* node) override;
  VoidResult onNodeSelected(NodeItem* node, bool selected) override;

signals:
  void nodeFocused(const QString& nodeId);
  void nodeSelected(const QList<QString>& nodeIds);
  void nodeRemoved(const QString& nodeId);
  void nodeRenamed(const QString& nodeId, const QString& name);

  void createFlow(const QString& nodeId);

private slots:
  void showContextMenu(const QPoint& pos);
  void onItemClicked(QTreeWidgetItem* item, int /* column */);
  void onSelectionChanged();

private:
  VoidResult addRootNode(NodeItem* node);
  VoidResult addLeafNode(NodeItem* node);

  void populateItem(QTreeWidgetItem* item, NodeItem* node);
  QTreeWidgetItem* getItemById(const QString& id);
};
