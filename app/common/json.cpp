#include "json.h"

#include <QBuffer>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>

#include "keys.h"
#include "types.h"

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

  configFile.close();

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

QJsonObject JSON::fromPointF(const QPointF& value)
{
  QJsonObject data;
  data[ConfigKeys::X] = value.x();
  data[ConfigKeys::Y] = value.y();

  return data;
}

QPointF JSON::toPointF(const QJsonObject& data)
{
  return {data[ConfigKeys::X].toDouble(), data[ConfigKeys::Y].toDouble()};
}

QJsonObject JSON::fromSizeF(const QSizeF& value)
{
  QJsonObject data;
  data[ConfigKeys::WIDTH] = value.width();
  data[ConfigKeys::HEIGHT] = value.height();

  return data;
}

QSizeF JSON::toSizeF(const QJsonObject& data)
{
  return {data[ConfigKeys::WIDTH].toDouble(), data[ConfigKeys::HEIGHT].toDouble()};
}

QJsonObject JSON::fromPixmap(const QPixmap& value)
{
  QByteArray pixmapData;
  QBuffer buffer(&pixmapData);
  buffer.open(QIODevice::WriteOnly);
  value.save(&buffer, Types::PIXMAP);

  QJsonObject data;
  data[ConfigKeys::DATA] = QString::fromLatin1(pixmapData.toBase64());

  return data;
}

QPixmap JSON::toPixmap(const QJsonObject& data)
{
  QString base64Data = data[ConfigKeys::DATA].toString();
  QByteArray byteArray = QByteArray::fromBase64(base64Data.toLatin1());

  QPixmap pixmap;
  pixmap.loadFromData(byteArray, Types::PIXMAP);

  return pixmap;
}
