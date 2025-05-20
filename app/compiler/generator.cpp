#include "generator.h"

#include "elements/node.h"
#include "logging.h"
#include "system/generator_plugin.h"

Generator::Generator()
{
}

void Generator::generate(GeneratorPlugin* generator, Canvas* canvas)
{
  if (!canvas)
  {
    LOG_ERROR("No canvas provided");
    return;
  }

  QList<INode*> nodes;

  for (const auto& item : canvas->items())
  {
    if (item->type() != NodeItem::Type)
      continue;

    // Use top level nodes only
    auto node = dynamic_cast<NodeItem*>(item);
    if (!node->parentNode())
      nodes.push_back(node);
  }

  LOG_INFO("======================================");
  LOG_INFO("Starting generation");
  // Main generation loop, we need to:
  // For each component:
  //    1. Find all starting points
  //    2. Define the functions
  //    3. Write the computations
  //    4. Connect the callbacks
  QString text = generator->generateCode(nodes);
  LOG_INFO("Generated code:");
  LOG_INFO(text.toStdString());
  LOG_INFO("======================================");
}
