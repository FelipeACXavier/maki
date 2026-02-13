#include "generator.h"

#include <QDir>

#include "elements/node.h"
#include "generator_plugin.h"
#include "logging.h"
#include "pipeline.h"

Generator::Generator(Pipeline* pipeline, QObject* parent)
    : QObject(parent)
    , mPipeline(pipeline)
{
  connect(mPipeline, &Pipeline::openClient, [this](const QString& url) { emit openClient(url); });
  connect(mPipeline, &Pipeline::finishedLast, [this] { emit generationEnded(); });
}

Pipeline* Generator::pipeline() const
{
  return mPipeline;
}

VoidResult Generator::generate(const QString& outputDir, maki::IGeneratorPlugin* generator)
{
  LOG_INFO("[VERIFY] SetName");
  pipeline()->setName(generator->languageName());

  LOG_INFO("[VERIFY] Verify");
  QString text = generator->verify(outputDir);

  LOG_INFO("[VERIFY] started");
  emit generationStarted(pipeline());

  auto ran = pipeline()->start();
  if (!ran.IsSuccess())
    return VoidResult::Failed("Failed to run pipeline: " + ran.ErrorMessage());

  return VoidResult();
}

VoidResult Generator::simulate(const QString& outputDir, maki::IGeneratorPlugin* generator)
{
  pipeline()->setName(generator->languageName());

  QString text = generator->simulate(outputDir);

  emit generationStarted(pipeline());

  auto ran = pipeline()->start();
  if (!ran.IsSuccess())
    return VoidResult::Failed("Failed to run pipeline: " + ran.ErrorMessage());

  return VoidResult();
}

VoidResult Generator::generatePipeline(const QString& outputDir, const QStringList& input, QStringList& output)
{
  LOG_INFO("Preparing generation");

  // Make sure the directory exists
  QDir dir = QDir(outputDir + "/generated");
  if (!dir.exists())
    dir.mkpath(".");

  for (const auto& file : input)
  {
    LOG_DEBUG("Will generate from file: %s", qPrintable(file));
    const QString command = "java";
    const QStringList arguments = {
        "-jar",
        "/home/ubuntu/rascal-0.40.9.jar",
        "Main.rsc",          // Entrypoint for KODA
        file,                // Input
        dir.absolutePath(),  // Output
    };

    QProcess* generate = new QProcess(this);
    generate->setWorkingDirectory("/home/ubuntu/KODA");
    generate->setProgram(command);
    generate->setArguments(arguments);

    pipeline()->add(generate, maki::OnFail::STOP);
  }

  output.append(dir.absolutePath());

  return VoidResult();
}

VoidResult Generator::verifyPipeline(const QStringList& input, QStringList& output)
{
  LOG_INFO("Preparing verification");

  for (const auto& folder : input)
  {
    QDir dir(folder);
    QStringList files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);

    QStringList fullPaths;
    for (const QString& f : files)
    {
      auto fullPath = dir.absoluteFilePath(f);
      if (fullPath.contains("/a_") || fullPath.contains("types"))
        continue;

      LOG_INFO("Will verify file: %s", qPrintable(fullPath));

      const QString command = "ide";
      const QStringList arguments = {"verify", fullPath};

      QProcess* generate = new QProcess(this);
      generate->setProgram(command);
      generate->setArguments(arguments);

      pipeline()->add(generate, maki::OnFail::STOP);
    }
  }

  return VoidResult();
}

VoidResult Generator::simulatePipeline(const QStringList& input, QStringList& output)
{
  LOG_INFO("Preparing simulation");

  for (const auto& folder : input)
  {
    QDir dir(folder);
    QStringList files = dir.entryList(QDir::Files | QDir::NoDotAndDotDot);

    QStringList fullPaths;
    for (const QString& f : files)
    {
      auto fullPath = dir.absoluteFilePath(f);
      if (!fullPath.contains("_task"))
        continue;

      LOG_INFO("Will simulate: %s", qPrintable(fullPath));

      const QString command = "ide";
      const QStringList arguments = {"simulate", fullPath};

      QProcess* generate = new QProcess(this);
      generate->setProgram(command);
      generate->setArguments(arguments);

      pipeline()->add(generate, maki::OnFail::STOP, "http://localhost:3000/trace");
      output.append(fullPath);
    }
  }

  return VoidResult();
}
