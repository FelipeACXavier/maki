#include "generator.h"

#include <QDir>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QWidget>
#include <oclero/qlementine/widgets/Label.hpp>

#include "generator_plugin.h"
#include "logging.h"
#include "notifications.h"
#include "style_helpers.h"

Generator::Generator(Pipeline* pipeline, QObject* parent)
    : QObject(parent)
    , mPipeline(pipeline)
    , mGenerationFolder("")
    , mProgressId("")
{
  connect(mPipeline, &Pipeline::finishedLast, [this](const Pipeline::Info& info, int exitCode, const QString& message) {
    mInfo = info;
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Generation Progress", progressContent());
    if (exitCode == 0)
      NOTIFY_INFO(notificationHeader(QFileInfo(mGenerationFolder).fileName()), "Generation finished: {}", message.toStdString());
    else
      NOTIFY_ERROR(notificationHeader(QFileInfo(mGenerationFolder).fileName()), "Generation failed: {}", message.toStdString());

    // Send an empty content so the widget is cleared
    NOTIFY_LONG_INFO(mProgressId, "Generation Progress", nullptr);
    mProgressId.clear();
    emit generationEnded(mGenerationFolder);
  });
  connect(mPipeline, &Pipeline::errorOccurred, [this](const Pipeline::Info& info, QProcess::ProcessError /* error */, const QString& message) {
    mInfo = info;
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Generation Progress", progressContent());
    NOTIFY_ERROR(notificationHeader(QFileInfo(mGenerationFolder).fileName()), "Error occurred: {} ", message.toStdString());
    mProgressId.clear();
  });
  connect(mPipeline, &Pipeline::startingPipeline, [this](const Pipeline::Info& info) {
    mInfo = info;
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Generation Progress", progressContent());
  });
  connect(mPipeline, &Pipeline::startingGroup, [this](const Pipeline::Info& info, const QString& groupName) {
    mInfo = info;
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Generation Progress", progressContent());
  });
  connect(mPipeline, &Pipeline::processStarted, [this](const Pipeline::Info& info, const QString& /* process */, const QStringList& /* arguments */) {
    mInfo = info;
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Generation Progress", progressContent());
  });
  connect(mPipeline, &Pipeline::finishedGroup, [this](const Pipeline::Info& info, const QString& groupName, int exitCode, const QString& message) {
    mInfo = info;
    mProgressId = NOTIFY_LONG_INFO(mProgressId, "Generation Progress", progressContent());
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

QWidget* Generator::progressContent()
{
  QWidget* container = new QWidget();
  container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  QVBoxLayout* layout = new QVBoxLayout(container);

  auto getRunningTask = [](Pipeline::GroupInfo group) {
    bool allDone = true;
    for (const auto& p : group.processes)
    {
      if (p.status == Pipeline::State::Running)
        return p.name;
      if (p.status == Pipeline::State::Error)
        return QString("Error");

      allDone = allDone && (p.status == Pipeline::State::Done);
    }

    return QString(allDone ? "Done" : "Waiting");
  };
  auto getCompleteTasks = [](Pipeline::GroupInfo group) {
    int count = 0;
    for (const auto& p : group.processes)
      if (p.status == Pipeline::State::Done)
        count++;

    return count;
  };

  // For each group, we have
  for (const auto& group : mInfo.groupInfo)
  {
    if (group.processes.isEmpty())
      continue;

    auto* row = new QWidget(container);
    row->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto* rowLayout = new QVBoxLayout(row);
    rowLayout->setContentsMargins(0, 0, 0, 0);
    rowLayout->setSpacing(4);

    // Header row: group name + count
    auto* header = new QWidget(row);
    header->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(0, 0, 0, 0);
    headerLayout->setSpacing(8);

    auto* nameLabel = new oclero::qlementine::Label(group.name, header);
    nameLabel->setRole(oclero::qlementine::TextRole::H4);
    nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto completed = getCompleteTasks(group);
    auto total = group.processes.count();
    auto* countLabel = new QLabel(QString("%1 / %2 tasks").arg(completed).arg(total), header);

    headerLayout->addWidget(nameLabel);
    headerLayout->addWidget(countLabel);

    // Progress bar
    auto* progress = new QProgressBar(row);
    progress->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    progress->setRange(0, total);
    progress->setValue(completed);
    progress->setTextVisible(false);

    // Current task label
    auto currentTask = getRunningTask(group);
    auto* currentTaskLabel = new oclero::qlementine::Label(currentTask, row);
    currentTaskLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    currentTaskLabel->setWordWrap(false);
    currentTaskLabel->setRole(oclero::qlementine::TextRole::H5);

    rowLayout->addWidget(header);
    rowLayout->addWidget(progress);
    rowLayout->addWidget(currentTaskLabel);

    layout->addWidget(row);
  }

  return container;
}
