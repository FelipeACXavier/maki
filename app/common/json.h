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

  template <class T>
  static T returnIf(const QJsonObject& object, const QString& key, T defaultValue = {})
  {
    if (!object.contains(key))
      return defaultValue;

    const QJsonValue value = object.value(key);
    if constexpr (std::is_same_v<T, std::string>)
      return value.toString().toStdString();
    else if constexpr (std::is_same_v<T, QString>)
      return value.toString();
    else if constexpr (std::is_same_v<T, bool>)
      return value.toBool();
    else if constexpr (std::is_same_v<T, int>)
      return value.toInt();
    else if constexpr (std::is_same_v<T, double>)
      return value.toDouble();
    else if constexpr (std::is_same_v<T, QJsonObject>)
      return value.toObject();
    else if constexpr (std::is_same_v<T, QJsonArray>)
      return value.toArray();
    else
      static_assert(std::is_same_v<T, void>, "Unsupported JSON type");
  }

  template <class T>
  static Result<T> returnOrFail(const QJsonObject& object, const QString& key)
  {
    if (!object.contains(key))
      return Result<T>::Failed("{} is not available", key);

    const QJsonValue value = object.value(key);
    if constexpr (std::is_same_v<T, std::string>)
      return value.toString().toStdString();
    else if constexpr (std::is_same_v<T, QString>)
      return value.toString();
    else if constexpr (std::is_same_v<T, bool>)
      return value.toBool();
    else if constexpr (std::is_same_v<T, int>)
      return value.toInt();
    else if constexpr (std::is_same_v<T, double>)
      return value.toDouble();
    else if constexpr (std::is_same_v<T, QJsonObject>)
      return value.toObject();
    else if constexpr (std::is_same_v<T, QJsonArray>)
      return value.toArray();

    return Result<T>::Failed("Unknown value type for key: {}", key);
  }
};
