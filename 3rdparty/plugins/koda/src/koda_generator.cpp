#include "koda_generator.h"

#include <QApplication>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QTextStream>
#include <filesystem>

#include "actions/cpp_action.h"
#include "actions/dezyne_action.h"
#include "actions/koda_action.h"
#include "actions/simulate_action.h"
#include "actions/verify_action.h"
#include "dzn_client/dezyne_simulator.h"
#include "dzn_client/simulation_scene.h"
#include "idocument.h"
#include "ilogging.h"
#include "ipipeline.h"
#include "isettings.h"
#include "iui.h"
#include "logging.h"
#include "pipeline_artifact.h"
#include "result.h"
#include "string_helpers.h"
#include "types.h"

#ifdef USE_ANTLR
#include "ast/koda_compiler.h"
#endif

#define APPEND_OR_RETURN_ON_FAILURE(v, func) \
  do                                         \
  {                                          \
    auto ret = func;                         \
    if (!ret.IsSuccess())                    \
      return ret;                            \
                                             \
    v += ret.Value();                        \
  } while (0)

VoidResult copyDirectory(const QString& sourceDir, const QString& targetDir)
{
  namespace fs = std::filesystem;

  fs::path libSrcPath = sourceDir.toStdString();
  fs::path libDstPath = targetDir.toStdString();

  std::error_code ec;
  if (!fs::exists(libSrcPath, ec))
    return VoidResult::Failed("Dezyne library source folder does not exist: " + libSrcPath.string());

  // Make sure the files exist
  fs::create_directories(libDstPath, ec);
  if (ec)
    return VoidResult::Failed("Could not create Dezyne library output folder: " + libDstPath.string());

  fs::copy(libSrcPath, libDstPath, fs::copy_options::recursive | fs::copy_options::overwrite_existing, ec);
  if (ec)
    return VoidResult::Failed("Could not copy Dezyne libraries to output folder: " + ec.message());

  return VoidResult();
}

bool KodaGenerator::setup()
{
  if (!mSimulator)
  {
    mSimulator = new DezyneSimulator(this);

    connect(mSimulator, &DezyneSimulator::simulationStarted, this, &KodaGenerator::simulationStarted);
    connect(mSimulator, &DezyneSimulator::simulationUpdated, this, &KodaGenerator::simulationUpdated);
  }

  // Start the ide daemon on a specific port
  // return startDaemon();
  return true;
}

bool KodaGenerator::tearDown()
{
  if (mDaemon == nullptr)
    return true;

  if (auto* tab = mServices->ui())
    tab->deregisterPlugin(languageName());

  if (mDaemon->state() == QProcess::Running)
  {
    mDaemon->terminate();

    if (!mDaemon->waitForFinished(2500))
    {
      mDaemon->kill();
      mDaemon->waitForFinished();
    }
  }

  mDaemon = nullptr;

  return true;
}

void KodaGenerator::buildSettings()
{
  // -d, --debug             enable debug ouput
  // -b, --view-port=PORT    browser view listen PORT [3002]
  // -e, --editor-port=PORT  editor listen PORT [3003]
  // -h, --help              display this help and exit
  // -i, --ide-port=PORT     ide command listen PORT [3001]
  // -l, --log-file=LOG      write output to LOG
  // -v, --verbose           be more verbose, show progress
  // -w, --http-port=PORT    web server http listen PORT [3000]

  maki::SettingField httpPort;
  httpPort.setKey("httpPort");
  httpPort.setLabel("HTTP Port");
  httpPort.setDescription("Web server HTTP port");
  httpPort.setDefaultValue(3000);

  QVariantMap httpMetadata;
  httpMetadata["min"] = 3000;
  httpMetadata["max"] = 4000;
  httpPort.setMetadata(httpMetadata);

  httpPort.setType(Types::PropertyTypes::INTEGER);
  mSettings.push_back(httpPort);

  maki::SettingField idePort;
  idePort.setKey("idePort");
  idePort.setLabel("IDE Port");
  idePort.setDescription("IDE command listen port");
  idePort.setDefaultValue(3001);
  idePort.setType(Types::PropertyTypes::INTEGER);
  mSettings.push_back(idePort);

  maki::SettingField viewPort;
  viewPort.setKey("viewPort");
  viewPort.setLabel("View Port");
  viewPort.setDescription("Browser view listen port");
  viewPort.setDefaultValue(3002);
  viewPort.setType(Types::PropertyTypes::INTEGER);
  mSettings.push_back(viewPort);

  maki::SettingField editorPort;
  editorPort.setKey("editorPort");
  editorPort.setLabel("Editor Port");
  editorPort.setDescription("Editor listen port");
  editorPort.setDefaultValue(3003);
  editorPort.setType(Types::PropertyTypes::INTEGER);
  mSettings.push_back(editorPort);

  maki::SettingField debug;
  debug.setKey("debug");
  debug.setLabel("Debug");
  debug.setDescription("Enable debug output");
  debug.setDefaultValue(false);
  debug.setType(Types::PropertyTypes::BOOLEAN);
  mSettings.push_back(debug);

  maki::SettingField verbose;
  verbose.setKey("verbose");
  verbose.setLabel("Verbose");
  verbose.setDescription("Be more verbose, show progress");
  verbose.setDefaultValue(false);
  verbose.setType(Types::PropertyTypes::BOOLEAN);
  mSettings.push_back(verbose);

  maki::SettingField taskOnly;
  taskOnly.setKey("taskOnly");
  taskOnly.setLabel("Task only");
  taskOnly.setDescription("Only verify the top level task");
  taskOnly.setDefaultValue(false);
  taskOnly.setType(Types::PropertyTypes::BOOLEAN);
  mSettings.push_back(taskOnly);

  maki::SettingField validateOnSave;
  validateOnSave.setKey("validateOnSave");
  validateOnSave.setLabel("Validate on save");
  validateOnSave.setDescription("Validate the system on every save");
  validateOnSave.setDefaultValue(false);
  validateOnSave.setType(Types::PropertyTypes::BOOLEAN);
  mSettings.push_back(validateOnSave);
}

void KodaGenerator::settingsChanged(const QVector<maki::SettingField>& settings)
{
  for (auto& setting : mSettings)
  {
    for (const auto& incoming : settings)
    {
      if (setting.getKey() != incoming.getKey())
        continue;

      setting.setValue(incoming.getValue());
    }
  }
}

void KodaGenerator::setHostServices(maki::IHostServices* services)
{
  mServices = services;

  if (auto logger = mServices->logger())
  {
    logging::gSourceName = languageName().toStdString();
    logging::gSilentLog = false;
    logger->registerPlugin(languageName(), logging::gLogToStream);
  }

  buildSettings();

  // Setup settings
  if (auto service = mServices->settings())
  {
    service->registerSettings(languageName(), version(), mSettings);

    auto settings = service->getPluginSettings(languageName());
    if (!settings.isEmpty())
      mSettings = settings;
  }

  if (auto pluginTab = mServices->ui())
    pluginTab->registerPlugin(languageName(), [this](QGraphicsScene* scene) {
      return createSimulationScene(scene, mLastUpdate);
    });
}

void KodaGenerator::setName(const QString& name)
{
  mName = name;
}

void KodaGenerator::setVersion(const QString& version)
{
  mVersion = maki::PluginVersion::fromString(version);
}

void KodaGenerator::setAssetDir(const QDir& dir)
{
  mAssetDir = dir;
}

QString KodaGenerator::languageName() const
{
  if (!mName.isEmpty())
    return mName;

#ifdef USE_ANTLR
  return "KODA_ANTLR";
#else
  return "KODA";
#endif
}

maki::PluginVersion KodaGenerator::version() const
{
  return mVersion;
}

VoidResult KodaGenerator::simulate(const maki::PipelineArtifact& artifact)
{
  LOG_INFO("Running simulation");

  if (mServices == nullptr)
    return VoidResult::Failed("Cannot proceed with simulation, no services provided");
  else if (mServices->ui() == nullptr)
    return VoidResult::Failed("Cannot proceed with simulation, no plugin tab provided");
  else if (mServices->document() == nullptr)
    return VoidResult::Failed("Cannot proceed with simulation, no document provided");
  else if (!setup())
    return VoidResult::Failed("Could not setup simulator");

  if (!artifact.paths.contains("modelDir"))
    return VoidResult::Failed("No model folder provided");
  if (!artifact.metadata.contains("sources"))
    return VoidResult::Failed("No sources provided");
  if (!artifact.metadata.contains("includes"))
    return VoidResult::Failed("No includes provided");

  const auto modelsFolder = artifact.paths["modelDir"].toString();
  const auto sourceFiles = artifact.metadata["sources"].toStringList();
  const auto includeFolders = artifact.metadata["includes"].toStringList();

  for (const QString& file : sourceFiles)
  {
    if (!file.contains("_task"))
      continue;

    LOG_INFO("Will simulate: %s", qPrintable(file));

    mSimulator->setWorkingDirectory(modelsFolder);
    mSimulator->setSimulationModel(file);
    mSimulator->setSimulationIncludes(includeFolders);
    return mSimulator->startSimulation(QUuid::createUuid().toString());
  }

  return VoidResult();
}

QList<std::shared_ptr<maki::IPipelineAction>> KodaGenerator::pipelineActions()
{
  return {
      std::make_shared<GenerateKodaAction>(this),
      std::make_shared<GenerateDezyneAction>(this),
      std::make_shared<GenerateCppAction>(this),
      std::make_shared<KodaVerifyAction>(this),
      std::make_shared<KodaSimulateAction>(this),
  };
}

Result<maki::PipelineArtifact> KodaGenerator::generateDezyne(const maki::PipelineArtifact& artifact, const QDir& outputFolder, maki::IPipeline* pipeline)
{
  if (!artifact.metadata.contains("sources"))
    return Result<maki::PipelineArtifact>::Failed("generateDezyne, missing input sources");

  const QStringList inputFiles = artifact.metadata["sources"].toStringList();
  if (!outputFolder.exists())
    outputFolder.mkpath(".");

  // Make sure the output is clean before the generation
  auto modelsOutputFolder = QDir(outputFolder.absolutePath() + "/models");
  if (modelsOutputFolder.exists())
    modelsOutputFolder.removeRecursively();

  modelsOutputFolder.mkpath(".");

#ifdef USE_ANTLR
  koda::Compiler compiler;
  for (const auto& file : inputFiles)
  {
    koda::CompilerOptions options;
    options.inputFile = file.toStdString();
    options.outputDir = modelsOutputFolder.absolutePath().toStdString();
    LOG_DEBUG("Generating from file: %s to %s", qPrintable(file), qPrintable(outputFolder.absolutePath()));
    auto parsed = compiler.parse(options);
    if (!parsed)
      return Result<maki::PipelineArtifact>::Failed(parsed.ErrorMessage());
    auto generated = compiler.generate();
    if (!generated)
      return Result<maki::PipelineArtifact>::Failed(generated.ErrorMessage());
  }

  QStringList includeFolders = {};
  QString libDstPath = modelsOutputFolder.absolutePath() + "/lib";
  if (mAssetDir)
  {
    LOG_DEBUG("Using asset dir: %s", qPrintable(mAssetDir->absolutePath()));
    QString libSrcPath = mAssetDir->absoluteFilePath("lib");
    auto copied = copyDirectory(libSrcPath, libDstPath);
    if (!copied.IsSuccess())
      return Result<maki::PipelineArtifact>::Failed(copied.ErrorMessage());

    includeFolders << libDstPath;
  }

  // Finally, set the files to be verified
  QStringList filters;
  filters << "*.dzn";

  QStringList outputFiles = {};
  for (const auto& file : modelsOutputFolder.entryList(filters, QDir::Files))
    outputFiles << modelsOutputFolder.absoluteFilePath(file);

  if (mAssetDir)
  {
    auto libDir = QDir(libDstPath);
    for (const auto& file : libDir.entryList(filters, QDir::Files))
      outputFiles << libDir.absoluteFilePath(file);
  }

  maki::PipelineArtifact output;
  output.paths = {
      {"modelDir", modelsOutputFolder.absolutePath()},
  };
  output.metadata = {
      {"sources", outputFiles},
      {"includes", includeFolders},
  };

  mDezyneOutputFolder = modelsOutputFolder;

  return output;
#else
  for (const auto& file : inputFiles)
  {
    LOG_DEBUG("Generating from file: %s to %s", qPrintable(file), qPrintable(outputFolder.absolutePath()));
    const QString command = "java";
    const QStringList arguments = {
        "-jar",
        QDir::homePath() + "/rascal-0.40.9.jar",
        "Main.rsc",                   // Entrypoint for KODA
        file,                         // Input
        outputFolder.absolutePath(),  // Output
        "ros"                         // Generator type
    };

    QProcess* generate = new QProcess(this);
    generate->setWorkingDirectory("/home/felaze/Documents/PhD/Programs/DSL/koda");
    generate->setProgram(command);
    generate->setArguments(arguments);

    pipeline->add(generate, maki::OnFail::STOP);
  }

  return maki::PipelineArtifact();
#endif
}

Result<maki::PipelineArtifact> KodaGenerator::generateCpp(const maki::PipelineArtifact& artifact, const QDir& outputFolder, maki::IPipeline* pipeline)
{
  if (!artifact.metadata.contains("sources"))
    return Result<maki::PipelineArtifact>::Failed("generateCpp, missing input sources");
  else if (!artifact.metadata.contains("includes"))
    return Result<maki::PipelineArtifact>::Failed("Missing input includes");

  const QStringList inputFiles = artifact.metadata["sources"].toStringList();
  const QStringList includeFolders = artifact.metadata["includes"].toStringList();
  if (!outputFolder.exists())
    outputFolder.mkpath(".");

  auto cppOutputFolder = QDir(outputFolder.absolutePath() + "/cpp");
  if (cppOutputFolder.exists())
    cppOutputFolder.removeRecursively();

  cppOutputFolder.mkpath(".");

  QStringList outputSourceFiles = {};
  QStringList outputHeaderFiles = {};

  pipeline->startGroup("CppGeneration");
  for (const auto& f : inputFiles)
  {
    auto fullPath = cppOutputFolder.absoluteFilePath(f);
    LOG_INFO("Will generate file: %s", qPrintable(fullPath));
    const QString command = "dzn";
    QStringList arguments = {
        "code",
        "-l",
        "c++",
        "-o",
        cppOutputFolder.absolutePath(),
        fullPath,
    };
    for (const auto& inc : includeFolders)
      arguments << "-I" << inc;

    QProcess* generate = new QProcess(this);
    generate->setProgram(command);
    generate->setArguments(arguments);

    QFileInfo info(fullPath);
    QString baseName = info.baseName();
    outputHeaderFiles << cppOutputFolder.absolutePath() + "/" + baseName + ".hh";
    outputSourceFiles << cppOutputFolder.absolutePath() + "/" + baseName + ".cc";

    pipeline->add(generate, maki::OnFail::STOP);
  }
  pipeline->endGroup();

  // Add dzn lib includes as well
  LOG_DEBUG("Using asset dir: %s", qPrintable(mAssetDir->absolutePath()));
  maki::PipelineArtifact output;
  output.paths = {
      {"includeDir", mAssetDir->absoluteFilePath("dzn_files/include")},
      {"sourceDir", {mAssetDir->absoluteFilePath("dzn_files/src")}},
  };
  output.metadata = {
      {"sources", outputSourceFiles},
      {"includes", outputHeaderFiles},
  };

  return output;
}

VoidResult KodaGenerator::verify(const maki::PipelineArtifact& artifact, const QDir& outputFolder, maki::IPipeline* pipeline)
{
  LOG_INFO("Running verification");

  if (!artifact.metadata.contains("sources"))
    return Result<maki::PipelineArtifact>::Failed("verify, missing input sources");
  else if (!artifact.metadata.contains("includes"))
    return Result<maki::PipelineArtifact>::Failed("verify, missing input includes");
  else if (mServices == nullptr)
    return VoidResult::Failed("Cannot proceed with verification, no services provided");
  else if (pipeline == nullptr)
    return VoidResult::Failed("Cannot proceed with verification, no pipeline provided");
  else if (mServices->document() == nullptr)
    return VoidResult::Failed("Cannot proceed with verification, no document provided");
  // else if (!setup())
  //   return VoidResult::Failed("Could not setup plugin");

  if (mServices->document()->getnodes().isEmpty())
    return VoidResult::Failed("Nothing to verify");

  const QStringList inputFiles = artifact.metadata["sources"].toStringList();
  const QStringList includeFolders = artifact.metadata["includes"].toStringList();

  auto taskOnly = getSetting("taskOnly");
  pipeline->startGroup("Verification");
  for (const QString& f : inputFiles)
  {
    if (f.contains("/a_") || f.contains("types"))
      continue;
    if (f.contains("arbiter") || f.contains("/lib/"))
      continue;
    if (taskOnly.getValue().isValid() && taskOnly.getValue().toBool() && !f.contains("_task"))
      continue;

    LOG_DEBUG("Will verify file: %s", qPrintable(f));
#ifdef USE_ANTLR
    const QString command = "ide";
#else
    const QString command = "dzn";
#endif
    QStringList arguments = {"verify", f};
    for (const auto& inc : includeFolders)
      arguments << "-I" << inc;

    QProcess* generate = new QProcess(this);
    generate->setProgram(command);
    generate->setArguments(arguments);

    pipeline->add(generate, maki::OnFail::STOP);
  }
  pipeline->endGroup();

  return VoidResult();
}

Result<maki::PipelineArtifact> KodaGenerator::generateKoda(const maki::PipelineArtifact& artifact, const QDir& outputFolder)
{
  mGeneratedFiles.clear();
  mOutputFolder = outputFolder;
  if (!mOutputFolder.exists())
    mOutputFolder.mkdir(".");

  LOG_DEBUG("Generating Koda files with %d nodes", mServices->document()->getnodes().size());
  QString code = "";

  for (const auto& node : mServices->document()->getnodes())
  {
    if (node->getnodeId() != "Koda::Task")
      continue;

    LOG_DEBUG("Generating code for top level node %s %s %d", qPrintable(node->getproperties()["name"].toString()), qPrintable(node->getnodeId()), node->getchildren().size());

    QString args = "";
    for (const auto& child : node->getchildren())
    {
      auto capabilityId = child->getproperties()["name"].toString();
      LOG_DEBUG("Generating code for capability %s %s %d", qPrintable(capabilityId), qPrintable(child->getnodeId()), child->getchildren().size());
      auto generated = generateCapability(*child);
      if (!generated)
        return Result<maki::PipelineArtifact>::Failed(generated.ErrorMessage());

      code += generated.Value();
      args += fixCase(capabilityId) + " req " + capabilityId + ", ";
    }

    auto generated = generateComponent(*node, code, args);
    if (!generated)
      return Result<maki::PipelineArtifact>::Failed(generated.ErrorMessage());
  }

  maki::PipelineArtifact output;
  output.metadata = {
      {"sources", mGeneratedFiles},
  };

  return output;
}

Result<QString> KodaGenerator::generateBehaviourNode(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  QString type = node.getnodeId();
  QString name = node.getproperties()["name"].toString();

  // LOG_DEBUG("Generating code for %s with %s", qPrintable(type), qPrintable(arg.name));

  if (type == "Koda::Success")
    return generateSuccess(node, arg, flow, format);
  else if (type == "Koda::Error")
    return generateError(node, arg, flow, format);
  else if (type == "Koda::Continue")
    return generateContinue(node, arg, flow, format);
  else if (type == "Koda::Async task")
    return generateAsyncTask(node, arg, flow, format);
  else if (type == "Koda::Sync task")
    return generateSyncTask(node, arg, flow, format);
  else if (type == "Koda::Strategy")
    return generateStrategy(node, arg, flow, format);
  else if (type == "Koda::Within")
    return generateWithin(node, arg, flow, format);
  else if (type == "Koda::Repeat")
    return generateRepeat(node, arg, flow, format);
  else if (type == "Koda::Every")
    return generateEvery(node, arg, flow, format);

  return code;
}

Result<QString> KodaGenerator::generateTransitions(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";

  for (const auto& transition : flow.gettransitions(node.getid()))
  {
    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
      APPEND_OR_RETURN_ON_FAILURE(code, generateBehaviourNode(*dst, arg, flow, format));
  }

  return code;
}

Result<QString> KodaGenerator::generateCapability(const INode& node)
{
  QString code = "";

  QString args = "";
  if (node.getproperties().contains("arguments"))
  {
    auto list = node.getproperties()["arguments"].toList();
    for (const auto& l : list)
    {
      auto l0 = l.toMap();
      args += l0["type"].toString() + " " + l0["id"].toString() + ", ";
    }
  }

  QString name = node.getproperties()["name"].toString();

  // mGeneratedDznFiles += QString::fromStdString("c" + ToLowerCase(name.toStdString(), 0, 1) + ".dzn");
  mGeneratedDznFiles += QString::fromStdString("i" + ToLowerCase(name.toStdString(), 0, name.size() - 1) + ".dzn");

  args.chop(2);
  code += "capability " + name + "(" + args + ") {\n";

  auto typeArray = node.getproperties()["type"].toJsonObject()["options"].toArray();
  if (typeArray.isEmpty())
    return Result<QString>::Failed("Type options is empty: " + name.toStdString());

  for (int i = 0; i < typeArray.size(); ++i)
  {
    const auto item = typeArray.at(i).toObject();
    auto callType = item["id"].toString();
    auto callOptions = item["options"].toArray();
    if (callOptions.size() < 2)
      return Result<QString>::Failed("Capability does not follow the expected format: " + name.toStdString());

    auto callRoute = callOptions.at(0).toObject()["default"].toString();
    auto callMessage = callOptions.at(1).toObject()["default"].toString();
    code += std::format("  {} \"{}\" \"{}\" {{\n", callType.toStdString(), callRoute.toStdString(), callMessage.toStdString());

    QString qualifier = "  ";
    for (const auto& f : node.getevents())
    {
      // LOG_DEBUG("%s match: %d %d", qPrintable(f->getname()), f->getlinksTo(), i);
      if (f->getlinksTo() != i)
        continue;

      QString args = "";
      for (const auto& arg : f->getarguments())
        args += Types::PropertyTypesToString(arg->gettype()) + " " + arg->getid() + ", ";

      args.chop(2);
      if (f->gettype() == Types::CallType::TRIGGER)
        qualifier = "    trigger: ";
      else if (f->gettype() == Types::CallType::ABORT)
        qualifier = "    abort: ";
      else if (f->gettype() == Types::CallType::IN)
        qualifier = "    in: ";
      else if (f->gettype() == Types::CallType::RETURN)
        qualifier = "    return: ";
      else if (f->gettype() == Types::CallType::ERROR)
        qualifier = "    error: ";
      else if (f->gettype() == Types::CallType::OUT)
        qualifier = "    out: ";

      code += qualifier + Types::PropertyTypesToString(f->getreturnType()) + " " + f->getname() + "(" + args + ");\n";
    }

    code += "  }\n";
  }

  code += "}\n";

  return code;
}

VoidResult KodaGenerator::generateComponent(const INode& node, const QString& incomingCode, const QString& arguments)
{
  QString code = "";

  // Generate necessary wrappers
  QString name = fixCase(node.getproperties()["name"].toString());
  mGeneratedDznFiles += QString::fromStdString("c" + ToLowerCase(name.toStdString(), 0, name.size() - 1) + ".dzn");
  mGeneratedDznFiles += QString::fromStdString("c" + ToLowerCase(name.toStdString(), 0, name.size() - 1) + "_task.dzn");

  // Create a file for each top level component
  QString fileName = mOutputFolder.filePath(name + ".kd");
  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    return VoidResult::Failed("Failed to open device for writing: " + fileName.toStdString());

  mGeneratedFiles.append(fileName);

  // Generate flows
  QString bodyCode = "";
  for (const auto& f : node.getevents())
  {
    if (!f->getnodes().empty())
      continue;

    LOG_DEBUG("Generating flow %s", qPrintable(f->getname()));

    QString args = "";
    for (auto& arg : f->getarguments())
      args += Types::PropertyTypesToString(arg->gettype()) + " " + arg->getid() + ", ";

    args.chop(2);
    QString qualifier = "";
    if (f->gettype() == Types::CallType::TRIGGER)
      qualifier = QStringLiteral("  trigger:");
    else if (f->gettype() == Types::CallType::ABORT)
      qualifier = QStringLiteral("  abort:");
    else if (f->gettype() == Types::CallType::RETURN)
      qualifier = QStringLiteral("  return:");
    else if (f->gettype() == Types::CallType::ERROR)
      qualifier = QStringLiteral("  error:");

    bodyCode += qualifier + "  " + Types::PropertyTypesToString(f->getreturnType()) + " " + f->getname() + "(" + args + ");\n";
  }

  for (const auto& f : node.getflows())
  {
    if (f->getnodes().empty())
      continue;

    LOG_DEBUG("Generating flow %s", qPrintable(f->getname()));
    // Find the start node
    for (const auto& n : f->getnodes())
    {
      if (n->getnodeId() != "Koda::Start")
        continue;

      auto generated = generateStart(node.getproperties()["name"].toString(), *n, *f, "    ");
      if (!generated.IsSuccess())
        return VoidResult::Failed(generated.ErrorMessage());

      code += generated.Value();
      break;
    }
  }

  QTextStream out(&file);
  for (const auto& imp : mImports)
    out << "import " + imp + ";\n";

  if (!mImports.isEmpty())
    out << "\n";

  if (!node.getfields().empty())
    bodyCode += "  vars {\n";

  QString args = arguments;
  for (const auto& state : node.getfields())
  {
    bodyCode += "    " + Types::PropertyTypesToString(state->gettype()) + " " + state->getid() + "_ = " + state->getid() + " : " + state->getdefaultValue().toString() + "\n";
    args += Types::PropertyTypesToString(state->gettype()) + " " + state->getid() + ", ";
  }

  if (!node.getfields().empty())
    bodyCode += "  }\n";

  out << incomingCode << "\n";

  // Remove trailing comma + space
  args.chop(2);

  out << "task " + name + "(" + args + ")\n";
  out << "{\n";

  // TODO(felaze): I really need to take some day to clean this and the widget codes.
  // - More widgets, maybe even one per property type
  // - The generation should use more inheritance, a lot of the code here is repeated
  // for (const auto& child : node.children)
  // {
  //   auto childName = fixCase(child->getproperties["name"].toString());
  //   out << "  requires i" + childName + " " + childName + ";\n";
  // }

  out << bodyCode << "\n";
  out << "  strategy {\n"
      << code << "  }\n";
  out << "}\n";

  file.close();

  return VoidResult();
}

Result<QString> KodaGenerator::generateStart(const QString& parent, const INode& node, const IFlow& flow, const QString& format)
{
  QString args = "";
  for (auto& arg : flow.getarguments())
    args += arg->getid() + ", ";
  args.chop(2);

  auto strategy = flow.getname();
  if (strategy != "main")
    strategy = "f" + strategy;

  QString code = "    " + strategy + (args.isEmpty() ? "" : "[" + args + "]") + ": ";

  Argument arg;
  APPEND_OR_RETURN_ON_FAILURE(code, generateTransitions(node, arg, flow, "  " + format));
  code += ";\n";

  return code;
}

Result<QString> KodaGenerator::generateSuccess(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  code += "end";
  return code;
}

Result<QString> KodaGenerator::generateContinue(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  code += "continue";
  return code;
}

Result<QString> KodaGenerator::generateError(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  if (arg.name.isEmpty())
    return code;

  code += format + "reply(" + arg.name + ");\n";
  return code;
}

QString KodaGenerator::createArguments(const QJsonArray& options) const
{
  QString args = "";

  for (int i = 1; i < options.size(); ++i)
  {
    const auto arg = options.at(i).toObject();
    const auto isVariable = arg["is_variable"].toBool();

    // TODO(felaze): An underscore should only be added if we are refering to a state variable
    args += QStringLiteral("%1%2, ").arg(arg["data"].toString(), isVariable ? "_" : "");
  }

  args.chop(2);
  return args;
}

Result<QString> KodaGenerator::generateAsyncTask(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  QJsonObject object = node.getproperties()["capability"].toJsonObject();
  QString val = object["data"].toString();
  if (val.isEmpty())
    return Result<QString>::Failed("AsyncTask component does not have a valid capability");

  // TODO: Check validity
  // - Arguments match the expectation
  // - Transitions are valid
  QJsonArray options = object["options"].toArray();
  QString args = createArguments(options);

  auto fixed = QString::fromStdString(ToLowerCase(val.toStdString(), 0, val.size() - 1));
  code += "(" + fixed + "(" + args + ")";
  for (const auto& transition : flow.gettransitions(node.getid()))
  {
    if (transition->getlabel() != "on error")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += " on error (";
      APPEND_OR_RETURN_ON_FAILURE(code, generateBehaviourNode(*dst, arg, flow, format));
      code += ")";
    }
  }

  for (const auto& transition : flow.gettransitions(node.getid()))
  {
    if (transition->getlabel() != "on abort")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += " on abort (";
      APPEND_OR_RETURN_ON_FAILURE(code, generateBehaviourNode(*dst, arg, flow, format));
      code += ")";
    }
  }

  for (const auto& transition : flow.gettransitions(node.getid()))
  {
    if (transition->getlabel() != "on")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += " on " + QString::fromStdString(ToLowerCase(transition->getevent().toStdString(), 0, transition->getevent().size() - 1)) + "() (";
      APPEND_OR_RETURN_ON_FAILURE(code, generateBehaviourNode(*dst, arg, flow, format));
      code += ")";
    }
  }

  bool hasOutTransitions = false;
  for (const auto& transition : flow.gettransitions(node.getid()))
  {
    if (transition->getlabel() != "")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      hasOutTransitions = true;
      code += ") --> ";
      APPEND_OR_RETURN_ON_FAILURE(code, generateBehaviourNode(*dst, arg, flow, format));
    }
  }

  if (!hasOutTransitions)
    code += ")";

  return code;
}

Result<QString> KodaGenerator::generateSyncTask(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  QJsonObject object = node.getproperties()["capability"].toJsonObject();
  QString val = object["data"].toString();
  if (val.isEmpty())
    return Result<QString>::Failed("SyncTask component does not have a valid capability");

  QJsonArray options = object["options"].toArray();
  if (val.isEmpty())
    return Result<QString>::Failed("SyncTask component is missing an associated call");

  // TODO: Check validity
  // - Arguments match the expectation
  // - Transitions are valid
  QString method = "";
  QString args = "";
  for (uint32_t i = 0; i < options.size(); ++i)
  {
    QJsonObject obj = options.at(i).toObject();
    if (i == 0)
      method = obj["data"].toString();
    else
      args += obj["data"].toString() + ", ";
  }
  args.chop(2);

  auto fixed = QString::fromStdString(ToLowerCase(val.toStdString(), 0, val.size() - 1));
  code += fixed + "." + method + "(" + args + ")";

  if (flow.gettransitions(node.getid()).size() > 0)
    code += " --> ";

  APPEND_OR_RETURN_ON_FAILURE(code, generateTransitions(node, arg, flow, format + "  "));

  return code;
}

Result<QString> KodaGenerator::generateWithin(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  int val = node.getproperties()["timeout"].toInt();

  code += "(within " + QString::number(val) + " do (";

  for (const auto& transition : flow.gettransitions(node.getid()))
  {
    LOG_INFO("Label: %s", qPrintable(transition->getlabel()));
    if (transition->getlabel() == "do")
    {
      auto dst = findDestination(transition->getdstId(), flow);
      if (dst != nullptr)
        APPEND_OR_RETURN_ON_FAILURE(code, generateBehaviourNode(*dst, arg, flow, format));
      break;
    }
  }

  code += ") else (";

  for (const auto& transition : flow.gettransitions(node.getid()))
  {
    if (transition->getlabel() == "else")
    {
      auto dst = findDestination(transition->getdstId(), flow);
      if (dst != nullptr)
        APPEND_OR_RETURN_ON_FAILURE(code, generateBehaviourNode(*dst, arg, flow, format));
      break;
    }
  }

  code += ")";

  for (const auto& transition : flow.gettransitions(node.getid()))
  {
    if (transition->getlabel() != "on error")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += "  on error (";
      APPEND_OR_RETURN_ON_FAILURE(code, generateBehaviourNode(*dst, arg, flow, format));
      code += ")";
    }
  }

  for (const auto& transition : flow.gettransitions(node.getid()))
  {
    if (transition->getlabel() != "on abort")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += "  on abort (";
      APPEND_OR_RETURN_ON_FAILURE(code, generateBehaviourNode(*dst, arg, flow, format));
      code += ")";
    }
  }

  for (const auto& transition : flow.gettransitions(node.getid()))
  {
    if (transition->getlabel() != "on")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += "  on " + QString::fromStdString(ToLowerCase(transition->getevent().toStdString(), 0, transition->getevent().size() - 1)) + "() (";
      APPEND_OR_RETURN_ON_FAILURE(code, generateBehaviourNode(*dst, arg, flow, format));
      code += ")";
    }
  }

  code += ")";

  for (const auto& transition : flow.gettransitions(node.getid()))
  {
    if (transition->getlabel() != "")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += " --> ";
      APPEND_OR_RETURN_ON_FAILURE(code, generateBehaviourNode(*dst, arg, flow, format));
    }
  }

  return code;
}

Result<QString> KodaGenerator::generateEvery(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  int val = node.getproperties()["rate"].toInt();

  code += "(every " + QString::number(val) + " { ";

  for (const auto& transition : flow.gettransitions(node.getid()))
  {
    if (transition->getlabel() != "")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
      APPEND_OR_RETURN_ON_FAILURE(code, generateBehaviourNode(*dst, arg, flow, format));

    break;
  }

  code += " }";

  for (const auto& transition : flow.gettransitions(node.getid()))
  {
    if (transition->getlabel() != "on error")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += "\n      on error (";
      APPEND_OR_RETURN_ON_FAILURE(code, generateBehaviourNode(*dst, arg, flow, format));
      code += ")";
    }
  }

  for (const auto& transition : flow.gettransitions(node.getid()))
  {
    if (transition->getlabel() != "on abort")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += "\n      on abort (";
      APPEND_OR_RETURN_ON_FAILURE(code, generateBehaviourNode(*dst, arg, flow, format));
      code += ")";
    }
  }

  for (const auto& transition : flow.gettransitions(node.getid()))
  {
    if (transition->getlabel() != "on")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += "\n      on " + QString::fromStdString(ToLowerCase(transition->getevent().toStdString(), 0, transition->getevent().size() - 1)) + "() (";
      APPEND_OR_RETURN_ON_FAILURE(code, generateBehaviourNode(*dst, arg, flow, format));
      code += ")";
    }
  }

  code += "\n    )";

  return code;
}

Result<QString> KodaGenerator::generateRepeat(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  QJsonObject object = node.getproperties()["strategy"].toJsonObject();
  QString val = object["data"].toString();
  QJsonArray options = object["options"].toArray();
  if (options.isEmpty())
    return Result<QString>::Failed("Repeat component does not have an associated flow");

  QString strategy = options[0].toObject()["data"].toString();
  if (strategy != "main")
    strategy = "f" + strategy;

  code += "repeat(" + strategy + ")";

  return code;
}

Result<QString> KodaGenerator::generateStrategy(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  QJsonObject object = node.getproperties()["strategy"].toJsonObject();
  QString val = object["data"].toString();
  QJsonArray options = object["options"].toArray();
  if (options.isEmpty())
    return Result<QString>::Failed("Strategy component does not have an associated flow");

  QString strategy = options[0].toObject()["data"].toString();

  QString args = "";
  for (uint32_t i = 1; i < options.size(); ++i)
  {
    QJsonObject obj = options.at(i).toObject();
    args += obj["data"].toString() + ", ";
  }
  args.chop(2);

  if (strategy != "main")
    strategy = "f" + strategy;

  code += strategy + (args.isEmpty() ? "" : "(" + args + ")");
  if (flow.gettransitions(node.getid()).size() > 0)
    code += " --> ";

  APPEND_OR_RETURN_ON_FAILURE(code, generateTransitions(node, arg, flow, format + "  "));

  return code;
}

std::shared_ptr<INode> KodaGenerator::findDestination(const QString& nodeId, const IFlow& flow) const
{
  for (const auto& dst : flow.getnodes())
  {
    if (dst->getid() == nodeId)
      return dst;
  }

  LOG_DEBUG("Could not find destination with id: %s", qPrintable(nodeId));
  return nullptr;
}

QString KodaGenerator::fixCase(const QString& name)
{
  return name.toLower().replace(" ", "_");
}

bool KodaGenerator::startDaemon()
{
  if (mDaemon)
    return true;

  mDaemon = new QProcess(this);

  connect(mDaemon, &QProcess::started, this, []() {
    LOG_DEBUG("Process started");
  });

  connect(mDaemon, &QProcess::readyReadStandardOutput, this, [this]() {
    LOG_DEBUG(mDaemon->readAllStandardOutput().trimmed().toStdString());
  });

  connect(mDaemon, &QProcess::readyReadStandardError, this, [this]() {
    LOG_DEBUG(mDaemon->readAllStandardError().trimmed().toStdString());
  });

  connect(mDaemon, &QProcess::finished, this, [](int exitCode, QProcess::ExitStatus status) {
    LOG_DEBUG("Daemon finished with code %d and status %d", exitCode, (int)status);
  });

  // connect(mDaemon, &QProcess::errorOccurred, this, [](QProcess::ProcessError e) {
  //   LOG_WARNING("Daemon error: %d", (int)e);
  // });

  // Non-blocking start
  QStringList arguments = {"daemon"};
  auto debug = getSetting("debug");
  if (debug.getValue().isValid() && debug.getValue().toBool())
    arguments.push_back("--debug");

  mDaemon->setProgram("ide");
  mDaemon->setArguments(arguments);
  mDaemon->start();

  return mDaemon->waitForStarted();
}

void KodaGenerator::simulationStarted()
{
  if (!mServices)
    return;

  if (auto pluginTab = mServices->ui())
  {
    LOG_INFO("Simulation started");
    pluginTab->openScene(languageName());
  }
}

maki::SettingField KodaGenerator::getSetting(const QString& key) const
{
  for (const auto& s : mSettings)
    if (s.getKey() == key)
      return s;

  return maki::SettingField();
}

void KodaGenerator::simulationUpdated(const QJsonObject& obj)
{
  LOG_DEBUG("Simulation updated");
  mLastUpdate = obj;
  mServices->ui()->updateScene(languageName());
}

VoidResult KodaGenerator::createSimulationScene(QGraphicsScene* scene, const QJsonObject& obj)
{
  // Nothing to render
  if (obj.isEmpty())
    return VoidResult();

  // auto pretty = QJsonDocument(obj).toJson(QJsonDocument::Indented);
  // LOG_DEBUG("Received message: %s", qPrintable(pretty));

  auto theme = mServices->ui()->currentTheme();
  if (!mTraceBuilder)
    mTraceBuilder = std::make_unique<TraceSceneBuilder>(theme, TraceSceneBuilder::Style{});

  auto clickHandler = [this](const QString& instance, const QString& labelText, bool illegal) {
    LOG_DEBUG("Sending data: %s %s", qPrintable(instance), qPrintable(labelText));
    mSimulator->triggerEvent(labelText);
  };

  QString err;
  if (!mTraceBuilder->buildScene(obj, scene, clickHandler, &err))
    return VoidResult::Failed("Failed to build scene: " + err.toStdString());

  return VoidResult();
}
