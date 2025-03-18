#pragma once

#include <QTreeWidget>

#include "result.h"

class NodeItem;
class QTreeWidgetItem;

class TreeMenu : public QTreeWidget
{
public:
  TreeMenu(QWidget* parent);

  VoidResult onNodeAdded(NodeItem* node);
  VoidResult onNodeRemoved(NodeItem* node);

signals:
  void nodeSelected(const QString& nodeId);
  void nodeRemoved(const QString& nodeId);

private:
  VoidResult addRootNode(NodeItem* node);
  VoidResult addLeafNode(NodeItem* node);

  void populateItem(QTreeWidgetItem* item, NodeItem* node);
  QTreeWidgetItem* getItemById(const QString& id);
};
