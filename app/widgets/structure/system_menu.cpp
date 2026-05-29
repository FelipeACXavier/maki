#include "system_menu.h"

#include <QHeaderView>
#include <QInputDialog>
#include <QMenu>

#include "app_configs.h"
#include "elements/flow.h"
#include "elements/node.h"
#include "logging.h"

static const int NAME_COLUMN = 0;
static const int TYPE_COLUMN = 1;

static const int ID_DATA = 0;
static const int TYPE_DATA = 1;
static const int CANVAS_DATA = 2;

SystemMenu::SystemMenu(QWidget* parent)
    : QTreeWidget(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  setColumnCount(2);
  setIndentation(15);
  setExpandsOnDoubleClick(false);
  setHeaderLabels({tr("Name"), tr("Type")});
  header()->setAlternatingRowColors(true);
  header()->setSectionResizeMode(0, QHeaderView::Stretch);

  setColumnWidth(1, 100);
  header()->setStretchLastSection(false);
  header()->setSectionResizeMode(1, QHeaderView::Fixed);
  header()->setTextElideMode(Qt::ElideRight);
  header()->setSectionsMovable(false);

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
  populateItem(newNode, QIcon(":/icons/flow_block.svg"), node->nodeName(), node->nodeType(), node->id(), Roles::NodeRole, flowId);

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
  // auto item = getItemById(node->id());
  // if (!item)
  //   return VoidResult::Failed("Modified item is not in the tree");
  // populateItem(item, QIcon(), node->nodeName(), node->nodeType(), node->id(), Roles::NodeRole);
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

  auto parent = findParentItemByRole(node->id(), Roles::Flows);
  if (!parent)
    return VoidResult::Failed("Tried to add flow with no parent node");

  QTreeWidgetItem* newFlow = new QTreeWidgetItem(parent);

  // Assign the tree information
  populateItem(newFlow, QIcon(":/icons/flow.svg"), flow->name(), tr("Flow"), flow->id(), Roles::FlowRole, flow->id());

  for (const auto& component : flow->getNodes())
  {
    QTreeWidgetItem* newComponent = new QTreeWidgetItem(newFlow);
    populateItem(newComponent, QIcon(":/icons/flow_block.svg"), component->getProperty("name").toString(), component->getnodeId(), component->getid(), Roles::NodeRole, flow->id());
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

void SystemMenu::populateItem(QTreeWidgetItem* item, const QIcon& icon, const QString& name,
                              const QString& type, const QString& data, const Roles role, const QString& canvas)
{
  item->setIcon(0, icon);
  item->setText(NAME_COLUMN, name);
  item->setText(TYPE_COLUMN, type);
  item->setData(ID_DATA, Qt::UserRole, data);  // Not shown to user
  item->setData(TYPE_DATA, Qt::UserRole, role);
  if (!canvas.isEmpty())
    item->setData(CANVAS_DATA, Qt::UserRole, canvas);
}

VoidResult SystemMenu::addRootNode(NodeItem* node)
{
  // Root nodes are composed of two subnodes, Capabilities and Flows
  QTreeWidgetItem* item = new QTreeWidgetItem(this);
  populateItem(item, QIcon(":/icons/task.svg"), node->nodeName(), node->nodeType(), node->id(), Roles::NodeRole);
  addTopLevelItem(item);

  QTreeWidgetItem* capabilities = new QTreeWidgetItem(item);
  populateItem(capabilities, QIcon(":/icons/structure.svg"), tr("Capabilities"), "", node->id(), Roles::Capabilities);

  QTreeWidgetItem* flows = new QTreeWidgetItem(item);
  populateItem(flows, QIcon(":/icons/behaviour.svg"), tr("Flows"), "", node->id(), Roles::Flows);

  return VoidResult();
}

VoidResult SystemMenu::addLeafNode(NodeItem* node)
{
  auto parent = static_cast<NodeItem*>(node->parentNode());
  if (!parent)
    return VoidResult::Failed("No parent, this should be a root");

  auto parentItem = findParentItemByRole(parent->id(), Roles::Capabilities);
  if (!parentItem)
    return VoidResult::Failed("The parent node is not on the tree");

  QTreeWidgetItem* item = new QTreeWidgetItem(parentItem);
  populateItem(item, QIcon(":/icons/capability.svg"), node->nodeName(), node->nodeType(), node->id(), Roles::NodeRole);

  return VoidResult();
}

QTreeWidgetItem* SystemMenu::getItemById(const QString& id) const
{
  for (QTreeWidgetItemIterator it(const_cast<SystemMenu*>(this)); *it; ++it)
  {
    if ((*it)->data(ID_DATA, Qt::UserRole).toString() == id)
      return *it;
  }

  return nullptr;
}

QTreeWidgetItem* SystemMenu::findParentItemByRole(const QString& id, Roles role) const
{
  auto parent = getItemById(id);
  if (!parent)
    return nullptr;

  for (int i = 0; i < parent->childCount(); ++i)
  {
    auto* child = parent->child(i);
    if (child->data(TYPE_DATA, Qt::UserRole) == role)
      return child;
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
    contextMenu.addAction(QIcon::fromTheme("search"), tr("Focus"), this, [this, selectedItem]() {
      emit nodeFocused(selectedItem->data(CANVAS_DATA, Qt::UserRole).toString(), selectedItem->data(ID_DATA, Qt::UserRole).toString());
    });
    contextMenu.addAction(QIcon::fromTheme("edit-delete"), tr("Delete"), this, [this, selectedItem]() {
      emit nodeRemoved(selectedItem->data(CANVAS_DATA, Qt::UserRole).toString(), selectedItem->data(ID_DATA, Qt::UserRole).toString());
    });
  }
  else if (selectedItem->data(TYPE_DATA, Qt::UserRole) == Roles::FlowRole)
  {
    contextMenu.addAction(QIcon::fromTheme("document-edit"), tr("Edit"), this, [this, selectedItem]() {
      editFlow(selectedItem);
    });
    contextMenu.addAction(QIcon::fromTheme("edit-delete"), tr("Delete"), this, [this, selectedItem]() {
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
