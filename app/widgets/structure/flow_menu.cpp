#include "flow_menu.h"

#include <QMenu>

#include "elements/flow.h"
#include "elements/node.h"
#include "logging.h"

static const int SYSTEM_FLOWS_INDEX = 0;
static const int COMPONENT_FLOWS_INDEX = 1;

static const int NAME_INDEX = 0;
static const int ID_DATA = 0;
static const int TYPE_DATA = 1;

FlowMenu::FlowMenu(QWidget* parent)
    : QTreeWidget(parent)
{
  // Add base menu items
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, &QTreeWidget::customContextMenuRequested, this, &FlowMenu::showContextMenu);
}

Result<QString> FlowMenu::addSystemFlow(const QString& flowName)
{
  QTreeWidgetItem* item = new QTreeWidgetItem(systemFlows());
  item->setText(NAME_INDEX, flowName);
  return QString("");
}

Result<QString> FlowMenu::addComponentFlow(NodeItem* node, const QString& flowName)
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
    newNode->setText(NAME_INDEX, node->nodeName());
    newNode->setData(ID_DATA, Qt::UserRole, node->id());
    newNode->setData(TYPE_DATA, Qt::UserRole, Roles::ComponentRole);

    newFlow = new QTreeWidgetItem(newNode);
  }

  // Create the new flow and add it to the node
  Flow* flow = new Flow(flowName);
  node->addFlow(flow);

  // Assign the tree information
  newFlow->setText(NAME_INDEX, flow->name());
  newFlow->setData(ID_DATA, Qt::UserRole, flow->id());
  newFlow->setData(TYPE_DATA, Qt::UserRole, Roles::FlowRole);

  return flow->id();
}

QTreeWidgetItem* FlowMenu::systemFlows()
{
  return topLevelItem(SYSTEM_FLOWS_INDEX);
}

QTreeWidgetItem* FlowMenu::componentFlows()
{
  return topLevelItem(COMPONENT_FLOWS_INDEX);
}

VoidResult FlowMenu::onNodeAdded(const QString& flowId, NodeItem* node)
{
  // Find parent
  auto parent = getNodeById(flowId);
  if (parent == nullptr)
    return VoidResult::Failed("Could not add node, no such flow");

  // Add item to current flow
  QTreeWidgetItem* newNode = new QTreeWidgetItem(parent);
  newNode->setText(NAME_INDEX, node->nodeName());
  newNode->setData(ID_DATA, Qt::UserRole, node->id());
  newNode->setData(TYPE_DATA, Qt::UserRole, Roles::NodeRole);

  return VoidResult();
}

VoidResult FlowMenu::onNodeRemoved(NodeItem* node)
{
  return VoidResult();
}

VoidResult FlowMenu::onNodeModified(NodeItem* node)
{
  return VoidResult();
}

VoidResult FlowMenu::onNodeSelected(NodeItem* node, bool selected)
{
  return VoidResult();
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

void FlowMenu::showContextMenu(const QPoint& pos)
{
  QTreeWidgetItem* selectedItem = itemAt(pos);
  if (!selectedItem)
    return;

  QMenu contextMenu(this);
  if (selectedItem->data(TYPE_DATA, Qt::UserRole) == Roles::ComponentRole)
  {
  }
  else if (selectedItem->data(TYPE_DATA, Qt::UserRole) == Roles::FlowRole)
  {
    contextMenu.addAction(tr("Edit"), this, [this, selectedItem]() { editFlow(selectedItem); });
    contextMenu.addAction(tr("Delete"), this, [this, selectedItem]() { removeFlow(selectedItem); });
  }
  else
  {
    LOG_WARNING("Unknown item type, ignoring action");
  }

  contextMenu.exec(viewport()->mapToGlobal(pos));
}

void FlowMenu::onItemClicked(QTreeWidgetItem* item, int /* column */)
{
}

void FlowMenu::editFlow(QTreeWidgetItem* item)
{
  auto node = item->parent();
  if (node == nullptr)
  {
    LOG_WARNING("Invalid flow, it has no parent");
    return;
  }

  auto nodeId = node->data(ID_DATA, Qt::UserRole).toString();
  auto flowId = item->data(ID_DATA, Qt::UserRole).toString();
  emit flowSelected(flowId, nodeId);
}

void FlowMenu::removeFlow(QTreeWidgetItem* item)
{
  auto node = item->parent();
  if (node == nullptr)
  {
    LOG_WARNING("Invalid flow, it has no parent");
    return;
  }

  auto nodeId = node->data(ID_DATA, Qt::UserRole).toString();
  auto flowId = item->data(ID_DATA, Qt::UserRole).toString();
  emit flowRemoved(flowId, nodeId);
}
