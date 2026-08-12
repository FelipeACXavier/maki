#include "type_registry.h"

#include "logging.h"

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

koda::types::TypeRegistrationResult TypeRegistry::add(const koda::types::TypeDefinition& definition)
{
  auto ret = koda::types::TypeRegistry::add(definition);
  if (ret.IsSuccess())
  {
    emit typeAdded(QString::fromStdString(definition.name.toString()));
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
    emit typeChanged(QString::fromStdString(definition.name.toString()));
    emit registryChanged();
  }

  return ret;
}

koda::types::TypeRegistrationResult TypeRegistry::remove(const std::string& fieldId)
{
  if (isBuiltin(fieldId))
    return koda::types::TypeRegistrationResult::Failed(koda::types::TypeRegistrationError::BuiltIn, "Built-in types cannot be removed");

  auto removed = removeById(fieldId);
  if (removed)
  {
    emit typeChanged(QString::fromStdString(fieldId));
    emit registryChanged();
  }

  return koda::types::TypeRegistrationResult(koda::types::TypeRegistrationError::None);
}

}  // namespace maki