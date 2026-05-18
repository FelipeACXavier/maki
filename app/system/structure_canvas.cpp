#include "structure_canvas.h"

#include "system/config_table.h"
#include "document/node_info.h"
#include "elements/node.h"

StructureCanvas::StructureCanvas(const QString& canvasId, std::shared_ptr<SaveInfo> storage, std::shared_ptr<ConfigurationTable> configTable, QObject* parent)
    : Canvas(canvasId, storage, configTable, parent)
{
}

Types::LibraryTypes StructureCanvas::type() const
{
  return Types::LibraryTypes::STRUCTURAL;
}

void StructureCanvas::updateParent(NodeItem* node, std::shared_ptr<NodeSaveInfo> storage, bool adding)
{
  Q_UNUSED(storage);

  if (!node || !adding)
    return;

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
