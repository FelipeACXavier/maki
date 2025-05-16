#include "flow_menu.h"

#include "elements/node.h"

static const int SYSTEM_FLOWS_INDEX = 0;
static const int COMPONENT_FLOWS_INDEX = 1;

static const int NAME_INDEX = 0;
static const int ID_DATA = 0;

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

VoidResult FlowMenu::addComponentFlow(NodeItem* node, const QString& flowName)
{
  auto parent = getNodeById(node->id());
  QTreeWidgetItem* newFlow = nullptr;
  if (parent)
  {
    newFlow = new QTreeWidgetItem(parent);
  }
  else
  {
    QTreeWidgetItem* newNode = new QTreeWidgetItem(componentFlows());
    newNode->setText(0, node->nodeName());
    newNode->setData(ID_DATA, Qt::UserRole, node->id());

    newFlow = new QTreeWidgetItem(newNode);
  }

  newFlow->setText(NAME_INDEX, flowName);
  // TODO: We need to set the flow id here.

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

QTreeWidgetItem* FlowMenu::getNodeById(const QString& id)
{
  for (QTreeWidgetItemIterator it(this); *it; ++it)
  {
    if ((*it)->data(ID_DATA, Qt::UserRole).toString() == id)
      return *it;
  }

  return nullptr;
}
