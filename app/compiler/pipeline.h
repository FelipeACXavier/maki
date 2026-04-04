#pragma once

#include <QProcess>
#include <QVector>
#include <mutex>

#include "ipipeline.h"
#include "result.h"

/**
 * @brief Executes and manages a sequence of external processes.
 *
 * Pipeline provides a simple orchestration mechanism for running a set of
 * QProcess instances sequentially. Each process can define a failure policy
 * and an optional callback that is invoked when the process finishes.
 *
 * The class emits signals to allow observers (such as UI components) to track
 * process execution, receive output streams, and react to failures.
 */
class Pipeline : public QObject, public maki::IPipeline
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a Pipeline instance.
   *
   * @param parent Optional Qt parent object.
   */
  Pipeline(QObject* parent = nullptr);

  /**
   * @brief Destructs a Pipeline instance.
   */
  ~Pipeline();

  /**
   * @brief Adds a process to the pipeline.
   *
   * The process will be executed in the order it was added. The behavior in
   * case of failure is determined by the provided OnFail policy.
   *
   * @param process Pointer to the QProcess to execute.
   * @param onFail Policy describing what should happen if the process fails.
   * @param callback Optional callback executed when the process finishes.
   * @return VoidResult indicating whether the process was successfully added.
   */
  VoidResult add(QProcess* process, maki::OnFail onFail, std::function<void()> callback = nullptr) override;

  /**
   * @brief Starts executing the pipeline.
   *
   * Processes will be executed sequentially until the pipeline finishes,
   * aborts, or encounters a failure depending on the configured policy.
   *
   * @return VoidResult indicating whether the pipeline started successfully.
   */
  VoidResult start();

  /**
   * @brief Returns the pipeline name.
   *
   * @return Name of the pipeline.
   */
  QString name() const;

  /**
   * @brief Sets the pipeline name.
   *
   * @param name New pipeline name.
   */
  void setName(const QString& name);

  /**
   * @brief Returns the number of processes registered in the pipeline.
   *
   * @return Total number of processes.
   */
  int size() const;

  /**
   * @brief Indicates whether the pipeline is currently running.
   *
   * @return True if a process is executing or the pipeline is active.
   */
  bool isRunning() const;

  /**
   * @brief Aborts the currently running pipeline.
   *
   * If a process is currently executing, the pipeline transitions to the
   * aborting state and stops further execution.
   *
   * @return VoidResult indicating whether the abort request succeeded.
   */
  VoidResult abort();

signals:
  /**
   * @brief Emitted before a process starts.
   *
   * Allows observers to track which external command is being executed.
   *
   * @param process Executable name.
   * @param arguments Command line arguments.
   */
  void startingProcess(const QString& process, const QStringList& arguments);

  /**
   * @brief Emitted when the currently running process finishes.
   *
   * @param exitCode Exit code returned by the process.
   * @param status Exit status reported by QProcess.
   */
  void finished(int exitCode, QProcess::ExitStatus status);

  /**
   * @brief Emitted when the last process in the pipeline finishes.
   */
  void finishedLast(int exitCode, const QString& message);

  /**
   * @brief Emitted when new data is available on the standard output stream.
   *
   * @param message Data read from the process stdout.
   */
  void readyReadStandardOutput(const QByteArray& message);

  /**
   * @brief Emitted when new data is available on the standard error stream.
   *
   * @param message Data read from the process stderr.
   */
  void readyReadStandardError(const QByteArray& message);

  /**
   * @brief Emitted when a process error occurs.
   *
   * @param error Error reported by QProcess.
   */
  void errorOccurred(QProcess::ProcessError error, const QString& message);

private slots:
  /**
   * @brief Slot triggered when the process produces standard output.
   */
  void onReadyReadStandardOutput();

  /**
   * @brief Slot triggered when the process produces standard error output.
   */
  void onReadyReadStandardError();

  /**
   * @brief Slot triggered when a process finishes execution.
   *
   * @param exitCode Exit code returned by the process.
   * @param status Exit status reported by QProcess.
   */
  void onFinished(int exitCode, QProcess::ExitStatus status);

  /**
   * @brief Slot triggered when a process error occurs.
   *
   * @param error Error reported by QProcess.
   */
  void onErrorOccurred(QProcess::ProcessError error);

private:
  /**
   * @brief Internal representation of a process registered in the pipeline.
   */
  struct PipelineProcess
  {
    /// Process to be executed.
    QProcess* process = nullptr;

    /// Policy describing behavior when the process fails.
    maki::OnFail onFail = maki::OnFail::STOP;

    /// Optional callback executed when the process finishes.
    std::function<void()> onFinish;
  };

  /**
   * @brief Internal state of the pipeline execution.
   */
  enum class State
  {
    Idle,     ///< Pipeline is not running.
    Running,  ///< Pipeline is currently executing processes.
    Aborting  ///< Pipeline is aborting execution.
  } mState;

  /// Mutex protecting access to the pipeline state.
  mutable std::mutex mStateMutex;

  /// Name of the pipeline.
  QString mName;

  /// Currently running process.
  PipelineProcess mRunningProcess;

  /// Queue of processes waiting to be executed.
  QVector<PipelineProcess> mProcesses;

  /**
   * @brief Starts the next process or ends the pipeline.
   *
   * This method decides whether to continue with the next process or finish
   * the pipeline depending on the exit status and failure policies.
   *
   * @param exitCode Exit code of the completed process.
   * @param status Exit status of the completed process.
   */
  void startNextOrEnd(int exitCode, QProcess::ExitStatus status);

  /**
   * @brief Processes incoming output data from the running process.
   *
   * This may perform filtering, buffering, or preprocessing of the data
   * before it is emitted through signals.
   *
   * @param data Data read from the process stream.
   * @return True if the data was handled successfully.
   */
  bool handleInputData(QByteArray& data) const;
};