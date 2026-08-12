#include "helpers.h"

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

std::string toString(PrimitiveKind kind)
{
  switch (kind)
  {
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
  }

  return "Unknown type kind";
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

}  // namespace koda::types