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

VoidResult TypeRegistry::registerNode(const QString& nodeId, const NodeConfig& node)
{
  if (nodeId.isEmpty())
    return VoidResult::Failed("Could not register node with no id");

  for (const auto& e : node.events)
  {
    if (e.type == Types::CallType::TRIGGER)
    {
      QStringList consumes;
      for (const auto& arg : e.arguments)
        consumes << QString::fromStdString(arg.type.toString());

      if (!consumes.isEmpty())
      {
        LOG_DEBUG("Registering consumer: {}", nodeId);
        for (const auto& c : consumes)
          LOG_DEBUG("   {}", c);

        mConsumers.insert(nodeId, consumes);
      }
    }
    else if (e.type == Types::CallType::RETURN)
    {
      QStringList produces;
      for (const auto& arg : e.arguments)
        produces << QString::fromStdString(arg.type.toString());

      if (!produces.isEmpty())
      {
        LOG_DEBUG("Registering producer: {}", nodeId);
        for (const auto& c : produces)
          LOG_DEBUG("   {}", c);

        mProducers.insert(nodeId, produces);
      }
    }
  }

  return VoidResult();
}

QStringList TypeRegistry::findProducers(const koda::types::TypeReference& type) const
{
  LOG_DEBUG("Looking for producers of: {}", type.toString());
  if (!type.isValid())
    return QStringList();

  QStringList out;
  for (const auto& key : mProducers.keys())
    if (mProducers[key].contains(QString::fromStdString(type.toString())))
      out << key;

  return out;
}

QStringList TypeRegistry::findConsumers(const koda::types::TypeReference& type) const
{
  LOG_DEBUG("Looking for consumers of: {}", type.toString());
  if (!type.isValid())
    return QStringList();

  QStringList out;
  for (const auto& key : mConsumers.keys())
    if (mConsumers[key].contains(QString::fromStdString(type.toString())))
      out << key;

  return out;
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

  LOG_INFO("Loading {} types from {}", types.toArray().size(), name);
  for (const auto& object : types.toArray())
  {
    if (!object.isObject())
      return VoidResult::Failed("type must be a json object");

    auto type = object.toObject();

    // Use the library name as the namespace
    auto result = typeDefinitionFromJson(type, name.toStdString());
    if (!result.IsSuccess())
    {
      LOG_WARNING("Error while converting type: {}", result.ErrorMessage());
      continue;
    }

    koda::types::TypeDefinition def = result.Value();
    auto registered = add(def);
    if (!registered)
      LOG_WARNING("Error while registering type: {}", registered.ErrorMessage());

    mLibraries.insert(name.toStdString());
  }

  return VoidResult();
}

QStringList TypeRegistry::libraries() const
{
  QStringList names;
  for (const auto& n : mLibraries)
    names << QString::fromStdString(n);

  return names;
}

bool TypeRegistry::isFromLibrary(const koda::types::QualifiedName& name) const
{
  return mLibraries.contains(name.namespaceString());
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
