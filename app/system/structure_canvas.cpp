#include "structure_canvas.h"

#include <memory>

#include "data_flow_model.h"
#include "elements/flow.h"
#include "logging.h"
#include "node_info.h"
#include "type_registry.h"

StructureCanvas::StructureCanvas(std::shared_ptr<SaveInfo> storage, const QString& canvasId, std::shared_ptr<ConfigurationTable> configTable,
                                 std::shared_ptr<EdgeRouter> router, QObject* parent)
    : Canvas(canvasId, configTable, router, parent)
    , mStorage(storage)
{
}

Types::LibraryTypes StructureCanvas::type() const
{
  return Types::LibraryTypes::STRUCTURAL;
}

void StructureCanvas::updateParent(NodeItem* node, std::shared_ptr<NodeSaveInfo> storage, bool adding)
{
  if (!adding)
    mStorage->removeNode(node->id());
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

void StructureCanvas::suggestCapability(NodeItem* node)
{
  // We don't need to check tasks
  if (!node->parentNode())
    return;

  LOG_DEBUG("Looking for suggestions for {}", node->nodeName());
  const auto config = getNodeConfig(node->nodeType());
  if (!config)
    return;

  QStringList producers;
  QStringList consumers;

  for (const auto& event : config->events)
  {
    for (const auto& argument : event.arguments)
    {
      const maki::DataPort port{.nodeId = node->nodeId(), .eventName = event.name, .argumentName = argument.id};

      if (event.type == Types::CallType::TRIGGER)
      {
        // This concrete input is already connected to some DataEntry.
        if (maki::DataFlowModel::instance().entryConsumedBy(port).has_value())
          continue;

        // The type comes from the capability definition.
        for (const auto& producer : maki::TypeRegistry::instance().findProducers(argument.type))
          if (!mStorage->taskHasCapability(node->parentNode()->id(), producer))
            producers << producer;
      }
      else if (event.type == Types::CallType::RETURN)
      {
        const auto entryId = maki::DataFlowModel::instance().entryProducedBy(port);

        // If this output already has a DataEntry and that entry
        // already has consumers, don't suggest more consumers.
        if (entryId)
        {
          const auto* entry = maki::DataFlowModel::instance().get(*entryId);
          if (entry && !entry->consumers.empty())
            continue;
        }

        for (const auto& consumer : maki::TypeRegistry::instance().findConsumers(argument.type))
          if (!mStorage->taskHasCapability(node->parentNode()->id(), consumer))
            consumers << consumer;
      }
    }
  }

  producers.removeDuplicates();
  consumers.removeDuplicates();

  if (producers.isEmpty() && consumers.isEmpty())
    return;

  // for (const auto& producer : producers)
  //   LOG_DEBUG("Producer suggestion: {}", producer);
  // for (const auto& consumer : consumers)
  //   LOG_DEBUG("Consumer suggestion: {}", consumer);

  suggestedNodes(node, consumers, producers);
}