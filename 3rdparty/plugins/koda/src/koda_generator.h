#pragma once

#include <QDir>
#include <QJsonObject>
#include <QObject>
#include <QProcess>
#include <memory>

#include "ast/ast.h"
#include "iplugin.h"
#include "isettings.h"
#include "pipeline_action.h"
#include "traceability_map.h"

class INode;
class IFlow;
class DezyneSimulator;

namespace koda
{
struct Error;
}

class KodaGenerator : public QObject, public maki::IPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID MAKI_GENERATORPLUGIN_IID FILE "koda.json")
  Q_INTERFACES(maki::IPlugin)

public:
  bool setup() override;
  bool tearDown() override;
  void setHostServices(maki::IHostServices* services) override;

  Result<maki::PipelineArtifact> generateKoda(const maki::PipelineArtifact& artifact, const QDir& outputFolder);
  Result<maki::PipelineArtifact> buildRosProject(const maki::PipelineArtifact& artifact, const QDir& outputFolder, const QString& rosFolder,
                                                 maki::IPipeline* pipeline);
  Result<maki::PipelineArtifact> launchRosProject(const maki::PipelineArtifact& artifact, const QDir& outputFolder, const QString& rosFolder,
                                                  maki::IPipeline* pipeline);

  QList<std::shared_ptr<maki::IPipelineAction>> pipelineActions() override;

private:
  std::shared_ptr<maki::TraceabilityMap> mTraceMap;
  std::shared_ptr<koda::System> mAST;

  void publishErrors(const std::vector<koda::Error>& errors) const;
};
