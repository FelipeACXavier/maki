#pragma once

#include <QJsonObject>

#include "result.h"

class JSON : public QJsonObject
{
public:
  JSON();
  JSON(const QJsonObject& other);

  static Result<JSON> fromFile(const char* filename);
  static Result<JSON> fromFile(const QString& filename);

  QString print() const;

  static QJsonArray toArray(const QJsonValue& value, char separator);
};
