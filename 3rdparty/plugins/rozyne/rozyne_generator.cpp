#include "rozyne_generator.h"

#include <QApplication>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QProcess>
#include <QTextStream>

#include "idocument.h"
#include "ipipeline.h"
#include "isettings.h"
#include "keys.h"
#include "logging.h"
#include "string_helpers.h"
#include "types.h"

bool RozyneGenerator::setup()
{
  // Start the ide daemon on a specific port
  return startDaemon();
}

bool RozyneGenerator::tearDown()
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

void RozyneGenerator::buildSettings()
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

void RozyneGenerator::setHostServices(maki::IHostServices* services)
{
  mServices = services;

  buildSettings();

  // Setup settings
  if (auto service = mServices->settings())
    service->registerSettings(languageName(), version(), mSettings);
}

QString RozyneGenerator::languageName() const
{
  return "KODA";
}

maki::PluginVersion RozyneGenerator::version() const
{
  return {"0", "0", "1"};
}

QString RozyneGenerator::simulate(const QString& outputFolder)
{
  LOG_INFO("Running simulation");

  if (mServices == nullptr)
  {
    LOG_ERROR("Cannot proceed with simulation, no services provided");
    return "";
  }
  else if (mServices->pipeline() == nullptr)
  {
    LOG_ERROR("Cannot proceed with simulation, no pipeline provided");
    return "";
  }
  else if (mServices->document() == nullptr)
  {
    LOG_ERROR("Cannot proceed with simulation, no document provided");
    return "";
  }

  if (mGeneratedDznFiles.empty())
  {
    // Generate and then simulate
  }
  else
  {
    // Simulate
    for (const QString& f : mGeneratedDznFiles)
    {
      auto fullPath = mDezyneOutputFolder.absoluteFilePath(f);
      if (!fullPath.contains("_task"))
        continue;

      LOG_INFO("Will simulate: %s", qPrintable(fullPath));

      const QString command = "ide";
      const QStringList arguments = {"simulate", fullPath};

      QProcess* generate = new QProcess(this);
      generate->setProgram(command);
      generate->setArguments(arguments);

      mServices->pipeline()->add(generate, maki::OnFail::OPEN_BROWSER, "http://localhost:3000/trace");
    }
  }

  return "";
}

QString RozyneGenerator::verify(const QString& outputFolder)
{
  LOG_INFO("Running verification");

  if (mServices == nullptr)
  {
    LOG_ERROR("Cannot proceed with verification, no services provided");
    return "";
  }
  else if (mServices->pipeline() == nullptr)
  {
    LOG_ERROR("Cannot proceed with verification, no pipeline provided");
    return "";
  }
  else if (mServices->document() == nullptr)
  {
    LOG_ERROR("Cannot proceed with verification, no document provided");
    return "";
  }

  mGeneratedDznFiles = {};
  mGeneratedFiles = {};

  // Create output folder
  mOutputFolder = QDir(outputFolder + "/" + languageName());
  if (!mOutputFolder.exists())
    mOutputFolder.mkpath(".");

  // Generate Koda from the model
  generateKoda(outputFolder);

  // Compile Koda to Dezyne
  mDezyneOutputFolder = QDir(mOutputFolder.absolutePath() + "/models");
  if (!mDezyneOutputFolder.exists())
    mDezyneOutputFolder.mkpath(".");

  LOG_INFO("Compiling Koda to Dezyne: %s %d", qPrintable(mDezyneOutputFolder.absolutePath()), mGeneratedFiles.size());
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
    };

    QProcess* generate = new QProcess(this);
    generate->setWorkingDirectory(QDir::homePath() + "/" + languageName());
    generate->setProgram(command);
    generate->setArguments(arguments);

    mServices->pipeline()->add(generate, maki::OnFail::STOP);
  }

  // TODO: We need to find a better solution for this, we cannot allow plugins to execute any scripts
  LOG_INFO("Running generate script");

  // QStringList files = mDezyneOutputFolder.entryList(QDir::Files | QDir::NoDotAndDotDot);
  for (const QString& f : mGeneratedDznFiles)
  {
    auto fullPath = mDezyneOutputFolder.absoluteFilePath(f);
    // if (fullPath.contains("/a_") || fullPath.contains("types"))
    // continue;

    LOG_INFO("Will verify file: %s", qPrintable(fullPath));

    const QString command = "ide";
    const QStringList arguments = {"verify", fullPath};
    QProcess* generate = new QProcess(this);
    generate->setProgram(command);
    generate->setArguments(arguments);

    mServices->pipeline()->add(generate, maki::OnFail::STOP);
  }

  return "";
}

QString RozyneGenerator::generateKoda(const QString& outputFolder)
{
  mGeneratedFiles.clear();

  LOG_DEBUG("Generating Koda files with %d nodes", mServices->document()->getnodes().size());
  QString code = "";

  for (const auto& node : mServices->document()->getnodes())
  {
    if (node->getnodeId() != "Mission::Component")
      continue;

    // TODO(felaze): Create file at this level
    LOG_DEBUG("Generating code for top level node %s %s %d", qPrintable(node->getproperties()["name"].toString()), qPrintable(node->getnodeId()), node->getchildren().size());

    QString args = "";
    for (const auto& child : node->getchildren())
    {
      auto capabilityId = child->getproperties()["name"].toString();
      LOG_DEBUG("Generating code for capability %s %s %d", qPrintable(capabilityId), qPrintable(child->getnodeId()), child->getchildren().size());
      code += generateCapability(*child);
      args += fixCase(capabilityId) + " req " + capabilityId + ", ";
    }

    code = generateComponent(*node, code, args);
  }

  code.chop(1);

  return code;
}

QList<QString> RozyneGenerator::generatedFiles() const
{
  return mGeneratedFiles;
}

// Add function per block type
QString RozyneGenerator::generateNode(const INode& node)
{
  QString code = "";
  QString type = node.getnodeId();
  QString name = node.getproperties()["name"].toString();

  LOG_DEBUG("Generating code for %s", qPrintable(type));

  if (type == "Generic::Component")
    code += generateComponent(node, code, "");

  return code;
}

QString RozyneGenerator::generateBehaviourNode(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  QString type = node.getnodeId();
  QString name = node.getproperties()["name"].toString();

  // LOG_DEBUG("Generating code for %s with %s", qPrintable(type), qPrintable(arg.name));

  if (type == "Mission::End")
    code += generateEnd(node, arg, flow, format);
  else if (type == "Mission::Error")
    code += generateError(node, arg, flow, format);
  else if (type == "Mission::Async task")
    code += generateAsyncTask(node, arg, flow, format);
  else if (type == "Mission::Sync task")
    code += generateSyncTask(node, arg, flow, format);
  else if (type == "Mission::Strategy")
    code += generateStrategy(node, arg, flow, format);
  else if (type == "Mission::Within")
    code += generateWithin(node, arg, flow, format);
  else if (type == "Mission::Repeat")
    code += generateRepeat(node, arg, flow, format);

  return code;
}

QString RozyneGenerator::generateTransitions(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
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

QString RozyneGenerator::generateCapability(const INode& node)
{
  QString code = "";

  // qDebug() << node.properties;
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
  mGeneratedDznFiles += QString::fromStdString("i" + ToLowerCase(name.toStdString(), 0, 1) + ".dzn");

  args.chop(2);
  code += "capability " + name + "(" + args + ") {\n";

  QString type = "async";
  QString rosType = node.getproperties()["type"].toString();
  auto typeComponents = Split(rosType.toStdString(), ' ');
  if (typeComponents.size() == 3 && typeComponents.at(0) != "action")
    type = "sync";

  code += "  " + rosType + "{\n";

  int inIndex = 0;
  QString qualifier = "  ";
  for (const auto& f : node.getflows())
  {
    if (f->gettype() != Types::ConnectorType::IN)
      continue;

    QString args = "";
    for (const auto& arg : f->getarguments())
      args += PropertyTypesToString(arg->gettype()) + " " + arg->getid() + ", ";

    args.chop(2);
    if (type == "async")
      qualifier = inIndex == 0 ? QStringLiteral("  trigger:") : QStringLiteral("  abort:");

    code += qualifier + "  " + PropertyTypesToString(f->getreturnType()) + " " + f->getname() + "(" + args + ");\n";
    inIndex++;
  }

  int outIndex = 0;
  for (const auto& f : node.getflows())
  {
    if (f->gettype() != Types::ConnectorType::OUT)
      continue;

    QString args = "";
    for (auto& arg : f->getarguments())
      args += PropertyTypesToString(arg->gettype()) + " " + arg->getid() + ", ";

    args.chop(2);
    if (type == "async")
      qualifier = outIndex == 0 ? QStringLiteral("  return:") : QStringLiteral("  error:");

    code += qualifier + "  " + PropertyTypesToString(f->getreturnType()) + " " + f->getname() + "(" + args + ");\n";
    outIndex++;
  }

  code += "  }\n";
  code += "}\n";

  return code;
}

QString RozyneGenerator::generateComponent(const INode& node, const QString& incomingCode, const QString& arguments)
{
  QString code = "";

  // Generate necessary wrappers
  QString name = fixCase(node.getproperties()["name"].toString());
  mGeneratedDznFiles += QString::fromStdString("c" + ToLowerCase(name.toStdString(), 0, 1) + ".dzn");
  mGeneratedDznFiles += QString::fromStdString("c" + ToLowerCase(name.toStdString(), 0, 1) + "_task.dzn");

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
      args += PropertyTypesToString(arg->gettype()) + " " + arg->getid() + ", ";

    args.chop(2);
    QString qualifier = "";
    if (f->gettype() == Types::ConnectorType::IN)
      qualifier = index == 0 ? QStringLiteral("  trigger:") : QStringLiteral("  abort:");
    else
      qualifier = index == 2 ? QStringLiteral("  return:") : QStringLiteral("  error:");

    bodyCode += qualifier + "  " + PropertyTypesToString(f->getreturnType()) + " " + f->getname() + "(" + args + ");\n";
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
      if (n->getnodeId() != "Mission::Start")
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
  //   auto childName = fixCase(child->getproperties["name"].toString());
  //   out << "  requires i" + childName + " " + childName + ";\n";
  // }

  out << bodyCode << "\n";
  out << "  strategy {\n"
      << code << "  }\n";
  out << "}\n";

  file.close();

  return code;
}

QString RozyneGenerator::generateStart(const QString& parent, const INode& node, const IFlow& flow, const QString& format)
{
  QString code = "    " + flow.getname() + ": ";

  Argument arg;
  code += generateTransitions(node, arg, flow, "  " + format);
  code += ";\n";

  return code;
}

QString RozyneGenerator::generateEnd(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  code += "end";
  return code;
}

QString RozyneGenerator::generateError(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  if (arg.name.isEmpty())
    return code;

  code += format + "reply(" + arg.name + ");\n";
  return code;
}

QString RozyneGenerator::generateAsyncTask(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  QJsonObject object = node.getproperties()["component"].toJsonObject();
  QString val = object["data"].toString();
  QJsonArray options = object["options"].toArray();
  QString args = options.size() > 0 ? options[0].toObject()["data"].toString() : "";

  // qDebug() << format + "generateAsyncTask (" + val + "): " << options;

  auto fixed = QString::fromStdString(ToLowerCase(val.toStdString(), 0, 1));
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
      code += " on " + QString::fromStdString(ToLowerCase(transition->getevent().toStdString(), 0, 1)) + "() (";
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

QString RozyneGenerator::generateSyncTask(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  QJsonObject object = node.getproperties()["component"].toJsonObject();
  QString val = object["data"].toString();
  QJsonArray options = object["options"].toArray();

  QString method = "";
  QString args = "";
  for (const auto& opt : options)
  {
    QJsonObject obj = opt.toObject();
    if (obj["id"] == "event")
      method = obj["data"].toString();
    else if (obj["id"] == "argument")
      args = obj["data"].toString();
  }

  // qDebug() << "Sync: " << node.properties;

  // qDebug() << format + "generateSyncTask (" + node.properties["name"].toString() + "): " << val["data"].toString() << " " << val["option_data"].toString();

  auto fixed = QString::fromStdString(ToLowerCase(val.toStdString(), 0, 1));
  code += fixed + "." + method + "(" + args + ")";

  if (node.gettransitions().size() > 0)
    code += " --> ";

  code += generateTransitions(node, arg, flow, format + "  ");

  return code;
}

QString RozyneGenerator::generateWithin(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  int val = node.getproperties()["timeout"].toInt();

  // qDebug() << node.properties;

  // QJsonObject object = node.properties["component"].toJsonObject();
  // QString val = object["data"].toString();
  // QJsonArray options = object["options"].toArray();
  // QString strategy = options.size() > 0 ? options[0].toObject()["data"].toString() : "";

  // qDebug() << format + "generateWithin (" + node.properties["name"].toString() + "): " << val;

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

  code += "))";

  for (const auto& transition : node.gettransitions())
  {
    if (transition->getlabel() == "do" || transition->getlabel() == "else")
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

QString RozyneGenerator::generateRepeat(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  QJsonObject object = node.getproperties()["component"].toJsonObject();
  QString val = object["data"].toString();
  QJsonArray options = object["options"].toArray();
  QString strategy = options.size() > 0 ? options[0].toObject()["data"].toString() : "";

  // qDebug() << "Repeat: " << node.properties;
  // qDebug() << format + "generateRepeat (" + node.properties["name"].toString() + "): " << val;

  // if (node.transitions.size() != 1)
  // {
  //   LOG_ERROR("%s must have one transition", qPrintable(node.nodeId));
  //   return code;
  // }

  code += "repeat(" + strategy + ")";

  // auto doTransition = node.transitions.at(0);
  // auto dstDo = findDestination(doTransition->getdstId, flow);
  // if (dstDo != nullptr)
  //     code += generateBehaviourNode(*dstDo, arg, flow, format);
  // code += ")";

  return code;
}

QString RozyneGenerator::generateStrategy(const INode& node, const Argument& arg, const IFlow& flow, const QString& format)
{
  QString code = "";
  QJsonObject object = node.getproperties()["component"].toJsonObject();
  QJsonArray options = object["options"].toArray();
  QString strategy = options.size() > 0 ? options[0].toObject()["data"].toString() : "";

  // qDebug() << "Strategy: " << node.properties;

  // qDebug() << format + "generateStrategy (" << node.properties["name"] << "): " << val["option_data"].toString();
  code += strategy;
  if (node.gettransitions().size() > 0)
    code += " --> ";

  code += generateTransitions(node, arg, flow, format + "  ");

  return code;
}

std::shared_ptr<INode> RozyneGenerator::findDestination(const QString& nodeId, const IFlow& flow) const
{
  for (const auto& dst : flow.getnodes())
  {
    if (dst->getid() == nodeId)
      return dst;
  }

  LOG_DEBUG("Could not find destination with id: %s", qPrintable(nodeId));
  return nullptr;
}

QString RozyneGenerator::fixCase(const QString& name)
{
  return name.toLower().replace(" ", "_");
}

bool RozyneGenerator::startDaemon()
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