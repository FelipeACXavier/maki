#pragma once

#include <QJsonArray>
#include <QJsonObject>
#include <QPixmap>
#include <QPointF>
#include <QSizeF>

#include "logging.h"
#include "result.h"

/**
 * @brief A class representing a JSON object.
 */
class JSON : public QJsonObject
{
public:
  /**
   * @brief Constructs an empty JSON object.
   */
  JSON();

  /**
   * @brief Constructs a JSON object from another QJsonObject.
   *
   * @param other The QJsonObject to copy.
   */
  JSON(const QJsonObject& other);

  /**
   * @brief Creates a JSON object from a file.
   *
   * @param filename The path to the file.
   * @return Result<JSON> A result containing the JSON object or an error.
   */
  static Result<JSON> fromFile(const char* filename);

  /**
   * @brief Creates a JSON object from a file.
   *
   * @param filename The path to the file.
   * @return Result<JSON> A result containing the JSON object or an error.
   */
  static Result<JSON> fromFile(const QString& filename);

  /**
   * @brief Converts the JSON object to a string representation.
   *
   * @return QString The string representation of the JSON object.
   */
  QString print() const;

  /**
   * @brief Creates a JSON array from a QVariantList.
   *
   * @param value The QVariantList to convert.
   * @param separator The separator character.
   * @return QString The resulting string.
   */
  static QString fromArray(const QVariantList& value, char separator);

  /**
   * @brief Creates a JSON array from a QVariantList.
   *
   * @param value The QVariantList to convert.
   * @param separator The separator character.
   * @return QJsonArray The resulting QJsonArray.
   */
  static QJsonArray toArray(const QJsonValue& value, char separator);

  /**
   * @brief Creates a JSON object from a QPointF.
   *
   * @param value The QPointF to convert.
   * @return QJsonObject The resulting QJsonObject.
   */
  static QJsonObject fromPointF(const QPointF& value);

  /**
   * @brief Converts a QJsonObject to a QPointF.
   *
   * @param value The QJsonObject to convert.
   * @return QPointF The resulting QPointF.
   */
  static QPointF toPointF(const QJsonObject& value);

  /**
   * @brief Creates a JSON object from a QSizeF.
   *
   * @param value The QSizeF to convert.
   * @return QJsonObject The resulting QJsonObject.
   */
  static QJsonObject fromSizeF(const QSizeF& value);

  /**
   * @brief Converts a QJsonObject to a QSizeF.
   *
   * @param value The QJsonObject to convert.
   * @return QSizeF The resulting QSizeF.
   */
  static QSizeF toSizeF(const QJsonObject& value);

  /**
   * @brief Creates a JSON object from a QPixmap.
   *
   * @param value The QPixmap to convert.
   * @return QJsonObject The resulting QJsonObject.
   */
  static QJsonObject fromPixmap(const QPixmap& value);

  /**
   * @brief Converts a QJsonObject to a QPixmap.
   *
   * @param value The QJsonObject to convert.
   * @return QPixmap The resulting QPixmap.
   */
  static QPixmap toPixmap(const QJsonObject& value);
};
