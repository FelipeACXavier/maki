#include "type_registry.h"

#include <QJsonArray>
#include <QJsonObject>

#include "keys.h"
#include "logging.h"
#include "type_helpers.h"

namespace maki
{

TypeRegistry& TypeRegistry::instance()
{
  static TypeRegistry registry;
  return registry;
}

TypeRegistry::TypeRegistry(QObject* parent)
    : QObject(parent)
    , koda::types::TypeRegistry()
{
  QString error;

  registerBuiltinTypes();
}

QStringList TypeRegistry::allTypeNames() const
{
  QStringList names;
  for (const auto& n : allTypes())
    names << QString::fromStdString(n->name.toString());

  return names;
}

QStringList TypeRegistry::userTypeNames() const
{
  QStringList names;
  for (const auto& n : allTypes())
    if (!isBuiltin(*n))
      names << QString::fromStdString(n->name.toString());

  return names;
}

QStringList TypeRegistry::builtinTypeNames() const
{
  QStringList names;
  for (const auto& n : allTypes())
    if (isBuiltin(*n))
      names << QString::fromStdString(n->name.toString());

  return names;
}

QStringList TypeRegistry::namespaces() const
{
  QStringList names;
  for (const auto& n : allTypes())
    names << QString::fromStdString(n->name.namespaceString());

  return names;
}

VoidResult TypeRegistry::loadFromLibrary(const JSON& json)
{
  auto name = json[ConfigKeys::NAME].toString();
  auto types = json[ConfigKeys::TYPES];
  if (!types.isArray())
    return VoidResult::Failed("types must be in a list in the format \"types\": []");

  for (const auto& object : types.toArray())
  {
    if (!object.isObject())
      return VoidResult::Failed("type must be a json object");

    auto type = object.toObject();

    auto result = typeDefinitionFromJson(type);
    if (!result.IsSuccess())
    {
      LOG_WARNING("Error while converting type: {}", result.ErrorMessage());
      continue;
    }

    koda::types::TypeDefinition def = result.Value();
    auto registered = add(def);
    if (!registered)
      LOG_WARNING("Error while registering type: {}", registered.ErrorMessage());
  }

  return VoidResult();
}

koda::types::TypeRegistrationResult TypeRegistry::add(const koda::types::TypeDefinition& definition)
{
  auto ret = koda::types::TypeRegistry::add(definition);
  if (ret.IsSuccess())
  {
    emit typeAdded(definition);
    emit registryChanged();
  }

  return ret;
}

koda::types::TypeRegistrationResult TypeRegistry::replace(const koda::types::TypeDefinition& definition)
{
  if (isBuiltin(definition))
    return koda::types::TypeRegistrationResult::Failed(koda::types::TypeRegistrationError::BuiltIn, "Built-in types cannot be replaced");

  auto ret = koda::types::TypeRegistry::replace(definition);
  if (ret.IsSuccess())
  {
    emit typeChanged(definition);
    emit registryChanged();
  }

  return ret;
}

koda::types::TypeRegistrationResult TypeRegistry::remove(const std::string& id)
{
  if (isBuiltin(id))
    return koda::types::TypeRegistrationResult::Failed(koda::types::TypeRegistrationError::BuiltIn, "Built-in types cannot be removed");

  const auto toRemove = *findById(id);
  auto removed = removeById(id);
  if (removed)
  {
    emit typeRemoved(toRemove);
    emit registryChanged();
  }

  return koda::types::TypeRegistrationResult(koda::types::TypeRegistrationError::None);
}

}  // namespace maki
