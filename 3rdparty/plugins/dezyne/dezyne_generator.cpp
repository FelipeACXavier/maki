#include "dezyne_generator.h"

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

QString DezyneGenerator::generateCode(std::shared_ptr<SaveInfo> storage)
{
  LOG_DEBUG("Starting generation");
  mStorage = storage;

  mOutputFolder = QDir(QDir::currentPath() + FOLDER);
  if (!mOutputFolder.exists())
    mOutputFolder.mkpath(".");

  QString code = "";
  for (const auto& node : mStorage->structuralNodes)
  {
    // TODO(felaze): Create file at this level
    LOG_DEBUG("Generating code for top level node %s %s %d", qPrintable(node->properties["name"].toString()), qPrintable(node->nodeId), node->children.size());
    code += generateNode(*node);
  }

  // code.chop(1);

  return code;
}

generator::Language DezyneGenerator::supportedLanguage() const
{
  return generator::Language::Dezyne;
}

QString DezyneGenerator::languageName() const
{
  return "Dezyne";
}

// Add function per block type
QString DezyneGenerator::generateNode(const NodeSaveInfo& node)
{
  QString code = "";
  QString type = node.nodeId;
  QString name = node.properties["name"].toString();

  LOG_DEBUG("Generating code for %s", qPrintable(type));

  if (type == "Utilities::Timer")
    code += generateTimer(node);
  else if (type == "Utilities::Authenticator")
    code += generateAuthenticator(node);
  else if (type == "Utilities::Siren")
    code += generateSiren(node);
  else if (type == "Utilities::Presence sensor")
    code += generatePresenceSensor(node);
  else if (type == "Generic::Component")
    code += generateComponent(node);
  else if (type == "Generic::Interface")
    code += generateInterface(node);
  
  return code;
}

QString DezyneGenerator::generateBehaviourNode(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "";
  QString type = node.nodeId;
  QString name = node.properties["name"].toString();

  LOG_DEBUG("Generating code for %s with %s", qPrintable(type), qPrintable(arg.name));

  if (type == "Generic::End")
    code += generateEnd(node, arg, flow, format);
  else if (type == "Generic::Error")
    code += generateError(node, arg, flow, format);
  else if (type == "Generic::Action")
    code += generateAction(node, arg, flow, format);
  else if (type == "Generic::Condition")
    code += generateCondition(node, arg, flow, format);
  else if (type == "Generic::Assign")
    code += generateAssign(node, arg, flow, format);
  else if (type == "Generic::State")
    code += generateState(node, arg, flow, format);

  return code;
}

QString DezyneGenerator::generateTransitions(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format)
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

QString DezyneGenerator::generateTimer(const NodeSaveInfo& node)
{
  QString name = fixCase(node.properties["name"].toString());
  // This is a default component with its own interface
  QFile file(mOutputFolder.filePath(name + ".dzn"));
  if (file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QFileInfo fileInfo(file.fileName());
    mImports.push_back(fileInfo.fileName());
    QTextStream out(&file);
    out << "interface i" + name << "\n";
    out << "{\n";
    out << "  in void set();\n";
    out << "  in void cancel();\n";
    out << "  out void timeout();\n\n";
    out << "  behavior\n";
    out << "  {\n";
    out << "    bool idle = true;\n";
    out << "    on set: idle = false;\n";
    out << "    on cancel: idle = true;\n";
    out << "    [!idle] on inevitable: {idle = true; timeout;}\n";
    out << "  }\n";
    out << "}";
    file.close();
  }
  
  QString code = "";
  for (const auto& f : node.flows)
  {
    LOG_DEBUG("Generating flow %s", qPrintable(f->name));
    // Find the start node
    for (const auto& n : f->nodes)
    {
      if (n->nodeId != "Generic::Start")
        continue;

      code += generateStart(node.properties[ConfigKeys::NAME].toString(), *n, *f, "    ");
      break;
    }
  }
  
  return code;  
}

QString DezyneGenerator::generateAuthenticator(const NodeSaveInfo& node)
{
  QString name = fixCase(node.properties["name"].toString());
  QFile file(mOutputFolder.filePath(name + ".dzn"));
  if (file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QFileInfo fileInfo(file.fileName());
    mImports.push_back(fileInfo.fileName());
    QTextStream out(&file);
    out << "interface i" + name << "\n";
    out << "{\n";
    out << "  in bool valid();\n\n";
    out << "  behavior\n";
    out << "  {\n";
    out << "    on valid: reply(true);\n";
    out << "    on valid: reply(false);\n";
    out << "  }\n";
    out << "}";
    file.close();
  }

  QString code = "";
  for (const auto& f : node.flows)
  {
    LOG_DEBUG("Generating flow %s", qPrintable(f->name));
    // Find the start node
    for (const auto& n : f->nodes)
    {
      if (n->nodeId != "Generic::Start")
        continue;

      code += generateStart(node.properties[ConfigKeys::NAME].toString(), *n, *f, "    ");
      break;
    }
  }
  
  return code;  
}

QString DezyneGenerator::generateSiren(const NodeSaveInfo& node)
{
  QString name = fixCase(node.properties["name"].toString());
  QFile file(mOutputFolder.filePath(name + ".dzn"));
  if (file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QFileInfo fileInfo(file.fileName());
    mImports.push_back(fileInfo.fileName());
    QTextStream out(&file);
    out << "interface i" + name << "\n";
    out << "{\n";
    out << "  in void enable();\n";
    out << "  in void disable();\n\n";
    out << "  behavior\n";
    out << "  {\n";
    out << "    bool enabled = true;\n";
    out << "    [enabled] on disable: enabled = false;\n";
    out << "    [!enabled] on enable: enabled = true;\n";
    out << "  }\n";
    out << "}";
    file.close();
  }

  QString code = "";
  for (const auto& f : node.flows)
  {
    LOG_DEBUG("Generating flow %s", qPrintable(f->name));
    // Find the start node
    for (const auto& n : f->nodes)
    {
      if (n->nodeId != "Generic::Start")
        continue;

      code += generateStart(node.properties[ConfigKeys::NAME].toString(), *n, *f, "    ");
      break;
    }
  }
  
  return code;  
}

QString DezyneGenerator::generatePresenceSensor(const NodeSaveInfo& node)
{
  QString name = fixCase(node.properties["name"].toString());
  QFile file(mOutputFolder.filePath(name + ".dzn"));
  if (file.open(QIODevice::WriteOnly | QIODevice::Text))
  {
    QFileInfo fileInfo(file.fileName());
    mImports.push_back(fileInfo.fileName());
    QTextStream out(&file);
    out << "interface i" + name << "\n";
    out << "{\n";
    out << "  in bool value();\n\n";
    out << "  behavior\n";
    out << "  {\n";
    out << "    on value: reply(true);\n";
    out << "    on value: reply(false);\n";
    out << "  }\n";
    out << "}";
    file.close();
  }
  QString code = "";
  for (const auto& f : node.flows)
  {
    LOG_DEBUG("Generating flow %s", qPrintable(f->name));
    // Find the start node
    for (const auto& n : f->nodes)
    {
      if (n->nodeId != "Generic::Start")
        continue;

      code += generateStart(node.properties[ConfigKeys::NAME].toString(), *n, *f, "    ");
      break;
    }
  }
  
  return code;  
}

QString DezyneGenerator::generateComponent(const NodeSaveInfo& node)
{
  QString code = "";

  // Generate necessary wrappers
  QString name = fixCase(node.properties["name"].toString());

  // Create a file for each top level component
  QFile file(mOutputFolder.filePath(name + ".dzn"));
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    LOG_WARNING("Failed to open device for writing");
    return code;
  }
  
  // Generate child code
  for (const auto& child : node.children)
    code += generateNode(*child);

  // Generate my structural code

  // Generate flows    
  for (const auto& f : node.flows)
  {
    LOG_DEBUG("Generating flow %s", qPrintable(f->name));
    // Find the start node
    for (const auto& n : f->nodes)
    {
      if (n->nodeId != "Generic::Start")
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
    
  out << "component " + name + "\n";
  out << "{\n";

  // TODO(felaze): I really need to take some day to clean this and the widget codes.
  // - More widgets, maybe even one per property type
  // - The generation should use more inheritance, a lot of the code here is repeated
  for (const auto& child : node.children)
  {
    auto childName = fixCase(child->properties["name"].toString());
    out << "  requires i" + childName + " " + childName + ";\n";
  }
      
  out << "\n  behaviour\n";
  out << "  {\n";
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
  out << code;
  out << "  }\n";
  out << "}\n";

  file.close();

  return code; 
}

QString DezyneGenerator::generateInterface(const NodeSaveInfo& node)
{
  QString code = "";

  // Generate necessary wrappers
  QString name = fixCase(node.properties["name"].toString());

  // Create a file for each top level component
  QFile file(mOutputFolder.filePath(name + ".dzn"));
  if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
  {
    LOG_WARNING("Failed to open device for writing");
    return code;
  }
  
  // Generate child code
  for (const auto& child : node.children)
    code += generateNode(*child);

  // Generate my structural code
  QString behaviour = "";
  behaviour += generateBehaviour(*node.behaviour);

  // Generate flows    
  for (const auto& f : node.flows)
  {
    LOG_DEBUG("Generating flow %s", qPrintable(f->name));
    // Find the start node
    for (const auto& n : f->nodes)
    {
      if (n->nodeId != "Generic::Start")
        continue;

      code += generateStart(node.properties[ConfigKeys::NAME].toString(), *n, *f, "    ");
      break;
    }
  }

  QTextStream out(&file);
    
  out << "interface i" + name + "\n";
  out << "{\n";

  for (const auto& event : node.flows)
  {
    QString args = "";
    out << "  " + Types::ConnectorTypeToString(event->type) + " " + Types::PropertyTypesToString(event->returnType) + " " + event->name + "(" + args + ");\n";  
  }
  
  // TODO(felaze): I really need to take some day to clean this and the widget codes.
  // - More widgets, maybe even one per property type
  // - The generation should use more inheritance, a lot of the code here is repeated
  for (const auto& child : node.children)
  {
    auto childName = fixCase(child->properties["name"].toString());
    out << "  requires i" + childName + " " + childName + ";\n";
  }
      
  out << "\n  behaviour\n";
  out << "  {\n";
  for (const auto& state : node.fields)
  {
    if (state.type == Types::PropertyTypes::ENUM)
    {
      out << "    enum " + state.options.at(0).defaultValue.toString() + " {\n";  
      QString enumValues = "";
      for (const auto& opt : state.options.at(1).options)
        enumValues += "      " + opt.id + ",\n";

      enumValues.chop(2);
      out << enumValues;
      out << "\n    };\n";

      out << "    " + state.options.at(0).defaultValue.toString() + " " + state.id + " = " + state.defaultValue.toString() + ";\n"; 
    }
    else
    {
      out << "    " + Types::PropertyTypesToString(state.type) + " " + state.id + " = " + state.defaultValue.toString() +  ";\n";       
    }
  }
  out << "\n";
  out << behaviour;
  out << "  }\n";
  out << "}\n";

  file.close();

  return code; 
}

QString DezyneGenerator::generateBehaviour(const FlowSaveInfo& flow)
{
  QString code = "";

  LOG_DEBUG("Generating behaviour");

  for (const auto& node : flow.nodes)
  {
    // Find the start node
    if (node->nodeId != "Generic::State")
      continue;
    
    Argument arg;
    mGeneratedIds.clear();
    
    LOG_DEBUG("Generating code for state: %s", qPrintable(node->properties["name"].toString()));
    code += "    [";
    auto info = node->properties["state"].toJsonObject();
    qDebug() << info;
    if (info[ConfigKeys::TYPE].toString() == Types::PropertyTypesToString(Types::PropertyTypes::ENUM))
      code += info["fields"].toString() + "." + info["events"].toString();
    else
      code += "aaah";
    
    code += "] {\n";
    for (const auto& transition : node->transitions)
    {
      QString name = fixCase(transition->event);
      if (transition->label == "optional")
        name = "optional";
      
      code += "      on " + name + " {\n";
      auto dst = findDestination(transition->dstId, flow);
      if (dst != nullptr)
        code += generateBehaviourNode(*dst, arg, flow, "        ");
      code += "      }\n";
    }
    code += "    }\n";
  }
  
  return code;
}

QString DezyneGenerator::generateState(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "";

  auto info = node.properties["state"].toJsonObject();
  qDebug() << info;
  if (info[ConfigKeys::TYPE].toString() == Types::PropertyTypesToString(Types::PropertyTypes::ENUM))
  {    
    code += format + info["fields"].toString() + " = " + info["events"].toString() + ";\n";
  }
  else
  {
    code += format + "blah";
  }

  mGeneratedIds.push_back(node.id);

  return code;
}

QString DezyneGenerator::generateStart(const QString& parent, const NodeSaveInfo& node, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "";
  code += format + "on " + fixCase(parent) + "." + fixCase(flow.name) + "(): {\n";

  Argument arg;
  code += generateTransitions(node, arg, flow, "  " + format);

  code += format + "}\n";
  return code;  
}

QString DezyneGenerator::generateEnd(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "";
  if (arg.name.isEmpty())
    return code;
  
  code += format + "reply(" + arg.name + ");\n";
  return code;  
}


QString DezyneGenerator::generateError(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "";
  if (arg.name.isEmpty())
    return code;
  
  code += format + "reply(" + arg.name + ");\n";
  return code;
}

QString DezyneGenerator::generateAction(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "";

  auto component = node.properties["component"].toJsonObject();
  std::shared_ptr<NodeSaveInfo> callee = mStorage->getNodeWithId(component["data_id"].toString());
  if (callee == nullptr)
  {
    LOG_WARNING("Could not find callee");
    return code;
  }

  std::shared_ptr<FlowSaveInfo> called = mStorage->getFlowWithId(component["option_data_id"].toString());
  if (called == nullptr)
  {
    LOG_WARNING("Could not find called");
    return code;
  }

  Argument returnValue = arg;
  code += format;
  if (called->returnType != Types::PropertyTypes::VOID)
  {
    code += Types::PropertyTypesToString(called->returnType) + " " + "valid" + " = ";
    returnValue.name = "valid";
  }
  
  code += fixCase(callee->properties["name"].toString()) + "." + fixCase(called->name) + "(";
  if (!called->arguments.isEmpty() && !arg.name.isEmpty())
    code += arg.name;
  
  code += ");\n";
  code += generateTransitions(node, returnValue, flow, format);

  return code;  
}

QString DezyneGenerator::generateCondition(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "";

  if (node.transitions.isEmpty())
    return code;
  
  auto ifTransition = node.transitions.at(0);
  auto ifNode = findDestination(ifTransition->dstId, flow);
  if (ifNode == nullptr)
    return code;

  auto condition = node.properties["condition"];
  code += format + "if (";

  if (condition.isValid() && !condition.isNull() && !condition.toString().isEmpty())
    code += condition.toString();
  else if (!arg.name.isEmpty())
    code += arg.name;
  
  code +=  ") {\n";
  code += generateBehaviourNode(*ifNode, arg, flow, "  " + format);

  if (node.transitions.size() == 1)
  {
    code += format + "}\n";
    return code;
  }

  auto elseTransition = node.transitions.at(1);
  auto elseNode = findDestination(elseTransition->dstId, flow);
  if (elseNode == nullptr)
    return code;
  
  code += format + "} else {\n";
  code += generateBehaviourNode(*elseNode, arg, flow, "  " + format);
  code += format + "}\n";

  return code;  
}

QString DezyneGenerator::generateAssign(const NodeSaveInfo& node, const Argument& arg, const FlowSaveInfo& flow, const QString& format)
{
  QString code = "";
  auto value = node.properties["state"];
  if (!value.isValid())
    return code;

  auto values = value.toJsonArray();
  for (const auto& val : values)
  {
    auto obj = val.toObject();    
    code += format + obj["variable"].toString() + " = " + obj["value"].toString() + "\n";
  }

  code += generateTransitions(node, arg, flow, format);
  
  return code;
}

std::shared_ptr<NodeSaveInfo> DezyneGenerator::findDestination(const QString& nodeId, const FlowSaveInfo& flow) const
{
  for (const auto& dst : flow.nodes)
  {
    if (dst->id == nodeId)
      return dst;
  }

  LOG_DEBUG("Could not find destination with id: %s", qPrintable(nodeId));
  return nullptr;
}

QString DezyneGenerator::fixCase(const QString& name)
{
  return name.toLower().replace(" ", "_");
}
