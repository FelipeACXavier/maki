#include "ros_copy.h"

#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>

#include "logging.h"
#include "pipeline_artifact.h"
#include "result.h"

VoidResult copyDirectoryRecursively(const QString& sourceDir, const QString& targetDir)
{
  namespace fs = std::filesystem;

  fs::path libSrcPath = sourceDir.toStdString();
  fs::path libDstPath = targetDir.toStdString();

  std::error_code ec;
  if (!fs::exists(libSrcPath, ec))
    return VoidResult::Failed("Source folder does not exist: " + libSrcPath.string());

  // Make sure the files exist
  fs::create_directories(libDstPath, ec);
  if (ec)
    return VoidResult::Failed("Could not create output folder: " + libDstPath.string());

  fs::copy(libSrcPath, libDstPath, fs::copy_options::recursive | fs::copy_options::overwrite_existing, ec);
  if (ec)
    return VoidResult::Failed("Could not copy to output folder: " + ec.message());

  return VoidResult();
}

VoidResult copyTo(const QFileInfo& info, const QString& targetDir)
{
  if (!info.exists())
    return VoidResult::Failed("File does not exist: " + info.absoluteFilePath().toStdString());

  QString source = info.absoluteFilePath();
  // LOG_INFO("Copying: %s to %s", qPrintable(source), qPrintable(targetDir));

  if (info.isDir())
  {
    auto copied = copyDirectoryRecursively(source, targetDir);
    if (!copied.IsSuccess())
      return VoidResult::Failed("Failed to copy directory: " + source.toStdString() +
                                " to " + targetDir.toStdString());
  }
  else if (info.isFile())
  {
    namespace fs = std::filesystem;
    std::error_code ec;
    fs::copy(source.toStdString(), targetDir.toStdString(), fs::copy_options::overwrite_existing, ec);
    if (ec)
      return VoidResult::Failed("Failed to copy " + source.toStdString() +
                                " to " + targetDir.toStdString() +
                                ": " + ec.message());
  }
  else
  {
    return VoidResult::Failed("Unsupported type for: " + source.toStdString());
  }

  return VoidResult();
}

KodaRosCopySources::KodaRosCopySources()
{
}

QString KodaRosCopySources::id() const
{
#ifdef USE_ANTLR
  return "koda_antlr.copy_sources";
#else
  return "koda.copy_sources";
#endif
}

QString KodaRosCopySources::displayName() const
{
  return "Copy Sources to Project";
}

QStringList KodaRosCopySources::consumes() const
{
  return {"cpp", "ros-project"};
}

QStringList KodaRosCopySources::produces() const
{
  return {"ros-project"};
}

QVariantMap KodaRosCopySources::defaultParameters() const
{
  QStringList ignores = {"callback.hh", "callback.cc"};
  return {
      {"ignore", ignores},
  };
}

maki::ResultArtifacts KodaRosCopySources::run(const maki::PipelineContext& context, const QVariantMap& parameters, maki::IPipeline* pipeline)
{
  LOG_INFO("Running %s", qPrintable(id()));
  const auto projectArtifacts = context.artifactsOfType("ros-project");
  if (projectArtifacts.isEmpty())
    return maki::ResultArtifacts::Failed("No artifacts available, requires \"ros-project\"");

  const auto cppArtifacts = context.artifactsOfType("cpp");
  if (cppArtifacts.isEmpty())
    return maki::ResultArtifacts::Failed("No artifacts available, requires \"cpp\"");

  // Generate files
  const auto project = projectArtifacts.front();

  // auto pretty = QJsonDocument(project.toJson()).toJson(QJsonDocument::Indented);
  // LOG_DEBUG("Artifact %s %s: \n%s", qPrintable(project.id), qPrintable(project.producer), qPrintable(pretty));

  // pretty = QJsonDocument(cpp.toJson()).toJson(QJsonDocument::Indented);
  // LOG_DEBUG("Artifact %s %s: \n%s", qPrintable(cpp.id), qPrintable(cpp.producer), qPrintable(pretty));

  // Get all cpp sources and headers
  QStringList includeFiles = {};
  QStringList sourceFiles = {};
  QStringList ignores = {};
  if (parameters.contains("ignores"))
    ignores = parameters["ignores"].toStringList();

  for (const auto& cpp : cppArtifacts)
  {
    if (cpp.metadata.contains("includes"))
      includeFiles += cpp.metadata["includes"].toStringList();
    if (cpp.paths.contains("includeDir"))
      includeFiles += cpp.paths["includeDir"].toString();

    if (cpp.metadata.contains("sources"))
      sourceFiles += cpp.metadata["sources"].toStringList();
    if (cpp.paths.contains("sourceDir"))
      sourceFiles += cpp.paths["sourceDir"].toString();
  }

  if (includeFiles.isEmpty() || sourceFiles.isEmpty())
    return maki::ResultArtifacts::Failed("No files to be copied");

  // Get destinations
  if (!project.paths.contains("includeDir"))
    return maki::ResultArtifacts::Failed("Cannot copy C++ files, no include dir provided");
  if (!project.paths.contains("sourceDir"))
    return maki::ResultArtifacts::Failed("Cannot copy C++ files, no source dir provided");

  const auto dstIncludes = project.paths["includeDir"].toString();
  const auto dstSources = project.paths["sourceDir"].toString();

  for (const auto& entry : includeFiles)
  {
    const QFileInfo info(entry);
    auto copied = copyTo(info, dstIncludes);
    if (!copied)
      return maki::ResultArtifacts::Failed(copied.ErrorMessage());
  }

  for (const auto& entry : sourceFiles)
  {
    const QFileInfo info(entry);
    auto copied = copyTo(info, dstSources);
    if (!copied)
      return maki::ResultArtifacts::Failed(copied.ErrorMessage());
  }

  return maki::Artifacts{};
}
