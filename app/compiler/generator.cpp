#include "generator.h"

#include "elements/connection.h"
#include "elements/connector.h"
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

  LOG_INFO("======================================");
  LOG_INFO("Starting generation");
  // Main generation loop, we need to:
  // For each component:
  //    1. Find all starting points
  //    2. Define the functions
  //    3. Write the computations
  //    4. Connect the callbacks
  QString text = generator->generateCode(canvas->items());
  LOG_INFO("Generated code:");
  LOG_INFO(text.toStdString());
  LOG_INFO("======================================");
}
