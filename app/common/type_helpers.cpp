#include "type_helpers.h"

#include <QJsonArray>

#include "json.h"
#include "keys.h"
#include "type_registry.h"

namespace maki
{
// -----------------------------------------------------------------------------------------------------------
// To JSON
QJsonObject qualifiedNameToJson(const koda::types::QualifiedName& name)
{
  QJsonObject json;
  QJsonArray namespaceArray;
  json["name"] = QString::fromStdString(name.name);
  for (const auto& s : name.namespacePath)
    namespaceArray.append(QString::fromStdString(s));

  json["namespace"] = namespaceArray;

  return json;
}

QJsonArray annotationsToJson(const koda::types::Annotations& annotations)
{
  QJsonArray out;
  for (const auto& [key, value] : annotations)
  {
    QJsonObject annotation;
    annotation["key"] = QString::fromStdString(key);
    annotation["value"] = QString::fromStdString(value);
    out.append(annotation);
  }
  return out;
}

QJsonObject typeReferenceToJson(const koda::types::TypeReference& reference)
{
  QJsonObject json;
  json["kind"] = QString::fromStdString(koda::types::toString(reference.kind()));

  if (reference.isList())
  {
    json["elementType"] = typeReferenceToJson(reference.elementType());
  }
  else if (reference.isOptional())
  {
    json["optionalValueType"] = typeReferenceToJson(reference.optionalValueType());
  }
  else if (reference.isPrimitive())
  {
    json["primitiveKind"] = QString::fromStdString(koda::types::toString(reference.primitiveKind()));
  }
  else if (reference.isNamed())
  {
    if (reference.namedType().id.has_value())
      json["id"] = QString::fromStdString(reference.namedType().id.value());

    json["qualifiedName"] = qualifiedNameToJson(reference.namedType().name);
  }
  else if (reference.isMap())
  {
    json["mapKeyType"] = typeReferenceToJson(reference.mapKeyType());
    json["mapValueType"] = typeReferenceToJson(reference.mapValueType());
  }

  return json;
}

QJsonObject typeDefinitionToJson(const koda::types::TypeDefinition& definition)
{
  QJsonObject json;
  json["kind"] = QString::fromStdString(koda::types::toString(definition.kind()));
  json["name"] = qualifiedNameToJson(definition.name);
  json["id"] = QString::fromStdString(definition.id);
  json["documentation"] = QString::fromStdString(definition.documentation);

  if (definition.isAlias())
  {
    json["target"] = typeReferenceToJson(definition.alias().target);
  }
  else if (definition.isEnum())
  {
    json["underlyingType"] = QString::fromStdString(koda::types::toString(definition.enumeration().underlyingType));
    QJsonArray enumValues;
    for (const auto& value : definition.enumeration().values)
    {
      QJsonObject object;
      object["name"] = QString::fromStdString(value.name);
      object["documentation"] = QString::fromStdString(value.documentation);
      object["annotations"] = annotationsToJson(value.annotations);

      if (value.value.has_value())
        object["value"] = QString::fromStdString(value.value.value());

      enumValues.append(object);
    }

    json["values"] = enumValues;
  }
  else if (definition.isPrimitive())
  {
    json["primitive"] = QString::fromStdString(koda::types::toString(definition.primitive().primitive));
  }
  else if (definition.isRecord())
  {
    if (definition.record().baseType.has_value())
      json["baseType"] = typeReferenceToJson(definition.record().baseType.value());

    QJsonArray fieldValues;
    for (const auto& value : definition.record().fields)
    {
      QJsonObject object;
      object["name"] = QString::fromStdString(value.name);
      object["documentation"] = QString::fromStdString(value.documentation);
      object["type"] = typeReferenceToJson(value.type);
      object["required"] = value.required;
      if (value.defaultValue)
        object["default"] = QString::fromStdString(value.defaultValue.value());
      object["annotations"] = annotationsToJson(value.annotations);

      fieldValues.append(object);
    }

    json["fields"] = fieldValues;
  }

  return json;
}

// -----------------------------------------------------------------------------------------------------------
// From JSON
Result<koda::types::QualifiedName> qualifiedNameFromJson(const QJsonObject& json, const std::string& nameSpace)
{
  if (!json.contains("name") || !json["name"].isString())
    return Result<koda::types::QualifiedName>::Failed("Qualified name must have a string name");

  koda::types::QualifiedName name;
  name.name = json["name"].toString().toStdString();

  if (!nameSpace.empty())
  {
    name.namespacePath.push_back(nameSpace);
    return name;
  }

  if (json.contains("namespace"))
  {
    if (!json["namespace"].isArray())
      return Result<koda::types::QualifiedName>::Failed("Qualified namespace must be an array");

    for (const auto& s : json["namespace"].toArray())
    {
      if (!s.isString())
        return Result<koda::types::QualifiedName>::Failed("Qualified namespace path must only have strings");

      name.namespacePath.push_back(s.toString().toStdString());
    }
  }

  return name;
}

Result<koda::types::Annotations> annotationsFromJson(const QJsonObject& json)
{
  if (!json.contains("annotations"))
    return koda::types::Annotations{};

  if (!json["annotations"].isArray())
    return Result<koda::types::Annotations>::Failed("Object has annotations but they are not an array");

  koda::types::Annotations annotations;
  for (const auto& object : json["annotations"].toArray())
  {
    if (!object.isObject())
      return Result<koda::types::Annotations>::Failed("Annotation must be a json object");

    QJsonObject obj = object.toObject();

    std::string key;
    std::string value;

    ASSIGN_OR_RETURN_ON_FAILURE_AS(key, JSON::returnOrFail<std::string>(obj, "key"), koda::types::Annotations);
    ASSIGN_OR_RETURN_ON_FAILURE_AS(value, JSON::returnOrFail<std::string>(obj, "value"), koda::types::Annotations);

    annotations.emplace(key, value);
  }

  return annotations;
}

Result<koda::types::TypeReference> typeReferenceFromJson(const QJsonObject& json, const std::string& nameSpace)
{
  using namespace koda::types;

  std::string kindString;
  ASSIGN_OR_RETURN_ON_FAILURE_AS(kindString, JSON::returnOrFail<std::string>(json, "kind"), TypeReference);
  auto kind = typeReferenceKindFromString(kindString);

  if (kind == TypeReferenceKind::List)
  {
    QJsonObject elementObject;
    TypeReference elementType;
    ASSIGN_OR_RETURN_ON_FAILURE_AS(elementObject, JSON::returnOrFail<QJsonObject>(json, "elementType"), TypeReference);
    ASSIGN_OR_RETURN_ON_FAILURE_AS(elementType, typeReferenceFromJson(elementObject, nameSpace), TypeReference);

    return TypeReference::list(elementType);
  }
  else if (kind == TypeReferenceKind::Optional)
  {
    QJsonObject elementObject;
    TypeReference elementType;
    ASSIGN_OR_RETURN_ON_FAILURE_AS(elementObject, JSON::returnOrFail<QJsonObject>(json, "optionalValueType"), TypeReference);
    ASSIGN_OR_RETURN_ON_FAILURE_AS(elementType, typeReferenceFromJson(elementObject, nameSpace), TypeReference);

    return TypeReference::optional(elementType);
  }
  else if (kind == TypeReferenceKind::Primitive)
  {
    std::string primitiveName;
    ASSIGN_OR_RETURN_ON_FAILURE_AS(primitiveName, JSON::returnOrFail<std::string>(json, "primitiveKind"), TypeReference);

    return TypeReference::primitive(primitiveKindFromString(primitiveName));
  }
  else if (kind == TypeReferenceKind::Named)
  {
    std::optional<TypeId> id;
    if (json.contains(ConfigKeys::ID) && json[ConfigKeys::ID].isString())
      id = json[ConfigKeys::ID].toString().toStdString();

    QJsonObject qnameObject;
    ASSIGN_OR_RETURN_ON_FAILURE_AS(qnameObject, JSON::returnOrFail<QJsonObject>(json, "qualifiedName"), TypeReference);

    QualifiedName qname;
    ASSIGN_OR_RETURN_ON_FAILURE_AS(qname, qualifiedNameFromJson(qnameObject), TypeReference);

    return TypeReference::named(qname, id);
  }
  else if (kind == TypeReferenceKind::Map)
  {
    QJsonObject keyObject, valueObject;
    TypeReference keyType, valueType;
    ASSIGN_OR_RETURN_ON_FAILURE_AS(keyObject, JSON::returnOrFail<QJsonObject>(json, "mapKeyType"), TypeReference);
    ASSIGN_OR_RETURN_ON_FAILURE_AS(valueObject, JSON::returnOrFail<QJsonObject>(json, "mapValueType"), TypeReference);
    ASSIGN_OR_RETURN_ON_FAILURE_AS(keyType, typeReferenceFromJson(keyObject, nameSpace), TypeReference);
    ASSIGN_OR_RETURN_ON_FAILURE_AS(valueType, typeReferenceFromJson(valueObject, nameSpace), TypeReference);

    return TypeReference::map(keyType, valueType);
  }

  return TypeReference{};
}

Result<koda::types::TypeDefinition> typeDefinitionFromJson(const QJsonObject& json, const std::string& nameSpace)
{
  using namespace koda::types;

  std::string kindString;
  ASSIGN_OR_RETURN_ON_FAILURE_AS(kindString, JSON::returnOrFail<std::string>(json, "kind"), TypeDefinition);
  auto kind = typeKindFromString(kindString);

  auto id = JSON::returnIf<std::string>(json, "id", "");                           // Optional
  auto defDocumentation = JSON::returnIf<std::string>(json, "documentation", "");  // Optional

  QualifiedName qname;
  Annotations defAnnotations;
  ASSIGN_OR_RETURN_ON_FAILURE_AS(qname, qualifiedNameFromJson(json["name"].toObject(), nameSpace), TypeDefinition);
  ASSIGN_OR_RETURN_ON_FAILURE_AS(defAnnotations, annotationsFromJson(json), TypeDefinition);

  if (kind == TypeKind::Primitive)
  {
    std::string primitiveName;
    ASSIGN_OR_RETURN_ON_FAILURE_AS(primitiveName, JSON::returnOrFail<std::string>(json, "primitive"), TypeDefinition);

    return TypeDefinition{
        .id = id,
        .name = qname,
        .data =
            PrimitiveTypeDefinition{
                .primitive = primitiveKindFromString(primitiveName),
            },
        .documentation = defDocumentation,
        .annotations = defAnnotations,
    };
  }
  else if (kind == TypeKind::Alias)
  {
    QJsonObject targetObject;
    ASSIGN_OR_RETURN_ON_FAILURE_AS(targetObject, JSON::returnOrFail<QJsonObject>(json, "target"), TypeDefinition);

    TypeReference target;
    ASSIGN_OR_RETURN_ON_FAILURE_AS(target, typeReferenceFromJson(json["target"].toObject(), nameSpace), TypeDefinition);

    return TypeDefinition{
        .id = id,
        .name = qname,
        .data = AliasTypeDefinition{.target = target},
        .documentation = defDocumentation,
        .annotations = defAnnotations,
    };
  }
  else if (kind == TypeKind::Enum)
  {
    std::string underlyingName;
    ASSIGN_OR_RETURN_ON_FAILURE_AS(underlyingName, JSON::returnOrFail<std::string>(json, "underlyingType"), TypeDefinition);

    std::vector<EnumValueDefinition> values;
    if (json.contains("value") && json["values"].isArray())
    {
      for (const auto& object : json["values"].toArray())
      {
        auto obj = object.toObject();

        std::string name;
        Annotations annotations;
        ASSIGN_OR_RETURN_ON_FAILURE_AS(name, JSON::returnOrFail<std::string>(obj, "name"), TypeDefinition);
        ASSIGN_OR_RETURN_ON_FAILURE_AS(annotations, annotationsFromJson(obj), TypeDefinition);

        std::string documentation = JSON::returnIf<std::string>(obj, "documentation", "");
        std::string value = JSON::returnIf<std::string>(obj, "value", "");

        values.push_back(EnumValueDefinition{
            .name = name,
            .value = value.empty() ? std::optional<std::string>() : value,
            .documentation = documentation,
            .annotations = annotations,
        });
      }
    }

    return TypeDefinition{
        .id = id,
        .name = qname,
        .data =
            EnumTypeDefinition{
                .underlyingType = enumKindFromString(underlyingName),
                .values = values,
            },
        .documentation = defDocumentation,
        .annotations = defAnnotations,
    };
  }
  else if (kind == TypeKind::Record)
  {
    std::optional<TypeReference> baseType;
    if (json.contains("baseType") && json["baseType"].isObject())
      ASSIGN_OR_RETURN_ON_FAILURE_AS(baseType, typeReferenceFromJson(json["baseType"].toObject(), nameSpace), TypeDefinition);

    std::vector<FieldDefinition> fields;
    if (json.contains("fields") && json["fields"].isArray())
    {
      for (const auto& object : json["fields"].toArray())
      {
        auto obj = object.toObject();

        std::string name;
        Annotations annotations;
        ASSIGN_OR_RETURN_ON_FAILURE_AS(name, JSON::returnOrFail<std::string>(obj, "name"), TypeDefinition);
        ASSIGN_OR_RETURN_ON_FAILURE_AS(annotations, annotationsFromJson(obj), TypeDefinition);

        bool required = JSON::returnIf<bool>(obj, "required", true);
        std::string documentation = JSON::returnIf<std::string>(obj, "documentation", "");

        std::optional<std::string> defaultValue;
        if (json.contains("default") && json["default"].isString())
          defaultValue = json["default"].toString().toStdString();

        TypeReference fieldType;
        QJsonObject fieldTypeObject;
        ASSIGN_OR_RETURN_ON_FAILURE_AS(fieldTypeObject, JSON::returnOrFail<QJsonObject>(obj, "type"), TypeDefinition);
        ASSIGN_OR_RETURN_ON_FAILURE_AS(fieldType, typeReferenceFromJson(fieldTypeObject, nameSpace), TypeDefinition);

        fields.push_back(FieldDefinition{
            .name = name,
            .type = fieldType,
            .defaultValue = defaultValue,
            .required = required,
            .documentation = documentation,
            .annotations = annotations,
        });
      }
    }

    return TypeDefinition{
        .id = id,
        .name = qname,
        .data =
            RecordTypeDefinition{
                .baseType = baseType,
                .fields = fields,
            },
        .documentation = defDocumentation,
        .annotations = defAnnotations,
    };
  }

  return Result<koda::types::TypeDefinition>::Failed("Unknown type {}", kindString);
}

// -----------------------------------------------------------------------------------------------------------
Types::PropertyTypes propertyTypeFromReference(const koda::types::TypeReference& reference)
{
  if (reference.isList())
    return Types::PropertyTypes::LIST;

  if (reference.isPrimitive())
  {
    const auto primitive = reference.primitiveKind();
    if (primitive == koda::types::PrimitiveKind::String)
      return Types::PropertyTypes::STRING;
    else if (primitive == koda::types::PrimitiveKind::Int64)
      return Types::PropertyTypes::INTEGER;
    else if (primitive == koda::types::PrimitiveKind::Float64)
      return Types::PropertyTypes::REAL;
    else if (primitive == koda::types::PrimitiveKind::Bool)
      return Types::PropertyTypes::BOOLEAN;
    else if (primitive == koda::types::PrimitiveKind::Void)
      return Types::PropertyTypes::VOID;
  }

  if (reference.isNamed())
  {
    const auto* def = maki::TypeRegistry::instance().findByName(reference.namedType().name);
    if (def)
    {
      def->print();
      if (def->isPrimitive())
        return propertyTypeFromReference(def->toReference());
      else if (def->isAlias())
        return propertyTypeFromReference(def->alias().target);
    }
  }

  return Types::PropertyTypes::UNKNOWN;
}

koda::types::TypeReference propertyTypeFromReference(const Types::PropertyTypes& type)
{
  if (type == Types::PropertyTypes::STRING)
    return koda::types::TypeReference::primitive(koda::types::PrimitiveKind::String);
  if (type == Types::PropertyTypes::INTEGER)
    return koda::types::TypeReference::primitive(koda::types::PrimitiveKind::Int64);
  if (type == Types::PropertyTypes::REAL)
    return koda::types::TypeReference::primitive(koda::types::PrimitiveKind::Float64);
  if (type == Types::PropertyTypes::BOOLEAN)
    return koda::types::TypeReference::primitive(koda::types::PrimitiveKind::Bool);
  if (type == Types::PropertyTypes::LIST)
    return koda::types::TypeReference::list(koda::types::TypeReference::primitive(koda::types::PrimitiveKind::Void));
  if (type == Types::PropertyTypes::VOID)
    return koda::types::TypeReference::primitive(koda::types::PrimitiveKind::Void);

  return koda::types::TypeReference{};
}

}  // namespace maki
