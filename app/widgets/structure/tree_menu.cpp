#include "tree_menu.h"

#include <QInputDialog>
#include <QMenu>

#include "elements/node.h"
#include "logging.h"

static const int NAME_COLUMN = 0;
static const int TYPE_COLUMN = 1;
static const int ID_COLUMN = 2;

TreeMenu::TreeMenu(QWidget* parent)
    : QTreeWidget(parent)
{
  setContextMenuPolicy(Qt::CustomContextMenu);

  connect(this, &QTreeWidget::customContextMenuRequested, this, &TreeMenu::showContextMenu);
  connect(this, &QTreeWidget::itemClicked, this, &TreeMenu::onItemClicked);
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

VoidResult TreeMenu::onNodeModified(NodeItem* node)
{
  auto item = getItemById(node->id());
  if (!item)
    return VoidResult::Failed("Modified item is not in the tree");

  populateItem(item, node);

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

void TreeMenu::showContextMenu(const QPoint& pos)
{
  QTreeWidgetItem* selectedItem = itemAt(pos);
  if (!selectedItem)
    return;

  QMenu contextMenu(this);
  QAction* deleteAction = contextMenu.addAction("Delete");
  // QAction* renameAction = contextMenu.addAction("Rename Node");
  QAction* focusAction = contextMenu.addAction("Focus");

  QAction* selectedAction = contextMenu.exec(viewport()->mapToGlobal(pos));
  if (selectedAction == deleteAction)
  {
    emit nodeRemoved(selectedItem->text(ID_COLUMN));
  }
  // else if (selectedAction == renameAction)
  // {
  //   bool ok;
  //   QString newName = QInputDialog::getText(this, "Rename Node", "New Name:", QLineEdit::Normal, selectedItem->text(NAME_COLUMN), &ok);
  //   if (ok && !newName.isEmpty())
  //     emit nodeRenamed(selectedItem->text(ID_COLUMN), newName);
  // }
  else if (selectedAction == focusAction)
  {
    emit nodeFocused(selectedItem->text(ID_COLUMN));
  }
}

void TreeMenu::onItemClicked(QTreeWidgetItem* item, int /* column */)
{
  if (!item)
    return;

  emit nodeSelected(item->text(ID_COLUMN));
}
