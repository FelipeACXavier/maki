#include "verify_action.h"

#include <QString>
#include <QVector>

#include "../koda_generator.h"
#include "koda_action.h"
#include "logging.h"
#include "pipeline_artifact.h"
#include "result.h"

KodaVerifyAction::KodaVerifyAction(KodaGenerator* generator)
    : mGenerator(generator)
{
}

QString KodaVerifyAction::id() const
{
#ifdef USE_ANTLR
  return "koda_antlr.verify_dezyne";
#else
  return "koda.verify_dezyne";
#endif
}

QString KodaVerifyAction::displayName() const
{
#ifdef USE_ANTLR
  return "Koda Antlr: Verify Dezyne";
#else
  return "Koda: Verify Dezyne";
#endif
}

QStringList KodaVerifyAction::consumes() const
{
  return {"dezyne"};
}

QStringList KodaVerifyAction::produces() const
{
  return {"verified_dezyne"};
}

maki::ResultArtifacts KodaVerifyAction::run(const maki::PipelineContext& context, const QVariantMap& parameters, maki::IPipeline* pipeline)
{
  LOG_INFO("Running %s", qPrintable(id()));
  // Get the canvas from the context
  const auto artifacts = context.artifactsOfType("dezyne");
  if (artifacts.isEmpty())
    return maki::ResultArtifacts::Failed("No artifacts available, requires \"dezyne\"");

  // Generate files
  const auto dezyne = artifacts.at(0);
  auto verified = mGenerator->verify(dezyne, context.buildDir, pipeline);
  if (!verified)
    return maki::ResultArtifacts::Failed(verified.ErrorMessage());

  return maki::Artifacts();
}