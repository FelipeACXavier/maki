#include "system_menu.h"

#include <QInputDialog>
#include <QMenu>

#include "elements/flow.h"
#include "elements/node.h"
#include "logging.h"
#include "theme.h"

static const int NAME_COLUMN = 0;
static const int TYPE_COLUMN = 1;

static const int ID_DATA = 0;
static const int TYPE_DATA = 1;
static const int CANVAS_DATA = 2;

SystemMenu::SystemMenu(QWidget* parent)
    : QTreeWidget(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, &QTreeWidget::customContextMenuRequested, this, &SystemMenu::showContextMenu);
  connect(this, &QTreeWidget::itemDoubleClicked, this, &SystemMenu::onItemClicked);
}

VoidResult SystemMenu::onNodeAdded(const QString& flowId, NodeItem* node)
{
  LOG_TRACE("Node added to system menu: %s - %s (%s)", qPrintable(node->id()), qPrintable(node->nodeType()), qPrintable(flowId));

  // Check if we are adding a structural node
  if (flowId == Config::MAIN_CANVAS)
  {
    auto parent = static_cast<NodeItem*>(node->parentNode());
    if (!parent)
    {
      LOG_DEBUG("%s is a root node", qPrintable(node->nodeType()));
      auto result = addRootNode(node);
      return result;
    }

    auto result = addLeafNode(node);
    return result;
  }

  auto parent = getItemById(flowId);
  if (parent == nullptr)
    return VoidResult::Failed("Could not add node, no such flow");

  // Add item to the tree
  QTreeWidgetItem* newNode = new QTreeWidgetItem(parent);
  newNode->setIcon(0, QIcon(":/icons/flow_block.svg"));
  newNode->setText(NAME_COLUMN, node->nodeName());
  newNode->setText(TYPE_COLUMN, node->nodeType());
  newNode->setData(ID_DATA, Qt::UserRole, node->id());
  newNode->setData(TYPE_DATA, Qt::UserRole, Roles::NodeRole);
  newNode->setData(CANVAS_DATA, Qt::UserRole, flowId);

  return VoidResult();
}

VoidResult SystemMenu::onNodeRemoved(const QString& flowId, const QString& nodeId, const QString& parentId)
{
  LOG_DEBUG("Node removed: %s (%s)", qPrintable(nodeId), qPrintable(flowId));

  if (flowId == Config::MAIN_CANVAS)
  {
    auto item = getItemById(nodeId);
    if (!item)
      return VoidResult::Failed("Node is not in the tree");

    if (parentId.isEmpty())
    {
      takeTopLevelItem(indexOfTopLevelItem(item));
      delete item;
      return VoidResult();
    }

    auto parentItem = getItemById(parentId);
    if (!parentItem)
      return VoidResult::Failed("The parent node is not on the tree");

    parentItem->removeChild(item);
    delete item;
  }
  else
  {
    auto flow = getItemById(flowId);
    if (!flow)
      return VoidResult::Failed("Flow is not in the tree");

    auto component = getItemById(nodeId);
    if (!component)
      return VoidResult::Failed("The node is not in the tree");

    if (component->data(TYPE_DATA, Qt::UserRole) != Roles::NodeRole)
      return VoidResult::Failed("Item is in the tree but it is not a node");

    flow->removeChild(component);
  }

  return VoidResult();
}

VoidResult SystemMenu::onNodeModified(const QString& flowId, NodeItem* node)
{
  if (flowId == Config::MAIN_CANVAS)
  {
    auto item = getItemById(node->id());
    if (!item)
      return VoidResult::Failed("Modified item is not in the tree");

    populateItem(item, node);
  }
  else
  {
    auto component = getItemById(node->id());
    if (component == nullptr)
      return VoidResult();

    populateItem(component, node);
  }

  return VoidResult();
}

VoidResult SystemMenu::onNodeSelected(const QString& flowId, NodeItem* node, bool selected)
{
  return VoidResult();
}

VoidResult SystemMenu::onFlowAdded(Flow* flow, NodeItem* node)
{
  if (!flow)
    return VoidResult();

  if (!flow->modifiable())
    return VoidResult();

  LOG_INFO("Adding flow: %s to %s", qPrintable(flow->name()), qPrintable(node->nodeType()));

  auto parent = getItemById(node->id());
  if (!parent)
    return VoidResult::Failed("Tried to add flow with no parent node");

  QTreeWidgetItem* newFlow = new QTreeWidgetItem(parent);

  // Assign the tree information
  newFlow->setIcon(0, QIcon(":/icons/behaviour.svg"));
  newFlow->setText(NAME_COLUMN, flow->name());
  newFlow->setText(TYPE_COLUMN, "Flow");
  newFlow->setData(ID_DATA, Qt::UserRole, flow->id());
  newFlow->setData(TYPE_DATA, Qt::UserRole, Roles::FlowRole);

  for (const auto& component : flow->getNodes())
  {
    QTreeWidgetItem* newComponent = new QTreeWidgetItem(newFlow);
    newComponent->setIcon(0, QIcon(":/icons/flow_block.svg"));
    newComponent->setText(NAME_COLUMN, component->getProperty("name").toString());
    newComponent->setText(TYPE_COLUMN, component->getnodeId());
    newComponent->setData(ID_DATA, Qt::UserRole, component->getid());
    newComponent->setData(TYPE_DATA, Qt::UserRole, Roles::NodeRole);
  }

  return VoidResult();
}

void collectTreeItems(QTreeWidgetItem* item, QSet<QTreeWidgetItem*>& out)
{
  if (!item)
    return;

  out.insert(item);
  for (int i = 0; i < item->childCount(); ++i)
    collectTreeItems(item->child(i), out);
}

VoidResult SystemMenu::onFlowRemoved(const QString& flowId, const QString& nodeId)
{
  auto nodeItem = getItemById(nodeId);
  if (nodeItem == nullptr)
    return VoidResult();

  auto flowItem = getItemById(flowId);
  if (flowItem == nullptr)
    return VoidResult();

  // If a flow is removed, all the icons from the child nodes also need to be removed
  QSet<QTreeWidgetItem*> itemsToRemove = {flowItem};
  collectTreeItems(flowItem, itemsToRemove);

  nodeItem->removeChild(flowItem);
  delete flowItem;

  return VoidResult();
}

void SystemMenu::populateItem(QTreeWidgetItem* item, NodeItem* node)
{
  item->setText(NAME_COLUMN, node->nodeName());
  item->setText(TYPE_COLUMN, node->nodeType());
  item->setData(ID_DATA, Qt::UserRole, node->id());  // Not shown to user
  item->setData(TYPE_DATA, Qt::UserRole, Roles::NodeRole);
}

VoidResult SystemMenu::addRootNode(NodeItem* node)
{
  QTreeWidgetItem* item = new QTreeWidgetItem(this);
  item->setIcon(0, QIcon(":/icons/structure.svg"));
  populateItem(item, node);
  addTopLevelItem(item);

  return VoidResult();
}

VoidResult SystemMenu::addLeafNode(NodeItem* node)
{
  auto parent = static_cast<NodeItem*>(node->parentNode());
  if (!parent)
    return VoidResult::Failed("No parent, this should be a root");

  auto parentItem = getItemById(parent->id());
  if (!parentItem)
    return VoidResult::Failed("The parent node is not on the tree");

  QTreeWidgetItem* item = new QTreeWidgetItem(parentItem);
  item->setIcon(0, QIcon(":/icons/capability.svg"));
  populateItem(item, node);

  return VoidResult();
}

QTreeWidgetItem* SystemMenu::getItemById(const QString& id)
{
  for (QTreeWidgetItemIterator it(this); *it; ++it)
  {
    if ((*it)->data(ID_DATA, Qt::UserRole).toString() == id)
      return *it;
  }

  return nullptr;
}

void SystemMenu::showContextMenu(const QPoint& pos)
{
  QTreeWidgetItem* selectedItem = itemAt(pos);
  if (!selectedItem)
    return;

  QMenu contextMenu(this);

  LOG_DEBUG("COntext menu type: %d", selectedItem->data(TYPE_DATA, Qt::UserRole).toInt());

  if (selectedItem->data(TYPE_DATA, Qt::UserRole) == Roles::NodeRole)
  {
    contextMenu.addAction(tr("Focus"), this, [this, selectedItem]() {
      emit nodeFocused(selectedItem->data(CANVAS_DATA, Qt::UserRole).toString(), selectedItem->data(ID_DATA, Qt::UserRole).toString());
    });
    contextMenu.addAction(tr("Delete"), this, [this, selectedItem]() {
      emit nodeRemoved(selectedItem->data(CANVAS_DATA, Qt::UserRole).toString(), selectedItem->data(ID_DATA, Qt::UserRole).toString());
    });
  }
  else if (selectedItem->data(TYPE_DATA, Qt::UserRole) == Roles::FlowRole)
  {
    contextMenu.addAction(tr("Edit"), this, [this, selectedItem]() {
      editFlow(selectedItem);
    });
    contextMenu.addAction(tr("Delete"), this, [this, selectedItem]() {
      removeFlow(selectedItem);
    });
  }
  else
  {
    return;
  }

  contextMenu.exec(viewport()->mapToGlobal(pos));
}

void SystemMenu::onItemClicked(QTreeWidgetItem* item, int /* column */)
{
  if (!item)
    return;

  if (item->data(TYPE_DATA, Qt::UserRole) == Roles::FlowRole)
  {
    editFlow(item);
  }
}

void SystemMenu::editFlow(QTreeWidgetItem* item)
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

void SystemMenu::removeFlow(QTreeWidgetItem* item)
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
