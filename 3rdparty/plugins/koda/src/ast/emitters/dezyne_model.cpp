#include "dezyne_model.h"

#include <algorithm>

#include "logging.h"

namespace koda::dezyne
{
namespace
{
bool sameSpan(const Span& a, const Span& b)
{
  return a.lineStart == b.lineStart && a.colStart == b.colStart &&
         a.lineEnd == b.lineEnd && a.colEnd == b.colEnd;
}
}  // namespace

void SymbolTable::clear()
{
  mSymbols.clear();
  mByQualifiedName.clear();
  mBySource.clear();
}

SymbolId SymbolTable::declare(SymbolKind kind, std::string name, std::string qualifiedName,
                              SymbolId parent, Provenance origin)
{
  if (auto it = mByQualifiedName.find(qualifiedName); it != mByQualifiedName.end())
    return it->second;

  const auto id = static_cast<SymbolId>(mSymbols.size());
  mSymbols.push_back(Symbol{id, kind, std::move(name), std::move(qualifiedName), parent, origin});
  mByQualifiedName[mSymbols.back().qualifiedName] = id;
  if (origin.sourceSymbol)
    mBySource.emplace(*origin.sourceSymbol, id);
  return id;
}

const Symbol* SymbolTable::get(SymbolId id) const
{
  return id < mSymbols.size() ? &mSymbols[id] : nullptr;
}

Symbol* SymbolTable::get(SymbolId id)
{
  return id < mSymbols.size() ? &mSymbols[id] : nullptr;
}

const Symbol* SymbolTable::find(std::string_view qualifiedName) const
{
  auto it = mByQualifiedName.find(std::string(qualifiedName));
  return it == mByQualifiedName.end() ? nullptr : get(it->second);
}

std::vector<const Symbol*> SymbolTable::children(SymbolId parent, std::optional<SymbolKind> kind) const
{
  std::vector<const Symbol*> result;
  for (const auto& symbol : mSymbols)
    if (symbol.parent == parent && (!kind || symbol.kind == *kind))
      result.push_back(&symbol);
  return result;
}

std::vector<const Symbol*> SymbolTable::fromKodaSymbol(koda::SymbolId source) const
{
  std::vector<const Symbol*> result;
  const auto [begin, end] = mBySource.equal_range(source);
  for (auto it = begin; it != end; ++it)
    if (const auto* symbol = get(it->second))
      result.push_back(symbol);
  return result;
}

std::vector<const Symbol*> SymbolTable::fromSpan(const Span& span) const
{
  std::vector<const Symbol*> result;
  for (const auto& symbol : mSymbols)
    if (symbol.origin.sourceSpan && sameSpan(*symbol.origin.sourceSpan, span))
      result.push_back(&symbol);
  return result;
}

void Model::clear()
{
  mSymbols.clear();
  mComponents.clear();
  mFiles.clear();
}

SymbolId Model::declareComponent(std::string name, std::string fileName, Provenance origin, bool helper, SymbolId parent)
{
  const auto id = mSymbols.declare(helper ? SymbolKind::HelperComponent : SymbolKind::Component, name, name, parent, origin);
  if (!component(id))
    mComponents.push_back(Component{id, std::move(fileName)});

  return id;
}

SymbolId Model::declarePort(SymbolId componentId, std::string name, PortDirection direction, PortProtocol protocol, Provenance origin)
{
  const auto* parent = mSymbols.get(componentId);
  const auto qualified = parent ? parent->qualifiedName + "." + name : name;
  const auto id = mSymbols.declare(SymbolKind::Port, std::move(name), qualified, componentId, origin);

  auto* owner = component(componentId);
  if (owner && std::none_of(owner->ports.begin(), owner->ports.end(), [id](const Port& p) { return p.symbol == id; }))
    owner->ports.push_back(Port{id, direction, protocol});

  return id;
}

SymbolId Model::declareInstance(SymbolId componentId, std::string name, std::string typeName, Provenance origin)
{
  const auto* parent = mSymbols.get(componentId);
  const auto qualified = parent ? parent->qualifiedName + "." + name : name;
  const auto id = mSymbols.declare(SymbolKind::Instance, std::move(name), qualified, componentId, origin);
  auto* owner = component(componentId);
  if (owner && std::none_of(owner->instances.begin(), owner->instances.end(), [id](const Instance& x) { return x.symbol == id; }))
    owner->instances.push_back(Instance{id, std::move(typeName)});

  return id;
}

SymbolId Model::declareConnection(SymbolId componentId, std::string name, std::string lhs, std::string rhs, Provenance origin)
{
  const auto* parent = mSymbols.get(componentId);
  const auto qualified = parent ? parent->qualifiedName + ".connection." + name : name;
  const auto id = mSymbols.declare(SymbolKind::Connection, std::move(name), qualified, componentId, origin);

  auto* owner = component(componentId);
  if (owner && std::none_of(owner->connections.begin(), owner->connections.end(), [id](const Connection& x) { return x.symbol == id; }))
    owner->connections.push_back(Connection{id, std::move(lhs), std::move(rhs)});

  return id;
}

Component* Model::component(SymbolId id)
{
  auto it = std::find_if(mComponents.begin(), mComponents.end(), [id](const Component& c) { return c.symbol == id; });
  return it == mComponents.end() ? nullptr : &*it;
}

const Component* Model::component(SymbolId id) const
{
  auto it = std::find_if(mComponents.begin(), mComponents.end(), [id](const Component& c) { return c.symbol == id; });
  return it == mComponents.end() ? nullptr : &*it;
}

Component* Model::findComponent(std::string_view name)
{
  const auto* symbol = mSymbols.find(name);
  return symbol ? component(symbol->id) : nullptr;
}

const Component* Model::findComponent(std::string_view name) const
{
  const auto* symbol = mSymbols.find(name);
  return symbol ? component(symbol->id) : nullptr;
}

const Port* Model::findPort(SymbolId componentId, std::string_view name) const
{
  const auto* owner = component(componentId);
  if (!owner)
    return nullptr;
  for (const auto& port : owner->ports)
  {
    const auto* symbol = mSymbols.get(port.symbol);
    if (symbol && symbol->name == name)
      return &port;
  }
  return nullptr;
}

void Model::setGeneratedFile(std::string path, std::string contents, Provenance origin)
{
  auto it = std::find_if(mFiles.begin(), mFiles.end(), [&](const GeneratedFile& f) { return f.path == path; });
  if (it != mFiles.end())
  {
    it->contents = std::move(contents);
    return;
  }

  const auto name = path;
  const auto id = mSymbols.declare(SymbolKind::GeneratedFile, name, "file:" + name, InvalidSymbol, origin);
  mFiles.push_back(GeneratedFile{id, std::move(path), std::move(contents)});
}

}  // namespace koda::dezyne
