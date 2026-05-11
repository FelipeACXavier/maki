#include "generator.h"

#include <QDir>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QWidget>
#include <oclero/qlementine/widgets/Label.hpp>

#include "iplugin.h"
#include "logging.h"
#include "notifications.h"
#include "result.h"
#include "style_helpers.h"

Generator::Generator(Pipeline* pipeline, QObject* parent)
    : QObject(parent)
    , mPipeline(pipeline)
    , mGenerationFolder("")
    , mProgressId("")
    , mIsRunning(false)
{
  connect(mPipeline, &Pipeline::finishedLast, [this](const Pipeline::Info& info, int exitCode, const QString& message) {
    if (!mIsRunning)
      return;

    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Generation Progress", mPipeline->progressWidget());
    if (exitCode == 0)
      NOTIFY_INFO(notificationHeader(QFileInfo(mGenerationFolder).fileName()), "Generation finished: {}", message.toStdString());
    else
      NOTIFY_ERROR(notificationHeader(QFileInfo(mGenerationFolder).fileName()), "Generation failed: {}", message.toStdString());

    // Send an empty content so the widget is cleared
    NOTIFY_LONG_INFO(mProgressId, "Generation Progress", nullptr);
    mProgressId.clear();
    mIsRunning = false;

    emit generationEnded(mGenerationFolder);
  });
  connect(mPipeline, &Pipeline::errorOccurred, [this](const Pipeline::Info& info, QProcess::ProcessError /* error */, const QString& message) {
    if (!mIsRunning)
      return;

    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Generation Progress", mPipeline->progressWidget());
    NOTIFY_ERROR(notificationHeader(QFileInfo(mGenerationFolder).fileName()), "Error occurred: {} ", message.toStdString());
    mProgressId.clear();
    mIsRunning = false;
  });
  connect(mPipeline, &Pipeline::startingPipeline, [this](const Pipeline::Info& info) {
    if (!mIsRunning)
      return;

    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Generation Progress", mPipeline->progressWidget());
  });
  connect(mPipeline, &Pipeline::startingGroup, [this](const Pipeline::Info& info, const QString& groupName) {
    if (!mIsRunning)
      return;

    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Generation Progress", mPipeline->progressWidget());
  });
  connect(mPipeline, &Pipeline::processStarted, [this](const Pipeline::Info& info, const QString& /* process */, const QStringList& /* arguments */) {
    if (!mIsRunning)
      return;

    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Generation Progress", mPipeline->progressWidget());
  });
  connect(mPipeline, &Pipeline::finishedGroup, [this](const Pipeline::Info& info, const QString& groupName, int exitCode, const QString& message) {
    if (!mIsRunning)
      return;

    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Generation Progress", mPipeline->progressWidget());
  });
}

Pipeline* Generator::pipeline() const
{
  return mPipeline;
}

VoidResult Generator::generate(const QString& outputDir, maki::IPlugin* generator)
{
  if (!generator)
    return VoidResult::Failed("No generator passed");

  // if (pipeline()->isRunning())
  // {
  //   LOG_INFO("Pipeline is running aborting");
  //   return pipeline()->abort();
  // }

  // mGenerationFolder = outputDir + "/" + generator->languageName();
  // pipeline()->setName(generator->languageName());

  // LOG_INFO("Runnning generator with folder: %s", qPrintable(mGenerationFolder));
  // auto verified = generator->verify(mGenerationFolder);
  // if (!verified)
  // {
  //   NOTIFY_ERROR(notificationHeader(generator->languageName()), verified.ErrorMessage());
  //   return verified;
  // }

  // if (pipeline()->size() > 0)
  //   emit generationStarted(pipeline());

  // LOG_DEBUG("Running generation pipeline");
  // auto ran = pipeline()->start();
  // if (!ran.IsSuccess())
  //   return VoidResult::Failed("Failed to run pipeline: " + ran.ErrorMessage());

  // mIsRunning = true;
  return VoidResult();
}

VoidResult Generator::simulate(const QString& outputDir, maki::IPlugin* generator)
{
  if (!generator)
    return VoidResult::Failed("No generator passed");

  // if (pipeline()->isRunning())
  //   return pipeline()->abort();

  // mGenerationFolder = outputDir + "/" + generator->languageName();

  // pipeline()->setName(generator->languageName());

  // auto simulated = generator->simulate(mGenerationFolder);
  // if (!simulated)
  // {
  //   NOTIFY_ERROR(notificationHeader(generator->languageName()), simulated.ErrorMessage());
  //   return simulated;
  // }

  // if (pipeline()->size() > 0)
  //   emit generationStarted(pipeline());

  // auto ran = pipeline()->start();
  // if (!ran.IsSuccess())
  //   return VoidResult::Failed("Failed to run pipeline: " + ran.ErrorMessage());

  // mIsRunning = true;
  return VoidResult();
}

std::string Generator::notificationHeader(const QString& languageName) const
{
  if (languageName.isEmpty())
    return "Generator";

  return std::format("Generator ({})", languageName.toStdString());
}
