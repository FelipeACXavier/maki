#include "generator.h"

#include <QDir>

#include "generator_plugin.h"
#include "logging.h"
#include "notifications.h"
#include "pipeline.h"

Generator::Generator(Pipeline* pipeline, QObject* parent)
    : QObject(parent)
    , mPipeline(pipeline)
    , mGenerationFolder("")
{
  connect(mPipeline, &Pipeline::finishedLast, [this](int exitCode, const QString& message) {
    emit generationEnded(mGenerationFolder);
    if (exitCode == 0)
      NOTIFY_INFO(notificationHeader(QFileInfo(mGenerationFolder).fileName()), "Generation finished: {}", message.toStdString());
    else
      NOTIFY_ERROR(notificationHeader(QFileInfo(mGenerationFolder).fileName()), "Generation failed: {}", message.toStdString());
  });
  connect(mPipeline, &Pipeline::errorOccurred, [this](QProcess::ProcessError /* error */, const QString& message) {
    NOTIFY_ERROR(notificationHeader(QFileInfo(mGenerationFolder).fileName()), "Error occurred: {} ", message.toStdString());
  });
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

  auto verified = generator->verify(outputDir);
  if (!verified)
  {
    NOTIFY_ERROR(notificationHeader(generator->languageName()), verified.ErrorMessage());
    return verified;
  }

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

  auto simulated = generator->simulate(outputDir);
  if (!simulated)
  {
    NOTIFY_ERROR(notificationHeader(generator->languageName()), simulated.ErrorMessage());
    return simulated;
  }

  if (pipeline()->size() > 0)
    emit generationStarted(pipeline());

  auto ran = pipeline()->start();
  if (!ran.IsSuccess())
    return VoidResult::Failed("Failed to run pipeline: " + ran.ErrorMessage());

  return VoidResult();
}

std::string Generator::notificationHeader(const QString& languageName) const
{
  if (languageName.isEmpty())
    return "Generator";

  return std::format("Generator ({})", languageName.toStdString());
}