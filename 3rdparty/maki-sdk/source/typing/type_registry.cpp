#include "typing/type_registry.h"

#include <algorithm>
#include <unordered_set>
#include <utility>

#include "logging.h"

namespace koda::types
{
// ===========================================================================================================
// TypeRegistry
TypeRegistry::TypeRegistry()
    : mTypes({})
    , mIdsByName({})
    , mBuiltinIds({})
{
}

void TypeRegistry::registerBuiltinTypes()
{
  const auto registerBuiltin = [this](std::string name, PrimitiveKind primitive) {
    const auto qname = QualifiedName(name);
    TypeDefinition definition{
        .id = qname.toId(),  // These can just use the qname as ids since they are immutable
        .name = qname,
        .data = PrimitiveTypeDefinition{.primitive = primitive},
    };

    const TypeRegistrationResult result = add(definition);
    if (result.IsSuccess())
      mBuiltinIds.insert_or_assign(primitive, definition.id);
  };

  registerBuiltin(koda::types::BooleanType, PrimitiveKind::Bool);
  registerBuiltin(koda::types::IntegerType, PrimitiveKind::Int64);
  registerBuiltin(koda::types::UnsignedType, PrimitiveKind::UInt64);
  registerBuiltin(koda::types::RealType, PrimitiveKind::Float64);
  registerBuiltin(koda::types::StringType, PrimitiveKind::String);
  registerBuiltin(koda::types::BytesType, PrimitiveKind::Bytes);
  registerBuiltin(koda::types::TimestampType, PrimitiveKind::Timestamp);
  registerBuiltin(koda::types::DurationType, PrimitiveKind::Duration);
  registerBuiltin(koda::types::VoidType, PrimitiveKind::Void);
}

TypeRegistrationResult TypeRegistry::add(const TypeDefinition& definition)
{
  const TypeRegistrationResult result = validateRegistration(definition);
  RETURN_ON_FAILURE(result);

  const TypeId id = definition.id;
  const std::string qualifiedName = nameKey(definition.name);

  if (definition.isPrimitive())
  {
    const PrimitiveKind primitive = definition.primitive().primitive;
    // Only use the first registered definition as the canonical built-in definition for a primitive.
    if (!mBuiltinIds.contains(primitive))
      mBuiltinIds.emplace(primitive, id);
  }

  mIdsByName.emplace(qualifiedName, id);
  mTypes.emplace(id, std::move(definition));

  return result;
}

TypeRegistrationResult TypeRegistry::replace(const TypeDefinition& definition)
{
  // Just add the new type in case it doesn't exist
  const auto existing = mTypes.find(definition.id);
  if (existing == mTypes.end())
  {
    LOG_TRACE("Could not find id: {} adding it", definition.id);
    return add(definition);
  }

  int sameNameCount = std::count_if(mTypes.begin(), mTypes.end(), [&definition](const auto& entry) {
    return entry.second.name == definition.name && entry.first != definition.id;
  });
  if (sameNameCount > 0)
    return TypeRegistrationResult::Failed(TypeRegistrationError::DuplicateName, "Name already exists: " + definition.name.toString());

  const TypeId replacedId = definition.id;
  const TypeRegistrationResult result = validateRegistration(definition, &replacedId);

  if (!result.IsSuccess())
    return result;

  const TypeDefinition oldDefinition = existing->second;

  mIdsByName.erase(nameKey(oldDefinition.name));

  if (oldDefinition.isPrimitive())
  {
    const PrimitiveKind oldPrimitive = oldDefinition.primitive().primitive;
    const auto builtin = mBuiltinIds.find(oldPrimitive);

    if (builtin != mBuiltinIds.end() && builtin->second == replacedId)
      mBuiltinIds.erase(builtin);
  }

  existing->second = definition;
  mIdsByName.emplace(nameKey(existing->second.name), existing->second.id);

  if (existing->second.isPrimitive())
  {
    const PrimitiveKind primitive = existing->second.primitive().primitive;
    if (!mBuiltinIds.contains(primitive))
      mBuiltinIds.emplace(primitive, existing->second.id);
  }

  return result;
}

bool TypeRegistry::removeById(const TypeId& id)
{
  const auto iterator = mTypes.find(id);
  if (iterator == mTypes.end())
    return false;

  const TypeDefinition& definition = iterator->second;
  mIdsByName.erase(nameKey(definition.name));
  if (definition.isPrimitive())
  {
    const PrimitiveKind primitive = definition.primitive().primitive;
    const auto builtin = mBuiltinIds.find(primitive);
    if (builtin != mBuiltinIds.end() && builtin->second == id)
      mBuiltinIds.erase(builtin);
  }

  mTypes.erase(iterator);
  return true;
}

void TypeRegistry::clear()
{
  mTypes.clear();
  mIdsByName.clear();
  mBuiltinIds.clear();
}

void TypeRegistry::removeIf(std::function<bool(const TypeDefinition& type)> condition)
{
  for (auto it = mTypes.begin(); it != mTypes.end();)
  {
    if (condition && condition(it->second))
    {
      mIdsByName.erase(nameKey(it->second.name));
      it = mTypes.erase(it);
    }
    else
    {
      ++it;
    }
  }
}

bool TypeRegistry::containsId(const TypeId& id) const
{
  return mTypes.contains(id);
}

bool TypeRegistry::containsName(const QualifiedName& name) const
{
  return mIdsByName.contains(nameKey(name));
}

bool TypeRegistry::containsName(const std::string& qualifiedName) const
{
  return containsName(QualifiedName(qualifiedName));
}

const TypeDefinition* TypeRegistry::findById(const TypeId& id) const
{
  const auto iterator = mTypes.find(id);
  if (iterator == mTypes.end())
    return nullptr;

  return &iterator->second;
}

TypeDefinition* TypeRegistry::findById(const TypeId& id)
{
  const auto iterator = mTypes.find(id);
  if (iterator == mTypes.end())
    return nullptr;

  return &iterator->second;
}

const TypeDefinition* TypeRegistry::findByName(const QualifiedName& name) const
{
  const auto idIterator = mIdsByName.find(nameKey(name));
  if (idIterator == mIdsByName.end())
    return nullptr;

  return findById(idIterator->second);
}

TypeDefinition* TypeRegistry::findByName(const QualifiedName& name)
{
  const auto idIterator = mIdsByName.find(nameKey(name));
  if (idIterator == mIdsByName.end())
    return nullptr;

  return findById(idIterator->second);
}

const TypeDefinition* TypeRegistry::findByName(const std::string& qualifiedName) const
{
  return findByName(QualifiedName(qualifiedName));
}

TypeDefinition* TypeRegistry::findByName(const std::string& qualifiedName)
{
  return findByName(QualifiedName(qualifiedName));
}

const TypeDefinition* TypeRegistry::resolve(const TypeReference& reference) const
{
  if (reference.isPrimitive())
    return builtinDefinition(reference.primitiveKind());

  if (!reference.isNamed())
    return nullptr;

  const TypeReference::Named& named = reference.namedType();
  if (named.id.has_value())
    if (const TypeDefinition* definition = findById(*named.id); definition != nullptr)
      return definition;

  return findByName(named.name);
}

std::optional<TypeReference> TypeRegistry::resolveAliases(const TypeReference& reference) const
{
  TypeReference current = reference;
  std::unordered_set<TypeId> visitedAliases;

  while (current.isNamed())
  {
    const TypeDefinition* definition = resolve(current);
    if (definition == nullptr)
      return std::nullopt;

    if (!definition->isAlias())
      return current;

    if (!visitedAliases.insert(definition->id).second)
      return std::nullopt;

    current = definition->alias().target;
  }

  if (current.isList())
  {
    const auto element = resolveAliases(current.elementType());
    if (!element.has_value())
      return std::nullopt;

    return TypeReference::list(*element);
  }

  if (current.isOptional())
  {
    const auto value = resolveAliases(current.optionalValueType());
    if (!value.has_value())
      return std::nullopt;

    return TypeReference::optional(*value);
  }

  if (current.isMap())
  {
    const auto key = resolveAliases(current.mapKeyType());
    const auto value = resolveAliases(current.mapValueType());

    if (!key.has_value() || !value.has_value())
      return std::nullopt;

    return TypeReference::map(*key, *value);
  }

  return current;
}

std::vector<FieldDefinition> TypeRegistry::fieldsOf(const QualifiedName& name) const
{
  return fieldsOf(TypeReference::named(name));
}

std::vector<FieldDefinition> TypeRegistry::fieldsOf(const TypeDefinition& definition) const
{
  if (!definition.isRecord())
    return {};

  return fieldsOf(definition.name);
}

std::vector<FieldDefinition> TypeRegistry::fieldsOf(const TypeReference& reference) const
{
  const auto resolvedReference = resolveAliases(reference);
  if (!resolvedReference.has_value())
    return {};

  const TypeDefinition* definition = resolve(*resolvedReference);
  if (definition == nullptr || !definition->isRecord())
    return {};

  std::vector<FieldDefinition> fields;
  std::vector<TypeId> activeTypes;

  if (!collectFields(*definition, fields, activeTypes))
    return {};

  return fields;
}

const FieldDefinition* TypeRegistry::findField(const TypeReference& recordType, const std::string& fieldName) const
{
  const auto resolvedReference = resolveAliases(recordType);
  if (!resolvedReference.has_value())
    return nullptr;

  const TypeDefinition* definition = resolve(*resolvedReference);

  if (definition == nullptr || !definition->isRecord())
    return nullptr;

  const TypeDefinition* current = definition;
  std::unordered_set<TypeId> visitedTypes;

  while (current != nullptr)
  {
    if (!visitedTypes.insert(current->id).second)
      return nullptr;

    const auto& fields = current->record().fields;

    const auto field =
        std::find_if(fields.begin(), fields.end(), [&fieldName](const FieldDefinition& candidate) { return candidate.name == fieldName; });

    if (field != fields.end())
      return &*field;

    if (!current->record().baseType.has_value())
      return nullptr;

    const auto resolvedBase = resolveAliases(*current->record().baseType);

    if (!resolvedBase.has_value())
      return nullptr;

    current = resolve(*resolvedBase);

    if (current != nullptr && !current->isRecord())
      return nullptr;
  }

  return nullptr;
}

bool TypeRegistry::isBuiltin(const std::string& type) const
{
  const auto definition = findById(type);
  if (!definition)
    return false;

  return isBuiltin(*definition);
}

bool TypeRegistry::isBuiltin(const TypeDefinition& type) const
{
  return type.isPrimitive() && mBuiltinIds.find(type.primitive().primitive) != mBuiltinIds.end();
}

bool TypeRegistry::isAssignable(const TypeReference& source, const TypeReference& target) const
{
  const auto resolvedSource = resolveAliases(source);
  const auto resolvedTarget = resolveAliases(target);

  if (!resolvedSource.has_value() || !resolvedTarget.has_value())
    return false;

  return isAssignableResolved(*resolvedSource, *resolvedTarget);
}

std::vector<TypeModelDiagnostic> TypeRegistry::validate() const
{
  std::vector<TypeModelDiagnostic> diagnostics;

  for (const auto& [id, definition] : mTypes)
  {
    const auto localDiagnostics = definition.validate();
    diagnostics.insert(diagnostics.end(), localDiagnostics.begin(), localDiagnostics.end());

    const std::string typePath = definition.name.toString();
    if (definition.isRecord())
    {
      if (definition.record().baseType.has_value())
      {
        const std::string basePath = typePath + ".base";

        if (validateReference(*definition.record().baseType, basePath, diagnostics))
        {
          const auto resolvedBase = resolveAliases(*definition.record().baseType);
          if (resolvedBase.has_value())
          {
            const TypeDefinition* baseDefinition = resolve(*resolvedBase);
            if (baseDefinition == nullptr || !baseDefinition->isRecord())
            {
              addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error, "record.invalid_base_type",
                            "The base type of a record must also be a record.", basePath);
            }
          }
        }
      }

      for (const FieldDefinition& field : definition.record().fields)
        validateReference(field.type, typePath + "." + field.name, diagnostics);

      if (hasInheritanceCycle(definition))
      {
        addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error, "record.inheritance_cycle",
                      "The record participates in an inheritance cycle.", typePath);
      }

      const std::vector<FieldDefinition> allFields = fieldsOf(TypeReference::named(definition.name, definition.id));

      std::unordered_set<std::string> inheritedNames;

      for (const FieldDefinition& field : allFields)
      {
        if (!inheritedNames.insert(field.name).second)
        {
          addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error, "record.inherited_field_conflict",
                        "The field '" + field.name +
                            "' is declared more than once in the "
                            "inheritance hierarchy.",
                        typePath + "." + field.name);
        }
      }
    }

    if (definition.isAlias())
    {
      validateReference(definition.alias().target, typePath, diagnostics);

      if (hasAliasCycle(definition))
        addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error, "alias.cycle", "The alias participates in an alias cycle.", typePath);
    }
  }

  return diagnostics;
}

std::vector<const TypeDefinition*> TypeRegistry::allTypes() const
{
  std::vector<const TypeDefinition*> result;
  result.reserve(mTypes.size());

  for (const auto& [id, definition] : mTypes)
    result.push_back(&definition);

  std::sort(result.begin(), result.end(),
            [](const TypeDefinition* left, const TypeDefinition* right) { return left->name.toString() < right->name.toString(); });

  return result;
}

std::size_t TypeRegistry::size() const
{
  return mTypes.size();
}

bool TypeRegistry::empty() const
{
  return mTypes.empty();
}

void TypeRegistry::print() const
{
  LOG_DEBUG("Registered types:");
  for (const auto& type : mTypes)
    type.second.print("  ");

  LOG_DEBUG("Ids:");
  for (const auto& id : mIdsByName)
    LOG_DEBUG("  {} -> {}", id.first, id.second);

  LOG_DEBUG("Buildin Ids:");
  for (const auto& id : mBuiltinIds)
    LOG_DEBUG("  {} -> {}", toString(id.first), id.second);
}

TypeRegistrationResult TypeRegistry::validateRegistration(const TypeDefinition& definition, const TypeId* replacedId) const
{
  if (definition.id.empty())
    return TypeRegistrationResult::Failed(TypeRegistrationError::EmptyId, "A type must have a stable ID.");

  if (definition.name.empty())
    return TypeRegistrationResult::Failed(TypeRegistrationError::EmptyName, "A type must have a qualified name.");

  const auto existingId = mTypes.find(definition.id);
  if (existingId != mTypes.end() && (replacedId == nullptr || existingId->first != *replacedId))
    return TypeRegistrationResult::Failed(TypeRegistrationError::DuplicateId,
                                          "A type with ID '" + definition.id + "' (" + definition.name.toString() + ") is already registered.");

  const std::string qualifiedName = nameKey(definition.name);
  const auto existingName = mIdsByName.find(qualifiedName);
  if (existingName != mIdsByName.end() && replacedId == nullptr)
    return TypeRegistrationResult::Failed(TypeRegistrationError::DuplicateName, "A type named '" + qualifiedName + "' is already registered.");

  const auto diagnostics = definition.validate();
  const bool hasErrors = std::any_of(diagnostics.begin(), diagnostics.end(), [](const TypeModelDiagnostic& diagnostic) {
    return diagnostic.severity == TypeModelDiagnostic::Severity::Error;
  });

  if (hasErrors)
  {
    std::string msg = "The type definition contains local validation errors:";
    for (const auto& diagnostic : diagnostics)
      msg += " " + diagnostic.message;

    return TypeRegistrationResult::Failed(TypeRegistrationError::InvalidDefinition, msg);
  }

  // We need to make sure that the underlying types exists.
  if (definition.isAlias())
  {
    const auto& target = definition.alias().target;
    std::vector<TypeModelDiagnostic> aliasDiagnostics;
    if (!validateReference(target, definition.name.toString(), aliasDiagnostics))
      return TypeRegistrationResult::Failed(
          TypeRegistrationError::InvalidDefinition,
          "Alias '" + definition.name.toString() + "' contains an unknown referenced type: " + aliasDiagnostics.front().message);
  }
  else if (definition.isRecord())
  {
    const auto record = definition.record();
    std::vector<TypeModelDiagnostic> recordDiagnostics;
    for (const auto& field : record.fields)
      if (!validateReference(field.type, field.name, recordDiagnostics))
        return TypeRegistrationResult::Failed(
            TypeRegistrationError::InvalidDefinition,
            "Record field '" + field.name + "' contains an unknown referenced type: " + recordDiagnostics.front().message);
  }

  return TypeRegistrationResult(TypeRegistrationError::None);
}

bool TypeRegistry::validateReference(const TypeReference& reference, const std::string& path, std::vector<TypeModelDiagnostic>& diagnostics) const
{
  if (reference.isPrimitive())
    return true;

  if (reference.isNamed())
  {
    if (resolve(reference) == nullptr)
    {
      addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error, "type.unresolved_reference",
                    "The referenced type '" + reference.toString() + "' could not be resolved.", path);
      return false;
    }

    return true;
  }

  if (reference.isList())
    return validateReference(reference.elementType(), path + "[]", diagnostics);

  if (reference.isOptional())
    return validateReference(reference.optionalValueType(), path + "?", diagnostics);

  if (reference.isMap())
  {
    const bool keyValid = validateReference(reference.mapKeyType(), path + ".key", diagnostics);
    const bool valueValid = validateReference(reference.mapValueType(), path + ".value", diagnostics);
    return keyValid && valueValid;
  }

  addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error, "type.unknown_type",
                "The referenced type '" + reference.toString() + "' is unknown", path);

  return false;
}

bool TypeRegistry::hasAliasCycle(const TypeDefinition& definition) const
{
  if (!definition.isAlias())
    return false;

  std::unordered_set<TypeId> visited;
  const TypeDefinition* current = &definition;

  while (current != nullptr && current->isAlias())
  {
    if (!visited.insert(current->id).second)
      return true;

    const TypeReference& target = current->alias().target;

    if (!target.isNamed())
      return false;

    current = resolve(target);
  }

  return false;
}

bool TypeRegistry::hasInheritanceCycle(const TypeDefinition& definition) const
{
  if (!definition.isRecord())
    return false;

  std::unordered_set<TypeId> visited;
  const TypeDefinition* current = &definition;

  while (current != nullptr && current->isRecord())
  {
    if (!visited.insert(current->id).second)
      return true;

    if (!current->record().baseType.has_value())
      return false;

    const auto resolvedBase = resolveAliases(*current->record().baseType);

    if (!resolvedBase.has_value())
      return false;

    current = resolve(*resolvedBase);
  }

  return false;
}

bool TypeRegistry::collectFields(const TypeDefinition& definition, std::vector<FieldDefinition>& fields, std::vector<TypeId>& activeTypes) const
{
  if (!definition.isRecord())
    return false;

  if (std::find(activeTypes.begin(), activeTypes.end(), definition.id) != activeTypes.end())
    return false;

  activeTypes.push_back(definition.id);

  if (definition.record().baseType.has_value())
  {
    const auto resolvedBase = resolveAliases(*definition.record().baseType);
    if (!resolvedBase.has_value())
    {
      activeTypes.pop_back();
      return false;
    }

    const TypeDefinition* base = resolve(*resolvedBase);
    if (base == nullptr || !base->isRecord())
    {
      activeTypes.pop_back();
      return false;
    }

    if (!collectFields(*base, fields, activeTypes))
    {
      activeTypes.pop_back();
      return false;
    }
  }

  fields.insert(fields.end(), definition.record().fields.begin(), definition.record().fields.end());

  activeTypes.pop_back();
  return true;
}

bool TypeRegistry::isAssignableResolved(const TypeReference& source, const TypeReference& target) const
{
  if (source == target)
    return true;

  // A non-optional value may be assigned to Optional<T>.
  if (target.isOptional())
  {
    if (source.isOptional())
      return isAssignable(source.optionalValueType(), target.optionalValueType());

    return isAssignable(source, target.optionalValueType());
  }

  // Optional<T> cannot be assigned to T without an explicit null check.
  if (source.isOptional())
    return false;

  if (source.isPrimitive() && target.isPrimitive())
    return isPrimitiveAssignable(source.primitiveKind(), target.primitiveKind());

  if (source.isList() && target.isList())
    return isAssignable(source.elementType(), target.elementType());

  if (source.isMap() && target.isMap())
  {
    // Map keys are kept invariant. Changing a key type can alter lookup
    // semantics.
    const bool keyCompatible = source.mapKeyType() == target.mapKeyType();
    const bool valueCompatible = isAssignable(source.mapValueType(), target.mapValueType());

    return keyCompatible && valueCompatible;
  }

  if (!source.isNamed() || !target.isNamed())
    return false;

  const TypeDefinition* sourceDefinition = resolve(source);
  const TypeDefinition* targetDefinition = resolve(target);
  if (sourceDefinition == nullptr || targetDefinition == nullptr)
    return false;

  if (sourceDefinition->id == targetDefinition->id)
    return true;

  // A derived record can be assigned to a variable of its base type.
  if (sourceDefinition->isRecord() && targetDefinition->isRecord())
  {
    const TypeDefinition* current = sourceDefinition;
    std::unordered_set<TypeId> visited;

    while (current != nullptr && current->isRecord())
    {
      if (!visited.insert(current->id).second)
        return false;

      if (current->id == targetDefinition->id)
        return true;

      if (!current->record().baseType.has_value())
        return false;

      const auto resolvedBase = resolveAliases(*current->record().baseType);

      if (!resolvedBase.has_value())
        return false;

      current = resolve(*resolvedBase);
    }
  }

  return false;
}

const TypeDefinition* TypeRegistry::builtinDefinition(PrimitiveKind primitive) const
{
  const auto iterator = mBuiltinIds.find(primitive);
  if (iterator == mBuiltinIds.end())
    return nullptr;

  return findById(iterator->second);
}

std::string TypeRegistry::nameKey(const QualifiedName& name)
{
  return name.toString();
}

}  // namespace koda::types