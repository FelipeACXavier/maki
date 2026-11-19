#include "koda_generator.h"

#include <QJsonDocument>

#include "actions/koda_action.h"
#include "ast/ir_json.h"
#include "ast/koda_compiler.h"
#include "idocument.h"
#include "ilogging.h"
#include "ipipeline.h"
#include "isettings.h"
#include "iui.h"
#include "logging.h"
#include "maki_to_koda.h"
#include "pipeline_artifact.h"
#include "result.h"
#include "typing/type_registry.h"

bool KodaGenerator::setup()
{
  return true;
}

bool KodaGenerator::tearDown()
{
  if (auto* tab = mServices->ui())
    tab->deregisterPlugin(languageName());

  return true;
}

void KodaGenerator::setHostServices(maki::IHostServices* services)
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

QList<std::shared_ptr<maki::IPipelineAction>> KodaGenerator::pipelineActions()
{
  return {
    std::make_shared<GenerateKodaAction>(this),
  };
}

void KodaGenerator::publishErrors(const std::vector<koda::Error>& errors) const
{
  if (!errors.empty())
  {
    auto firstError = errors.front();
    if (mServices)
      mServices->errorOnNode(QString::fromStdString(firstError.nodeId), QString::fromStdString(firstError.flowId), QString::fromStdString(firstError.message));
  }
}

Result<maki::PipelineArtifact> KodaGenerator::generateKoda(const maki::PipelineArtifact& artifact, const QDir& outputFolder)
{
  if (!outputFolder.exists())
    outputFolder.mkdir(".");

  LOG_DEBUG("Generating Koda files with {} nodes", mServices->document()->getnodes().size());
  QString code = "";

  // Clean before generation
  if (mTraceMap)
    mTraceMap->clear();

  const auto* typeRegistry = mServices->document()->getTypesRegistry();
  koda::MakiToKoda makiToKoda(typeRegistry, mTraceMap);
  const auto missionParameters = mServices->document()->getparameters();
  auto generated = makiToKoda.generate(mServices->document()->getnodes(), missionParameters);
  publishErrors(makiToKoda.getErrors());
  if (!generated)
    return Result<maki::PipelineArtifact>::Failed(generated.ErrorMessage());

  QString fileName = outputFolder.filePath("task.kd");
  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    return Result<maki::PipelineArtifact>::Failed("Failed to open device for writing: " + fileName.toStdString());

  QTextStream out(&file);
  out << generated.Value();
  file.close();

  // Generate the IR
  koda::Compiler compiler;
  // if (parameters.contains("Verbose"))
  //   options.verbose = 1;

  koda::CompilerOptions options;
  options.ast = std::make_shared<koda::System>(makiToKoda.getAST());
  options.typeRegistry = std::make_shared<koda::types::TypeRegistry>(*mServices->document()->getTypesRegistry());
  options.traceability = mTraceMap;
  options.inputFile = fileName.toStdString();

  RETURN_ON_FAILURE_AS(compiler.parse(options), maki::PipelineArtifact);
  RETURN_ON_FAILURE_AS(compiler.generate(), maki::PipelineArtifact);

  const auto jsonIr = koda::IrJson::toJson(compiler.getIR(), compiler.getSymbols(), {});

  // Serialize it
  maki::PipelineArtifact output;

  const auto json = QJsonDocument(jsonIr).toJson(QJsonDocument::Indented);
  QString jsonFileName = outputFolder.filePath("ir.json");
  QFile jsonFile(jsonFileName);
  if (jsonFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    jsonFile.write(json);
    jsonFile.close();
  }
  else
  {
    LOG_ERROR("Could not open file for writing: {}", jsonFileName);
  }

  // Add it to the output
  output.metadata = {
    {"sources", {fileName}},
    {"koda.ir", {jsonIr}},
  };

  return output;
}
