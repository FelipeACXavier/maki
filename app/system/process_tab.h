#pragma once

#include <QProcess>
#include <QTextCharFormat>
#include <QWidget>

#include "compiler/pipeline.h"

class QTextBrowser;

/**
 * @brief A tab widget for displaying process output.
 */
class ProcessTab : public QWidget
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a new ProcessTab with the given parent.
   *
   * @param parent The parent widget.
   */
  explicit ProcessTab(QWidget* parent = nullptr);

  /**
   * @brief Sets the pipeline for this tab.
   *
   * @param pipeline The pipeline to set.
   */
  void setPipeline(Pipeline* pipeline);

signals:
  /**
   * @brief Emitted when a process starts.
   */
  void processStarted();

  /**
   * @brief Emitted when a process finishes.
   *
   * @param exitCode The exit code of the process.
   * @param status The exit status of the process.
   */
  void processFinished(int exitCode, QProcess::ExitStatus status);

private slots:
  /**
   * @brief Handles the start of a process.
   *
   * @param info Information about the pipeline.
   * @param process The name of the process.
   * @param arguments The arguments for the process.
   */
  void onStartingProcess(const Pipeline::Info& info, const QString& process, const QStringList& arguments);

  /**
   * @brief Handles the completion of a process.
   *
   * @param info Information about the pipeline.
   * @param exitCode The exit code of the process.
   * @param status The exit status of the process.
   */
  void onFinished(const Pipeline::Info& info, int exitCode, QProcess::ExitStatus status);

  /**
   * @brief Handles the completion of the last process.
   *
   * @param info Information about the pipeline.
   * @param exitCode The exit code of the process.
   * @param message A message related to the process.
   */
  void onFinishedLast(const Pipeline::Info& info, int exitCode, const QString& message);

  /**
   * @brief Handles ready read standard output.
   *
   * @param message The data read from standard output.
   */
  void onReadyReadStandardOutput(const QByteArray& message);

  /**
   * @brief Handles ready read standard error.
   *
   * @param message The data read from standard error.
   */
  void onReadyReadStandardError(const QByteArray& message);

  /**
   * @brief Handles an error occurring during process execution.
   *
   * @param info Information about the pipeline.
   * @param error The type of error that occurred.
   * @param process The name of the process.
   */
  void onErrorOccurred(const Pipeline::Info& info, QProcess::ProcessError error, const QString& process);

private:
  QTextBrowser* mOutput;  /// Pointer to the text browser for displaying output.

  Pipeline* mPipeline;  /// Pointer to the pipeline being processed.

  QTextCharFormat mCurrentFormat;  /// Current text format for output.

  int mOverwriteNextLine = 0;  // after ESC[1F]

  /**
   * @brief Appends text to the output.
   *
   * @param text The text to append.
   */
  void appendText(const QString& text);

  /**
   * @brief Handles process data.
   *
   * @param raw The raw data received from the process.
   */
  void handleProcessData(const QByteArray& raw);

  /**
   * @brief Applies SGR (Select Graphic Rendition) codes to the current format.
   *
   * @param codes A list of SGR codes.
   */
  void applySgr(const QList<int>& codes);

  /**
   * @brief Deletes the last line from the text cursor.
   *
   * @param cursor The text cursor.
   */
  void deleteLastLine(QTextCursor& cursor);
};
