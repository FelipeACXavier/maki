#pragma once

#include <QObject>

#include "client/dezyne_simulator.h"
#include "client/simulation_scene.h"
#include "iplugin.h"
#include "isettings.h"
#include "pipeline_action.h"
#include "traceability_map.h"

namespace maki
{
class DezynePlugin : public QObject, public maki::IPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID MAKI_GENERATORPLUGIN_IID FILE "dezyne.json")
  Q_INTERFACES(maki::IPlugin)

public:
  // =================================================================
  // General plugin methods
  bool setup() override;
  bool tearDown() override;
  void setHostServices(maki::IHostServices* services) override;

  QList<std::shared_ptr<maki::IPipelineAction>> pipelineActions() override;

  // =================================================================
  // For the plugin actions
  VoidResult verify(const maki::PipelineArtifact& artifacts, const QDir& outputFolder, maki::IPipeline* pipeline);
  VoidResult simulate(const maki::PipelineArtifact& artifact);
  Result<maki::PipelineArtifact> generateCpp(const maki::PipelineArtifact& artifact, const QDir& outputFolder, maki::IPipeline* pipeline);
  Result<maki::PipelineArtifact> generateDezyne(const maki::PipelineArtifact& artifact, const QDir& outputFolder, const maki::ValueMap& parameters,
                                                maki::IPipeline* pipeline);

private:
  void buildSettings() override;

  // =================================================================
  // Members and methods used in Dezyne simulation
  struct Candidate
  {
    QString instance;
    QString instanceState;
    QVector<QString> labels;
    maki::MakiSource source;
    QString event;
  };

  struct SimulationControlConfig
  {
    QString text;
    QString iconPath;
    bool requiresAbort = true;
    QColor buttonColor;
    QColor highlightColor;
  };

  QProcess* mDaemon = nullptr;                       // Dezyne daemon, it needs to be started for verification and simulation
  DezyneSimulator* mSimulator = nullptr;             // The simulator client
  QJsonObject mLastUpdate;                           // Last update from the simulator
  std::unique_ptr<TraceSceneBuilder> mTraceBuilder;  // The scene that displays the current simulator state
  QVector<Candidate> mPreviousCandidates;

  bool startDaemon();
  bool stopDaemon();

  void simulationStarted();
  void simulationUpdated(const QJsonObject& obj);
  VoidResult createSimulationScene(QGraphicsScene* scene, const QJsonObject& obj);

  QWidget* createControl(const QString&, const QString& iconPath, const QString& event, int labelWidth, QWidget* parent);
  std::optional<SimulationControlConfig> getConfigFromCandidate(const Candidate& candidate, const QString& label, const maki::Theme& theme);
  QString getCandidatesState(const QString& instance, const QJsonArray& json);
  QVector<Candidate> collectCandidates(const QJsonObject& json);
  void highlightActiveNode(const QJsonObject& json, QGraphicsScene* currentScene, const maki::Theme& theme);
};

}  // namespace maki
