#include "pipeline.h"

#include <QProcess>
#include <QRegularExpression>
#include <QStandardPaths>

#include "logging.h"

static const int SUCCESS = 0;
static const QRegularExpression ansiRe("\x1B\\[[0-9;?]*[ -/]*[@-~]", QRegularExpression::DontCaptureOption);

Pipeline::Pipeline(QWidget* parent)
    : QWidget(parent)
    , mName("")
    , mRunningProcess({nullptr, OnFail::STOP, ""})
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

VoidResult Pipeline::add(QProcess* process, OnFail onFail)
{
  QString exe = QStandardPaths::findExecutable(process->program());
  if (exe.isEmpty())
    return VoidResult::Failed("Executable not found in PATH");

  connect(process, &QProcess::finished, this, &Pipeline::onFinished);
  connect(process, &QProcess::readyReadStandardOutput, this, &Pipeline::onReadyReadStandardOutput);
  connect(process, &QProcess::readyReadStandardError, this, &Pipeline::onReadyReadStandardError);
  connect(process, &QProcess::errorOccurred, this, &Pipeline::onErrorOccurred);

  mProcesses.append({process, onFail});

  return VoidResult();
}

VoidResult Pipeline::add(QProcess* process, OnFail onFail, const QString& options)
{
  auto result = add(process, onFail);
  if (!result.IsSuccess())
    return result;

  mProcesses.last().options = options;

  return VoidResult();
}

VoidResult Pipeline::start()
{
  if (mProcesses.isEmpty())
    return VoidResult::Failed("Nothing to run");

  mRunningProcess = mProcesses.first();
  const QString name = mRunningProcess.process->program();
  const QStringList args = mRunningProcess.process->arguments();

  emit startingProcess(name, args);
  mRunningProcess.process->start();

  // This makes the ide work
  mRunningProcess.process->closeWriteChannel();

  if (!mRunningProcess.process->waitForStarted())
    LOG_WARNING("Command not found or not executable!");

  return VoidResult();
}

void Pipeline::startNextOrEnd(int exitCode, QProcess::ExitStatus status)
{
  if (mProcesses.isEmpty())
  {
    emit finishedLast();
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

  if (!mRunningProcess.options.isEmpty())
  {
    LOG_DEBUG("Opening client: %s", qPrintable(mRunningProcess.options));
    emit openClient(mRunningProcess.options);
  }

  mProcesses.removeFirst();

  // In case of success
  if (exitCode == SUCCESS)
  {
    startNextOrEnd(exitCode, status);
  }
  else
  {
    if (mRunningProcess.onFail == OnFail::CONTINUE)
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

  qDebug() << "QProcess error:" << error << "errorString:" << mRunningProcess.process->errorString();

  emit errorOccurred(error);
  mProcesses.clear();
}
