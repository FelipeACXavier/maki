#include "generator.h"

#include <QDir>

#include "elements/node.h"
#include "generator_plugin.h"
#include "logging.h"
#include "pipeline.h"

Generator::Generator(std::shared_ptr<SaveInfo> storage, QWidget* parent)
    : QWidget(parent)
    , mStorage(storage)
    , mPipeline(new Pipeline(this))
{
  connect(mPipeline, &Pipeline::openClient, [this](const QString& url) { emit openClient(url); });
}

void Generator::generate(const QString& outputDir, GeneratorPlugin* generator, const GenerationOptions& option)
{
  if (!mStorage)
  {
    LOG_ERROR("No storage available");
    return;
  }

  LOG_INFO("======================================");
  LOG_INFO("Starting generation");
  // Main generation loop, we need to:
  // For each component:
  //    1. Find all starting points
  //    2. Define the functions
  //    3. Write the computations
  //    4. Connect the callbacks
  QString text = generator->generateCode(outputDir, mStorage);
  // LOG_INFO("Generated code:");
  // LOG_INFO("%s", qPrintable(text));
  LOG_INFO("======================================");

  mPipeline->setName(generator->languageName());

  LOG_INFO("Generation done, wrote files to %s", qPrintable(outputDir));

  // Move these commands to the plugin interface
  QStringList generationOutput = {};
  auto generatedFiles = generator->generatedFiles();

  auto result = generatePipeline(outputDir, generatedFiles, generationOutput);
  if (!result.IsSuccess())
  {
    LOG_ERROR("Failed to prepare the generation pipeline: %s", result.ErrorMessage().c_str());
    return;
  }

  if (option.pipeline == Types::GenerationOptions::GenerateVerify)
  {
    QStringList verificationOutput = {};
    auto verify = verifyPipeline(generationOutput, verificationOutput);
    if (!verify.IsSuccess())
    {
      LOG_ERROR("Failed to prepare the verification pipeline: %s", verify.ErrorMessage().c_str());
      return;
    }
  }
  else if (option.pipeline == Types::GenerationOptions::GenerateSimulate)
  {
    QStringList simulationOutput = {};
    auto simulate = simulatePipeline(generationOutput, simulationOutput);
    if (!simulate.IsSuccess())
    {
      LOG_ERROR("Failed to prepare the simlation pipeline: %s", simulate.ErrorMessage().c_str());
      return;
    }
  }

  emit generationStarted(mPipeline);

  auto ran = mPipeline->start();
  if (!ran.IsSuccess())
    LOG_ERROR("Failed to run ipeline: %s", ran.ErrorMessage().c_str());
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

    mPipeline->add(generate, Pipeline::OnFail::STOP);
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
      LOG_INFO("Will verify file: %s", qPrintable(fullPath));

      const QString command = "dzn";
      const QStringList arguments = {"verify", fullPath};

      QProcess* generate = new QProcess(this);
      generate->setProgram(command);
      generate->setArguments(arguments);

      mPipeline->add(generate, Pipeline::OnFail::STOP);
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

      mPipeline->add(generate, Pipeline::OnFail::STOP, "http://localhost:3000/trace");
      output.append(fullPath);
    }
  }

  return VoidResult();
}
