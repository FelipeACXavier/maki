#include "dezyne_action.h"

#include "../koda_generator.h"

GenerateDezyneAction::GenerateDezyneAction(KodaGenerator* generator)
    : mGenerator(generator)
{
}
QString GenerateDezyneAction::id() const
{
  return "koda.generate_dezyne";
}

QString GenerateDezyneAction::displayName() const
{
  return "Generate Dezyne";
}

QStringList GenerateDezyneAction::consumes() const
{
  return {"koda"};  // Diagram information
}

QStringList GenerateDezyneAction::produces() const
{
  return {"dezyne"};
}

VoidResult GenerateDezyneAction::run(maki::PipelineContext& context, const QVariantMap& parameters)
{
  // Use context.canvas
  // Generate files
  // Add artefacts

  context.addArtifact({
      .id = "dezyne.generated",
      .type = "dezyne",
      .producer = id(),
      .paths = {},
  });

  return VoidResult();
}