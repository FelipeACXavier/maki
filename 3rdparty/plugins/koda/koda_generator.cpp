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

#include "dezyne_simulator.h"
#include "idocument.h"
#include "ilogging.h"
#include "ipipeline.h"
#include "isettings.h"
#include "itab.h"
#include "keys.h"
#include "logging.h"
#include "simulation_scene.h"
#include "string_helpers.h"
#include "style_helpers.h"
#include "types.h"

#ifdef USE_ANTLR
#include "ast/koda_compiler.h"
#endif

#define ADD_VARIABLE_OR_VALUE(array, arg, isVariable)                                               \
  do                                                                                                \
  {                                                                                                 \
    if (isVariable)                                                                                 \
      args += QStringLiteral("%1%2, ").arg(arg[ConfigKeys::DATA].toString(), isVariable ? "_", ""); \
    else                                                                                            \
      args += QStringLiteral("%1, ").arg(arg[ConfigKeys::DATA].toString());                         \
  } while (false);

// TODO: Use std::filesystem instead
bool copyDirectory(const QString& sourceDir, const QString& targetDir)
{
  QDir src(sourceDir);
  if (!src.exists())
  {
    LOG_ERROR("Source folder does not exist %s", qPrintable(sourceDir));
    return false;
  }

  if (!QDir().mkpath(targetDir))
  {
    LOG_ERROR("Could not create destination dir %s", qPrintable(targetDir));
    return false;
  }

  QDirIterator it(sourceDir, QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
  while (it.hasNext())
  {
    it.next();
    QFileInfo info = it.fileInfo();

    QString relativePath = src.relativeFilePath(info.absoluteFilePath());
    QString outPath = QDir(targetDir).filePath(relativePath);

    if (info.isDir())
    {
      if (!QDir().mkpath(outPath))
      {
        LOG_ERROR("Could not create sub-dir %s", qPrintable(outPath));
        return false;
      }
    }
    else if (info.isFile())
    {
      QDir().mkpath(QFileInfo(outPath).absolutePath());
      QFile::remove(outPath);  // because QFile::copy won't overwrite
      if (!QFile::copy(info.absoluteFilePath(), outPath))
      {
        LOG_ERROR("Could not copy file %s to %s", qPrintable(info.absoluteFilePath()), qPrintable(outPath));
        return false;
      }
    }
  }

  return true;
}

bool KodaGenerator::setup()
{
  mSimulator = new DezyneSimulator(this);

  connect(mSimulator, &DezyneSimulator::simulationStarted, this, &KodaGenerator::simulationStarted);
  connect(mSimulator, &DezyneSimulator::simulationUpdated, this, &KodaGenerator::simulationUpdated);

  // LOG_ERROR_ON_FAILURE(mSimulator->startSimulation(QUuid::createUuid().toString()));

  // Start the ide daemon on a specific port
  return true;  // startDaemon();
}

bool KodaGenerator::tearDown()
{
  if (mDaemon == nullptr)
    return true;

  if (mDaemon->state() == QProcess::Running)
  {
    mDaemon->terminate();

    if (!mDaemon->waitForFinished(2500))
    {
      mDaemon->kill();
      mDaemon->waitForFinished();
    }
  }

  mDaemon->deleteLater();
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
}

void KodaGenerator::setHostServices(maki::IHostServices* services)
{
  mServices = services;

  buildSettings();

  // Setup settings
  if (auto service = mServices->settings())
    service->registerSettings(languageName(), version(), mSettings);

  if (auto pluginTab = mServices->pluginTab())
    pluginTab->registerPlugin(languageName(), [this](QGraphicsScene* scene) {
      return createSimulationScene(scene, mLastUpdate);
    });

  if (auto logger = mServices->logger())
    logger->registerPlugin(languageName(), logging::gLogToStream);
}

QString KodaGenerator::languageName() const
{
#ifdef USE_ANTLR
  return "KODA_ANTLR";
#else
  return "KODA";
#endif
}

maki::PluginVersion KodaGenerator::version() const
{
  return {"0", "0", "1"};
}

VoidResult KodaGenerator::simulate(const QString& outputFolder)
{
  LOG_INFO("Running simulation");

  if (mServices == nullptr)
    return VoidResult::Failed("Cannot proceed with simulation, no services provided");
  else if (mServices->pipeline() == nullptr)
    return VoidResult::Failed("Cannot proceed with simulation, no pipeline provided");
  else if (mServices->pluginTab() == nullptr)
    return VoidResult::Failed("Cannot proceed with simulation, no plugin tab provided");
  else if (mServices->document() == nullptr)
    return VoidResult::Failed("Cannot proceed with simulation, no document provided");

  if (mGeneratedDznFiles.empty())
  {
    RETURN_ON_FAILURE(verify(outputFolder));

    // Start simulation after verification is done
    const QString command = "echo";
    const QStringList arguments = {"\"Running sim\""};
    QProcess* generate = new QProcess(this);
    generate->setProgram(command);
    generate->setArguments(arguments);

    mServices->pipeline()->add(generate, maki::OnFail::EXECUTE, [this]() { startSimulation(); });
  }
  else
  {
    startSimulation();
  }

  return VoidResult();
}

void KodaGenerator::startSimulation()
{
  for (const QString& f : mGeneratedDznFiles)
  {
    auto fullPath = mDezyneOutputFolder.absoluteFilePath(f);
    if (!fullPath.contains("_task"))
      continue;

    LOG_INFO("Will simulate: %s", qPrintable(fullPath));

    mSimulator->setWorkingDirectory(mDezyneOutputFolder.absolutePath());
    mSimulator->setSimulationModel(fullPath);

    QList<QString> includes = {mDezyneOutputFolder.absolutePath()};
    mSimulator->setSimulationIncludes(includes);

    mSimulator->startSimulation(QUuid::createUuid().toString());
    mServices->pluginTab()->openScene(languageName());
  }
}

VoidResult KodaGenerator::verify(const QString& outputFolder)
{
  LOG_INFO("Running verification");

  if (mServices == nullptr)
    return VoidResult::Failed("Cannot proceed with verification, no services provided");
  else if (mServices->pipeline() == nullptr)
    return VoidResult::Failed("Cannot proceed with verification, no pipeline provided");
  else if (mServices->document() == nullptr)
    return VoidResult::Failed("Cannot proceed with verification, no document provided");

  mGeneratedDznFiles = {};
  mGeneratedFiles = {};

  // Create output folder
  mOutputFolder = QDir(outputFolder + "/" + languageName());
  if (!mOutputFolder.exists())
    mOutputFolder.mkpath(".");

  // Clear the errors before generation
  mErrors = {};

  // Generate Koda from the model
  generateKoda(outputFolder);

  if (!mErrors.isEmpty())
  {
    for (const auto& error : mErrors)
      LOG_WARNING("%s", qPrintable(error.message));

    return VoidResult::Failed("Failed to generate KODA file");
  }

  // Compile Koda to Dezyne
  mDezyneOutputFolder = QDir(mOutputFolder.absolutePath() + "/models");
  if (!mDezyneOutputFolder.exists())
    mDezyneOutputFolder.mkpath(".");

  QStringList includeFolders = {};

  LOG_INFO("Compiling %s to Dezyne: %s %d", qPrintable(languageName()), qPrintable(mDezyneOutputFolder.absolutePath()), mGeneratedFiles.size());
#ifdef USE_ANTLR
  // First, we compile the program
  koda::Compiler compiler;
  for (const auto& file : mGeneratedFiles)
  {
    koda::CompilerOptions options;
    options.inputFile = file.toStdString();
    options.outputDir = mDezyneOutputFolder.absolutePath().toStdString();
    RETURN_ON_FAILURE(compiler.parse(options));
    RETURN_ON_FAILURE(compiler.generate());
  }

  // Then we make sure the dezyne libraries are available
  QString libSrcPath = getDirPathFor("share/plugins/" + languageName() + "/lib");
  QString libDstPath = mDezyneOutputFolder.absolutePath() + "/lib";
  if (!copyDirectory(libSrcPath, libDstPath))
    return VoidResult::Failed("Could not copy dezyne libraries to output folder");

  // Make sure the correct file are included
  includeFolders << mDezyneOutputFolder.absolutePath();
  includeFolders << libDstPath;

  // Finally, set the files to be verified
  QStringList filters;
  filters << "*.dzn";
  mGeneratedDznFiles = mDezyneOutputFolder.entryList(filters, QDir::Files);
#else
  for (const auto& file : mGeneratedFiles)
  {
    LOG_DEBUG("Will generate Dezyne from KODA file: %s", qPrintable(file));
    const QString command = "java";
    const QStringList arguments = {
        "-jar",
        QDir::homePath() + "/rascal-0.40.9.jar",
        "Main.rsc",                    // Entrypoint for KODA
        file,                          // Input
        mOutputFolder.absolutePath(),  // Output
        "ros"                          // Generator type
    };

    QProcess* generate = new QProcess(this);
    generate->setWorkingDirectory("/home/felaze/Documents/PhD/Programs/DSL/koda");
    generate->setProgram(command);
    generate->setArguments(arguments);

    mServices->pipeline()->add(generate, maki::OnFail::STOP);
  }
#endif

  // TODO: We need to find a better solution for this, we shouldn't allow plugins to execute any scripts
  for (const QString& f : mGeneratedDznFiles)
  {
    auto fullPath = mDezyneOutputFolder.absoluteFilePath(f);
    if (fullPath.contains("/a_") || fullPath.contains("types"))
      continue;

    LOG_INFO("Will verify file: %s", qPrintable(fullPath));

    const QString command = "dzn";
    QStringList arguments = {"verify", fullPath};
    for (const auto& inc : includeFolders)
      arguments << "-I" << inc;

    QProcess* generate = new QProcess(this);
    generate->setProgram(command);
    generate->setArguments(arguments);

    mServices->pipeline()->add(generate, maki::OnFail::STOP);
  }

  return VoidResult();
}

QString KodaGenerator::generateKoda(const QString& outputFolder)
{
  mGeneratedFiles.clear();

  LOG_DEBUG("Generating Koda files with %d nodes", mServices->document()->getnodes().size());
  QString code = "";

  for (const auto& node : mServices->document()->getnodes())
  {
    if (node->getnodeId() != "Koda::Task")
      continue;

    // TODO(felaze): Create file at this level
    LOG_DEBUG("Generating code for top level node %s %s %d", qPrintable(node->getproperties()[ConfigKeys::NAME].toString()), qPrintable(node->getnodeId()), node->getchildren().size());

    QString args = "";
    for (const auto& child : node->getchildren())
    {
      auto capabilityId = child->getproperties()[ConfigKeys::NAME].toString();
      LOG_DEBUG("Generating code for capability %s %s %d", qPrintable(capabilityId), qPrintable(child->getnodeId()), child->getchildren().size());
      code += generateCapability(*child);
      args += fixCase(capabilityId) + " req " + capabilityId + ", ";
    }

    code = generateComponent(*node, code, args);
  }

  code.chop(1);

  return code;
}

QList<QString> KodaGenerator::generatedFiles() const
{
  return mGeneratedFiles;
}

// Add function per block type
QString KodaGenerator::generateNode(const INode& node)
{
  QString code = "";
  QString type = node.getnodeId();
  QString name = node.getproperties()[ConfigKeys::NAME].toString();

  LOG_DEBUG("Generating code for %s", qPrintable(type));

  if (type == "Generic::Component")
    code += generateComponent(node, code, "");

  return code;
}

QString KodaGenerator::generateBehaviourNode(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  QString type = node.getnodeId();
  QString name = node.getproperties()[ConfigKeys::NAME].toString();

  // LOG_DEBUG("Generating code for %s with %s", qPrintable(type), qPrintable(arg.name));

  if (type == "Koda::Success")
    code += generateSuccess(node, arg, flow, format);
  else if (type == "Koda::Error")
    code += generateError(node, arg, flow, format);
  else if (type == "Koda::Continue")
    code += generateContinue(node, arg, flow, format);
  else if (type == "Koda::Async task")
    code += generateAsyncTask(node, arg, flow, format);
  else if (type == "Koda::Sync task")
    code += generateSyncTask(node, arg, flow, format);
  else if (type == "Koda::Strategy")
    code += generateStrategy(node, arg, flow, format);
  else if (type == "Koda::Within")
    code += generateWithin(node, arg, flow, format);
  else if (type == "Koda::Repeat")
    code += generateRepeat(node, arg, flow, format);
  else if (type == "Koda::Every")
    code += generateEvery(node, arg, flow, format);

  return code;
}

QString KodaGenerator::generateTransitions(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";

  for (const auto& transition : node.gettransitions())
  {
    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
      code += generateBehaviourNode(*dst, arg, flow, format);
  }

  return code;
}

QString KodaGenerator::generateCapability(const INode& node)
{
  QString code = "";

  // qDebug() << node.properties;
  QString args = "";
  if (node.getproperties().contains(ConfigKeys::ARGUMENTS))
  {
    auto list = node.getproperties()[ConfigKeys::ARGUMENTS].toList();
    for (const auto& l : list)
    {
      auto l0 = l.toMap();
      args += l0[ConfigKeys::TYPE].toString() + " " + l0[ConfigKeys::ID].toString() + ", ";
    }
  }

  QString name = node.getproperties()[ConfigKeys::NAME].toString();

  // mGeneratedDznFiles += QString::fromStdString("c" + ToLowerCase(name.toStdString(), 0, 1) + ".dzn");
  mGeneratedDznFiles += QString::fromStdString("i" + ToLowerCase(name.toStdString(), 0, name.size() - 1) + ".dzn");

  args.chop(2);
  code += "capability " + name + "(" + args + ") {\n";

  auto typeArray = node.getproperties()[ConfigKeys::TYPE].toJsonObject()[ConfigKeys::OPTIONS].toArray();
  for (int i = 0; i < typeArray.size(); ++i)
  {
    const auto item = typeArray.at(i).toObject();
    auto callType = item[ConfigKeys::ID].toString();
    auto callOptions = item[ConfigKeys::OPTIONS].toArray();
    if (callOptions.size() < 2)
    {
      LOG_WARNING("Capability %s does not follow the expected format", qPrintable(name));
      continue;
    }

    auto callRoute = callOptions.at(0).toObject()[ConfigKeys::DEFAULT].toString();
    auto callMessage = callOptions.at(1).toObject()[ConfigKeys::DEFAULT].toString();
    code += std::format("  {} \"{}\" \"{}\" {{\n", callType.toStdString(), callRoute.toStdString(), callMessage.toStdString());

    QString qualifier = "  ";
    for (const auto& f : node.getflows())
    {
      LOG_DEBUG("%s match: %d %d", qPrintable(f->getname()), f->getlinksTo(), i);
      if (f->getlinksTo() != i)
      {
        continue;
      }

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

QString KodaGenerator::generateComponent(const INode& node, const QString& incomingCode, const QString& arguments)
{
  QString code = "";

  // Generate necessary wrappers
  QString name = fixCase(node.getproperties()[ConfigKeys::NAME].toString());
  mGeneratedDznFiles += QString::fromStdString("c" + ToLowerCase(name.toStdString(), 0, name.size() - 1) + ".dzn");
  mGeneratedDznFiles += QString::fromStdString("c" + ToLowerCase(name.toStdString(), 0, name.size() - 1) + "_task.dzn");

  // Create a file for each top level component
  QString fileName = mOutputFolder.filePath(name + ".kd");
  QFile file(fileName);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    LOG_WARNING("Failed to open device for writing");
    return code;
  }

  mGeneratedFiles.append(fileName);

  // Generate flows
  int index = 0;
  QString bodyCode = "";
  for (const auto& f : node.getflows())
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
    index++;
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

      code += generateStart(node.getproperties()[ConfigKeys::NAME].toString(), *n, *f, "    ");
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
  //   auto childName = fixCase(child->getproperties[ConfigKeys::NAME].toString());
  //   out << "  requires i" + childName + " " + childName + ";\n";
  // }

  out << bodyCode << "\n";
  out << "  strategy {\n"
      << code << "  }\n";
  out << "}\n";

  file.close();

  return code;
}

QString KodaGenerator::generateStart(const QString& parent, const INode& node, const IFlow& flow, const QString& format)
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
  code += generateTransitions(node, arg, flow, "  " + format);
  code += ";\n";

  return code;
}

QString KodaGenerator::generateSuccess(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  code += "end";
  return code;
}

QString KodaGenerator::generateContinue(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  code += "continue";
  return code;
}

QString KodaGenerator::generateError(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
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
    const auto isVariable = arg[ConfigKeys::IS_VARIABLE].toBool();

    // TODO(felaze): An underscore should only be added if we are refering to a state variable
    args += QStringLiteral("%1%2, ").arg(arg[ConfigKeys::DATA].toString(), isVariable ? "_" : "");
  }

  args.chop(2);
  return args;
}

QString KodaGenerator::generateAsyncTask(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  QJsonObject object = node.getproperties()["capability"].toJsonObject();
  QString val = object[ConfigKeys::DATA].toString();
  if (val.isEmpty())
  {
    mErrors.push_back({node.getid(),
                       node.getnodeId(),
                       flow.getid(),
                       QString("AsyncTask component does not have a valid capability")});
    return code;
  }

  // TODO: Check validity
  // - Arguments match the expectation
  // - Transitions are valid
  QJsonArray options = object[ConfigKeys::OPTIONS].toArray();
  QString args = createArguments(options);

  auto fixed = QString::fromStdString(ToLowerCase(val.toStdString(), 0, val.size() - 1));
  code += "(" + fixed + "(" + args + ")";
  for (const auto& transition : node.gettransitions())
  {
    if (transition->getlabel() != "on error")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += " on error (";
      code += generateBehaviourNode(*dst, arg, flow, format);
      code += ")";
    }
  }

  for (const auto& transition : node.gettransitions())
  {
    if (transition->getlabel() != "on abort")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += " on abort (";
      code += generateBehaviourNode(*dst, arg, flow, format);
      code += ")";
    }
  }

  for (const auto& transition : node.gettransitions())
  {
    if (transition->getlabel() != "on")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += " on " + QString::fromStdString(ToLowerCase(transition->getevent().toStdString(), 0, transition->getevent().size() - 1)) + "() (";
      code += generateBehaviourNode(*dst, arg, flow, format);
      code += ")";
    }
  }

  bool hasOutTransitions = false;
  for (const auto& transition : node.gettransitions())
  {
    if (transition->getlabel() != "")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      hasOutTransitions = true;
      code += ") --> ";
      code += generateBehaviourNode(*dst, arg, flow, format);
    }
  }

  if (!hasOutTransitions)
    code += ")";

  return code;
}

QString KodaGenerator::generateSyncTask(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  QJsonObject object = node.getproperties()["capability"].toJsonObject();
  QString val = object[ConfigKeys::DATA].toString();
  if (val.isEmpty())
  {
    mErrors.push_back({node.getid(),
                       node.getnodeId(),
                       flow.getid(),
                       QString("SyncTask component does not have a valid capability")});
    return code;
  }

  QJsonArray options = object[ConfigKeys::OPTIONS].toArray();
  if (val.isEmpty())
  {
    mErrors.push_back({node.getid(),
                       node.getnodeId(),
                       flow.getid(),
                       QString("SyncTask component %1 is missing an associated call").arg(val)});
    return code;
  }

  // TODO: Check validity
  // - Arguments match the expectation
  // - Transitions are valid
  QString method = "";
  QString args = "";
  for (uint32_t i = 0; i < options.size(); ++i)
  {
    QJsonObject obj = options.at(i).toObject();
    if (i == 0)
      method = obj[ConfigKeys::DATA].toString();
    else
      args += obj[ConfigKeys::DATA].toString() + ", ";
  }
  args.chop(2);

  auto fixed = QString::fromStdString(ToLowerCase(val.toStdString(), 0, val.size() - 1));
  code += fixed + "." + method + "(" + args + ")";

  if (node.gettransitions().size() > 0)
    code += " --> ";

  code += generateTransitions(node, arg, flow, format + "  ");

  return code;
}

QString KodaGenerator::generateWithin(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  int val = node.getproperties()["timeout"].toInt();

  code += "(within " + QString::number(val) + " do (";

  for (const auto& transition : node.gettransitions())
  {
    if (transition->getlabel() == "do")
    {
      auto dst = findDestination(transition->getdstId(), flow);
      if (dst != nullptr)
        code += generateBehaviourNode(*dst, arg, flow, format);
      break;
    }
  }

  code += ") else (";

  for (const auto& transition : node.gettransitions())
  {
    if (transition->getlabel() == "else")
    {
      auto dst = findDestination(transition->getdstId(), flow);
      if (dst != nullptr)
        code += generateBehaviourNode(*dst, arg, flow, format);
      break;
    }
  }

  code += ")";

  for (const auto& transition : node.gettransitions())
  {
    if (transition->getlabel() != "on error")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += "  on error (";
      code += generateBehaviourNode(*dst, arg, flow, format);
      code += ")";
    }
  }

  for (const auto& transition : node.gettransitions())
  {
    if (transition->getlabel() != "on abort")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += "  on abort (";
      code += generateBehaviourNode(*dst, arg, flow, format);
      code += ")";
    }
  }

  for (const auto& transition : node.gettransitions())
  {
    if (transition->getlabel() != "on")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += "  on " + QString::fromStdString(ToLowerCase(transition->getevent().toStdString(), 0, transition->getevent().size() - 1)) + "() (";
      code += generateBehaviourNode(*dst, arg, flow, format);
      code += ")";
    }
  }

  code += ")";

  for (const auto& transition : node.gettransitions())
  {
    if (transition->getlabel() != "")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += " --> ";
      code += generateBehaviourNode(*dst, arg, flow, format);
    }
  }

  return code;
}

QString KodaGenerator::generateEvery(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  int val = node.getproperties()["rate"].toInt();

  code += "(every " + QString::number(val) + " { ";

  for (const auto& transition : node.gettransitions())
  {
    if (transition->getlabel() != "")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
      code += generateBehaviourNode(*dst, arg, flow, format);

    break;
  }

  code += " }";

  for (const auto& transition : node.gettransitions())
  {
    if (transition->getlabel() != "on error")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += "\n  on error (";
      code += generateBehaviourNode(*dst, arg, flow, format);
      code += ")";
    }
  }

  for (const auto& transition : node.gettransitions())
  {
    if (transition->getlabel() != "on abort")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += "\n  on abort (";
      code += generateBehaviourNode(*dst, arg, flow, format);
      code += ")";
    }
  }

  for (const auto& transition : node.gettransitions())
  {
    if (transition->getlabel() != "on")
      continue;

    auto dst = findDestination(transition->getdstId(), flow);
    if (dst != nullptr)
    {
      code += "\n  on " + QString::fromStdString(ToLowerCase(transition->getevent().toStdString(), 0, transition->getevent().size() - 1)) + "() (";
      code += generateBehaviourNode(*dst, arg, flow, format);
      code += ")";
    }
  }

  code += ")";

  return code;
}

QString KodaGenerator::generateRepeat(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  QJsonObject object = node.getproperties()["strategy"].toJsonObject();
  QString val = object[ConfigKeys::DATA].toString();
  QJsonArray options = object[ConfigKeys::OPTIONS].toArray();
  if (options.isEmpty())
  {
    mErrors.push_back({node.getid(),
                       node.getnodeId(),
                       flow.getid(),
                       QString("Repeat component %1 does not have an associated flow").arg(val)});
    return code;
  }

  QString strategy = options[0].toObject()[ConfigKeys::DATA].toString();

  code += "repeat(" + strategy + ")";

  return code;
}

QString KodaGenerator::generateStrategy(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  QJsonObject object = node.getproperties()["strategy"].toJsonObject();
  QString val = object[ConfigKeys::DATA].toString();
  QJsonArray options = object[ConfigKeys::OPTIONS].toArray();
  if (options.isEmpty())
  {
    mErrors.push_back({node.getid(),
                       node.getnodeId(),
                       flow.getid(),
                       QString("Strategy component %1 does not have an associated flow").arg(val)});
    return code;
  }

  QString strategy = options[0].toObject()[ConfigKeys::DATA].toString();

  QString args = "";
  for (uint32_t i = 1; i < options.size(); ++i)
  {
    QJsonObject obj = options.at(i).toObject();
    args += obj[ConfigKeys::DATA].toString() + ", ";
  }
  args.chop(2);

  if (strategy != "main")
    strategy = "f" + strategy;

  code += strategy + (args.isEmpty() ? "" : "(" + args + ")");
  if (node.gettransitions().size() > 0)
    code += " --> ";

  code += generateTransitions(node, arg, flow, format + "  ");

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
  mDaemon = new QProcess(this);

  connect(mDaemon, &QProcess::started, this, []() {
    qDebug() << "Process started";
  });

  connect(mDaemon, &QProcess::readyReadStandardOutput, this, [this]() {
    qDebug() << mDaemon->readAllStandardOutput();
  });

  connect(mDaemon, &QProcess::readyReadStandardError, this, [this]() {
    qDebug() << mDaemon->readAllStandardError();
  });

  connect(mDaemon, &QProcess::finished, this, [this](int exitCode, QProcess::ExitStatus status) {
    qDebug() << "Finished:" << exitCode << status;
    if (exitCode != 0)
      mDaemon->deleteLater();  // important
  });

  connect(mDaemon, &QProcess::errorOccurred, this, [](QProcess::ProcessError e) {
    qWarning() << "Process error:" << e;
  });

  // Non-blocking start
  mDaemon->start("ide", {"daemon"});

  return mDaemon->waitForStarted();
}

void KodaGenerator::simulationStarted()
{
  LOG_INFO("Simulation started");
}

void KodaGenerator::simulationUpdated(const QJsonObject& obj)
{
  LOG_DEBUG("Simulation updated");
  mLastUpdate = obj;
  mServices->pluginTab()->updateScene(languageName());
}

VoidResult KodaGenerator::createSimulationScene(QGraphicsScene* scene, const QJsonObject& obj)
{
  // Nothing to render
  if (obj.isEmpty())
    return VoidResult();

  // auto pretty = QJsonDocument(obj).toJson(QJsonDocument::Indented);
  // LOG_DEBUG("Received message: %s", qPrintable(pretty));

  auto theme = mServices->pluginTab()->currentTheme();
  auto fonts = mServices->pluginTab()->labelFont();

  TraceSceneBuilder builder(theme, fonts, TraceSceneBuilder::Style{});
  auto clickHandler = [this](const QString& instance, const QString& labelText, bool illegal) {
    LOG_DEBUG("Sending data: %s %s", qPrintable(instance), qPrintable(labelText));
    mSimulator->triggerEvent(labelText);
  };

  QString err;
  if (!builder.buildScene(obj, scene, clickHandler, &err))
    return VoidResult::Failed("Failed to build scene: " + err.toStdString());

  return VoidResult();
}