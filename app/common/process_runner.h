#pragma once

#include <QObject>
#include <QProcessEnvironment>
#include <QString>

class ProcessRunner : public QObject
{
  Q_OBJECT
public:
  ProcessRunner(QObject* parent = nullptr);

  void addEnvVariable(const QString& key, const QString& value);
  void removeEnvVariable(const QString& key, const QString& value);

  void setCwd(const QString& path);
  void setTimeout(int ms);

  using FinshedCallback = std::function<void(int, const QString&)>;
  using LogCallback = std::function<void(const QString&)>;

  int runSync(const QString& cmd, QString& result);
  void runAsync(const QString& cmd, FinshedCallback finishedCb, LogCallback stdoutCb = nullptr, LogCallback stderrCb = nullptr);

  static int ExecuteSync(const QString& cmd, QString& result);
  static void ExecuteAsync(const QString& cmd, QObject* parent,
                           FinshedCallback finishedCb, LogCallback stdoutCb = nullptr, LogCallback stderrCb = nullptr);

private:
  QProcessEnvironment mEnv;
  QString mCwd;
  int mTimeout;
};
