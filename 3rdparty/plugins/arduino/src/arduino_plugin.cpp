#include "arduino_plugin.h"

#include <memory>

#include "actions/generate_cpp.h"
#include "ast/koda_compiler.h"
#include "compiler/koda_arduino_plugin.h"
#include "ilogging.h"
#include "ipipeline.h"
#include "isettings.h"
#include "logging.h"

namespace maki
{
bool ArduinoPlugin::setup()
{
  return true;
}

bool ArduinoPlugin::tearDown()
{
  return true;
}

void ArduinoPlugin::setHostServices(maki::IHostServices* services)
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

void ArduinoPlugin::setAssetDir(const QDir& dir)
{
  mAssetDir = dir;
}

void ArduinoPlugin::settingsChanged(const QVector<maki::SettingField>& settings)
{
}

QList<std::shared_ptr<maki::IPipelineAction>> ArduinoPlugin::pipelineActions()
{
  return {
      std::make_shared<ArduinoGenerateCpp>(this),
  };
}

Result<maki::PipelineArtifact> ArduinoPlugin::generateCpp(const maki::PipelineArtifact& artifact, const QDir& outputFolder, maki::IPipeline* pipeline)
{
  if (!artifact.metadata.contains("sources"))
    return Result<maki::PipelineArtifact>::Failed("generateDezyne, missing input sources");

  const QStringList inputFiles = artifact.metadata["sources"].toStringList();
  if (!outputFolder.exists())
    outputFolder.mkpath(".");

  auto modelsOutputFolder = QDir(outputFolder.absolutePath() + "/arduino");
  if (modelsOutputFolder.exists())
    modelsOutputFolder.removeRecursively();

  modelsOutputFolder.mkpath(".");

  koda::Compiler compiler;
  auto arduinoPlugin = std::make_shared<KodaArduinoPlugin>();
  auto added = compiler.addPlugin(arduinoPlugin);
  if (!added)
    return Result<maki::PipelineArtifact>::Failed("Could not add arduino plugin to Koda");

  for (const auto& file : inputFiles)
  {
    koda::CompilerOptions options;
    options.inputFile = file.toStdString();
    options.outputDir = modelsOutputFolder.absolutePath().toStdString();
    // Make sure we only execute the arduino part
    options.pluginRule = koda::CompilerOptions::PluginOption::PluginsOnly;

    LOG_DEBUG("Generating from file: {} to {}", file, outputFolder.absolutePath());
    auto parsed = compiler.parse(options);
    if (!parsed)
      return Result<maki::PipelineArtifact>::Failed(parsed.ErrorMessage());

    auto generated = compiler.generate();
    if (!generated)
      return Result<maki::PipelineArtifact>::Failed(generated.ErrorMessage());
  }

  return maki::PipelineArtifact{};
}

void ArduinoPlugin::buildSettings()
{
}

}  // namespace maki