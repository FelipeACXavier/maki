#include "dezyne_generator.h"

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "inode.h"

static const QString FOLDER = "/generated";

QString DezyneGenerator::generateCode(const QList<NodeSaveInfo>& nodes)
{
  qDebug() << "Starting generation";

  mOutputFolder = QDir(QDir::currentPath() + FOLDER);
  if (!mOutputFolder.exists())
    mOutputFolder.mkpath(".");

  QString code = "";
  for (const auto& node : nodes)
  {
    // TODO(felaze): Create file at this level
    qDebug() << "Generating code for top level node" << node.properties["name"] << node.nodeId << node.children.size();

    code += generateChildCode(node.children);
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

QString DezyneGenerator::generateChildCode(const QVector<std::shared_ptr<NodeSaveInfo>>& nodes)
{
  qDebug() << "Generating child";
  QString code = "";
  for (const auto& node : nodes)
  {
    code += generateNode(*node);
    code += generateChildCode(node->children);
  }

  return code;
}

QString DezyneGenerator::generateNode(const NodeSaveInfo& node)
{
  QString code = "";
  QString type = node.nodeId;
  QString name = node.properties["name"].toString();
  qDebug() << "Generating code for" << type;

  if (type == "Utilities::Timer")
  {
    // This is a default component with its own interface
    QFile file(mOutputFolder.filePath(name + ".dzn"));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
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
  }
  else if (type == "Utilities::Authenticator")
  {
    QFile file(mOutputFolder.filePath(name + ".dzn"));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
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
  }
  else if (type == "Utilities::Siren")
  {
    QFile file(mOutputFolder.filePath(name + ".dzn"));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
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
  }
  else if (type == "Utilities::Presence sensor")
  {
    QFile file(mOutputFolder.filePath(name + ".dzn"));
    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
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
  }

  return code;
}
