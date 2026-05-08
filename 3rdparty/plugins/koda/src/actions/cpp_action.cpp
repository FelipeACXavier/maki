#include "cpp_action.h"

#include "../koda_generator.h"

GenerateCppAction::GenerateCppAction(KodaGenerator* generator)
    : mGenerator(generator)
{
}

QString GenerateCppAction::id() const
{
  return "koda.generate_dezyne_cpp";
}

QString GenerateCppAction::displayName() const
{
  return "Koda: Generate Dezyne Cpp";
}

QStringList GenerateCppAction::consumes() const
{
  return {"dezyne"};  // Diagram information
}

QStringList GenerateCppAction::produces() const
{
  return {"dezyne_cpp"};
}

VoidResult GenerateCppAction::run(maki::PipelineContext& context, const QVariantMap& parameters)
{
  // Use context.canvas
  // Generate files
  // Add artefacts
  context.addArtifact({
      .id = "dezyne_cpp.generated",
      .type = "dezyne_cpp",
      .producer = id(),
      .paths = {},
  });

  return VoidResult();
}