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
  void setName(const QString& name) override;
  void setVersion(const QString& name) override;
  void setAssetDir(const QDir& dir) override;

  QString languageName() const override;
  maki::PluginVersion version() const override;

  Result<maki::PipelineArtifact> generateKoda(const maki::PipelineArtifact& artifact, const QDir& outputFolder);
  Result<maki::PipelineArtifact> generateCpp(const maki::PipelineArtifact& artifact, const QDir& outputFolder, maki::IPipeline* pipeline);
  Result<maki::PipelineArtifact> generateDezyne(const maki::PipelineArtifact& artifact, const QDir& outputFolder);

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

  maki::PluginVersion mVersion;
  QString mName;
  std::optional<QDir> mAssetDir;

  QProcess* mDaemon = nullptr;
  DezyneSimulator* mSimulator = nullptr;
  QJsonObject mLastUpdate;

  std::unique_ptr<TraceSceneBuilder> mTraceBuilder;

  struct Argument
  {
    QString name = "";
  };

  // Generic generators
  Result<QString> generateBehaviourNode(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  Result<QString> generateTransitions(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);

  // These are the block generators
  VoidResult generateComponent(const INode& node, const QString& code, const QString& args);
  Result<QString> generateCapability(const INode& node);

  Result<QString> generateStart(const QString& parent, const INode& node, const IFlow& flow, const QString& format);
  Result<QString> generateSuccess(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  Result<QString> generateError(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  Result<QString> generateContinue(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);

  Result<QString> generateAsyncTask(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  Result<QString> generateSyncTask(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  Result<QString> generateWithin(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  Result<QString> generateEvery(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  Result<QString> generateRepeat(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  Result<QString> generateStrategy(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);

  // Helpers
  QString fixCase(const QString& name);
  std::shared_ptr<INode> findDestination(const QString& nodeId, const IFlow& flow) const;
  QString createArguments(const QJsonArray& options) const;

  void simulationStarted();
  void simulationUpdated(const QJsonObject& obj);
  VoidResult createSimulationScene(QGraphicsScene* scene, const QJsonObject& obj);

  // Setup
  bool startDaemon();
  void buildSettings();

  maki::SettingField getSetting(const QString& key) const;
};
