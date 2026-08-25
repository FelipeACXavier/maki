#include "explain_action.h"

#include "../ollama_plugin.h"
#include "logging.h"
#include "pipeline_artifact.h"
#include "result.h"

namespace maki
{
OllamaExplainAction::OllamaExplainAction(OllamaPlugin* generator)
    : mGenerator(generator)
{
}

QString OllamaExplainAction::id() const
{
  return "ollama.explain";
}

QString OllamaExplainAction::displayName() const
{
  return "Ollama explain";
}

QStringList OllamaExplainAction::consumes() const
{
  return {"koda"};
}

QStringList OllamaExplainAction::produces() const
{
  return {"ai-text"};
}

maki::ResultArtifacts OllamaExplainAction::run(const maki::PipelineContext& context, const QVariantMap& parameters, maki::IPipeline* pipeline)
{
  LOG_INFO("Running {}", id());
  // Get the canvas from the context
  const auto artifacts = context.artifactsOfType("koda");
  if (artifacts.isEmpty())
    return maki::ResultArtifacts::Failed("No artifacts available, requires \"koda\"");

  // Generate files
  const auto koda = artifacts.at(0);
  if (!koda.metadata.contains("sources"))
    return maki::ResultArtifacts::Failed("No sources in koda artifact");

  const auto filename = koda.metadata["sources"].toStringList();
  if (filename.isEmpty())
    return maki::ResultArtifacts::Failed("Empty sources in koda artifact");

  QFile file(filename.at(0));
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return maki::ResultArtifacts::Failed("Failed to open KODA source file: " + file.fileName().toStdString() +
                                         ": " + file.errorString().toStdString());

  const QString fileContents = QString::fromUtf8(file.readAll());

  auto prompted = mGenerator->sendRequest("Can you please explain the task being orchestrated by this KODA file:\n" + fileContents, pipeline);
  if (!prompted)
    return maki::ResultArtifacts::Failed(prompted.ErrorMessage());

  return maki::Artifacts{};
}
}  // namespace maki