#pragma once

#include <QJsonArray>
#include <QJsonObject>

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

}  // namespace maki
