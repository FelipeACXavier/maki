#include "koda_action.h"

#include <QString>
#include <QVector>

#include "../koda_generator.h"
#include "logging.h"
#include "pipeline_artifact.h"
#include "result.h"

GenerateKodaAction::GenerateKodaAction(KodaGenerator* generator)
    : mGenerator(generator)
{
}

QString GenerateKodaAction::id() const
{
#ifdef USE_ANTLR
  return "koda_antlr.generate_koda";
#else
  return "koda.generate_koda";
#endif
}

QString GenerateKodaAction::displayName() const
{
#ifdef USE_ANTLR
  return "Koda Antlr: Generate Koda";
#else
  return "Koda: Generate Koda";
#endif
}

QStringList GenerateKodaAction::consumes() const
{
  return {"maki"};  // Diagram information
}

QStringList GenerateKodaAction::produces() const
{
  return {"koda"};
}

maki::ResultArtifacts GenerateKodaAction::run(const maki::PipelineContext& context, const QVariantMap& parameters, maki::IPipeline* pipeline)
{
  LOG_INFO("Running %s", qPrintable(id()));
  // Get the canvas from the context
  // const auto artifacts = context.artifactsOfType("maki");
  // if (artifacts.isEmpty())
  //   return VoidResult::Failed("No artifacts available, requires \"maki\"");

  // const auto maki = artifacts.at(0);
  // QVector<std::shared_ptr<INode>> nodes;
  // maki >> nodes;
  // for (const auto& n : nodes)
  //   LOG_DEBUG("Will explore node: %s", qPrintable(n->id()));

  // Generate files
  mGenerator->generateKoda(context.buildDir);

  // Add artefacts
  maki::PipelineArtifact artifact = {
      .id = "koda.generated",
      .type = "koda",
      .producer = id(),
      .metadata = {
          {"sources", mGenerator->generatedFiles()},
      },
  };

  return maki::Artifacts{ artifact };
}