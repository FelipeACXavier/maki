#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "helpers.h"
#include "result.h"
#include "type_definition.h"

namespace koda::types
{

enum class TypeRegistrationError
{
  None,
  EmptyId,
  EmptyName,
  DuplicateId,
  DuplicateName,
  InvalidDefinition
};

typedef DataResult<TypeRegistrationError> TypeRegistrationResult;

class TypeRegistry
{
public:
  TypeRegistry();

  void registerBuiltinTypes();

  TypeRegistrationResult add(const TypeDefinition& definition);
  TypeRegistrationResult replace(const TypeDefinition& definition);

  void clear();
  bool removeById(const TypeId& id);

  bool containsId(const TypeId& id) const;
  bool containsName(const QualifiedName& name) const;
  bool containsName(const std::string& qualifiedName) const;

  const TypeDefinition* findById(const TypeId& id) const;
  TypeDefinition* findById(const TypeId& id);

  const TypeDefinition* findByName(const QualifiedName& name) const;
  TypeDefinition* findByName(const QualifiedName& name);
  const TypeDefinition* findByName(const std::string& qualifiedName) const;
  TypeDefinition* findByName(const std::string& qualifiedName);

  const TypeDefinition* resolve(const TypeReference& reference) const;

  std::vector<FieldDefinition> fieldsOf(const QualifiedName& name) const;
  std::vector<FieldDefinition> fieldsOf(const TypeReference& reference) const;
  std::vector<FieldDefinition> fieldsOf(const TypeDefinition& definition) const;

  const FieldDefinition* findField(const TypeReference& recordType, const std::string& fieldName) const;

  bool isAssignable(const TypeReference& source, const TypeReference& target) const;

  std::vector<TypeModelDiagnostic> validate() const;
  std::vector<const TypeDefinition*> allTypes() const;

  std::size_t size() const;
  bool empty() const;
  void print() const;

private:
  std::unordered_map<TypeId, TypeDefinition> mTypes;
  std::unordered_map<std::string, TypeId> mIdsByName;
  std::unordered_map<PrimitiveKind, TypeId> mBuiltinIds;

  std::optional<TypeReference> resolveAliases(const TypeReference& reference) const;
  std::optional<TypeReference> resolveAliases(const QualifiedName& name) const;

  TypeRegistrationResult validateRegistration(const TypeDefinition& definition, const TypeId* replacedId = nullptr) const;
  bool validateReference(const TypeReference& reference, const std::string& path, std::vector<TypeModelDiagnostic>& diagnostics) const;
  bool hasAliasCycle(const TypeDefinition& definition) const;
  bool hasInheritanceCycle(const TypeDefinition& definition) const;
  bool collectFields(const TypeDefinition& definition, std::vector<FieldDefinition>& fields, std::vector<TypeId>& activeTypes) const;
  bool isAssignableResolved(const TypeReference& source, const TypeReference& target) const;
  const TypeDefinition* builtinDefinition(PrimitiveKind primitive) const;
  static std::string nameKey(const QualifiedName& name);
};

}  // namespace koda::types