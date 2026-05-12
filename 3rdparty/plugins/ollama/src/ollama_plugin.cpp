#include "ollama_plugin.h"

#include <memory>

#include "actions/explain_action.h"
#include "ilogging.h"
#include "ipipeline.h"
#include "isettings.h"
#include "logging.h"
#include "pipeline_action.h"
#include "pipeline_artifact.h"

namespace maki
{
bool OllamaPlugin::setup()
{
  return true;
}

bool OllamaPlugin::tearDown()
{
  return true;
}

void OllamaPlugin::setHostServices(maki::IHostServices* services)
{
  mServices = services;
  if (auto logger = mServices->logger())
  {
    logging::gSourceName = languageName().toStdString();
    logging::gSilentLog = false;
    logger->registerPlugin(languageName(), logging::gLogToStream);
  }

  buildSettings();
}

void OllamaPlugin::setName(const QString& name)
{
  mName = name;
}

void OllamaPlugin::setVersion(const QString& version)
{
  mVersion = PluginVersion::fromString(version);
}

void OllamaPlugin::setAssetDir(const QDir& dir)
{
  mAssetDir = dir;
}

QString OllamaPlugin::languageName() const
{
  return mName;
}

maki::PluginVersion OllamaPlugin::version() const
{
  return mVersion;
}

void OllamaPlugin::settingsChanged(const QVector<maki::SettingField>& settings)
{
}

Result<maki::PipelineArtifact> OllamaPlugin::sendRequest(const QString& request, maki::IPipeline* pipeline)
{
  pipeline->startGroup("Ollama request");

  const QString command = "ollama";
  QStringList arguments = {
      "run",
      "qwen2.5-coder:7b-instruct",
      request,
  };

  LOG_DEBUG("Sending prompt: %s", qPrintable(arguments.join("\n")));

  QProcess* generate = new QProcess(this);
  generate->setProgram(command);
  generate->setArguments(arguments);

  pipeline->add(generate, maki::OnFail::STOP);
  pipeline->endGroup();

  return maki::PipelineArtifact{};
}

QList<std::shared_ptr<maki::IPipelineAction>> OllamaPlugin::pipelineActions()
{
  return {
      std::make_shared<OllamaExplainAction>(this),
  };
}

void OllamaPlugin::buildSettings()
{
}

}  // namespace maki