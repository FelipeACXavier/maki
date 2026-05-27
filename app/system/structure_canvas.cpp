#include "structure_canvas.h"

#include <memory>

#include "document/node_info.h"
#include "elements/flow.h"
#include "elements/node.h"
#include "logging.h"
#include "node_info.h"
#include "system/config_table.h"

StructureCanvas::StructureCanvas(std::shared_ptr<SaveInfo> storage, const QString& canvasId, std::shared_ptr<ConfigurationTable> configTable, QObject* parent)
    : Canvas(canvasId, configTable, parent)
    , mStorage(storage)
{
}

Types::LibraryTypes StructureCanvas::type() const
{
  return Types::LibraryTypes::STRUCTURAL;
}

void StructureCanvas::finalizeAfterLoad()
{
  for (QGraphicsItem* item : items())
  {
    if (item->type() != NodeItem::Type)
      continue;

    auto* node = static_cast<NodeItem*>(item);
    if (node->isTaskContainer() && node->parentNode() == nullptr)
      node->layoutSubtasks();
  }
}

void StructureCanvas::updateParent(NodeItem* node, std::shared_ptr<NodeSaveInfo> storage, bool adding)
{
  Q_UNUSED(storage);

  if (!node)
    return;

  if (!adding)
  {
    mStorage->removeNode(node->id());
    return;
  }

  if (mDeferStructuralLayout)
  {
    if (node->isTaskContainer())
      node->ensureSubtaskConnector(this);
    return;
  }

  NodeItem* parent = node->parentNode();
  if (parent && parent->isTaskContainer())
  {
    if (node->isTaskContainer())
      parent->layoutSubtasks();
    else
    {
      parent->relayoutCapabilitySlots();
      parent->layoutSubtasks();
    }
  }

  if (node->isTaskContainer())
    node->ensureSubtaskConnector(this);
}

void StructureCanvas::addedItemNode(NodeItem* node, std::shared_ptr<NodeSaveInfo> info)
{
  if (node->parentNode() == nullptr)
  {
    // Adding task
    mStorage->addNode(info);
  }

  Canvas::addedItemNode(node, info);
}
