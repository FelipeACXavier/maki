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
    CONTINUE,
    OPEN_BROWSER
  };

  Pipeline(QWidget* parent = nullptr);

  VoidResult add(QProcess* process, OnFail onFail);
  VoidResult add(QProcess* process, OnFail onFail, const QString& options);
  VoidResult start();

  QString name() const;
  void setName(const QString& name);

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
    OnFail onFail = OnFail::STOP;
    QString options = "";
  };

  QString mName;
  PipelineProcess mRunningProcess;
  QVector<PipelineProcess> mProcesses;

  void startNextOrEnd(int exitCode, QProcess::ExitStatus status);
  bool handleInputData(QByteArray& data) const;
};