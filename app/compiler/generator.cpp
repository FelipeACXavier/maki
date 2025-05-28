#include "generator.h"

#include "elements/node.h"
#include "generator_plugin.h"
#include "logging.h"

Generator::Generator(std::shared_ptr<SaveInfo> storage)
  : mStorage(storage)
{
}

void Generator::generate(GeneratorPlugin* generator)
{
  if (!mStorage)
  {
    LOG_ERROR("No storage available");
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
  QString text = generator->generateCode(mStorage);
  // LOG_INFO("Generated code:");
  // LOG_INFO("%s", qPrintable(text));
  LOG_INFO("======================================");
}
