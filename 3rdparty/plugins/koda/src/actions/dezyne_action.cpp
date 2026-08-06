#include "dezyne_action.h"

#include <QString>

#include "../koda_generator.h"
#include "logging.h"
#include "pipeline_artifact.h"
#include "types.h"

GenerateDezyneAction::GenerateDezyneAction(KodaGenerator* generator)
    : mGenerator(generator)
{
}

QString GenerateDezyneAction::id() const
{
#ifdef USE_ANTLR
  return "koda_antlr.generate_dezyne";
#else
  return "koda.generate_dezyne";
#endif
}

QString GenerateDezyneAction::displayName() const
{
#ifdef USE_ANTLR
  return "Generate Dezyne";
#else
  return "Generate Dezyne";
#endif
}

QStringList GenerateDezyneAction::consumes() const
{
  return {"koda"};  // Diagram information
}

QStringList GenerateDezyneAction::produces() const
{
  return {"dezyne"};
}

QVariantMap GenerateDezyneAction::defaultParameters() const
{
  return {};
}

QVector<maki::ActionParameter> GenerateDezyneAction::parameters() const
{
  return {maki::ActionParameter("Debug", Types::PropertyTypes::BOOLEAN, false)};
}

maki::ResultArtifacts GenerateDezyneAction::run(const maki::PipelineContext& context, const QVariantMap& parameters, maki::IPipeline* pipeline)
{
  LOG_INFO("Running {}", qPrintable(id()));

  const auto artifacts = context.artifactsOfType("koda");
  if (artifacts.isEmpty())
    return maki::ResultArtifacts::Failed("No artifacts available, requires \"koda\"");

  const auto koda = artifacts.at(0);
  auto result = mGenerator->generateDezyne(koda, context.buildDir, pipeline);
  if (!result.IsSuccess())
    return maki::ResultArtifacts::Failed(result.ErrorMessage());

  maki::PipelineArtifact artifact = result.Value();
  artifact.id = "dezyne.generated";
  artifact.type = "dezyne";
  artifact.producer = id();

  return maki::Artifacts{artifact};
}