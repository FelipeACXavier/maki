#pragma once

#include <QTreeWidget>

#include "result.h"

class NodeItem;

class FlowMenu : public QTreeWidget
{
  Q_OBJECT
public:
  FlowMenu(QWidget* parent);

  VoidResult addSystemFlow(const QString& flowName);
  VoidResult addComponentFlow(NodeItem* node, const QString& flowName);

signals:
  void flowSelected();
  void flowRemoved();
  void flowRenamed();

private:
  QTreeWidgetItem* systemFlows();
  QTreeWidgetItem* componentFlows();

  QTreeWidgetItem* getNodeById(const QString& id);
};
