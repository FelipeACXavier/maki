#pragma once

#include <QProcess>
#include <QWidget>

class QTextBrowser;
class Pipeline;

class ProcessTab : public QWidget
{
  Q_OBJECT
public:
  explicit ProcessTab(Pipeline* pipeline, QWidget* parent = nullptr);

signals:
  // Emitted when the process finishes (so the owner can react, e.g. rename/close tab)
  void processFinished(int exitCode, QProcess::ExitStatus status);

private slots:
  void onReadyReadStandardOutput(const QString& message);
  void onReadyReadStandardError(const QString& message);
  void onStartingProcess(const QString& process, const QStringList& arguments);
  void onFinished(int exitCode, QProcess::ExitStatus status);
  void onFinishedLast();
  void onErrorOccurred(QProcess::ProcessError error);

private:
  QTextBrowser* mOutput;
  Pipeline* mPipeline;

  void appendText(const QString& text);
};
