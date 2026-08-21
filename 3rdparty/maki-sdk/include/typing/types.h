#pragma once

#include <map>
#include <string>
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
  Unknown = 0,
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
  Duration,
  Void
};

enum class TypeKind
{
  Unknown = 0,
  Primitive,
  Record,
  Enum,
  Alias
};

enum class TypeReferenceKind
{
  Unknown = 0,
  Primitive,
  Named,
  List,
  Optional,  // TODO: Remove
  Map
};

enum class EnumUnderlyingKind
{
  Unknown,
  Int32,
  UInt32,
  String
};

// The default, builtin types
static const std::string BooleanType = "builtin::Bool";
static const std::string IntegerType = "builtin::Integer";
static const std::string UnsignedType = "builtin::Unsigned";
static const std::string RealType = "builtin::Real";
static const std::string StringType = "builtin::String";
static const std::string BytesType = "builtin::Bytes";
static const std::string TimestampType = "builtin::Timestamp";
static const std::string DurationType = "builtin::Duration";
static const std::string VoidType = "builtin::Void";
static const std::string NAMESPACE_SEPARATOR = "::";

static const std::string BooleanBase = "bool";
static const std::string IntegerBase = "int";
static const std::string UnsignedBase = "uint";
static const std::string RealBase = "real";
static const std::string StringBase = "string";
static const std::string BytesBase = "bytes";
static const std::string TimestampBase = "timestamp";
static const std::string DurationBase = "duration";
static const std::string VoidBase = "void";

struct QualifiedName
{
  QualifiedName() = default;
  QualifiedName(const std::string& value, const std::string& separator = NAMESPACE_SEPARATOR);

  std::vector<std::string> namespacePath;
  std::string name;

  bool empty() const;
  std::string toId() const;
  std::string namespaceString(const std::string& separator = NAMESPACE_SEPARATOR) const;
  std::string toString(const std::string& separator = NAMESPACE_SEPARATOR) const;

  auto operator<=>(const QualifiedName&) const = default;
};
}  // namespace koda::types