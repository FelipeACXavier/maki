#include "flow_menu.h"

static const int SYSTEM_FLOWS_INDEX = 0;
static const int COMPONENT_FLOWS_INDEX = 1;

static const int NAME_INDEX = 0;

FlowMenu::FlowMenu(QWidget* parent)
    : QTreeWidget(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);
}

VoidResult FlowMenu::addSystemFlow(const QString& flowName)
{
  QTreeWidgetItem* item = new QTreeWidgetItem(systemFlows());
  item->setText(NAME_INDEX, flowName);
  return VoidResult();
}

VoidResult FlowMenu::addComponentFlow(const QString& flowName)
{
  QTreeWidgetItem* item = new QTreeWidgetItem(componentFlows());
  item->setText(NAME_INDEX, flowName);
  return VoidResult();
}

QTreeWidgetItem* FlowMenu::systemFlows()
{
  return topLevelItem(SYSTEM_FLOWS_INDEX);
}

QTreeWidgetItem* FlowMenu::componentFlows()
{
  return topLevelItem(COMPONENT_FLOWS_INDEX);
}
