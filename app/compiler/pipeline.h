#pragma once

#include <QProcess>
#include <QVector>
#include <QWidget>

#include "result.h"

class Pipeline : public QWidget
{
  Q_OBJECT
public:
  enum class OnFail
  {
    STOP = 0,
    CONTINUE
  };

  Pipeline(QWidget* parent = nullptr);

  VoidResult add(QProcess* process, OnFail onFail);
  VoidResult start();

  QString name() const;
  void setName(const QString& name);

signals:
  // Emitted when the process finishes (so the owner can react, e.g. rename/close tab)
  void startingProcess(const QString& process, const QStringList& arguments);
  void finished(int exitCode, QProcess::ExitStatus status);
  void finishedLast();
  void readyReadStandardOutput(const QString& message);
  void readyReadStandardError(const QString& message);
  void errorOccurred(QProcess::ProcessError error);

private slots:
  void onReadyReadStandardOutput();
  void onReadyReadStandardError();
  void onFinished(int exitCode, QProcess::ExitStatus status);
  void onErrorOccurred(QProcess::ProcessError error);

private:
  struct PipelineProcess
  {
    QProcess* process;
    OnFail onFail;
  };

  QString mName;
  PipelineProcess mRunningProcess;
  QVector<PipelineProcess> mProcesses;

  void startNextOrEnd(int exitCode, QProcess::ExitStatus status);
};