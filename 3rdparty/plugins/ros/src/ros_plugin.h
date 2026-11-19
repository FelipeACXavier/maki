#pragma once

#include <QObject>

#include "iplugin.h"
#include "isettings.h"
#include "pipeline_action.h"

namespace maki
{
class ROSPlugin : public QObject, public maki::IPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID MAKI_GENERATORPLUGIN_IID FILE "ros.json")
  Q_INTERFACES(maki::IPlugin)

public:
  // =================================================================
  // General plugin methods
  bool setup() override;
  bool tearDown() override;
  void setHostServices(maki::IHostServices* services) override;
  QList<std::shared_ptr<maki::IPipelineAction>> pipelineActions() override;

  Result<maki::PipelineArtifact> buildRosProject(const maki::PipelineArtifact& artifact, const QDir& outputFolder, const QString& rosFolder,
                                                 maki::IPipeline* pipeline);
  Result<maki::PipelineArtifact> launchRosProject(const maki::PipelineArtifact& artifact, const QDir& outputFolder, const QString& rosFolder,
                                                  maki::IPipeline* pipeline);

private:
  void buildSettings() override;
};

}  // namespace maki
