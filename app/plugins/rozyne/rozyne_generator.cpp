#include "rozyne_generator.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "keys.h"
#include "elements/save_info.h"
#include "logging.h"
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
  for (const auto& node : mStorage->structuralNodes)
  {
    if (node->nodeId == "Generic::Component")
      continue;

    LOG_DEBUG("Generating code for top level node %s %s %d", qPrintable(node->properties["name"].toString()), qPrintable(node->nodeId), node->children.size());
    code += generateCapability(*node);
  }

  for (const auto& node : mStorage->structuralNodes)
  {
    if (node->nodeId != "Generic::Component")
      continue;
    
    // TODO(felaze): Create file at this level
    LOG_DEBUG("Generating code for top level node %s %s %d", qPrintable(node->properties["name"].toString()), qPrintable(node->nodeId), node->children.size());
    code = generateComponent(*node, code);
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
    code += generateComponent(node, code);
  
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
  
  QString name = node.properties["name"].toString();
  QString type = fixCase(node.properties["type"].toString());
  code += type + " component " + name + "() {\n";

  if (type == "sync")
    code += "  accepts {\n";

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
      qualifier = inIndex == 0? QStringLiteral("  trigger:") : QStringLiteral("  abort:");

    code += qualifier + "  " + PropertyTypesToString(f->returnType) + " " + f->name + "(" + args + ");\n";
    inIndex++;
  }

  if (type == "sync")
    code += "  }\n  emits {";

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
      qualifier = outIndex == 0? QStringLiteral("  return:") : QStringLiteral("  error:");

    code += qualifier + "  " + PropertyTypesToString(f->returnType) + " " + f->name + "(" + args + ");\n";
    outIndex++;
  }

  if (type == "sync")
    code += "  }\n";

  code += "}\n";

  return code;
}

QString RozyneGenerator::generateComponent(const NodeSaveInfo& node, const QString& incomingCode)
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
  for (const auto& f : node.flows)
  {
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
    
  out << incomingCode << "\n";

  out << "task component " + name + "()\n";
  out << "{\n";

  // TODO(felaze): I really need to take some day to clean this and the widget codes.
  // - More widgets, maybe even one per property type
  // - The generation should use more inheritance, a lot of the code here is repeated
  for (const auto& child : node.children)
  {
    auto childName = fixCase(child->properties["name"].toString());
    out << "  requires i" + childName + " " + childName + ";\n";
  }
      
  for (const auto& state : node.fields)
  {
    if (state.type == Types::PropertyTypes::ENUM)
    {
      out << "   enum " + state.options.at(0).defaultValue.toString() + " {\n";  
      QString enumValues = "";
      for (const auto& opt : state.options.at(1).options)
        enumValues += "    " + opt.id + ",\n";

      enumValues.chop(2);
      out << enumValues;
      out << "  }\n";

      out << "  " + state.options.at(0).defaultValue.toString() + " " + state.id + " = " + state.defaultValue.toString() + ";\n"; 
    }
    else
    {
      out << "  " + Types::PropertyTypesToString(state.type) + " " + state.id + " = " + state.defaultValue.toString() + ";\n";       
    }
  }
  out << "strategy {\n" << code << "}";
  out << "}\n";

  file.close();

  return code; 
}

QString RozyneGenerator::generateStart(const QString& parent, const NodeSaveInfo& node, const FlowSaveInfo& flow, const QString& format)
{
  QString code = flow.name + ": ";

  Argument arg;
  code += generateTransitions(node, arg, flow, "  " + format);
  code += ";\n\n";

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
  QString val = node.properties["component"].toString();
  // qDebug() << format + "generateAsyncTask (" + node.properties["name"].toString() + "): " << val;
  
  auto fixed = QString::fromStdString(ToLowerCase(val.toStdString(), 0, 1));
  code += "(" + fixed + "()";
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
  QJsonObject val = node.properties["component"].toJsonObject();
  // qDebug() << format + "generateSyncTask (" + node.properties["name"].toString() + "): " << val["data"].toString() << " " << val["option_data"].toString();

  auto str = val["data"].toString();
  auto fixed = QString::fromStdString(ToLowerCase(str.toStdString(), 0, 1));
  code += fixed + "." + val["option_data"].toString() + "()";

  if (node.transitions.size() > 0)
    code += " --> ";

  code += generateTransitions(node, arg, flow, format + "  ");

  return code;
}

QString RozyneGenerator::generateWithin(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "";
  int val = node.properties["time"].toInt();
  // qDebug() << format + "generateWithin (" + node.properties["name"].toString() + "): " << val;

  if (node.transitions.size() != 2)
  {
    LOG_ERROR("%s must have two transitions", qPrintable(node.nodeId));
    return code;
  }

  code += "within " + QString::number(val) + " do (";

  auto doTransition = node.transitions.at(0);
  auto dstDo = findDestination(doTransition->dstId, flow);
  if (dstDo != nullptr)
      code += generateBehaviourNode(*dstDo, arg, flow, format);
  
  code += ") else (";
      
  auto elseTransition = node.transitions.at(1);
  auto dstElse = findDestination(elseTransition->dstId, flow);
  if (dstElse != nullptr)
      code += generateBehaviourNode(*dstElse, arg, flow, format);

  code += ")";

  return code;
}

QString RozyneGenerator::generateRepeat(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "";
  QJsonObject val = node.properties["component"].toJsonObject();
  qDebug() << format + "generateRepeat (" + node.properties["name"].toString() + "): " << val;

  // if (node.transitions.size() != 1)
  // {
  //   LOG_ERROR("%s must have one transition", qPrintable(node.nodeId));
  //   return code;
  // }

  code += "repeat(" + val["option_data"].toString() + ")";

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
  QJsonObject val = node.properties["component"].toJsonObject();
  // QString strategy = val["option_data"].toString();

  // qDebug() << format + "generateStrategy (" << node.properties["name"] << "): " << val["option_data"].toString();
  code += val["option_data"].toString();
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
