#pragma once

#include <QDir>
#include <QJsonObject>
#include <QObject>
#include <QProcess>

#include "iplugin.h"
#include "isettings.h"
#include "pipeline_action.h"

namespace maki
{
class OllamaPlugin : public QObject, public maki::IPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID MAKI_GENERATORPLUGIN_IID FILE "ollama.json")
  Q_INTERFACES(maki::IPlugin)

public:
  bool setup() override;
  bool tearDown() override;
  void setHostServices(maki::IHostServices* services) override;
  void setAssetDir(const QDir& dir) override;

  void settingsChanged(const QVector<maki::SettingField>& settings) override;

  Result<maki::PipelineArtifact> sendRequest(const QString& request, maki::IPipeline* pipeline);
  QList<std::shared_ptr<maki::IPipelineAction>> pipelineActions() override;

private:
  std::optional<QDir> mAssetDir;
  maki::IHostServices* mServices = nullptr;
  QVector<maki::SettingField> mSettings = {};

  void buildSettings();
};

}  // namespace maki