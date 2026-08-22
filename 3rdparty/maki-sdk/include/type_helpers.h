#pragma once

#include <QJsonArray>
#include <QJsonObject>

#include "logging.h"
#include "result.h"
#include "types.h"
#include "typing/type_definition.h"

namespace maki
{

QJsonObject qualifiedNameToJson(const koda::types::QualifiedName& name);
QJsonArray annotationsToJson(const koda::types::Annotations& annotations);
QJsonObject typeReferenceToJson(const koda::types::TypeReference& reference);
QJsonObject typeDefinitionToJson(const koda::types::TypeDefinition& definition);

Result<koda::types::Annotations> annotationsFromJson(const QJsonObject& json);
Result<koda::types::QualifiedName> qualifiedNameFromJson(const QJsonObject& json, const std::string& nameSpace = "");
Result<koda::types::TypeReference> typeReferenceFromJson(const QJsonObject& json, const std::string& nameSpace = "");
Result<koda::types::TypeDefinition> typeDefinitionFromJson(const QJsonObject& json, const std::string& nameSpace = "");

Types::PropertyTypes propertyTypeFromReference(const koda::types::TypeReference& reference);
koda::types::TypeReference propertyTypeFromReference(const Types::PropertyTypes& type);

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

}  // namespace maki

namespace koda::types
{
QDataStream& operator<<(QDataStream& out, const koda::types::TypeReference& type);
QDataStream& operator>>(QDataStream& in, koda::types::TypeReference& type);
}  // namespace koda::types