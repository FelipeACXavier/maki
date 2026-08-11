#pragma once

#include <cstdint>
#include <limits>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "ast/ast.h"
#include "ast/symbol.h"

namespace koda::dezyne
{
using SymbolId = std::uint32_t;
inline constexpr SymbolId InvalidSymbol = std::numeric_limits<SymbolId>::max();

enum class SymbolKind
{
  Component,
  HelperComponent,
  Port,
  Instance,
  Connection,
  GeneratedFile
};

enum class PortDirection
{
  Provides,
  Requires
};
enum class PortProtocol
{
  Action,
  Signal,
  Alarm,
  Unknown
};

struct Provenance
{
  std::optional<koda::SymbolId> sourceSymbol;
  std::optional<Span> sourceSpan;
};

struct Symbol
{
  SymbolId id = InvalidSymbol;
  SymbolKind kind = SymbolKind::Component;
  std::string name;
  std::string qualifiedName;
  SymbolId parent = InvalidSymbol;
  Provenance origin;

  static std::string kindToString(const SymbolKind kind)
  {
    switch (kind)
    {
      case SymbolKind::Component:
        return "Component";
      case SymbolKind::HelperComponent:
        return "HelperComponent";
      case SymbolKind::Port:
        return "Port";
      case SymbolKind::Instance:
        return "Instance";
      case SymbolKind::Connection:
        return "Connection";
      case SymbolKind::GeneratedFile:
        return "GeneratedFile";
      default:
        return "Unknown";
    }
  }
};

class SymbolTable
{
public:
  void clear();
  SymbolId declare(SymbolKind kind, std::string name, std::string qualifiedName, SymbolId parent = InvalidSymbol, Provenance origin = {});

  const Symbol* get(SymbolId id) const;
  Symbol* get(SymbolId id);
  const Symbol* find(std::string_view qualifiedName) const;
  std::vector<const Symbol*> children(SymbolId parent, std::optional<SymbolKind> kind = std::nullopt) const;
  std::vector<const Symbol*> fromKodaSymbol(koda::SymbolId source) const;
  std::vector<const Symbol*> fromSpan(const Span& span) const;

  const std::vector<Symbol>& all() const
  {
    return mSymbols;
  }

private:
  std::vector<Symbol> mSymbols;
  std::unordered_map<std::string, SymbolId> mByQualifiedName;
  std::unordered_multimap<koda::SymbolId, SymbolId> mBySource;
};

struct Port
{
  SymbolId symbol = InvalidSymbol;
  PortDirection direction = PortDirection::Requires;
  PortProtocol protocol = PortProtocol::Unknown;
};

struct Instance
{
  SymbolId symbol = InvalidSymbol;
  std::string typeName;
};

struct Connection
{
  SymbolId symbol = InvalidSymbol;
  std::string lhs;
  std::string rhs;
};

struct Component
{
  SymbolId symbol = InvalidSymbol;
  std::string fileName;
  std::vector<std::string> imports;
  std::vector<Port> ports;
  std::vector<Instance> instances;
  std::vector<Connection> connections;
  std::vector<std::string> behaviour;
};

struct GeneratedFile
{
  SymbolId symbol = InvalidSymbol;
  std::string path;
  std::string contents;
};

class Model
{
public:
  void clear();

  SymbolId declareComponent(std::string name, std::string fileName, Provenance origin = {}, bool helper = false, SymbolId parent = InvalidSymbol);
  SymbolId declarePort(SymbolId component, std::string name, PortDirection direction, PortProtocol protocol, Provenance origin = {});
  SymbolId declareInstance(SymbolId component, std::string name, std::string typeName, Provenance origin = {});
  SymbolId declareConnection(SymbolId component, std::string name, std::string lhs, std::string rhs, Provenance origin = {});

  Component* component(SymbolId symbol);
  const Component* component(SymbolId symbol) const;
  Component* findComponent(std::string_view name);
  const Component* findComponent(std::string_view name) const;
  const Port* findPort(SymbolId component, std::string_view name) const;

  void setGeneratedFile(std::string path, std::string contents, Provenance origin = {});

  SymbolTable mSymbols;
  std::vector<Component> mComponents;
  std::vector<GeneratedFile> mFiles;
};

}  // namespace koda::dezyne
