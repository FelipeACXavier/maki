#pragma once

#include <string>
#include <vector>

#include "types.h"

namespace koda::types
{
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

void addDiagnostic(std::vector<TypeModelDiagnostic>& diagnostics, TypeModelDiagnostic::Severity severity, const std::string& code,
                   const std::string& message, const std::string& path);

std::string toString(PrimitiveKind kind);
std::string toString(TypeKind kind);
std::string toString(TypeReferenceKind kind);
std::string toString(EnumUnderlyingKind kind);

EnumUnderlyingKind enumKindFromString(const std::string& kind);
}  // namespace koda::types