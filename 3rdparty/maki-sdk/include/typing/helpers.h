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

std::string toBuiltInString(PrimitiveKind kind);
bool isBuiltin(QualifiedName name);

bool isSignedInteger(PrimitiveKind kind);
bool isUnsignedInteger(PrimitiveKind kind);
bool isInteger(PrimitiveKind kind);
bool isFloatingPoint(PrimitiveKind kind);
bool isNumeric(PrimitiveKind kind);
bool isPrimitiveAssignable(PrimitiveKind source, PrimitiveKind target);

int integerWidth(PrimitiveKind kind);
int floatingPointWidth(PrimitiveKind kind);

PrimitiveKind primitiveKindFromString(const std::string& kind);
TypeKind typeKindFromString(const std::string& kind);
TypeReferenceKind typeReferenceKindFromString(const std::string& kind);
EnumUnderlyingKind enumKindFromString(const std::string& kind);
EnumUnderlyingKind enumKindFromPrimitive(const PrimitiveKind& kind);
PrimitiveKind enumKindFromEnumKind(const EnumUnderlyingKind& kind);

std::string makeUuid();
QualifiedName convertQualifiedName(const std::string& name);
std::string buildInQualifiedName(const std::string& name);

}  // namespace koda::types