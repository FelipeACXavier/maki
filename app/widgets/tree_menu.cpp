#include "tree_menu.h"

#include "elements/node.h"
#include "logging.h"

static const int NAME_COLUMN = 0;
static const int TYPE_COLUMN = 1;
static const int ID_COLUMN = 2;

TreeMenu::TreeMenu(QWidget* parent)
    : QTreeWidget(parent)
{
}

VoidResult TreeMenu::onNodeAdded(NodeItem* node)
{
  LOG_DEBUG("Node added: %s", qPrintable(node->id()));

  auto parent = static_cast<NodeItem*>(node->parentNode());
  if (!parent)
    return addRootNode(node);

  return addLeafNode(node);
}

VoidResult TreeMenu::onNodeRemoved(NodeItem* node)
{
  LOG_INFO("Node removed: %s", qPrintable(node->id()));

  auto item = getItemById(node->id());
  if (!item)
    return VoidResult::Failed("Node is not in the tree");

  auto parent = static_cast<NodeItem*>(node->parentNode());
  if (!parent)
  {
    takeTopLevelItem(indexOfTopLevelItem(item));
    return VoidResult();
  }

  auto parentItem = getItemById(parent->id());
  if (!parentItem)
    return VoidResult::Failed("The parent node is not on the tree");

  parentItem->removeChild(item);

  return VoidResult();
}

void TreeMenu::populateItem(QTreeWidgetItem* item, NodeItem* node)
{
  item->setText(NAME_COLUMN, node->nodeName());
  item->setText(TYPE_COLUMN, node->nodeType());
  item->setText(ID_COLUMN, node->id());
}

VoidResult TreeMenu::addRootNode(NodeItem* node)
{
  QTreeWidgetItem* item = new QTreeWidgetItem(this);
  populateItem(item, node);
  addTopLevelItem(item);

  return VoidResult();
}

VoidResult TreeMenu::addLeafNode(NodeItem* node)
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

QTreeWidgetItem* TreeMenu::getItemById(const QString& id)
{
  for (QTreeWidgetItemIterator it(this); *it; ++it)
  {
    if ((*it)->text(ID_COLUMN) == id)
      return *it;
  }

  return nullptr;
}
