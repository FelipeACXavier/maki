#pragma once

#include <QDir>
#include <QJsonObject>
#include <QObject>
#include <QProcess>

#include "iplugin.h"
#include "isettings.h"
#include "pipeline_action.h"

class ProjectPage;
class QPlainTextEdit;

namespace maki
{

class ProjectPage;

class PlatformIOPlugin : public QObject, public maki::IPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID MAKI_GENERATORPLUGIN_IID FILE "platformio.json")
  Q_INTERFACES(maki::IPlugin)

public:
  bool setup() override;
  bool tearDown() override;
  void setHostServices(maki::IHostServices* services) override;
  void setName(const QString& name) override;
  void setVersion(const QString& version) override;
  void setAssetDir(const QDir& dir) override;

  QString languageName() const override;
  maki::PluginVersion version() const override;

  void settingsChanged(const QVector<maki::SettingField>& settings) override;

  QList<std::shared_ptr<maki::IPipelineAction>> pipelineActions() override;
  Result<maki::PipelineArtifact> initialiseProject(const QDir& outputFolder, maki::IPipeline* pipeline);
  Result<maki::PipelineArtifact> buildProject(const maki::PipelineArtifact& artifact, const QDir& outputFolder, maki::IPipeline* pipeline);

private:
  maki::PluginVersion mVersion;
  QString mName;
  std::optional<QDir> mAssetDir;

  maki::IHostServices* mServices = nullptr;

  QString mPioExecutable;
  QString mCurrentOutputFolder;

  maki::ProjectPage* mProjectPage = nullptr;
  QPlainTextEdit* mOutputPanel = nullptr;

  QProcess* mMonitorProcess = nullptr;

  QVector<maki::SettingField> mSettings = {};
  QList<QString> mGeneratedFiles = {};

  VoidResult writePlatformIni(const QDir& outputFolder);
  VoidResult writeMainCpp(const QString& outputFolder);

  void refreshDevices();
  void appendOutput(const QString& text);

  VoidResult runPio(const QString& workingDir, const QStringList& arguments, bool waitForFinished = true);

  void buildSettings();
};

}  // namespace maki