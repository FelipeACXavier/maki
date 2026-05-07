#pragma once

#include <QDir>
#include <QJsonObject>
#include <QObject>
#include <QProcess>
#include <memory>

#include "dzn_client/simulation_scene.h"
#include "generator_plugin.h"
#include "isettings.h"

class INode;
class IFlow;
class DezyneSimulator;

class KodaGenerator : public QObject, public maki::IGeneratorPlugin
{
#ifdef USE_ANTLR
  Q_OBJECT
  Q_PLUGIN_METADATA(IID MAKI_GENERATORPLUGIN_IID FILE "koda_generator_antlr.json")
  Q_INTERFACES(maki::IGeneratorPlugin)
#else
  Q_OBJECT
  Q_PLUGIN_METADATA(IID MAKI_GENERATORPLUGIN_IID FILE "koda_generator.json")
  Q_INTERFACES(maki::IGeneratorPlugin)
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

  VoidResult verify(const QString& outputFolder) override;
  VoidResult simulate(const QString& outputFolder) override;

  QList<QString> generatedFiles() const override;
  void settingsChanged(const QVector<maki::SettingField>& settings) override;

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

  QVector<QString> mGeneratedIds = {};
  QProcess* mDaemon = nullptr;
  DezyneSimulator* mSimulator = nullptr;
  QJsonObject mLastUpdate;

  std::unique_ptr<TraceSceneBuilder> mTraceBuilder;

  struct Error
  {
    QString nodeId;
    QString nodeType;
    QString flowId;
    QString message;
  };

  QVector<Error> mErrors;

  struct Argument
  {
    QString name = "";
  };

  QString generateKoda(const QString& outputFolder);

  // Generic generators
  QString generateNode(const INode& node);
  QString generateBehaviourNode(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  QString generateTransitions(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);

  // These are the block generators
  QString generateComponent(const INode& node, const QString& code, const QString& args);
  QString generateCapability(const INode& node);

  QString generateStart(const QString& parent, const INode& node, const IFlow& flow, const QString& format);
  QString generateSuccess(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  QString generateError(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  QString generateContinue(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);

  QString generateAsyncTask(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  QString generateSyncTask(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  QString generateWithin(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  QString generateEvery(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  QString generateRepeat(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  QString generateStrategy(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);

  // Helpers
  QString fixCase(const QString& name);
  std::shared_ptr<INode> findDestination(const QString& nodeId, const IFlow& flow) const;
  QString createArguments(const QJsonArray& options) const;

  void startSimulation();
  void simulationStarted();
  void simulationUpdated(const QJsonObject& obj);
  VoidResult createSimulationScene(QGraphicsScene* scene, const QJsonObject& obj);

  // Setup
  bool startDaemon();
  void buildSettings();

  maki::SettingField getSetting(const QString& key) const;
};
