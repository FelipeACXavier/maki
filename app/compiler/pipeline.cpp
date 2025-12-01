#include "pipeline.h"

#include <QProcess>
#include <QStandardPaths>

#include "logging.h"

static const int SUCCESS = 0;
static const int FAILURE = -1;

Pipeline::Pipeline(QWidget* parent)
    : QWidget(parent)
    , mName("")
    , mRunningProcess({nullptr, OnFail::STOP})
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

VoidResult Pipeline::start()
{
  if (mProcesses.isEmpty())
    return VoidResult::Failed("Nothing to run");

  mRunningProcess = mProcesses.first();
  const QString name = mRunningProcess.process->program();
  const QStringList args = mRunningProcess.process->arguments();

  emit startingProcess(name, args);
  mRunningProcess.process->start();

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

void Pipeline::onReadyReadStandardOutput()
{
  if (!mRunningProcess.process)
  {
    LOG_WARNING("Process ready to read stdout, but there are no processes running");
    return;
  }

  QByteArray data = mRunningProcess.process->readAllStandardOutput();

  // Look for the DSR query: ESC [ 6 n
  const QByteArray dsrQuery = "\x1b[6n";
  if (data.contains(dsrQuery))
  {
    // Remove it from displayed output
    data.replace(dsrQuery, "");

    // Fake a reply: say cursor is at row 1, col 1
    QByteArray reply = "\x1b[1;1R";
    mRunningProcess.process->write(reply);
    return;
  }

  const QString text = QString::fromLocal8Bit(data);
  emit readyReadStandardOutput(text);
}

void Pipeline::onReadyReadStandardError()
{
  if (!mRunningProcess.process)
  {
    LOG_WARNING("Process ready to read srderr, but there are no processes running");
    return;
  }

  const QString text = QString::fromLocal8Bit(mRunningProcess.process->readAllStandardError());
  emit readyReadStandardError(text);
}

void Pipeline::onFinished(int exitCode, QProcess::ExitStatus status)
{
  if (!mRunningProcess.process)
  {
    LOG_WARNING("Process finished, but there are no processes running");
    return;
  }

  emit finished(exitCode, status);

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
