#include "process_runner.h"

#include <QByteArray>
#include <QProcess>

#include "logging.h"

ProcessRunner::ProcessRunner(QObject* parent)
    : mCwd("")
    , mTimeout(-1)
{
}

void ProcessRunner::addEnvVariable(const QString& key, const QString& value)
{
  mEnv.insert(key, value);
}

void ProcessRunner::removeEnvVariable(const QString& key, const QString& value)
{
  if (mEnv.contains(key))
    mEnv.remove(key);
}

void ProcessRunner::setCwd(const QString& path)
{
  mCwd = path;
}

void ProcessRunner::setTimeout(int ms)
{
  mTimeout = ms;
}

int ProcessRunner::runSync(const QString& cmd, QString& result)
{
  QProcess proc;

  // Merge stdout + stderr so result contains everything
  proc.setProcessChannelMode(QProcess::MergedChannels);

  // Start using a shell-style command line
  proc.startCommand(cmd);

  if (!proc.waitForStarted())
  {
    LOG_ERROR("Failed to start process: %s", qPrintable(cmd));
    return -1;
  }

  // -1 = wait indefinitely; you can add a timeout if you want
  if (!proc.waitForFinished(-1))
  {
    LOG_WARNING("Process '%s' did not finish in time", qPrintable(cmd));
    proc.kill();
    proc.waitForFinished();
    return -1;
  }

  result = QString(proc.readAll());

  int exitCode = proc.exitCode();
  if (proc.exitStatus() == QProcess::CrashExit)
  {
    LOG_WARNING("Process '%s' crashed (code %d)", qPrintable(cmd), exitCode);
    return -1;
  }

  if (exitCode != 0)
  {
    LOG_DEBUG("Process '%s' exited with code %d", qPrintable(cmd), exitCode);
  }

  return exitCode;
}

void ProcessRunner::runAsync(const QString& cmd, FinshedCallback finishedCb, LogCallback stdoutCb, LogCallback stderrCb)
{
  // QProcess must be heap-allocated if we want "self-delete" after finish
  QProcess* proc = new QProcess(this);

  if (stdoutCb || stderrCb)
    proc->setProcessChannelMode(QProcess::SeparateChannels);
  else
    proc->setProcessChannelMode(QProcess::MergedChannels);

  QObject::connect(proc, &QProcess::finished, this,
                   [proc, finishedCb](int exitCode, QProcess::ExitStatus status) {
                     QString output = QString(proc->readAll());

                     // Use -1 or a special value to indicate crash
                     if (status == QProcess::CrashExit)
                       exitCode = -1;

                     if (finishedCb)
                       finishedCb(exitCode, output);

                     proc->deleteLater();
                   });

  if (stdoutCb)
  {
    QObject::connect(proc, &QProcess::readyReadStandardOutput, this, [proc, stdoutCb]() {
      QString output = QString(proc->readAllStandardOutput());
      if (stdoutCb)
        stdoutCb(output);
    });
  }

  if (stderrCb)
  {
    QObject::connect(proc, &QProcess::readyReadStandardError, this, [proc, stderrCb]() {
      QString output = QString(proc->readAllStandardError());
      if (stderrCb)
        stderrCb(output);
    });
  }

  proc->startCommand(cmd);
}

int ProcessRunner::ExecuteSync(const QString& cmd, QString& result)
{
  ProcessRunner runner;
  return runner.runSync(cmd, result);
}

void ProcessRunner::ExecuteAsync(const QString& cmd, QObject* parent, FinshedCallback finishedCb, LogCallback stdoutCb, LogCallback stderrCb)
{
  ProcessRunner runner(parent);
  runner.runAsync(cmd, finishedCb, stdoutCb, stderrCb);
}