#pragma once

#include <QProcess>
#include <QTextCharFormat>
#include <QWidget>

class QTextBrowser;
class Pipeline;

class ProcessTab : public QWidget
{
  Q_OBJECT
public:
  explicit ProcessTab(QWidget* parent = nullptr);

  void setPipeline(Pipeline* pipeline);

signals:
  // Emitted when the process finishes (so the owner can react, e.g. rename/close tab)
  void processStarted();
  void processFinished(int exitCode, QProcess::ExitStatus status);

private slots:
  void onStartingProcess(const QString& process, const QStringList& arguments);
  void onFinished(int exitCode, QProcess::ExitStatus status);
  void onFinishedLast(int exitCode, const QString& message);
  void onReadyReadStandardOutput(const QByteArray& message);
  void onReadyReadStandardError(const QByteArray& message);
  void onErrorOccurred(QProcess::ProcessError error, const QString& process);

private:
  QTextBrowser* mOutput;
  Pipeline* mPipeline;
  QTextCharFormat mCurrentFormat;

  int mOverwriteNextLine = 0;  // after ESC[1F]

  void appendText(const QString& text);

  void handleProcessData(const QByteArray& raw);
  void applySgr(const QList<int>& codes);
  void deleteLastLine(QTextCursor& cursor);
};
