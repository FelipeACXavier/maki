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

VoidResult FlowMenu::addSystemFlow(const QString& flowName)
{
  QTreeWidgetItem* item = new QTreeWidgetItem(systemFlows());
  item->setText(NAME_INDEX, flowName);
  return VoidResult();
}

Result<Flow*> FlowMenu::addComponentFlow(NodeItem* node, const QString& flowName)
{
  auto parent = getItemById(node->id());
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
  Flow* flow = node->createFlow(flowName, nullptr);

  // Assign the tree information
  newFlow->setText(NAME_INDEX, flow->name());
  newFlow->setData(ID_DATA, Qt::UserRole, flow->id());
  newFlow->setData(TYPE_DATA, Qt::UserRole, Roles::FlowRole);

  return flow;
}

QTreeWidgetItem* FlowMenu::systemFlows()
{
  return topLevelItem(SYSTEM_FLOWS_INDEX);
}

QTreeWidgetItem* FlowMenu::componentFlows()
{
  return topLevelItem(COMPONENT_FLOWS_INDEX);
}

VoidResult FlowMenu::onFlowAdded(Flow* flow, NodeItem* node)
{
  auto parent = getItemById(node->id());
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

  // Assign the tree information
  newFlow->setText(NAME_INDEX, flow->name());
  newFlow->setData(ID_DATA, Qt::UserRole, flow->id());
  newFlow->setData(TYPE_DATA, Qt::UserRole, Roles::FlowRole);

  for (const auto& component : flow->getNodes())
  {
    QTreeWidgetItem* newComponent = new QTreeWidgetItem(newFlow);
    newComponent->setText(NAME_INDEX, component->properties["name"].toString());
    newComponent->setData(ID_DATA, Qt::UserRole, component->id);
    newComponent->setData(TYPE_DATA, Qt::UserRole, Roles::NodeRole);
  }

  return VoidResult();
}

VoidResult FlowMenu::onFlowRemoved(const QString& flowId, NodeItem* node)
{
  auto nodeItem = getItemById(node->id());
  if (nodeItem == nullptr)
    return VoidResult::Failed("No node to delete");

  auto flowItem = getItemById(flowId);
  if (flowItem == nullptr)
    return VoidResult::Failed("No flow to delete");

  nodeItem->removeChild(flowItem);
  delete flowItem;

  if (nodeItem->childCount() == 0)
  {
    componentFlows()->removeChild(nodeItem);
    delete nodeItem;
  }

  return VoidResult();
}

VoidResult FlowMenu::onNodeAdded(const QString& flowId, NodeItem* node)
{
  // Find parent
  auto parent = getItemById(flowId);
  if (parent == nullptr)
    return VoidResult::Failed("Could not add node, no such flow");

  // Add item to the tree
  QTreeWidgetItem* newNode = new QTreeWidgetItem(parent);
  newNode->setText(NAME_INDEX, node->nodeName());
  newNode->setData(ID_DATA, Qt::UserRole, node->id());
  newNode->setData(TYPE_DATA, Qt::UserRole, Roles::NodeRole);

  return VoidResult();
}

VoidResult FlowMenu::onNodeRemoved(const QString& flowId, NodeItem* node)
{
  auto flow = getItemById(flowId);
  if (!flow)
    return VoidResult::Failed("Flow is not in the tree");

  auto component = getItemById(node->id());
  if (!component)
    return VoidResult::Failed("The node is not in the tree");

  if (component->data(TYPE_DATA, Qt::UserRole) != Roles::NodeRole)
    return VoidResult::Failed("Item is in the tree but it is not a node");

  flow->removeChild(component);

  return VoidResult();
}

VoidResult FlowMenu::onNodeModified(const QString& /* flowId */, NodeItem* node)
{
  auto component = getItemById(node->id());
  if (component == nullptr)
    return VoidResult();

  component->setText(NAME_INDEX, node->nodeName());

  return VoidResult();
}

VoidResult FlowMenu::onNodeSelected(const QString& flowId, NodeItem* node, bool selected)
{
  return VoidResult();
}

QTreeWidgetItem* FlowMenu::getItemById(const QString& id)
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
  LOG_DEBUG("Removing flow");
  emit flowRemoved(flowId, nodeId);
}
