#include "koda_action.h"

#include "../koda_generator.h"

GenerateKodaAction::GenerateKodaAction(KodaGenerator* generator)
    : mGenerator(generator)
{
}

QString GenerateKodaAction::id() const
{
  return "koda.generate_koda";
}

QString GenerateKodaAction::displayName() const
{
  return "Koda: Generate Koda";
}

QStringList GenerateKodaAction::consumes() const
{
  return {"maki"};  // Diagram information
}

QStringList GenerateKodaAction::produces() const
{
  return {"koda"};
}

VoidResult GenerateKodaAction::run(maki::PipelineContext& context, const QVariantMap& parameters)
{
  // Use context.canvas
  // Generate files
  // Add artefacts
  context.addArtifact({
      .id = "koda.generated",
      .type = "koda",
      .producer = id(),
      .paths = {},
  });

  return VoidResult();
}