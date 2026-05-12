#include "cpp_action.h"

#include "../koda_generator.h"
#include "logging.h"
#include "pipeline_artifact.h"

GenerateCppAction::GenerateCppAction(KodaGenerator* generator)
    : mGenerator(generator)
{
}

QString GenerateCppAction::id() const
{
#ifdef USE_ANTLR
  return "koda_antlr.generate_dezyne_cpp";
#else
  return "koda.generate_dezyne_cpp";
#endif
}

QString GenerateCppAction::displayName() const
{
#ifdef USE_ANTLR
  return "Koda Antlr: Generate Dezyne Cpp";
#else
  return "Koda: Generate Dezyne Cpp";
#endif
}

QStringList GenerateCppAction::consumes() const
{
  return {"dezyne"};  // Diagram information
}

QStringList GenerateCppAction::produces() const
{
  return {"cpp"};
}

maki::ResultArtifacts GenerateCppAction::run(const maki::PipelineContext& context, const QVariantMap& parameters, maki::IPipeline* pipeline)
{
  LOG_INFO("Running %s", qPrintable(id()));
  // Get the canvas from the context
  const auto artifacts = context.artifactsOfType("dezyne");
  if (artifacts.isEmpty())
    return maki::ResultArtifacts::Failed("No artifacts available, requires \"dezyne\"");

  // Generate files
  const auto dezyne = artifacts.at(0);
  auto generated = mGenerator->generateCpp(dezyne, context.buildDir, pipeline);
  if (!generated)
    return maki::ResultArtifacts::Failed(generated.ErrorMessage());

  maki::PipelineArtifact artifact = generated.Value();
  artifact.id = "dezyne.generate_dezyne_cpp";
  artifact.type = "cpp";
  artifact.producer = id();

  return maki::Artifacts{artifact};
}