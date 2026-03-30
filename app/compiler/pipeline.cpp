#include "pipeline.h"

#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>

#include "logging.h"

static const int SUCCESS = 0;
static const QRegularExpression ansiRe("\x1B\\[[0-9;?]*[ -/]*[@-~]", QRegularExpression::DontCaptureOption);

Pipeline::Pipeline(QObject* parent)
    : QObject(parent)
    , mState(State::Idle)
    , mName("")
    , mRunningProcess({nullptr, maki::OnFail::STOP, nullptr})
{
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
  return mProcesses.size();
}

bool Pipeline::isRunning() const
{
  std::unique_lock<std::mutex> lock(mStateMutex);
  return mState != State::Idle;
}

VoidResult Pipeline::abort()
{
  // Nothing to abort
  if (!mRunningProcess.process)
    return VoidResult();

  {
    std::unique_lock<std::mutex> lock(mStateMutex);
    mState = State::Aborting;
  }

  // Abort the running process
  if (mRunningProcess.process->state() == QProcess::Running)
  {
    mRunningProcess.process->terminate();

    if (!mRunningProcess.process->waitForFinished(2500))
    {
      mRunningProcess.process->kill();
      mRunningProcess.process->waitForFinished();
    }
  }

  mProcesses.clear();

  {
    std::unique_lock<std::mutex> lock(mStateMutex);
    mState = State::Idle;
  }

  return VoidResult();
}

VoidResult Pipeline::add(QProcess* process, maki::OnFail onFail, std::function<void()> callback)
{
  QString exe = QStandardPaths::findExecutable(process->program());
  if (exe.isEmpty())
    return VoidResult::Failed("Executable not found in PATH: " + process->program().toStdString());

  connect(process, &QProcess::finished, this, &Pipeline::onFinished);
  connect(process, &QProcess::readyReadStandardOutput, this, &Pipeline::onReadyReadStandardOutput);
  connect(process, &QProcess::readyReadStandardError, this, &Pipeline::onReadyReadStandardError);
  connect(process, &QProcess::errorOccurred, this, &Pipeline::onErrorOccurred);

  PipelineProcess pp;
  pp.process = process;
  pp.onFail = onFail;
  pp.onFinish = callback;

  mProcesses.append(pp);

  return VoidResult();
}

VoidResult Pipeline::start()
{
  if (mProcesses.isEmpty())
    return VoidResult();

  {
    std::unique_lock<std::mutex> lock(mStateMutex);
    if (mState == State::Aborting)
    {
      LOG_DEBUG("Trying to start during abort");
      return VoidResult();
    }
  }

  mRunningProcess = mProcesses.first();
  const QString name = mRunningProcess.process->program();
  const QStringList args = mRunningProcess.process->arguments();

  emit startingProcess(name, args);
  mRunningProcess.process->start();

  // This makes the ide work
  mRunningProcess.process->closeWriteChannel();

  if (!mRunningProcess.process->waitForStarted())
    return VoidResult::Failed("Command not found or not executable! ");

  {
    std::unique_lock<std::mutex> lock(mStateMutex);
    mState = State::Running;
  }

  return VoidResult();
}

void Pipeline::startNextOrEnd(int exitCode, QProcess::ExitStatus status)
{
  if (mProcesses.isEmpty())
  {
    mRunningProcess.process = nullptr;
    emit finishedLast();

    {
      std::unique_lock<std::mutex> lock(mStateMutex);
      mState = State::Idle;
    }
  }
  else
  {
    start();
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
    mRunningProcess.process->write(reply);
    return false;
  }

  // Remove all ansi codes
  // QString text = QString::fromLocal8Bit(data);
  // text.remove(ansiRe);
  return true;
}

void Pipeline::onReadyReadStandardOutput()
{
  if (!mRunningProcess.process)
  {
    LOG_WARNING("Process ready to read stdout, but there are no processes running");
    return;
  }

  QByteArray data = mRunningProcess.process->readAllStandardOutput();

  if (handleInputData(data))
    emit readyReadStandardOutput(data);
}

void Pipeline::onReadyReadStandardError()
{
  if (!mRunningProcess.process)
  {
    LOG_WARNING("Process ready to read srderr, but there are no processes running");
    return;
  }

  QByteArray data = mRunningProcess.process->readAllStandardError();
  // QString text = handleInputData(data);

  if (handleInputData(data))
    emit readyReadStandardError(data);
}

void Pipeline::onFinished(int exitCode, QProcess::ExitStatus status)
{
  if (!mRunningProcess.process)
  {
    LOG_WARNING("Process finished, but there are no processes running");
    return;
  }

  LOG_DEBUG("Process finished: %s", qPrintable(mRunningProcess.process->program()));
  emit finished(exitCode, status);

  if (mRunningProcess.onFinish)
    mRunningProcess.onFinish();

  mProcesses.removeFirst();

  // In case of success
  if (exitCode == SUCCESS)
  {
    startNextOrEnd(exitCode, status);
  }
  else
  {
    if (mRunningProcess.onFail == maki::OnFail::CONTINUE)
    {
      startNextOrEnd(SUCCESS, status);
    }
    else
    {
      emit finishedLast();
      mProcesses.clear();
    }
  }
}

void Pipeline::onErrorOccurred(QProcess::ProcessError error)
{
  if (!mRunningProcess.process)
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

  emit errorOccurred(error);
  mProcesses.clear();

  {
    std::unique_lock<std::mutex> lock(mStateMutex);
    mState = State::Idle;
  }
}
