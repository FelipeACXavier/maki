#include "typing/helpers.h"

#include <array>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>

namespace koda::types
{
void addDiagnostic(std::vector<TypeModelDiagnostic>& diagnostics, TypeModelDiagnostic::Severity severity, const std::string& code,
                   const std::string& message, const std::string& path)
{
  diagnostics.push_back(TypeModelDiagnostic{
      .severity = severity,
      .code = std::move(code),
      .message = std::move(message),
      .path = std::move(path),
  });
}

bool isBuiltin(QualifiedName qname)
{
  auto name = qname.toString();
  return name == IntegerType || name == UnsignedType || name == RealType || name == StringType || name == BytesType || name == TimestampType ||
         name == DurationType || name == VoidType;
}

bool isSignedInteger(PrimitiveKind kind)
{
  switch (kind)
  {
    case PrimitiveKind::Int8:
    case PrimitiveKind::Int16:
    case PrimitiveKind::Int32:
    case PrimitiveKind::Int64:
      return true;

    default:
      return false;
  }
}

bool isUnsignedInteger(PrimitiveKind kind)
{
  switch (kind)
  {
    case PrimitiveKind::UInt8:
    case PrimitiveKind::UInt16:
    case PrimitiveKind::UInt32:
    case PrimitiveKind::UInt64:
      return true;

    default:
      return false;
  }
}

bool isInteger(PrimitiveKind kind)
{
  return isSignedInteger(kind) || isUnsignedInteger(kind);
}

bool isFloatingPoint(PrimitiveKind kind)
{
  return kind == PrimitiveKind::Float32 || kind == PrimitiveKind::Float64;
}

bool isNumeric(PrimitiveKind kind)
{
  return isInteger(kind) || isFloatingPoint(kind);
}

int integerWidth(PrimitiveKind kind)
{
  switch (kind)
  {
    case PrimitiveKind::Int8:
    case PrimitiveKind::UInt8:
      return 8;

    case PrimitiveKind::Int16:
    case PrimitiveKind::UInt16:
      return 16;

    case PrimitiveKind::Int32:
    case PrimitiveKind::UInt32:
      return 32;

    case PrimitiveKind::Int64:
    case PrimitiveKind::UInt64:
      return 64;

    default:
      return 0;
  }
}

int floatingPointWidth(PrimitiveKind kind)
{
  switch (kind)
  {
    case PrimitiveKind::Float32:
      return 32;

    case PrimitiveKind::Float64:
      return 64;

    default:
      return 0;
  }
}

bool isPrimitiveAssignable(PrimitiveKind source, PrimitiveKind target)
{
  if (source == target)
    return true;

  if (!isNumeric(source) || !isNumeric(target))
    return false;

  // Integer -> floating point is accepted.
  //
  // This is a semantic conversion rather than a guarantee that every integer
  // can be represented exactly by the target floating-point format.
  if (isInteger(source) && isFloatingPoint(target))
    return true;

  if (isFloatingPoint(source) && isFloatingPoint(target))
    return floatingPointWidth(source) <= floatingPointWidth(target);

  if (isSignedInteger(source) && isSignedInteger(target))
    return integerWidth(source) <= integerWidth(target);

  if (isUnsignedInteger(source) && isUnsignedInteger(target))
    return integerWidth(source) <= integerWidth(target);

  // An unsigned integer can only be widened safely to a larger signed type.
  //
  // UInt8  -> Int16  true
  // UInt32 -> Int32  false
  // UInt64 -> Int64  false
  if (isUnsignedInteger(source) && isSignedInteger(target))
    return integerWidth(source) < integerWidth(target);

  // Signed -> unsigned is not implicitly safe because the source may be
  // negative.
  return false;
}

std::string toBuiltInString(PrimitiveKind kind)
{
  switch (kind)
  {
    case PrimitiveKind::Bool:
      return BooleanBase;

    case PrimitiveKind::Int8:
    case PrimitiveKind::Int16:
    case PrimitiveKind::Int32:
    case PrimitiveKind::Int64:
      return IntegerBase;

    case PrimitiveKind::UInt8:
    case PrimitiveKind::UInt16:
    case PrimitiveKind::UInt32:
    case PrimitiveKind::UInt64:
      return UnsignedBase;

    case PrimitiveKind::Float32:
    case PrimitiveKind::Float64:
      return RealBase;

    case PrimitiveKind::String:
      return StringBase;

    case PrimitiveKind::Bytes:
      return BytesBase;

    case PrimitiveKind::Timestamp:
      return TimestampBase;

    case PrimitiveKind::Duration:
      return DurationBase;

    case PrimitiveKind::Void:
      return VoidBase;

    default:
    case PrimitiveKind::Unknown:
      return "unknown";
  }
}

std::string toString(PrimitiveKind kind)
{
  switch (kind)
  {
    case PrimitiveKind::Unknown:
      return "unknown";

    case PrimitiveKind::Bool:
      return "bool";

    case PrimitiveKind::Int8:
      return "int8";

    case PrimitiveKind::Int16:
      return "int16";

    case PrimitiveKind::Int32:
      return types::IntegerBase;

    case PrimitiveKind::Int64:
      return "int64";

    case PrimitiveKind::UInt8:
      return "uint8";

    case PrimitiveKind::UInt16:
      return "uint16";

    case PrimitiveKind::UInt32:
      return types::UnsignedBase;

    case PrimitiveKind::UInt64:
      return "uint64";

    case PrimitiveKind::Float32:
      return "float32";

    case PrimitiveKind::Float64:
      return types::RealBase;

    case PrimitiveKind::String:
      return types::StringBase;

    case PrimitiveKind::Bytes:
      return types::BytesBase;

    case PrimitiveKind::Timestamp:
      return types::TimestampBase;

    case PrimitiveKind::Duration:
      return types::DurationBase;

    case PrimitiveKind::Void:
      return types::VoidBase;
  }

  return "Unknown primitive kind";
}

std::string toString(TypeKind kind)
{
  switch (kind)
  {
    case TypeKind::Primitive:
      return "Primitive";

    case TypeKind::Record:
      return "Record";

    case TypeKind::Enum:
      return "Enum";

    case TypeKind::Alias:
      return "Alias";

    case TypeKind::Unknown:
      return "Unknown";
  }

  return "Unknown";
}

std::string toString(TypeReferenceKind kind)
{
  switch (kind)
  {
    case TypeReferenceKind::Primitive:
      return "Primitive";

    case TypeReferenceKind::Named:
      return "Named";

    case TypeReferenceKind::List:
      return "List";

    case TypeReferenceKind::Optional:
      return "Optional";

    case TypeReferenceKind::Map:
      return "Map";

    case TypeReferenceKind::Unknown:
      return "Unknown";
  }

  return "Unknown";
}

std::string toString(EnumUnderlyingKind kind)
{
  switch (kind)
  {
    case EnumUnderlyingKind::Int32:
      return types::IntegerBase;

    case EnumUnderlyingKind::UInt32:
      return types::UnsignedBase;

    case EnumUnderlyingKind::String:
      return types::StringBase;

    case EnumUnderlyingKind::Unknown:
      return "unknown";
  }

  return "Unknown enum underlying kind";
}

PrimitiveKind primitiveKindFromString(const std::string& kind)
{
  // First check the base types
  if (kind == types::BooleanBase || kind == "Bool" || kind == BooleanType)
    return PrimitiveKind::Bool;
  else if (kind == types::IntegerBase || kind == "int32" || kind == IntegerType || kind == "Int32")
    return PrimitiveKind::Int32;
  else if (kind == types::UnsignedBase || kind == "uint32" || kind == UnsignedType || kind == "UInt32")
    return PrimitiveKind::UInt32;
  else if (kind == types::RealBase || kind == "real64" || kind == RealType || kind == "Float64")
    return PrimitiveKind::Float64;
  else if (kind == types::StringBase || kind == StringType || kind == "String")
    return PrimitiveKind::String;
  else if (kind == types::BytesBase || kind == BytesType || kind == "Bytes")
    return PrimitiveKind::Bytes;
  else if (kind == types::TimestampBase || kind == TimestampType || kind == "Timestamp")
    return PrimitiveKind::Timestamp;
  else if (kind == types::DurationBase || kind == DurationType || kind == "Duration")
    return PrimitiveKind::Duration;
  else if (kind == types::VoidBase || kind == VoidType || kind == "Void")
    return PrimitiveKind::Void;
  // Then the rest which we might introduce some day
  else if (kind == "int8" || kind == "Int8")
    return PrimitiveKind::Int8;
  else if (kind == "int16" || kind == "Int16")
    return PrimitiveKind::Int16;
  else if (kind == "int64" || kind == "Int64")
    return PrimitiveKind::Int64;
  else if (kind == "uint8" || kind == "UInt8")
    return PrimitiveKind::UInt8;
  else if (kind == "uint16" || kind == "Uint16")
    return PrimitiveKind::UInt16;
  else if (kind == "uint64" || kind == "UInt64")
    return PrimitiveKind::UInt64;
  else if (kind == "real32" || kind == "Float32")
    return PrimitiveKind::Float32;

  return PrimitiveKind::Unknown;
}

TypeKind typeKindFromString(const std::string& kind)
{
  if (kind == "Primitive")
    return TypeKind::Primitive;
  if (kind == "Record")
    return TypeKind::Record;
  if (kind == "Enum")
    return TypeKind::Enum;
  if (kind == "Alias")
    return TypeKind::Alias;

  return TypeKind::Unknown;
}

TypeReferenceKind typeReferenceKindFromString(const std::string& kind)
{
  if (kind == "Primitive")
    return TypeReferenceKind::Primitive;
  else if (kind == "Named")
    return TypeReferenceKind::Named;
  else if (kind == "List")
    return TypeReferenceKind::List;
  else if (kind == "Optional")
    return TypeReferenceKind::Optional;
  else if (kind == "Map")
    return TypeReferenceKind::Map;
  else
    return TypeReferenceKind::Unknown;
}

EnumUnderlyingKind enumKindFromString(const std::string& kind)
{
  if (kind == types::IntegerBase)
    return EnumUnderlyingKind::Int32;
  else if (kind == types::UnsignedBase)
    return EnumUnderlyingKind::UInt32;
  else if (kind == types::StringBase)
    return EnumUnderlyingKind::String;
  else
    return EnumUnderlyingKind::Unknown;
}

EnumUnderlyingKind enumKindFromPrimitive(const PrimitiveKind& kind)
{
  switch (kind)
  {
    case PrimitiveKind::Int8:
    case PrimitiveKind::Int16:
    case PrimitiveKind::Int32:
    case PrimitiveKind::Int64:
      return EnumUnderlyingKind::Int32;

    case PrimitiveKind::UInt8:
    case PrimitiveKind::UInt16:
    case PrimitiveKind::UInt32:
    case PrimitiveKind::UInt64:
      return EnumUnderlyingKind::UInt32;

    case PrimitiveKind::String:
      return EnumUnderlyingKind::String;

    case PrimitiveKind::Bytes:
    case PrimitiveKind::Timestamp:
    case PrimitiveKind::Duration:
    case PrimitiveKind::Void:
    case PrimitiveKind::Unknown:
    case PrimitiveKind::Bool:
    case PrimitiveKind::Float32:
    case PrimitiveKind::Float64:
      return EnumUnderlyingKind::Unknown;
  }

  return EnumUnderlyingKind::Unknown;
}

std::string makeUuid()
{
  static thread_local std::mt19937_64 rng{std::random_device{}()};
  static thread_local std::uniform_int_distribution<unsigned int> dist(0, 255);

  std::array<unsigned char, 16> bytes{};

  for (auto& b : bytes)
    b = static_cast<unsigned char>(dist(rng));

  // UUID v4
  bytes[6] = static_cast<unsigned char>((bytes[6] & 0x0F) | 0x40);
  bytes[8] = static_cast<unsigned char>((bytes[8] & 0x3F) | 0x80);

  std::ostringstream out;
  out << std::hex << std::setfill('0');

  for (size_t i = 0; i < bytes.size(); ++i)
  {
    out << std::setw(2) << static_cast<int>(bytes[i]);

    if (i == 3 || i == 5 || i == 7 || i == 9)
      out << '-';
  }

  return out.str();
}

QualifiedName convertQualifiedName(const std::string& name)
{
  if (name == types::BooleanBase)
    return types::BooleanType;
  if (name == types::IntegerBase)
    return types::IntegerType;
  if (name == types::UnsignedBase)
    return types::UnsignedType;
  if (name == types::RealBase)
    return types::RealType;
  if (name == types::StringBase)
    return types::StringType;
  if (name == types::BytesBase)
    return types::BytesType;
  if (name == types::TimestampBase)
    return types::TimestampType;
  if (name == types::DurationBase)
    return types::DurationType;
  if (name == types::VoidBase)
    return types::VoidType;

  return types::QualifiedName(name);
}

std::string buildInQualifiedName(const std::string& name)
{
  if (name == types::BooleanType)
    return types::BooleanBase;
  if (name == types::IntegerType)
    return types::IntegerBase;
  if (name == types::UnsignedType)
    return types::UnsignedBase;
  if (name == types::RealType)
    return types::RealBase;
  if (name == types::StringType)
    return types::StringBase;
  if (name == types::BytesType)
    return types::BytesBase;
  if (name == types::TimestampType)
    return types::TimestampBase;
  if (name == types::DurationType)
    return types::DurationBase;
  if (name == types::VoidType)
    return types::VoidBase;

  return name;
}

}  // namespace koda::types