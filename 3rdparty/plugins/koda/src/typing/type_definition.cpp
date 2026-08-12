#include "type_definition.h"

#include <algorithm>
#include <stdexcept>
#include <unordered_set>
#include <utility>

#include "logging.h"

namespace koda::types
{

template <typename T>
const T& getValue(const TypeDefinitionData& data, const std::string& expectedType)
{
  const auto* value = std::get_if<T>(&data);
  if (value == nullptr)
    throw std::logic_error("Type definition is not a " + expectedType);

  return *value;
}

template <typename T>
T& getValue(TypeDefinitionData& data, const std::string& expectedType)
{
  auto* value = std::get_if<T>(&data);
  if (value == nullptr)
    throw std::logic_error("Type definition is not a " + expectedType);

  return *value;
}

bool containsWhitespace(const std::string& value)
{
  return std::any_of(value.begin(), value.end(), [](unsigned char character) { return std::isspace(character) != 0; });
}

// ===========================================================================================================
// TypeDefinition
TypeDefinition TypeDefinition::createPrimitive(const std::string& name, PrimitiveKind kind, const std::string& id)
{
  const auto qname = QualifiedName(name);
  TypeDefinition type{
      .id = id.empty() ? qname.toId() : id,
      .name = qname,
      .data = PrimitiveTypeDefinition{.primitive = kind},
  };

  return type;
}

TypeDefinition TypeDefinition::createAlias(const std::string& name, const QualifiedName& alias, const std::string& id)
{
  const auto qname = QualifiedName(name);
  TypeDefinition type{
      .id = id.empty() ? qname.toId() : id,
      .name = qname,
      .data = AliasTypeDefinition{.target = TypeReference::named(alias.name, alias.toId())},
  };

  return type;
}

TypeDefinition TypeDefinition::createAlias(const std::string& name, const TypeReference& alias, const std::string& id)
{
  const auto qname = QualifiedName(name);
  TypeDefinition type{
      .id = id.empty() ? qname.toId() : id,
      .name = qname,
      .data = AliasTypeDefinition{.target = alias},
  };

  return type;
}

TypeDefinition TypeDefinition::createRecord(const std::string& name, const std::map<std::string, QualifiedName>& fields, const std::string& base,
                                            const std::string& id)
{
  const auto qname = QualifiedName(name);
  std::vector<FieldDefinition> ifields;
  for (const auto& [fname, ftype] : fields)
    ifields.push_back(FieldDefinition{
        .name = fname,
        .type = TypeReference::named(ftype, ftype.toId()),
    });

  TypeDefinition type{
      .id = id.empty() ? qname.toId() : id,
      .name = qname,
      .data =
          RecordTypeDefinition{
              .baseType = base.empty() ? std::nullopt : std::optional<TypeReference>(TypeReference::named(QualifiedName(base))),
              .fields = ifields,
          },
  };

  return type;
}

TypeDefinition TypeDefinition::createList(const std::string& name, const QualifiedName& listType, const std::string& id)
{
  const auto qname = QualifiedName(name);
  TypeDefinition type{
      .id = id.empty() ? qname.toId() : id,
      .name = qname,
      .data = AliasTypeDefinition{.target = TypeReference::list(TypeReference::named(listType, listType.toId()))},
  };

  return type;
}

TypeDefinition TypeDefinition::createMap(const std::string& name, const QualifiedName& keyType, const QualifiedName& fieldType, const std::string& id)
{
  const auto qname = QualifiedName(name);
  TypeDefinition type{
      .id = id.empty() ? qname.toId() : id,
      .name = qname,
      .data = AliasTypeDefinition{.target = TypeReference::map(TypeReference::named(keyType, keyType.toId()),
                                                               TypeReference::named(fieldType, fieldType.toId()))},
  };

  return type;
}

std::optional<std::string> toEnumValue(EnumUnderlyingKind kind, const std::optional<std::string>& value)
{
  if (!value.has_value())
    return std::nullopt;

  return kind == EnumUnderlyingKind::String ? "\"" + value.value() + "\"" : value.value();
}

TypeDefinition TypeDefinition::createEnum(const std::string& name, EnumUnderlyingKind kind,
                                          const std::map<std::string, std::optional<std::string>>& fields, const std::string& id)
{
  const auto qname = QualifiedName(name);
  std::vector<EnumValueDefinition> values;
  for (const auto& [fname, ftype] : fields)
  {
    values.push_back(EnumValueDefinition{
        .name = fname,
        .value = toEnumValue(kind, ftype),
    });
  }

  TypeDefinition type{
      .id = id.empty() ? qname.toId() : id,
      .name = qname,
      .data = EnumTypeDefinition{.underlyingType = kind, .values = values},
  };

  return type;
}

TypeKind TypeDefinition::kind() const
{
  if (std::holds_alternative<PrimitiveTypeDefinition>(data))
    return TypeKind::Primitive;
  else if (std::holds_alternative<RecordTypeDefinition>(data))
    return TypeKind::Record;
  else if (std::holds_alternative<EnumTypeDefinition>(data))
    return TypeKind::Enum;
  else
    return TypeKind::Alias;
}

bool TypeDefinition::isPrimitive() const
{
  return kind() == TypeKind::Primitive;
}

bool TypeDefinition::isRecord() const
{
  return kind() == TypeKind::Record;
}

bool TypeDefinition::isEnum() const
{
  return kind() == TypeKind::Enum;
}

bool TypeDefinition::isAlias() const
{
  return kind() == TypeKind::Alias;
}

const PrimitiveTypeDefinition& TypeDefinition::primitive() const
{
  return getValue<PrimitiveTypeDefinition>(data, "primitive");
}

PrimitiveTypeDefinition& TypeDefinition::primitive()
{
  return getValue<PrimitiveTypeDefinition>(data, "primitive");
}

const RecordTypeDefinition& TypeDefinition::record() const
{
  return getValue<RecordTypeDefinition>(data, "record");
}

RecordTypeDefinition& TypeDefinition::record()
{
  return getValue<RecordTypeDefinition>(data, "record");
}

const EnumTypeDefinition& TypeDefinition::enumeration() const
{
  return getValue<EnumTypeDefinition>(data, "enum");
}

EnumTypeDefinition& TypeDefinition::enumeration()
{
  return getValue<EnumTypeDefinition>(data, "enum");
}

const AliasTypeDefinition& TypeDefinition::alias() const
{
  return getValue<AliasTypeDefinition>(data, "alias");
}

AliasTypeDefinition& TypeDefinition::alias()
{
  return getValue<AliasTypeDefinition>(data, "alias");
}

std::vector<TypeModelDiagnostic> TypeDefinition::validate() const
{
  std::vector<TypeModelDiagnostic> diagnostics;
  const std::string typePath = name.toString();

  if (id.empty())
    addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error, "type.empty_id", "The type has no stable ID.", typePath);

  if (name.name.empty())
    addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error, "type.empty_name", "The type has no name.", typePath);
  else if (containsWhitespace(name.name))
    addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error, "type.invalid_name", "The type name contains whitespace.", typePath);

  for (const auto& namespacePart : name.namespacePath)
    if (namespacePart.empty())
      addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error, "type.empty_namespace_part", "The type namespace contains an empty component.",
                    typePath);
    else if (containsWhitespace(namespacePart))
      addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error, "type.invalid_namespace_part",
                    "A type namespace component contains whitespace.", typePath);

  if (isRecord())
  {
    std::unordered_set<std::string> fieldNames;
    for (const FieldDefinition& field : record().fields)
    {
      const std::string fieldPath = typePath.empty() ? field.name : typePath + "." + field.name;

      if (field.name.empty())
      {
        addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error, "record.empty_field_name", "The record contains a field without a name.",
                      fieldPath);

        continue;
      }

      if (containsWhitespace(field.name))
      {
        addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error, "record.invalid_field_name", "The field name contains whitespace.",
                      fieldPath);
      }

      if (!fieldNames.insert(field.name).second)
      {
        addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error, "record.duplicate_field",
                      "The record contains more than one field named '" + field.name + "'.", fieldPath);
      }

      if (!field.required && field.defaultValue.has_value())
      {
        addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Warning, "record.optional_field_with_default",
                      "The optional field has a default value. Ensure the distinction "
                      "between an absent value and the default value is intentional.",
                      fieldPath);
      }
    }
  }

  if (isEnum())
  {
    std::unordered_set<std::string> enumValueNames;
    std::unordered_set<std::string> explicitValues;

    for (const EnumValueDefinition& enumValue : enumeration().values)
    {
      const std::string valuePath = typePath.empty() ? enumValue.name : typePath + "." + enumValue.name;
      if (enumValue.name.empty())
      {
        addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error, "enum.empty_value_name", "The enum contains a value without a name.",
                      valuePath);

        continue;
      }

      if (!enumValueNames.insert(enumValue.name).second)
      {
        addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error, "enum.duplicate_value_name",
                      "The enum contains more than one value named '" + enumValue.name + "'.", valuePath);
      }

      if (enumValue.value.has_value() && !explicitValues.insert(*enumValue.value).second)
      {
        addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Warning, "enum.duplicate_explicit_value",
                      "More than one enum item uses the explicit value " + *enumValue.value + ".", valuePath);
      }
    }

    if (enumeration().values.empty())
      addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Warning, "enum.empty", "The enum contains no values.", typePath);
  }

  return diagnostics;
}

void TypeDefinition::print(const std::string& indent) const
{
  LOG_DEBUG("{}Type Id: {} Name: {}:", indent, id, name.toString());
  if (isPrimitive())
  {
    LOG_DEBUG("{} Primitive: {}", indent, toString(primitive().primitive));
  }
  else if (isRecord())
  {
    LOG_DEBUG("{}  Record: {}", indent, record().baseType.value_or(TypeReference::named("No type")).toString());
    for (const auto& field : record().fields)
      LOG_DEBUG("{}    {} {} {}", indent, field.name, field.type.toString(), field.defaultValue.value_or("No default"));
  }
  else if (isEnum())
  {
    LOG_DEBUG("{}  Enum: {}", indent, toString(enumeration().underlyingType));
    for (const auto& field : enumeration().values)
      LOG_DEBUG("{}    {} {}", indent, field.name, field.value.value_or("No value"));
  }
  else if (isAlias())
  {
    LOG_DEBUG("{}    {}", indent, alias().target.toString());
  }
}

}  // namespace koda::types