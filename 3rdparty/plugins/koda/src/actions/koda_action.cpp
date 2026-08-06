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
  return "Generate Koda";
#else
  return "Generate Koda";
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
  LOG_INFO("Running {}", id());
  // Get the canvas from the context
  // const auto artifacts = context.artifactsOfType("maki");
  // if (artifacts.isEmpty())
  //   return VoidResult::Failed("No artifacts available, requires \"maki\"");

  // const auto maki = artifacts.at(0);
  // QVector<std::shared_ptr<INode>> nodes;
  // maki >> nodes;
  // for (const auto& n : nodes)
  //   LOG_DEBUG("Will explore node: {}", n->id());

  // Generate files
  auto output = mGenerator->generateKoda(maki::PipelineArtifact{}, context.buildDir);
  if (!output.IsSuccess())
    return maki::ResultArtifacts::Failed(output.ErrorMessage());

  // Add artefacts
  maki::PipelineArtifact artifact = output.Value();
  artifact.id = "koda.generated";
  artifact.type = "koda";
  artifact.producer = id();

  return maki::Artifacts{artifact};
}