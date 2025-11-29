#pragma once

#include <QProcess>
#include <QWidget>

class QTextBrowser;

class ProcessTab : public QWidget
{
  Q_OBJECT

public:
  explicit ProcessTab(QWidget* parent = nullptr);

  // Start a process and stream its output into the tab
  void startProcess(const QString& program, const QStringList& arguments = {});

signals:
  // Emitted when the process finishes (so the owner can react, e.g. rename/close tab)
  void processFinished(int exitCode, QProcess::ExitStatus status);

private slots:
  void onReadyReadStandardOutput();
  void onReadyReadStandardError();
  void onFinished(int exitCode, QProcess::ExitStatus status);
  void onErrorOccurred(QProcess::ProcessError error);

private:
  QTextBrowser* m_output;
  QProcess* m_process;

  void appendText(const QString& text);
};
