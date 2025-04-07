#pragma once

#include <QJsonObject>
#include <QPixmap>
#include <QPointF>
#include <QSizeF>

#include "result.h"

class JSON : public QJsonObject
{
public:
  JSON();
  JSON(const QJsonObject& other);

  static Result<JSON> fromFile(const char* filename);
  static Result<JSON> fromFile(const QString& filename);

  QString print() const;

  static QString fromArray(const QVariantList& value, char separator);
  static QJsonArray toArray(const QJsonValue& value, char separator);

  static QJsonObject fromPointF(const QPointF& value);
  static QPointF toPointF(const QJsonObject& value);

  static QJsonObject fromSizeF(const QSizeF& value);
  static QSizeF toSizeF(const QJsonObject& value);

  static QJsonObject fromPixmap(const QPixmap& value);
  static QPixmap toPixmap(const QJsonObject& value);
};
