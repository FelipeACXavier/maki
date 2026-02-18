#pragma once

#include <QProcess>
#include <QVector>
#include <mutex>

#include "ipipeline.h"
#include "result.h"

class Pipeline : public QObject, public maki::IPipeline
{
  Q_OBJECT
public:
  Pipeline(QObject* parent = nullptr);

  VoidResult add(QProcess* process, maki::OnFail onFail, const QString& options = {}) override;
  VoidResult start();

  QString name() const;
  void setName(const QString& name);

  int size() const;
  bool isRunning() const;
  VoidResult abort();

signals:
  // Emitted when the process finishes (so the owner can react, e.g. rename/close tab)
  void startingProcess(const QString& process, const QStringList& arguments);
  void finished(int exitCode, QProcess::ExitStatus status);
  void finishedLast();
  void readyReadStandardOutput(const QByteArray& message);
  void readyReadStandardError(const QByteArray& message);
  void errorOccurred(QProcess::ProcessError error);

  void openClient(const QString& url);

private slots:
  void onReadyReadStandardOutput();
  void onReadyReadStandardError();
  void onFinished(int exitCode, QProcess::ExitStatus status);
  void onErrorOccurred(QProcess::ProcessError error);

private:
  struct PipelineProcess
  {
    QProcess* process = nullptr;
    maki::OnFail onFail = maki::OnFail::STOP;
    QString options = "";
  };

  enum class State
  {
    Idle,
    Running,
    Aborting
  } mState;

  mutable std::mutex mStateMutex;

  QString mName;
  PipelineProcess mRunningProcess;
  QVector<PipelineProcess> mProcesses;

  void startNextOrEnd(int exitCode, QProcess::ExitStatus status);
  bool handleInputData(QByteArray& data) const;
};