#include "structure_canvas.h"

#include <memory>

#include "data_flow_model.h"
#include "elements/flow.h"
#include "keys.h"
#include "logging.h"
#include "type_registry.h"

StructureCanvas::StructureCanvas(const QString& canvasId, std::shared_ptr<EdgeRouter> router, QObject* parent)
    : Canvas(canvasId, router, parent)
{
}

Types::LibraryTypes StructureCanvas::type() const
{
  return Types::LibraryTypes::STRUCTURAL;
}

QString StructureCanvas::requestCreateNode(NodeSaveInfo info, NodeCreation creation)
{
  LOG_DEBUG("StructureCanvas::requestCreateNode: {}=={}", info.getnodeId(), ConfigKeys::TASK_NODE);
  // The model must know the ID before it is inserted.
  if (info.getid().isEmpty())
    info.setId(QUuid::createUuid().toString());

  const auto id = info.getid();
  mPendingCreations.insert(id, creation);
  auto storage = std::make_shared<NodeSaveInfo>(info);

  auto exists = SaveInfo::instance().getNodeWithId(info.getid());
  if (!exists)
    SaveInfo::instance().addNode(storage);
  else
    LOG_ERROR("Node {} already exists", storage->getid());

  return id;
}

void StructureCanvas::requestRemoveNode(NodeSaveInfo info)
{
  SaveInfo::instance().removeNode(info.getid());
}

void StructureCanvas::addedItemNode(NodeItem* node, std::shared_ptr<NodeSaveInfo> info)
{
  // if (node->nodeId() == ConfigKeys::TASK_NODE)
  // {
  //   // Adding task
  //   const auto exists = SaveInfo::instance().getNodeWithId(info->getid());
  //   if (!exists)
  //     SaveInfo::instance().addNode(info);
  // }

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
          if (!SaveInfo::instance().taskHasCapability(node->parentNode()->id(), producer))
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
          if (!SaveInfo::instance().taskHasCapability(node->parentNode()->id(), consumer))
            consumers << consumer;
      }
    }
  }

  producers.removeDuplicates();
  consumers.removeDuplicates();

  if (producers.isEmpty() && consumers.isEmpty())
    return;

  suggestedNodes(node, consumers, producers);
}
