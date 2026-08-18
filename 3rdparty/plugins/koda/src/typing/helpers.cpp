#include "helpers.h"

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
      return BooleanType;

    case PrimitiveKind::Int8:
    case PrimitiveKind::Int16:
    case PrimitiveKind::Int32:
    case PrimitiveKind::Int64:
      return IntegerType;

    case PrimitiveKind::UInt8:
    case PrimitiveKind::UInt16:
    case PrimitiveKind::UInt32:
    case PrimitiveKind::UInt64:
      return UnsignedType;

    case PrimitiveKind::Float32:
    case PrimitiveKind::Float64:
      return RealType;

    case PrimitiveKind::String:
      return StringType;

    case PrimitiveKind::Bytes:
      return BytesType;

    case PrimitiveKind::Timestamp:
      return TimestampType;

    case PrimitiveKind::Duration:
      return DurationType;

    case PrimitiveKind::Void:
      return VoidType;

    default:
    case PrimitiveKind::Unknown:
      return "Unknown";
  }
}

std::string toString(PrimitiveKind kind)
{
  switch (kind)
  {
    case PrimitiveKind::Unknown:
      return "Unknown";

    case PrimitiveKind::Bool:
      return "Bool";

    case PrimitiveKind::Int8:
      return "Int8";

    case PrimitiveKind::Int16:
      return "Int16";

    case PrimitiveKind::Int32:
      return "Int32";

    case PrimitiveKind::Int64:
      return "Int64";

    case PrimitiveKind::UInt8:
      return "UInt8";

    case PrimitiveKind::UInt16:
      return "UInt16";

    case PrimitiveKind::UInt32:
      return "UInt32";

    case PrimitiveKind::UInt64:
      return "UInt64";

    case PrimitiveKind::Float32:
      return "Float32";

    case PrimitiveKind::Float64:
      return "Float64";

    case PrimitiveKind::String:
      return "String";

    case PrimitiveKind::Bytes:
      return "Bytes";

    case PrimitiveKind::Timestamp:
      return "Timestamp";

    case PrimitiveKind::Duration:
      return "Duration";

    case PrimitiveKind::Void:
      return "Void";
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
      return "Int32";

    case EnumUnderlyingKind::UInt32:
      return "UInt32";

    case EnumUnderlyingKind::String:
      return "String";

    case EnumUnderlyingKind::Unknown:
      return "Unknown";
  }

  return "Unknown enum underlying kind";
}

PrimitiveKind primitiveKindFromString(const std::string& kind)
{
  if (kind == "Bool" || kind == BooleanType)
    return PrimitiveKind::Bool;
  else if (kind == "Int8")
    return PrimitiveKind::Int8;
  else if (kind == "Int16")
    return PrimitiveKind::Int16;
  else if (kind == "Int32" || kind == IntegerType)
    return PrimitiveKind::Int32;
  else if (kind == "Int64")
    return PrimitiveKind::Int64;
  else if (kind == "UInt8")
    return PrimitiveKind::UInt8;
  else if (kind == "UInt16")
    return PrimitiveKind::UInt16;
  else if (kind == "UInt32" || kind == UnsignedType)
    return PrimitiveKind::UInt32;
  else if (kind == "UInt64")
    return PrimitiveKind::UInt64;
  else if (kind == "Float32")
    return PrimitiveKind::Float32;
  else if (kind == "Float64" || kind == RealType)
    return PrimitiveKind::Float64;
  else if (kind == "String" || kind == StringType)
    return PrimitiveKind::String;
  else if (kind == "Bytes" || kind == BytesType)
    return PrimitiveKind::Bytes;
  else if (kind == "Timestamp" || kind == TimestampType)
    return PrimitiveKind::Timestamp;
  else if (kind == "Duration" || kind == DurationType)
    return PrimitiveKind::Duration;
  else if (kind == "Void" || kind == VoidType)
    return PrimitiveKind::Void;

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
  if (kind == "Int32")
    return EnumUnderlyingKind::Int32;
  else if (kind == "UInt32")
    return EnumUnderlyingKind::UInt32;
  else if (kind == "String")
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

}  // namespace koda::types