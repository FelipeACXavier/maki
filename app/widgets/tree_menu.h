#pragma once

#include <QTreeWidget>

#include "result.h"

class NodeItem;
class QTreeWidgetItem;

class TreeMenu : public QTreeWidget
{
  Q_OBJECT
public:
  TreeMenu(QWidget* parent);

  VoidResult onNodeAdded(NodeItem* node);
  VoidResult onNodeRemoved(NodeItem* node);
  VoidResult onNodeModified(NodeItem* node);

signals:
  void nodeFocused(const QString& nodeId);
  void nodeSelected(const QString& nodeId);
  void nodeRemoved(const QString& nodeId);
  void nodeRenamed(const QString& nodeId, const QString& name);

private slots:
  void showContextMenu(const QPoint& pos);
  void onItemClicked(QTreeWidgetItem* item, int /* column */);

private:
  VoidResult addRootNode(NodeItem* node);
  VoidResult addLeafNode(NodeItem* node);

  void populateItem(QTreeWidgetItem* item, NodeItem* node);
  QTreeWidgetItem* getItemById(const QString& id);
};
