#include "generate_cpp.h"

#include "../arduino_plugin.h"
#include "logging.h"
#include "pipeline_artifact.h"

ArduinoGenerateCpp::ArduinoGenerateCpp(maki::ArduinoPlugin* generator)
    : mGenerator(generator)
{
}

QString ArduinoGenerateCpp::id() const
{
  return "arduino.generate_cpp";
}

QString ArduinoGenerateCpp::displayName() const
{
  return "Generate Arduino source files";
}

QStringList ArduinoGenerateCpp::consumes() const
{
  return {"koda"};
}

QStringList ArduinoGenerateCpp::produces() const
{
  return {"cpp"};
}

maki::ResultArtifacts ArduinoGenerateCpp::run(const maki::PipelineContext& context, const QVariantMap& parameters, maki::IPipeline* pipeline)
{
  LOG_INFO("Running {}", id());

  const auto artifacts = context.artifactsOfType("koda");
  if (artifacts.isEmpty())
    return maki::ResultArtifacts::Failed("No artifacts available, requires \"koda\"");

  // Generate files
  const auto koda = artifacts.at(0);

  auto generated = mGenerator->generateCpp(koda, context.buildDir, pipeline);
  if (!generated)
    return maki::ResultArtifacts::Failed(generated.ErrorMessage());

  // maki::PipelineArtifact artifact = generated.Value();
  // artifact.id = id();
  // artifact.type = "cpp";
  // artifact.producer = id();

  return maki::Artifacts{};
}