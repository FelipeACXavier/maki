#include "json_plugin.h"

QString JSONGenerator::generateCode(const QList<INode*>& items)
{
  QString code = "{\n";
  for (const auto& item : items)
  {
    for (const auto& field : item->fields())
    {
      if (field.type == Types::PropertyTypes::STRING)
      {
        code += QString("\"%1\": \"%2\",\n").arg(field.id, field.defaultValue.toString());
      }
      else if (field.type == Types::PropertyTypes::REAL)
      {
        code += QString("\"%1\": %2,\n").arg(field.id, field.defaultValue.toString());
      }
      else if (field.type == Types::PropertyTypes::INTEGER)
      {
        code += QString("\"%1\": %2,\n").arg(field.id, field.defaultValue.toString());
      }
    }

    code += generateCode(item->children());
  }

  code.chop(1);

  return code + "}\n";
}

generator::Language JSONGenerator::supportedLanguage() const
{
  return generator::Language::JSON;
}

QString JSONGenerator::languageName() const
{
  return "JSON";
}
