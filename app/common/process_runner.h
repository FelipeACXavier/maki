#pragma once

#include <QObject>
#include <QProcessEnvironment>
#include <QString>

/**
 * @brief Class for running processes with customizable environment and options.
 */
class ProcessRunner : public QObject
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a new ProcessRunner object.
   *
   * @param parent Pointer to the parent QObject, if any.
   */
  ProcessRunner(QObject* parent = nullptr);

  /**
   * @brief Adds an environment variable to the process.
   *
   * @param key The name of the environment variable.
   * @param value The value of the environment variable.
   */
  void addEnvVariable(const QString& key, const QString& value);

  /**
   * @brief Removes an environment variable from the process.
   *
   * @param key The name of the environment variable to remove.
   * @param value The value of the environment variable to remove.
   */
  void removeEnvVariable(const QString& key, const QString& value);

  /**
   * @brief Sets the current working directory for the process.
   *
   * @param path The path to set as the current working directory.
   */
  void setCwd(const QString& path);

  /**
   * @brief Sets the timeout for the process in milliseconds.
   *
   * @param ms The timeout value in milliseconds.
   */
  void setTimeout(int ms);

  using FinshedCallback = std::function<void(int, const QString&)>;
  using LogCallback = std::function<void(const QString&)>;

  /**
   * @brief Runs a command synchronously and returns the result.
   *
   * @param cmd The command to run.
   * @param result Reference to a string where the output will be stored.
   * @return int The exit code of the process.
   */
  int runSync(const QString& cmd, QString& result);

  /**
   * @brief Runs a command asynchronously with callbacks for finished and logging.
   *
   * @param cmd The command to run.
   * @param finishedCb Callback function called when the process finishes.
   * @param stdoutCb Optional callback function for standard output logging.
   * @param stderrCb Optional callback function for standard error logging.
   */
  void runAsync(const QString& cmd, FinshedCallback finishedCb, LogCallback stdoutCb = nullptr, LogCallback stderrCb = nullptr);

  /**
   * @brief Runs a command synchronously and returns the result.
   *
   * @param cmd The command to run.
   * @param result Reference to a string where the output will be stored.
   * @return int The exit code of the process.
   */
  static int ExecuteSync(const QString& cmd, QString& result);

  /**
   * @brief Runs a command asynchronously with callbacks for finished and logging.
   *
   * @param cmd The command to run.
   * @param parent Pointer to the parent QObject.
   * @param finishedCb Callback function called when the process finishes.
   * @param stdoutCb Optional callback function for standard output logging.
   * @param stderrCb Optional callback function for standard error logging.
   */
  static void ExecuteAsync(const QString& cmd, QObject* parent,
                           FinshedCallback finishedCb, LogCallback stdoutCb = nullptr, LogCallback stderrCb = nullptr);

private:
  QProcessEnvironment mEnv; /// Environment variables for the process.
  QString mCwd; /// Current working directory for the process.
  int mTimeout; /// Timeout for the process in milliseconds.
};
