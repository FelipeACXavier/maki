#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "helpers.h"
#include "type_reference.h"

namespace koda::types
{

struct FieldDefinition
{
  std::string name;
  TypeReference type;

  // A source-level default expression. The shared type model does not attempt
  // to interpret values because KODA and MAKI may represent expressions
  // differently.
  std::optional<std::string> defaultValue;

  bool required = true;
  std::string documentation;
  Annotations annotations;

  bool operator==(const FieldDefinition&) const = default;
};

struct PrimitiveTypeDefinition
{
  PrimitiveKind primitive;
  bool operator==(const PrimitiveTypeDefinition&) const = default;
};

struct RecordTypeDefinition
{
  // A record may inherit from one other record initially. This can later be
  // changed to a vector if multiple inheritance becomes a real requirement.
  std::optional<TypeReference> baseType;
  std::vector<FieldDefinition> fields;
  bool operator==(const RecordTypeDefinition&) const = default;
};

struct EnumValueDefinition
{
  std::string name;

  // Stored as text so integer-backed and string-backed enums share one model.
  //
  // Examples:
  //   "0"
  //   "42"
  //   "\"driving\""
  std::optional<std::string> value;

  std::string documentation;
  Annotations annotations;

  bool operator==(const EnumValueDefinition&) const = default;
};

struct EnumTypeDefinition
{
  EnumUnderlyingKind underlyingType = EnumUnderlyingKind::Int32;
  std::vector<EnumValueDefinition> values;

  bool operator==(const EnumTypeDefinition&) const = default;
};

struct AliasTypeDefinition
{
  TypeReference target;

  bool operator==(const AliasTypeDefinition&) const = default;
};

using TypeDefinitionData = std::variant<PrimitiveTypeDefinition, RecordTypeDefinition, EnumTypeDefinition, AliasTypeDefinition>;

struct TypeDefinition
{
  TypeId id;
  QualifiedName name;
  TypeDefinitionData data;

  std::string documentation;
  Annotations annotations;

  TypeKind kind() const;

  bool isPrimitive() const;
  bool isRecord() const;
  bool isEnum() const;
  bool isAlias() const;

  const PrimitiveTypeDefinition& primitive() const;
  PrimitiveTypeDefinition& primitive();

  const RecordTypeDefinition& record() const;
  RecordTypeDefinition& record();

  const EnumTypeDefinition& enumeration() const;
  EnumTypeDefinition& enumeration();

  const AliasTypeDefinition& alias() const;
  AliasTypeDefinition& alias();

  static TypeDefinition createPrimitive(const std::string& name, PrimitiveKind kind);
  static TypeDefinition createAlias(const std::string& name, const QualifiedName& alias);
  static TypeDefinition createRecord(const std::string& name, const std::map<std::string, QualifiedName>& fields, const std::string& base = "");
  static TypeDefinition createMap(const std::string& name, const QualifiedName& keyType, const QualifiedName& fieldType);
  static TypeDefinition createList(const std::string& name, const QualifiedName& listType);
  static TypeDefinition createEnum(const std::string& name, EnumUnderlyingKind kind, const std::map<std::string, std::optional<std::string>>& fields);

  // Performs validation that only requires one definition.
  //
  // Cross-type checks such as unresolved references, inheritance cycles and alias
  // cycles belong in TypeRegistry because they require access to other types.
  std::vector<TypeModelDiagnostic> validate() const;

  void print(const std::string& indent = "") const;

  bool operator==(const TypeDefinition&) const = default;
};

}  // namespace koda::types