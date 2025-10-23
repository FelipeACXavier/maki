#include "rozyne_generator.h"

#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonObject>
#include <QTextStream>

#include "elements/save_info.h"
#include "keys.h"
#include "logging.h"
#include "string_helpers.h"
#include "types.h"

static const QString FOLDER = "/generated";

QString RozyneGenerator::generateCode(std::shared_ptr<SaveInfo> storage)
{
  LOG_DEBUG("Starting generation");
  mStorage = storage;

  mOutputFolder = QDir(QDir::currentPath() + FOLDER);
  if (!mOutputFolder.exists())
    mOutputFolder.mkpath(".");

  QString code = "";
  // for (const auto& node : mStorage->structuralNodes)
  // {
  //   if (node->nodeId == "Generic::Component")
  //     continue;

  //   LOG_DEBUG("Generating code for top level node %s %s %d", qPrintable(node->properties["name"].toString()), qPrintable(node->nodeId), node->children.size());
  //   code += generateCapability(*node);
  // }

  for (const auto& node : mStorage->structuralNodes)
  {
    if (node->nodeId != "Mission::Component")
      continue;

    // TODO(felaze): Create file at this level
    LOG_DEBUG("Generating code for top level node %s %s %d", qPrintable(node->properties["name"].toString()), qPrintable(node->nodeId), node->children.size());

    QString args = "";
    for (const auto& child : node->children)
    {
      auto capabilityId = child->properties["name"].toString();
      LOG_DEBUG("Generating code for capability %s %s %d", qPrintable(capabilityId), qPrintable(child->nodeId), child->children.size());
      code += generateCapability(*child);
      args += fixCase(capabilityId) + " req " + capabilityId + ", ";
    }

    code = generateComponent(*node, code, args);
  }

  code.chop(1);

  return code;
}

generator::Language RozyneGenerator::supportedLanguage() const
{
  return generator::Language::Rozyne;
}

QString RozyneGenerator::languageName() const
{
  return "Rozyne";
}

// Add function per block type
QString RozyneGenerator::generateNode(const NodeSaveInfo& node)
{
  QString code = "";
  QString type = node.nodeId;
  QString name = node.properties["name"].toString();

  LOG_DEBUG("Generating code for %s", qPrintable(type));

  if (type == "Generic::Component")
    code += generateComponent(node, code, "");

  return code;
}

QString RozyneGenerator::generateBehaviourNode(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "";
  QString type = node.nodeId;
  QString name = node.properties["name"].toString();

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

QString RozyneGenerator::generateTransitions(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "";

  for (const auto& transition : node.transitions)
  {
    auto dst = findDestination(transition->dstId, flow);
    if (dst != nullptr)
      code += generateBehaviourNode(*dst, arg, flow, format);
  }

  return code;
}

QString RozyneGenerator::generateCapability(const NodeSaveInfo& node)
{
  QString code = "";

  qDebug() << node.properties;
  QString args = "";
  if (node.properties.contains("arguments"))
  {
    auto list = node.properties["arguments"].toList();
    for (const auto& l : list)
    {
      auto l0 = l.toMap();
      args += l0["type"].toString() + " " + l0["id"].toString() + ", ";
    }
  }

  QString name = node.properties["name"].toString();
  args.chop(2);
  code += "capability " + name + "(" + args + ") {\n";

  QString type = "async";
  QString rosType = node.properties["type"].toString();
  auto typeComponents = Split(rosType.toStdString(), ' ');
  if (typeComponents.size() == 3 && typeComponents.at(0) != "action")
    type = "sync";

  code += "  " + rosType + "{\n";

  int inIndex = 0;
  QString qualifier = "  ";
  for (const auto& f : node.flows)
  {
    if (f->type != Types::ConnectorType::IN)
      continue;

    QString args = "";
    for (auto& arg : f->arguments)
      args += PropertyTypesToString(arg.type) + " " + arg.id + ", ";

    args.chop(2);
    if (type == "async")
      qualifier = inIndex == 0 ? QStringLiteral("  trigger:") : QStringLiteral("  abort:");

    code += qualifier + "  " + PropertyTypesToString(f->returnType) + " " + f->name + "(" + args + ");\n";
    inIndex++;
  }

  int outIndex = 0;
  for (const auto& f : node.flows)
  {
    if (f->type != Types::ConnectorType::OUT)
      continue;

    QString args = "";
    for (auto& arg : f->arguments)
      args += PropertyTypesToString(arg.type) + " " + arg.id + ", ";

    args.chop(2);
    if (type == "async")
      qualifier = outIndex == 0 ? QStringLiteral("  return:") : QStringLiteral("  error:");

    code += qualifier + "  " + PropertyTypesToString(f->returnType) + " " + f->name + "(" + args + ");\n";
    outIndex++;
  }

  code += "  }\n";
  code += "}\n";

  return code;
}

QString RozyneGenerator::generateComponent(const NodeSaveInfo& node, const QString& incomingCode, const QString& arguments)
{
  QString code = "";

  // Generate necessary wrappers
  QString name = fixCase(node.properties["name"].toString());

  // Create a file for each top level component
  QFile file(mOutputFolder.filePath(name + ".rzn"));
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    LOG_WARNING("Failed to open device for writing");
    return code;
  }

  // Generate child code
  // for (const auto& child : node.children)
  //   code += generateNode(*child);

  // Generate my structural code

  // Generate flows
  int index = 0;
  QString bodyCode = "";
  for (const auto& f : node.flows)
  {
    if (!f->nodes.empty())
      continue;

    LOG_DEBUG("Generating flow %s", qPrintable(f->name));

    QString args = "";
    for (auto& arg : f->arguments)
      args += PropertyTypesToString(arg.type) + " " + arg.id + ", ";

    args.chop(2);
    QString qualifier = "";
    if (f->type == Types::ConnectorType::IN)
      qualifier = index == 0 ? QStringLiteral("  trigger:") : QStringLiteral("  abort:");
    else
      qualifier = index == 2 ? QStringLiteral("  return:") : QStringLiteral("  error:");

    bodyCode += qualifier + "  " + PropertyTypesToString(f->returnType) + " " + f->name + "(" + args + ");\n";
    index++;
  }

  for (const auto& f : node.flows)
  {
    if (f->nodes.empty())
      continue;

    LOG_DEBUG("Generating flow %s", qPrintable(f->name));
    // Find the start node
    for (const auto& n : f->nodes)
    {
      if (n->nodeId != "Mission::Start")
        continue;

      code += generateStart(node.properties[ConfigKeys::NAME].toString(), *n, *f, "    ");
      break;
    }
  }

  QTextStream out(&file);
  for (const auto& imp : mImports)
    out << "import " + imp + ";\n";

  if (!mImports.isEmpty())
    out << "\n";

  if (!node.fields.empty())
    bodyCode += "  vars {\n";

  QString args = arguments;
  for (const auto& state : node.fields)
  {
    bodyCode += "    " + Types::PropertyTypesToString(state.type) + " " + state.id + "_ = " + state.id + " : " + state.defaultValue.toString() + "\n";
    args += Types::PropertyTypesToString(state.type) + " " + state.id + ", ";
  }

  if (!node.fields.empty())
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
  //   auto childName = fixCase(child->properties["name"].toString());
  //   out << "  requires i" + childName + " " + childName + ";\n";
  // }

  out << bodyCode << "\n";
  out << "  strategy {\n"
      << code << "  }\n";
  out << "}\n";

  file.close();

  return code;
}

QString RozyneGenerator::generateStart(const QString& parent, const NodeSaveInfo& node, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "    " + flow.name + ": ";

  Argument arg;
  code += generateTransitions(node, arg, flow, "  " + format);
  code += ";\n";

  return code;
}

QString RozyneGenerator::generateEnd(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "";
  code += "end";
  return code;
}

QString RozyneGenerator::generateError(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "";
  if (arg.name.isEmpty())
    return code;

  code += format + "reply(" + arg.name + ");\n";
  return code;
}

QString RozyneGenerator::generateAsyncTask(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "";
  QJsonObject object = node.properties["component"].toJsonObject();
  QString val = object["data"].toString();
  QJsonArray options = object["options"].toArray();
  QString args = options.size() > 0 ? options[0].toObject()["data"].toString() : "";

  // qDebug() << format + "generateAsyncTask (" + val + "): " << options;

  auto fixed = QString::fromStdString(ToLowerCase(val.toStdString(), 0, 1));
  code += "(" + fixed + "(" + args + ")";
  for (const auto& transition : node.transitions)
  {
    if (transition->label != "on error")
      continue;

    auto dst = findDestination(transition->dstId, flow);
    if (dst != nullptr)
    {
      code += " on error (";
      code += generateBehaviourNode(*dst, arg, flow, format);
      code += ")";
    }
  }

  for (const auto& transition : node.transitions)
  {
    if (transition->label != "on abort")
      continue;

    auto dst = findDestination(transition->dstId, flow);
    if (dst != nullptr)
    {
      code += " on abort (";
      code += generateBehaviourNode(*dst, arg, flow, format);
      code += ")";
    }
  }

  for (const auto& transition : node.transitions)
  {
    if (transition->label != "on")
      continue;

    auto dst = findDestination(transition->dstId, flow);
    if (dst != nullptr)
    {
      code += " on " + QString::fromStdString(ToLowerCase(transition->event.toStdString(), 0, 1)) + "() (";
      code += generateBehaviourNode(*dst, arg, flow, format);
      code += ")";
    }
  }

  bool hasOutTransitions = false;
  for (const auto& transition : node.transitions)
  {
    if (transition->label != "")
      continue;

    auto dst = findDestination(transition->dstId, flow);
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

QString RozyneGenerator::generateSyncTask(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "";
  QJsonObject object = node.properties["component"].toJsonObject();
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

  if (node.transitions.size() > 0)
    code += " --> ";

  code += generateTransitions(node, arg, flow, format + "  ");

  return code;
}

QString RozyneGenerator::generateWithin(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "";
  int val = node.properties["timeout"].toInt();

  qDebug() << node.properties;

  // QJsonObject object = node.properties["component"].toJsonObject();
  // QString val = object["data"].toString();
  // QJsonArray options = object["options"].toArray();
  // QString strategy = options.size() > 0 ? options[0].toObject()["data"].toString() : "";

  // qDebug() << format + "generateWithin (" + node.properties["name"].toString() + "): " << val;

  code += "(within " + QString::number(val) + " do (";

  for (const auto& transition : node.transitions)
  {
    if (transition->label == "do")
    {
      auto dst = findDestination(transition->dstId, flow);
      if (dst != nullptr)
        code += generateBehaviourNode(*dst, arg, flow, format);
      break;
    }
  }

  code += ") else (";

  for (const auto& transition : node.transitions)
  {
    if (transition->label == "else")
    {
      auto dst = findDestination(transition->dstId, flow);
      if (dst != nullptr)
        code += generateBehaviourNode(*dst, arg, flow, format);
      break;
    }
  }

  code += "))";

  for (const auto& transition : node.transitions)
  {
    if (transition->label == "do" || transition->label == "else")
      continue;

    auto dst = findDestination(transition->dstId, flow);
    if (dst != nullptr)
    {
      code += " --> ";
      code += generateBehaviourNode(*dst, arg, flow, format);
    }
  }

  return code;
}

QString RozyneGenerator::generateRepeat(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "";
  QJsonObject object = node.properties["component"].toJsonObject();
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
  // auto dstDo = findDestination(doTransition->dstId, flow);
  // if (dstDo != nullptr)
  //     code += generateBehaviourNode(*dstDo, arg, flow, format);
  // code += ")";

  return code;
}

QString RozyneGenerator::generateStrategy(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "";
  QJsonObject object = node.properties["component"].toJsonObject();
  QJsonArray options = object["options"].toArray();
  QString strategy = options.size() > 0 ? options[0].toObject()["data"].toString() : "";

  // qDebug() << "Strategy: " << node.properties;

  // qDebug() << format + "generateStrategy (" << node.properties["name"] << "): " << val["option_data"].toString();
  code += strategy;
  if (node.transitions.size() > 0)
    code += " --> ";

  code += generateTransitions(node, arg, flow, format + "  ");

  return code;
}

std::shared_ptr<NodeSaveInfo> RozyneGenerator::findDestination(const QString& nodeId, const FlowSaveInfo& flow) const
{
  for (const auto& dst : flow.nodes)
  {
    if (dst->id == nodeId)
      return dst;
  }

  LOG_DEBUG("Could not find destination with id: %s", qPrintable(nodeId));
  return nullptr;
}

QString RozyneGenerator::fixCase(const QString& name)
{
  return name.toLower().replace(" ", "_");
}
