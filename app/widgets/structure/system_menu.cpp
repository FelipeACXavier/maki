#include "system_menu.h"

#include <QInputDialog>
#include <QMenu>

#include "elements/node.h"
#include "logging.h"

static const int NAME_COLUMN = 0;
static const int TYPE_COLUMN = 1;
static const int ID_DATA = 0;

SystemMenu::SystemMenu(QWidget* parent)
    : QTreeWidget(parent)
{
  setSelectionMode(QAbstractItemView::ExtendedSelection);
  connect(this, &QTreeWidget::itemSelectionChanged, this, &SystemMenu::onSelectionChanged);

  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, &QTreeWidget::customContextMenuRequested, this, &SystemMenu::showContextMenu);
  // connect(this, &QTreeWidget::itemClicked, this, &SystemMenu::onItemClicked);
}

VoidResult SystemMenu::onNodeAdded(NodeItem* node)
{
  LOG_DEBUG("Node added: %s", qPrintable(node->id()));

  auto parent = static_cast<NodeItem*>(node->parentNode());
  if (!parent)
    return addRootNode(node);

  return addLeafNode(node);
}

VoidResult SystemMenu::onNodeRemoved(NodeItem* node)
{
  LOG_INFO("Node removed: %s", qPrintable(node->id()));

  auto item = getItemById(node->id());
  if (!item)
    return VoidResult::Failed("Node is not in the tree");

  auto parent = static_cast<NodeItem*>(node->parentNode());
  if (!parent)
  {
    takeTopLevelItem(indexOfTopLevelItem(item));
    delete item;
    return VoidResult();
  }

  auto parentItem = getItemById(parent->id());
  if (!parentItem)
    return VoidResult::Failed("The parent node is not on the tree");

  parentItem->removeChild(item);
  delete item;

  return VoidResult();
}

VoidResult SystemMenu::onNodeModified(NodeItem* node)
{
  auto item = getItemById(node->id());
  if (!item)
    return VoidResult::Failed("Modified item is not in the tree");

  populateItem(item, node);

  return VoidResult();
}

VoidResult SystemMenu::onNodeSelected(NodeItem* /* node */, bool /* selected */)
{
  return VoidResult();
}

void SystemMenu::populateItem(QTreeWidgetItem* item, NodeItem* node)
{
  item->setText(NAME_COLUMN, node->nodeName());
  item->setText(TYPE_COLUMN, node->nodeType());
  item->setData(ID_DATA, Qt::UserRole, node->id());  // Not shown to user
}

VoidResult SystemMenu::addRootNode(NodeItem* node)
{
  QTreeWidgetItem* item = new QTreeWidgetItem(this);
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
  contextMenu.addAction(tr("New flow"), this, []() { LOG_WARNING("Not implemented"); });
  contextMenu.addAction(tr("Focus"), this, [this, selectedItem]() { emit nodeFocused(selectedItem->data(ID_DATA, Qt::UserRole).toString()); });
  contextMenu.addAction(tr("Delete"), this, [this, selectedItem]() { emit nodeRemoved(selectedItem->data(ID_DATA, Qt::UserRole).toString()); });

  contextMenu.exec(viewport()->mapToGlobal(pos));
}

void SystemMenu::onItemClicked(QTreeWidgetItem* item, int /* column */)
{
  if (!item)
    return;

  emit nodeSelected({item->data(ID_DATA, Qt::UserRole).toString()});
}

void SystemMenu::onSelectionChanged()
{
  QList<QTreeWidgetItem*> items = selectedItems();

  QList<QString> selectedNodes;
  for (QTreeWidgetItem* item : items)
    selectedNodes.push_back(item->data(ID_DATA, Qt::UserRole).toString());

  emit nodeSelected(selectedNodes);
}
