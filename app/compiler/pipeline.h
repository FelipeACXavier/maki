#pragma once

#include <QVector>
#include <QWidget>
#include <mutex>

#include "ipipeline.h"
#include "result.h"

#pragma once

#ifndef __EMSCRIPTEN__
#include <QProcess>
#else
#include <QByteArray>
#include <QObject>
#include <QString>
#include <QStringList>
class QProcess : public QObject
{
  Q_OBJECT

public:
  enum ProcessState
  {
    NotRunning,
    Starting,
    Running
  };

  enum ExitStatus
  {
    NormalExit,
    CrashExit
  };

  enum ProcessError
  {
    FailedToStart,
    Crashed,
    Timedout,
    WriteError,
    ReadError,
    UnknownError
  };

  explicit QProcess(QObject* parent = nullptr)
      : QObject(parent)
  {
  }

  QString program() const
  {
    return mProgram;
  }
  void setProgram(const QString& program)
  {
    mProgram = program;
  }

  QStringList arguments() const
  {
    return mArguments;
  }
  void setArguments(const QStringList& arguments)
  {
    mArguments = arguments;
  }

  ProcessState state() const
  {
    return mState;
  }

  void start()
  {
    mState = Running;
    emit started();

    mState = NotRunning;
    emit finished(1, NormalExit);
  }

  bool waitForStarted(int msecs = 30000)
  {
    Q_UNUSED(msecs)
    return false;
  }

  bool waitForFinished(int msecs = 30000)
  {
    Q_UNUSED(msecs)
    return true;
  }

  void closeWriteChannel()
  {
  }
  void terminate()
  {
    mState = NotRunning;
  }
  void kill()
  {
    mState = NotRunning;
  }

  qint64 write(const QByteArray& data)
  {
    Q_UNUSED(data)
    return -1;
  }

  QByteArray readAllStandardOutput()
  {
    return {};
  }
  QByteArray readAllStandardError()
  {
    return {};
  }

signals:
  void started();
  void finished(int exitCode, QProcess::ExitStatus exitStatus);
  void readyReadStandardOutput();
  void readyReadStandardError();
  void errorOccurred(QProcess::ProcessError error);

private:
  QString mProgram;
  QStringList mArguments;
  ProcessState mState = NotRunning;
};
#endif

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
   * @brief Internal state of the pipeline execution.
   */
  enum class State
  {
    Unknown,   ///< Not initialized
    Idle,      ///< Not running.
    Running,   ///< Currently executing processes.
    Aborting,  ///< Aborting execution.
    Done,      ///< Done successfully
    Error      ///< Done with error
  };

  struct ProcessInfo
  {
    QString name = "";              /// Name of the process.
    State status = State::Unknown;  /// Current status of the process.
  };

  struct GroupInfo
  {
    QString name = "";                    /// Name of the group.
    QVector<ProcessInfo> processes = {};  /// List of processes in this group.
  };

  struct Info
  {
    QString current = "";               /// Name of the current process.
    QString currentPath = "";           /// Name of the current process path.
    QVector<GroupInfo> groupInfo = {};  /// List of all groups and their processes.
  };

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
#ifndef __EMSCRIPTEN__
  VoidResult add(QProcess* process, maki::OnFail onFail, std::function<void(int& exitCode, QProcess::ExitStatus& status)> callback = nullptr) override;
#else
  VoidResult add(QProcess* process, maki::OnFail onFail, std::function<void(int& exitCode, QProcess::ExitStatus& status)> callback = nullptr);
#endif

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

  void startGroup(const QString& groupName) override;
  void endGroup() override;

  QWidget* progressWidget(bool subMenu = false) const;

signals:
  /**
   * @brief Emitted before a process starts.
   *
   * Allows observers to track which external command is being executed.
   *
   * @param info Information about the pipeline and group.
   * @param process Executable name.
   * @param arguments Command line arguments.
   */
  void startingProcess(const Info& info, const QString& process, const QStringList& arguments);

  /**
   * @brief Emitted after a process starts.
   *
   * Allows observers to track which external command is being executed.
   *
   * @param info Information about the pipeline and group.
   * @param process Executable name.
   * @param arguments Command line arguments.
   */
  void processStarted(const Info& info, const QString& process, const QStringList& arguments);

  /**
   * @brief Emitted when the currently running process finishes.
   *
   * @param info Information about the pipeline and group.
   * @param exitCode Exit code returned by the process.
   * @param status Exit status reported by QProcess.
   */
  void finished(const Info& info, int exitCode, QProcess::ExitStatus status);

  /**
   * @brief Emitted when the last process in the pipeline finishes.
   *
   * @param info Information about the pipeline and group.
   * @param exitCode Exit code returned by the process.
   * @param message Additional message.
   */
  void finishedLast(const Info& info, int exitCode, const QString& message);

  /**
   * @brief Emitted when the complete pipeline is started
   *
   * @param info Information about the pipeline and group.
   */
  void startingPipeline(const Info& info);

  /**
   * @brief Emitted before a group starts.
   *
   * Allows observers to track which group is being executed
   *
   * @param info Information about the pipeline and group.
   * @param groupName Name of the group which is starting
   */
  void startingGroup(const Info& info, const QString& groupName);

  /**
   * @brief Emitted when the last process in a group finishes.
   *
   * @param info Information about the pipeline and group.
   * @param groupName Name of the group which finished
   * @param exitCode Exit code returned by the process.
   * @param message Additional message.
   */
  void finishedGroup(const Info& info, const QString& groupName, int exitCode, const QString& message);

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
   * @param info Information about the pipeline and group.
   * @param error Error reported by QProcess.
   * @param message Additional message.
   */
  void errorOccurred(const Info& info, QProcess::ProcessError error, const QString& message);

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
    std::function<void(int& exitCode, QProcess::ExitStatus& status)> onFinish;

    /// Current state of a single process
    State state = State::Unknown;
  };

  /**
   * @brief Internal representation of a group of processes
   */
  struct PipelineGroup
  {
    /// Name of the group.
    QString name = "";

    /// Queue of processes waiting to be executed.
    std::vector<std::shared_ptr<PipelineProcess>> processes;

    int size() const
    {
      return processes.size();
    }

    bool isEmpty() const
    {
      return processes.size() == 0;
    }

    int getNextIndex()
    {
      for (int i = 0; i < (int)processes.size(); ++i)
        if (processes.at(i)->state == State::Idle)
          return i;

      return -1;
    }
  };

  /// Mutex protecting access to the pipeline state.
  mutable std::mutex mStateMutex;
  State mState;

  /// Name of the pipeline.
  QString mName;

  /// Currently running process.
  std::shared_ptr<PipelineProcess> mRunningProcess;

  uint32_t mGroupIndex;
  QString mCurrentGroup;
  std::vector<std::shared_ptr<PipelineGroup>> mGroups;

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

  void clearGroups();
  int getIndexOfGroup(const QString& name) const;
  int getIndexofFirstNonEmptyGroup() const;
  std::shared_ptr<Pipeline::PipelineGroup> getGroup();

  VoidResult start(const QString& groupName, bool first = false);

  Info constructInfo() const;

  QString getRunningTask(GroupInfo info) const;
  bool tasksInError(GroupInfo info) const;
  int getCompleteTasks(GroupInfo info) const;
};
