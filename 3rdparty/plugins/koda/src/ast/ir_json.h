#pragma once

#include <QJsonObject>
#include <QString>
#include <vector>

#include "ast/koda_ir.h"
#include "ast/symbol_registry.h"
#include "result.h"
#include "typing/type_definition.h"

namespace koda
{
// Self-contained, versioned representation of the data consumed by emitters.
// The caller supplies user-defined types from its TypeRegistry; built-ins can
// remain registered by TypeRegistry itself.
struct IrArchive
{
  ir::Program program;
  SymbolRegistry symbols;
  std::vector<types::TypeDefinition> typeDefinitions;
};

class IrJson final
{
public:
  static constexpr int FormatVersion = 1;

  // Throws std::runtime_error on malformed in-memory trees (e.g., cycles).
  static QJsonObject toJson(const ir::Program& program, const SymbolRegistry& symbols, const std::vector<types::TypeDefinition>& typeDefinitions = {});

  // Validates the wire format and recreates the complete IR and symbol scopes.
  static Result<IrArchive> fromJson(const QJsonObject& json);

  static VoidResult save(const QString& filename, const ir::Program& program, const SymbolRegistry& symbols,
                         const std::vector<types::TypeDefinition>& typeDefinitions = {});

  static Result<IrArchive> load(const QString& filename);
};
}  // namespace koda
