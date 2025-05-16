#pragma once

#include <QTreeWidget>

class QTreeWidgetItem;

class FlowMenu : public QTreeWidget
{
  Q_OBJECT
public:
  FlowMenu(QWidget* parent);
};
