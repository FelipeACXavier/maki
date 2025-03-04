#include "json.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>

JSON::JSON()
    : QJsonObject()
{
}

JSON::JSON(const QJsonObject& other)
    : QJsonObject(other)
{
}

Result<JSON> JSON::fromFile(const char* filename)
{
  return fromFile(QString(filename));
}

Result<JSON> JSON::fromFile(const QString& filename)
{
  QFile configFile(filename);

  if (!configFile.open(QFile::ReadOnly))
    return Result<JSON>::Failed("Failed to open configuration");

  QJsonDocument document = QJsonDocument::fromJson(configFile.readAll());
  return Result<JSON>(document.object());
}

QString JSON::print() const
{
  return QJsonDocument(*this).toJson(QJsonDocument::Indented);
}

QString JSON::fromArray(const QVariantList& value, char separator)
{
  QString out;
  for (const auto& v : value)
    out += v.toString() + separator;

  // Remove trailing separator
  out.chop(1);

  return out;
}

QJsonArray JSON::toArray(const QJsonValue& value, char separator)
{
  QJsonArray array;
  for (const auto& item : value.toString().split(separator))
    array.push_back(item.trimmed());

  return array;
}
