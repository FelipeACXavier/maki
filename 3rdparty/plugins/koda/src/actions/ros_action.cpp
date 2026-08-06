#include "ros_action.h"

#include <QString>
#include <QVector>

#ifdef USE_ANTLR
#include "../ast/koda_compiler.h"
// #include "../ros_emitter.h"
#endif
#include "logging.h"
#include "pipeline_artifact.h"
#include "result.h"

static const QString SIMULATE_KEY = "Simulate";
static const QString START_WAIT = "Start wait";

GenerateRosAction::GenerateRosAction()
{
}

QString GenerateRosAction::id() const
{
#ifdef USE_ANTLR
  return "koda_antlr.generate_ros";
#else
  return "koda.generate_ros";
#endif
}

QString GenerateRosAction::displayName() const
{
#ifdef USE_ANTLR
  return "Generate ROS";
#else
  return "Generate ROS";
#endif
}

QStringList GenerateRosAction::consumes() const
{
  return {"koda"};  // Diagram information
}

QStringList GenerateRosAction::produces() const
{
  return {"ros-project"};
}

QVector<maki::ActionParameter> GenerateRosAction::parameters() const
{
  return {
      maki::ActionParameter(SIMULATE_KEY, Types::PropertyTypes::BOOLEAN, true),
      maki::ActionParameter(START_WAIT, Types::PropertyTypes::REAL, 10.0),
  };
}

maki::ResultArtifacts GenerateRosAction::run(const maki::PipelineContext& context, const QVariantMap& parameters, maki::IPipeline* pipeline)
{
  LOG_INFO("Running {}", id());
  // Get the canvas from the context
  const auto artifacts = context.artifactsOfType("koda");
  if (artifacts.isEmpty())
    return maki::ResultArtifacts::Failed("No artifacts available, requires \"koda\"");

  const auto koda = artifacts.at(0);
  if (!koda.metadata.contains("sources"))
    return maki::ResultArtifacts::Failed("GenerateRosAction, missing input sources");

  auto outputFolder = context.buildDir;
  const QStringList inputFiles = koda.metadata["sources"].toStringList();
  if (!outputFolder.exists())
    outputFolder.mkpath(".");

  // Make sure the output is clean before the generation
  auto modelsOutputFolder = QDir(outputFolder.absolutePath() + "/ros");
  if (modelsOutputFolder.exists())
    modelsOutputFolder.removeRecursively();

  modelsOutputFolder.mkpath(".");

  maki::PipelineArtifact artifact = {};

#ifdef USE_ANTLR
  // koda::CompilerOptions options;
  // options.outputDir = modelsOutputFolder.absolutePath().toStdString();
  // options.dryRun = true;
  // options.verbose = 0;
  // options.pluginRule = koda::CompilerOptions::PluginOption::NoPlugins;

  // koda::RosEmitterOptions emitterOptions;
  // emitterOptions.outputDir = modelsOutputFolder.absolutePath().toStdString();
  // if (parameters.contains(SIMULATE_KEY))
  //   emitterOptions.simulation = parameters[SIMULATE_KEY].toBool();
  // if (parameters.contains(START_WAIT))
  //   emitterOptions.startWait = parameters[START_WAIT].toString().toDouble();

  // koda::Compiler compiler;
  // for (const auto& file : inputFiles)
  // {
  //   LOG_DEBUG("Parsing from file: {} to {}", file, outputFolder.absolutePath());
  //   // Make sure the input file is set before calling the compiler
  //   options.inputFile = file.toStdString();
  //   auto parsed = compiler.parse(options);
  //   if (!parsed.IsSuccess())
  //     return maki::ResultArtifacts::Failed(parsed.ErrorMessage());

  //   LOG_DEBUG("Generating from file: {} to {}", file, outputFolder.absolutePath());
  //   auto generated = compiler.generate();
  //   if (!generated.IsSuccess())
  //   {
  //     LOG_ERROR(generated.ErrorMessage());
  //     return maki::ResultArtifacts::Failed(generated.ErrorMessage());
  //   }

  //   koda::RosEmitter emitter;
  //   auto emitted = emitter.emitRos(compiler.getAST(), compiler.getIR(), emitterOptions);
  //   if (!emitted.ok)
  //     for (const auto& error : emitted.errors)
  //       LOG_ERROR(error);
  // }

  // // Add artefacts
  // artifact.paths = {
  //     {"rootDir", modelsOutputFolder.absolutePath()},
  //     {"includeDir", modelsOutputFolder.absolutePath() + "/include"},
  //     {"sourceDir", modelsOutputFolder.absolutePath() + "/src"},
  //     {"launchDir", modelsOutputFolder.absolutePath() + "/launch"},
  //     {"configDir", modelsOutputFolder.absolutePath() + "/config"},
  // };
  // artifact.metadata = {
  //     {"package", modelsOutputFolder.absolutePath() + "/package.xml"},
  //     {"cmake", modelsOutputFolder.absolutePath() + "/CMakeLists.txt"},
  //     {"packageName", QString::fromStdString(emitterOptions.packageName)},
  //     {"launchFile", QString::fromStdString(emitterOptions.launchFile)},
  // };

  // artifact.id = "koda.generate_ros";
  // artifact.type = "ros-project";
  // artifact.producer = id();
#else
  return maki::ResultArtifacts::Failed("Not supported");
#endif

  return maki::Artifacts{artifact};
}