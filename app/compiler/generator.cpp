#include "generator.h"

#include <QDir>

#include "elements/node.h"
#include "generator_plugin.h"
#include "logging.h"
#include "pipeline.h"

Generator::Generator(Pipeline* pipeline, QObject* parent)
    : QObject(parent)
    , mPipeline(pipeline)
    , mGenerationFolder("")
{
  connect(mPipeline, &Pipeline::finishedLast, [this] { emit generationEnded(mGenerationFolder); });
}

Pipeline* Generator::pipeline() const
{
  return mPipeline;
}

VoidResult Generator::generate(const QString& outputDir, maki::IGeneratorPlugin* generator)
{
  if (pipeline()->isRunning())
  {
    LOG_INFO("Pipeline is running aborting");
    return pipeline()->abort();
  }

  mGenerationFolder = outputDir + "/" + generator->languageName();
  pipeline()->setName(generator->languageName());

  RETURN_ON_FAILURE(generator->verify(outputDir));

  if (pipeline()->size() > 0)
    emit generationStarted(pipeline());

  LOG_DEBUG("Running generation pipeline");
  auto ran = pipeline()->start();
  if (!ran.IsSuccess())
    return VoidResult::Failed("Failed to run pipeline: " + ran.ErrorMessage());

  return VoidResult();
}

VoidResult Generator::simulate(const QString& outputDir, maki::IGeneratorPlugin* generator)
{
  if (pipeline()->isRunning())
    return pipeline()->abort();

  mGenerationFolder = outputDir + "/" + generator->languageName();

  pipeline()->setName(generator->languageName());

  RETURN_ON_FAILURE(generator->simulate(outputDir));

  emit generationStarted(pipeline());

  auto ran = pipeline()->start();
  if (!ran.IsSuccess())
    return VoidResult::Failed("Failed to run pipeline: " + ran.ErrorMessage());

  return VoidResult();
}
