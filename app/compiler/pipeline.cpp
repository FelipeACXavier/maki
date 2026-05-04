#include "pipeline.h"

#include <qobject.h>

#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>

#include "logging.h"
#include "result.h"

static const QString DEFAULT_GROUP = "Default";
static const int SUCCESS = 0;
static const QRegularExpression ansiRe("\x1B\\[[0-9;?]*[ -/]*[@-~]", QRegularExpression::DontCaptureOption);

Pipeline::Pipeline(QObject* parent)
    : QObject(parent)
    , mState(State::Idle)
    , mName("")
    , mRunningProcess(nullptr)
    , mGroupIndex(0)
    , mCurrentGroup("")
{
  // Pipeline always has at least one, default, group
  auto group = std::make_shared<PipelineGroup>();
  group->name = DEFAULT_GROUP;
  group->processes = {};
  mGroups.push_back(group);
}

Pipeline::~Pipeline()
{
  abort();
}

QString Pipeline::name() const
{
  return mName;
}

void Pipeline::setName(const QString& name)
{
  mName = name;
}

int Pipeline::size() const
{
  int accumulation = 0;
  for (const auto& group : mGroups)
    accumulation += group->size();

  return accumulation;
}

bool Pipeline::isRunning() const
{
  std::unique_lock<std::mutex> lock(mStateMutex);
  return mState != State::Idle;
}

VoidResult Pipeline::abort()
{
  // Nothing to abort
  if (!mRunningProcess || !mRunningProcess->process)
    return VoidResult();

  {
    std::unique_lock<std::mutex> lock(mStateMutex);
    mState = State::Aborting;
  }

  // Abort the running process
  auto process = mRunningProcess->process;
  if (process->state() == QProcess::Running)
  {
    process->terminate();

    if (!process->waitForFinished(2500))
    {
      process->kill();
      process->waitForFinished();
    }
  }

  clearGroups();

  {
    std::unique_lock<std::mutex> lock(mStateMutex);
    mState = State::Idle;
  }

  return VoidResult();
}

int Pipeline::getIndexOfGroup(const QString& name) const
{
  for (uint32_t i = 0; i < mGroups.size(); ++i)
    if (mGroups.at(i)->name == name)
      return (int)i;

  return -1;
}

std::shared_ptr<Pipeline::PipelineGroup> Pipeline::getGroup()
{
  if (mGroupIndex >= mGroups.size())
    return nullptr;

  return mGroups.at(mGroupIndex);
}

int Pipeline::getIndexofFirstNonEmptyGroup() const
{
  for (uint32_t i = mGroupIndex; i < mGroups.size(); ++i)
  {
    const auto group = mGroups.at(i);
    if (!group->isEmpty())
      return (int)i;
  }

  return -1;
}

void Pipeline::clearGroups()
{
  for (auto it = mGroups.begin(); it != mGroups.end();)
  {
    // We must keep the default group
    if ((*it)->name == DEFAULT_GROUP)
    {
      (*it)->processes.clear();
      it++;
    }
    else
    {
      it = mGroups.erase(it);
    }
  }

  mGroupIndex = 0;
}

void Pipeline::startGroup(const QString& groupName)
{
  int index = getIndexOfGroup(groupName);
  if (index >= 0)
  {
    mGroupIndex = index;
    return;
  }

  auto group = std::make_shared<PipelineGroup>();
  group->name = groupName;
  group->processes = {};

  mGroupIndex = mGroups.size();
  mGroups.push_back(group);
}

void Pipeline::endGroup()
{
  // Reset to the default group index
  mGroupIndex = 0;
}

VoidResult Pipeline::add(QProcess* process, maki::OnFail onFail, std::function<void()> callback)
{
  QString exe = QStandardPaths::findExecutable(process->program());
  if (exe.isEmpty())
    return VoidResult::Failed("Executable not found in PATH: " + process->program().toStdString());

  auto group = getGroup();
  if (!group)
    return VoidResult::Failed("Invalid group");

  connect(process, &QProcess::finished, this, &Pipeline::onFinished);
  connect(process, &QProcess::readyReadStandardOutput, this, &Pipeline::onReadyReadStandardOutput);
  connect(process, &QProcess::readyReadStandardError, this, &Pipeline::onReadyReadStandardError);
  connect(process, &QProcess::errorOccurred, this, &Pipeline::onErrorOccurred);

  auto pp = std::make_shared<PipelineProcess>();
  pp->process = process;
  pp->onFail = onFail;
  pp->onFinish = callback;
  pp->state = State::Idle;

  group->processes.push_back(pp);

  LOG_DEBUG("Adding process to group: %s (%d)", qPrintable(group->name), mGroupIndex);

  return VoidResult();
}

VoidResult Pipeline::start()
{
  // Always reset on startup
  mGroupIndex = 0;
  mCurrentGroup.clear();
  return start(DEFAULT_GROUP, true);
}

VoidResult Pipeline::start(const QString& groupName, bool first)
{
  {
    std::unique_lock<std::mutex> lock(mStateMutex);
    if (mState == State::Aborting)
    {
      LOG_DEBUG("Trying to start during abort");
      return VoidResult();
    }
  }

  int index = getIndexOfGroup(groupName);
  if (index < 0)
    return VoidResult::Failed("No group with name: " + groupName.toStdString());

  auto group = mGroups.at(index);
  if (group->isEmpty())
  {
    // We try to run the first non-empty group
    index = getIndexofFirstNonEmptyGroup();
    if (index < 0)
      return VoidResult();

    group = mGroups.at(index);
    mGroupIndex = index;
  }

  int processIndex = group->getNextIndex();
  if (processIndex < 0)
    return VoidResult::Failed("Something went wrong, no process to start");

  mRunningProcess = group->processes.at(processIndex);
  if (mRunningProcess->state != State::Idle)
    return VoidResult::Failed("Something went wrong, cannot start process that is not idle");

  const QString name = mRunningProcess->process->program();
  const QStringList args = mRunningProcess->process->arguments();

  mRunningProcess->state = State::Running;

  if (first)
    emit startingPipeline(constructInfo());

  LOG_DEBUG("Starting group: %s, was running %s", qPrintable(group->name), qPrintable(mCurrentGroup));
  if (mCurrentGroup != group->name)
  {
    mCurrentGroup = group->name;
    emit startingGroup(constructInfo(), group->name);
  }

  emit startingProcess(constructInfo(), name, args);
  mRunningProcess->process->start();

  // This makes the ide work
  mRunningProcess->process->closeWriteChannel();

  if (!mRunningProcess->process->waitForStarted())
    return VoidResult::Failed("Command not found or not executable!");

  emit processStarted(constructInfo(), name, args);

  {
    std::unique_lock<std::mutex> lock(mStateMutex);
    mState = State::Running;
  }

  return VoidResult();
}

void Pipeline::startNextOrEnd(int exitCode, QProcess::ExitStatus status)
{
  if (!mRunningProcess)
  {
    LOG_DEBUG("What happened here? How can we have finished with no process");
    return;
  }

  mRunningProcess->state = exitCode == SUCCESS ? State::Done : State::Error;
  mRunningProcess->process = nullptr;

  bool aborting = false;
  {
    std::unique_lock<std::mutex> lock(mStateMutex);
    aborting = mState == State::Aborting;
  }

  LOG_DEBUG("Process ended with code: %d", exitCode);

  auto group = getGroup();
  if (exitCode != SUCCESS)
  {
    // On failure, we clear the queue manually
    // There is no "continue" after failure option yet
    emit finishedGroup(constructInfo(), group->name, exitCode, aborting ? "Aborted" : "Error");
    emit finishedLast(constructInfo(), exitCode, aborting ? "Aborted" : "Error");
    clearGroups();

    std::unique_lock<std::mutex> lock(mStateMutex);
    mState = State::Idle;

    return;
  }

  if (group->getNextIndex() < 0)
  {
    emit finishedGroup(constructInfo(), group->name, exitCode, "Success");

    // Check if there are more groups to run
    LOG_DEBUG("Done running group: %s. (%d of %d)", qPrintable(group->name), mGroupIndex + 1, mGroups.size());
    if (mGroupIndex + 1 < mGroups.size())
    {
      // Move on to the next group
      mGroupIndex += 1;
      auto nextGroup = getGroup();
      start(nextGroup->name);
    }
    else
    {
      // If everything finished, we can emit last
      emit finishedLast(constructInfo(), exitCode, "Success");

      std::unique_lock<std::mutex> lock(mStateMutex);
      mState = State::Idle;
      clearGroups();
    }
  }
  else
  {
    LOG_DEBUG("Running next process");

    // Keep running the same group
    start(group->name);
  }
}

bool Pipeline::handleInputData(QByteArray& data) const
{
  // Look for the DSR query: ESC [ 6 n
  const QByteArray dsrQuery = "\x1b[6n";
  if (data.contains(dsrQuery))
  {
    // Remove it from displayed output
    data.replace(dsrQuery, "");

    // Fake a reply: say cursor is at row 1, col 1
    QByteArray reply = "\x1b[1;1R";
    mRunningProcess->process->write(reply);
    return false;
  }

  // Remove all ansi codes
  // QString text = QString::fromLocal8Bit(data);
  // text.remove(ansiRe);
  return true;
}

void Pipeline::onReadyReadStandardOutput()
{
  if (!mRunningProcess || !mRunningProcess->process)
  {
    LOG_WARNING("Process ready to read stdout, but there are no processes running");
    return;
  }

  QByteArray data = mRunningProcess->process->readAllStandardOutput();

  if (handleInputData(data))
    emit readyReadStandardOutput(data);
}

void Pipeline::onReadyReadStandardError()
{
  if (!mRunningProcess || !mRunningProcess->process)
  {
    LOG_WARNING("Process ready to read srderr, but there are no processes running");
    return;
  }

  QByteArray data = mRunningProcess->process->readAllStandardError();
  // QString text = handleInputData(data);

  if (handleInputData(data))
    emit readyReadStandardError(data);
}

void Pipeline::onFinished(int exitCode, QProcess::ExitStatus status)
{
  if (!mRunningProcess || !mRunningProcess->process)
  {
    LOG_WARNING("Process finished, but there are no processes running");
    return;
  }

  LOG_DEBUG("Process finished: %s", qPrintable(mRunningProcess->process->program()));
  emit finished(constructInfo(), exitCode, status);

  if (mRunningProcess->onFinish)
    mRunningProcess->onFinish();

  startNextOrEnd(
      (exitCode == SUCCESS || mRunningProcess->onFail == maki::OnFail::CONTINUE) ? SUCCESS : exitCode,
      status);
}

void Pipeline::onErrorOccurred(QProcess::ProcessError error)
{
  if (!mRunningProcess || !mRunningProcess->process)
  {
    LOG_WARNING("Process errored, but there are no processes running");
    return;
  }

  {
    std::unique_lock<std::mutex> lock(mStateMutex);
    if (mState == State::Aborting)
    {
      LOG_DEBUG("Process error during abort");
      return;
    }
  }

  QString errorMessage = "";
  switch (error)
  {
    case QProcess::ProcessError::FailedToStart:
      errorMessage = "Process failed to start";
      break;
    case QProcess::ProcessError::Crashed:
      errorMessage = "Process crashed";
      break;
    case QProcess::ProcessError::Timedout:
      errorMessage = "Process timed out";
      break;
    case QProcess::ProcessError::ReadError:
      errorMessage = "Could not read location";
      break;
    case QProcess::ProcessError::WriteError:
      errorMessage = "Could not write location";
      break;
    case QProcess::ProcessError::UnknownError:
      errorMessage = "Unknown process error";
    default:
      break;
  }
  emit errorOccurred(constructInfo(), error, errorMessage);
  clearGroups();

  {
    std::unique_lock<std::mutex> lock(mStateMutex);
    mState = State::Idle;
  }
}

Pipeline::Info Pipeline::constructInfo() const
{
  Info info;
  for (const auto& g : mGroups)
  {
    GroupInfo groupInfo;
    groupInfo.name = g->name;
    for (const auto& p : g->processes)
    {
      ProcessInfo pInfo;
      pInfo.name = p->process ? QString("%1 %2").arg(p->process->program(), p->process->arguments().join(' ')) : "Done";
      pInfo.status = p->state;
      groupInfo.processes.push_back(pInfo);
    }
    info.groupInfo.push_back(groupInfo);
  }

  return info;
}
