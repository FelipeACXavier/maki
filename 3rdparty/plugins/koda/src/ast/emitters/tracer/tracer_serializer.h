#pragma once

#include <map>
#include <string>

#include "ast/koda_ir.h"
#include "ast/symbol_registry.h"
#include "result.h"
#include "tracer_json.h"

namespace koda::tracer
{
// Converts resolved IR into the strictly supported koda.reference.model.v1 format.
// This is intentionally a structural translation, NOT an execution semantics engine.
// In particular it never infers outcomes from the Dezyne/nuXmv backends.
class Serializer
{
public:
  Serializer(const ir::Program& program, const SymbolRegistry& symbols);
  Result<Json> serializeTask(const ir::Component& task) const;
  // Deterministic, nondeterministic-outcome fixture derived only from IR call kinds.
  Result<Json> serializeDefaultMocks(const ir::Component& task) const;

private:
  const SymbolRegistry& mSymbols;
  std::map<std::string, const ir::Component*> mCapabilities;

  Result<Json> serializeNode(const ir::PStrategy& node, const ir::Component& task) const;
  Result<Json> serializeHandlers(Json action, const std::vector<ir::PHandler>& handlers,
                                 const ir::Component& task, const ir::Strategy& parent) const;
  Result<Json> serializeExpression(const ir::PExpression& expr) const;
  Result<Json> serializeCall(const ir::Strategy::Call& call, const ir::Component& task, const ir::Strategy& node) const;
  Result<Json> serializeChildren(const std::vector<ir::PStrategy>& children, const ir::Component& task,
                                 const std::string& key, const ir::Strategy& parent) const;
  Result<Json> serializeCapabilities(const ir::Component& task) const;
  Result<std::map<std::string, std::string>> callableActions(const ir::Component& capability) const;
  const ir::Argument* capabilityInstance(const ir::Component& task, SymbolId symbol) const;
  static std::string location(const ir::Strategy& node);
};
}  // namespace koda::tracer
