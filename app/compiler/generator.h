#pragma once

#include <QObject>
#include <QString>

#include "pipeline.h"
#include "result.h"

namespace maki
{
class IGeneratorPlugin;
}

/**
 * @brief Handles code generation and simulation workflows using generator plugins.
 *
 * The Generator class coordinates the execution of generator plugins through a
 * Pipeline. It is responsible for invoking generation or simulation procedures
 * and emitting signals when these workflows start and finish.
 *
 * Generator plugins provide the actual implementation for producing artifacts
 * or launching simulations based on the current model state.
 */
class Generator : public QObject
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a Generator instance.
   *
   * @param pipeline Pipeline used to execute generation-related processes.
   * @param parent Optional Qt parent object.
   */
  Generator(Pipeline* pipeline, QObject* parent = nullptr);

  /**
   * @brief Generates output artifacts using the specified generator plugin.
   *
   * The generator plugin is responsible for producing code or other output
   * files in the provided output directory. The process is executed through
   * the configured pipeline.
   *
   * @param outputDir Directory where generated artifacts will be written.
   * @param generator Generator plugin responsible for producing the output.
   * @return VoidResult indicating whether generation was successfully initiated.
   */
  VoidResult generate(const QString& outputDir, maki::IGeneratorPlugin* generator);

  /**
   * @brief Runs a simulation using the specified generator plugin.
   *
   * Similar to the generation workflow, but intended for launching simulation
   * environments or related runtime artifacts produced by the generator plugin.
   *
   * @param outputDir Directory containing the generated artifacts required for simulation.
   * @param generator Generator plugin responsible for preparing the simulation.
   * @return VoidResult indicating whether simulation was successfully initiated.
   */
  VoidResult simulate(const QString& outputDir, maki::IGeneratorPlugin* generator);

  /**
   * @brief Returns the pipeline used for generation workflows.
   *
   * @return Pointer to the associated Pipeline instance.
   */
  Pipeline* pipeline() const;

  /**
   * @brief Performs initialization and setup of the generator.
   *
   * This may include configuring the pipeline, preparing generation
   * directories, or registering required callbacks.
   */
  void setup();

signals:
  /**
   * @brief Emitted when the generation pipeline starts.
   *
   * Observers may use this signal to display progress indicators
   * or track generation activity.
   *
   * @param pipeline Pipeline responsible for executing the generation steps.
   */
  void generationStarted(Pipeline* pipeline);

  /**
   * @brief Emitted when generation has completed.
   *
   * @param outputFolder Directory where the generated artifacts were written.
   */
  void generationEnded(const QString& outputFolder);

private:
  /// Pipeline responsible for executing generation and simulation processes.
  Pipeline* mPipeline = nullptr;

  /// Directory where generated artifacts are stored.
  QString mGenerationFolder;

  /// Holds information regarding the current generation pipeline
  Pipeline::Info mInfo;

  /// Holds the id of the progress widget
  QString mProgressId;

  bool mIsRunning;

  std::string notificationHeader(const QString& languageName) const;  /// Returns a header string for notifications.

  QWidget* progressContent();  /// Creates and returns the progress content widget.
};
