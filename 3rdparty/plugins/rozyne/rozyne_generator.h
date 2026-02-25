#pragma once

#include <QDir>
#include <QJsonObject>
#include <QObject>
#include <QProcess>

#include "generator_plugin.h"
#include "isettings.h"

class INode;
class IFlow;
class DezyneSimulator;

class RozyneGenerator : public QObject, public maki::IGeneratorPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID MAKI_GENERATORPLUGIN_IID FILE "rozyne_generator.json")
  Q_INTERFACES(maki::IGeneratorPlugin)

public:
  bool setup() override;
  bool tearDown() override;
  void setHostServices(maki::IHostServices* services) override;

  QString languageName() const override;
  maki::PluginVersion version() const override;

  QString verify(const QString& outputFolder) override;
  QString simulate(const QString& outputFolder) override;

  QList<QString> generatedFiles() const override;

private:
  QDir mOutputFolder;
  QDir mDezyneOutputFolder;
  QVector<QString> mImports;
  QList<QString> mGeneratedFiles = {};
  QList<QString> mGeneratedDznFiles = {};
  maki::IHostServices* mServices = nullptr;
  QVector<maki::SettingField> mSettings = {};

  QVector<QString> mGeneratedIds = {};
  QProcess* mDaemon = nullptr;
  DezyneSimulator* mSimulator = nullptr;
  QJsonObject mLastUpdate;

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
  QString generateEnd(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  QString generateError(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);

  QString generateAsyncTask(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  QString generateSyncTask(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  QString generateWithin(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  QString generateRepeat(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);
  QString generateStrategy(const INode& node, const Argument& arg, const IFlow& flow, const QString& format);

  // Helpers
  QString fixCase(const QString& name);
  std::shared_ptr<INode> findDestination(const QString& nodeId, const IFlow& flow) const;

  void simulationStarted();
  void simulationUpdated(const QJsonObject& obj);
  VoidResult createSimulationScene(QGraphicsScene* scene, const QJsonObject& obj);

  // Setup
  bool startDaemon();
  void buildSettings();
};
