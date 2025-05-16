#pragma once

#include <QTreeWidget>

#include "result.h"

class FlowMenu : public QTreeWidget
{
  Q_OBJECT
public:
  FlowMenu(QWidget* parent);

  VoidResult addSystemFlow(const QString& flowName);
  VoidResult addComponentFlow(const QString& flowName);

signals:
  void flowSelected();
  void flowRemoved();
  void flowRenamed();

private:
  QTreeWidgetItem* systemFlows();
  QTreeWidgetItem* componentFlows();
};
