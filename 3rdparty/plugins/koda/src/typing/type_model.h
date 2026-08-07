#pragma once

#include <map>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace koda::types
{
// Stable identity of a user-defined type.
//
// The editor should generate this once when the type is created. References use
// the ID rather than only the name, meaning a type can later be renamed without
// breaking every reference to it.
using TypeId = std::string;

// Additional information that does not affect the semantics of the type.
//
// Examples:
//   ros.message = "geometry_msgs/msg/Pose"
//   robotics.semantic_type = "Pose"
//   editor.color = "#ff00ff"
using Annotations = std::map<std::string, std::string>;

enum class PrimitiveKind
{
  Bool,
  Int8,
  Int16,
  Int32,
  Int64,
  UInt8,
  UInt16,
  UInt32,
  UInt64,
  Float32,
  Float64,
  String,
  Bytes,
  Timestamp,
  Duration
};

enum class TypeKind
{
  Primitive,
  Record,
  Enum,
  Alias
};

enum class TypeReferenceKind
{
  Primitive,
  Named,
  List,
  Optional,  // TODO: Remove
  Map
};

enum class EnumUnderlyingKind
{
  Int32,
  UInt32,
  String
};

struct QualifiedName
{
  QualifiedName(const std::string& value, const std::string& separator = "::");

  std::vector<std::string> namespacePath;
  std::string name;

  bool empty() const;
  std::string toString(const std::string& separator = "::") const;

  auto operator<=>(const QualifiedName&) const = default;
};

class TypeReference
{
public:
  struct Primitive
  {
    PrimitiveKind kind;
    auto operator<=>(const Primitive&) const = default;
  };

  struct Named
  {
    // The ID is authoritative when available.
    //
    // name is retained as a human-readable fallback and is also useful while
    // parsing unresolved source code.
    std::optional<TypeId> id;
    QualifiedName name;

    auto operator<=>(const Named&) const = default;
  };

  struct List
  {
    // Indices into an internal node store are intentionally avoided. A nested
    // TypeReference is represented recursively through the implementation.
    std::size_t elementNode;

    auto operator<=>(const List&) const = default;
  };

  struct Optional
  {
    std::size_t valueNode;

    auto operator<=>(const Optional&) const = default;
  };

  struct Map
  {
    std::size_t keyNode;
    std::size_t valueNode;

    auto operator<=>(const Map&) const = default;
  };

  TypeReference() = delete;

  static TypeReference primitive(PrimitiveKind kind);
  static TypeReference named(QualifiedName name, std::optional<TypeId> id = std::nullopt);
  static TypeReference named(std::string name, std::optional<TypeId> id = std::nullopt);
  static TypeReference list(TypeReference elementType);
  static TypeReference optional(TypeReference valueType);
  static TypeReference map(TypeReference keyType, TypeReference valueType);

  TypeReferenceKind kind() const;

  bool isPrimitive() const;
  bool isNamed() const;
  bool isList() const;
  bool isOptional() const;
  bool isMap() const;

  PrimitiveKind primitiveKind() const;
  const Named& namedType() const;

  TypeReference elementType() const;
  TypeReference optionalValueType() const;
  TypeReference mapKeyType() const;
  TypeReference mapValueType() const;

  std::string toString() const;

  bool structurallyEquals(const TypeReference& other) const;

  bool operator==(const TypeReference& other) const;
  bool operator!=(const TypeReference& other) const;

private:
  struct Node
  {
    using Value = std::variant<Primitive, Named, List, Optional, Map>;

    Value value;
  };

  explicit TypeReference(std::vector<Node> nodes, std::size_t rootNode);

  static std::size_t appendTree(std::vector<Node>& target, const TypeReference& source);

  const Node& root() const;
  TypeReference subtree(std::size_t rootNode) const;

  std::vector<Node> mNodes;
  std::size_t mRootNode = 0;
};

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

using TypeDefinitionData = std::variant<
    PrimitiveTypeDefinition,
    RecordTypeDefinition,
    EnumTypeDefinition,
    AliasTypeDefinition>;

struct TypeModelDiagnostic
{
  enum class Severity
  {
    Warning,
    Error
  };

  Severity severity = Severity::Error;
  std::string code;
  std::string message;

  // Optional semantic path, for example:
  //   robotics::Pose.position
  //   robotics::Status.values.Driving
  std::string path;

  bool operator==(const TypeModelDiagnostic&) const = default;
};

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

  // Performs validation that only requires one definition.
  //
  // Cross-type checks such as unresolved references, inheritance cycles and alias
  // cycles belong in TypeRegistry because they require access to other types.
  std::vector<TypeModelDiagnostic> validate() const;

  bool operator==(const TypeDefinition&) const = default;
};

std::string toString(PrimitiveKind kind);
std::string toString(TypeKind kind);
std::string toString(TypeReferenceKind kind);
std::string toString(EnumUnderlyingKind kind);

}  // namespace koda::types