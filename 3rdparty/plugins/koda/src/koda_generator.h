#pragma once

#include <QDir>
#include <QJsonObject>
#include <QObject>
#include <QProcess>
#include <memory>

#include "dzn_client/simulation_scene.h"
#include "iplugin.h"
#include "isettings.h"
#include "pipeline_action.h"

class INode;
class IFlow;
class DezyneSimulator;

class KodaGenerator : public QObject, public maki::IPlugin
{
#ifdef USE_ANTLR
  Q_OBJECT
  Q_PLUGIN_METADATA(IID MAKI_GENERATORPLUGIN_IID FILE "koda_generator_antlr.json")
  Q_INTERFACES(maki::IPlugin)
#else
  Q_OBJECT
  Q_PLUGIN_METADATA(IID MAKI_GENERATORPLUGIN_IID FILE "koda_generator.json")
  Q_INTERFACES(maki::IPlugin)
#endif

public:
  bool setup() override;
  bool tearDown() override;
  void setHostServices(maki::IHostServices* services) override;
  void setAssetDir(const QDir& dir) override;

  Result<maki::PipelineArtifact> generateKoda(const maki::PipelineArtifact& artifact, const QDir& outputFolder);
  Result<maki::PipelineArtifact> generateCpp(const maki::PipelineArtifact& artifact, const QDir& outputFolder, maki::IPipeline* pipeline);
  Result<maki::PipelineArtifact> generateDezyne(const maki::PipelineArtifact& artifact, const QDir& outputFolder, maki::IPipeline* pipeline);
  Result<maki::PipelineArtifact> buildRosProject(const maki::PipelineArtifact& artifact, const QDir& outputFolder, maki::IPipeline* pipeline);
  Result<maki::PipelineArtifact> launchRosProject(const maki::PipelineArtifact& artifact, const QDir& outputFolder, maki::IPipeline* pipeline);

  VoidResult verify(const maki::PipelineArtifact& artifacts, const QDir& outputFolder, maki::IPipeline* pipeline);
  VoidResult simulate(const maki::PipelineArtifact& artifact);

  void settingsChanged(const QVector<maki::SettingField>& settings) override;

  QList<std::shared_ptr<maki::IPipelineAction>> pipelineActions() override;

private:
  QDir mOutputFolder;
  QDir mDezyneOutputFolder;
  QVector<QString> mImports;
  QList<QString> mGeneratedFiles = {};
  QList<QString> mGeneratedDznFiles = {};
  maki::IHostServices* mServices = nullptr;
  QVector<maki::SettingField> mSettings = {};

  std::optional<QDir> mAssetDir;

  QProcess* mDaemon = nullptr;
  DezyneSimulator* mSimulator = nullptr;
  QJsonObject mLastUpdate;

  std::unique_ptr<TraceSceneBuilder> mTraceBuilder;

  void simulationStarted();
  void simulationUpdated(const QJsonObject& obj);
  VoidResult createSimulationScene(QGraphicsScene* scene, const QJsonObject& obj);

  // Setup
  bool startDaemon();
  void buildSettings();

  maki::SettingField getSetting(const QString& key) const;
};
