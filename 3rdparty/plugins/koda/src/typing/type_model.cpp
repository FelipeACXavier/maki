#include "type_model.h"

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <unordered_set>
#include <utility>

#include "string_helpers.h"

namespace koda::types
{

template <typename T>
const T& getValue(const TypeDefinitionData& data, const std::string& expectedType)
{
  const auto* value = std::get_if<T>(&data);
  if (value == nullptr)
    throw std::logic_error("Type definition is not a " + expectedType);

  return *value;
}

template <typename T>
T& getValue(TypeDefinitionData& data, const std::string& expectedType)
{
  auto* value = std::get_if<T>(&data);
  if (value == nullptr)
    throw std::logic_error("Type definition is not a " + expectedType);

  return *value;
}

std::vector<std::string> split(const std::string& value, const std::string& separator)
{
  std::vector<std::string> splitList;
  size_t prev = 0;
  for (size_t idx = value.find(separator); idx != std::string::npos; idx = value.find(separator, idx + 1))
  {
    splitList.push_back(value.substr(prev, idx - prev));
    prev = idx + separator.size();
  }

  std::string remainder = value.substr(prev);
  if (!remainder.empty())
    splitList.push_back(remainder);

  return splitList;
}

bool containsWhitespace(const std::string& value)
{
  return std::any_of(value.begin(), value.end(), [](unsigned char character) {
    return std::isspace(character) != 0;
  });
}

void addDiagnostic(std::vector<TypeModelDiagnostic>& diagnostics, TypeModelDiagnostic::Severity severity, std::string code, std::string message, std::string path)
{
  diagnostics.push_back(TypeModelDiagnostic{
      .severity = severity,
      .code = std::move(code),
      .message = std::move(message),
      .path = std::move(path),
  });
}

// ===========================================================================================================
// QualifiedName
bool QualifiedName::empty() const
{
  return name.empty();
}

std::string QualifiedName::toString(const std::string& separator) const
{
  std::ostringstream stream;
  for (const auto& namespacePart : namespacePath)
  {
    if (stream.tellp() > 0)
      stream << separator;

    stream << namespacePart;
  }

  if (!name.empty())
  {
    if (stream.tellp() > 0)
      stream << separator;

    stream << name;
  }

  return stream.str();
}

QualifiedName::QualifiedName(const std::string& value, const std::string& separator)
{
  const auto parts = split(value, separator);
  if (parts.empty())
    throw std::invalid_argument("No value passed");

  name = parts.back();
  if (parts.size() > 1)
    namespacePath.assign(parts.begin(), parts.end() - 1);
}

// ===========================================================================================================
// TypeReference
TypeReference::TypeReference(std::vector<Node> nodes, std::size_t rootNode)
    : mNodes(std::move(nodes))
    , mRootNode(rootNode)
{
  if (mNodes.empty() || mRootNode >= mNodes.size())
    throw std::invalid_argument("Invalid TypeReference node tree");
}

TypeReference TypeReference::primitive(PrimitiveKind kind)
{
  return TypeReference(
      {
          Node{
              .value = Primitive{
                  .kind = kind,
              },
          },
      },
      0);
}

TypeReference TypeReference::named(QualifiedName name, std::optional<TypeId> id)
{
  return TypeReference(
      {
          Node{
              .value = Named{
                  .id = std::move(id),
                  .name = std::move(name),
              },
          },
      },
      0);
}

TypeReference TypeReference::named(std::string name, std::optional<TypeId> id)
{
  return named(QualifiedName(name), std::move(id));
}

TypeReference TypeReference::list(TypeReference elementType)
{
  std::vector<Node> nodes;
  const std::size_t elementNode = appendTree(nodes, elementType);

  const std::size_t rootNode = nodes.size();
  nodes.push_back(Node{
      .value = List{
          .elementNode = elementNode,
      },
  });

  return TypeReference(std::move(nodes), rootNode);
}

TypeReference TypeReference::optional(TypeReference valueType)
{
  std::vector<Node> nodes;
  const std::size_t valueNode = appendTree(nodes, valueType);

  const std::size_t rootNode = nodes.size();
  nodes.push_back(Node{
      .value = Optional{
          .valueNode = valueNode,
      },
  });

  return TypeReference(std::move(nodes), rootNode);
}

TypeReference TypeReference::map(TypeReference keyType, TypeReference valueType)
{
  std::vector<Node> nodes;

  const std::size_t keyNode = appendTree(nodes, keyType);
  const std::size_t valueNode = appendTree(nodes, valueType);

  const std::size_t rootNode = nodes.size();
  nodes.push_back(Node{
      .value = Map{
          .keyNode = keyNode,
          .valueNode = valueNode,
      },
  });

  return TypeReference(std::move(nodes), rootNode);
}

TypeReferenceKind TypeReference::kind() const
{
  return std::visit([](const auto& value) -> TypeReferenceKind {
    using Value = std::decay_t<decltype(value)>;
    if constexpr (std::is_same_v<Value, Primitive>)
      return TypeReferenceKind::Primitive;
    else if constexpr (std::is_same_v<Value, Named>)
      return TypeReferenceKind::Named;
    else if constexpr (std::is_same_v<Value, List>)
      return TypeReferenceKind::List;
    else if constexpr (std::is_same_v<Value, Optional>)
      return TypeReferenceKind::Optional;
    else
      return TypeReferenceKind::Map;
  },
                    root().value);
}

bool TypeReference::isPrimitive() const
{
  return kind() == TypeReferenceKind::Primitive;
}

bool TypeReference::isNamed() const
{
  return kind() == TypeReferenceKind::Named;
}

bool TypeReference::isList() const
{
  return kind() == TypeReferenceKind::List;
}

bool TypeReference::isOptional() const
{
  return kind() == TypeReferenceKind::Optional;
}

bool TypeReference::isMap() const
{
  return kind() == TypeReferenceKind::Map;
}

PrimitiveKind TypeReference::primitiveKind() const
{
  const auto* value = std::get_if<Primitive>(&root().value);
  if (value == nullptr)
    throw std::logic_error("Type reference is not primitive");

  return value->kind;
}

const TypeReference::Named& TypeReference::namedType() const
{
  const auto* value = std::get_if<Named>(&root().value);
  if (value == nullptr)
    throw std::logic_error("Type reference is not named");

  return *value;
}

TypeReference TypeReference::elementType() const
{
  const auto* value = std::get_if<List>(&root().value);
  if (value == nullptr)
    throw std::logic_error("Type reference is not a list");

  return subtree(value->elementNode);
}

TypeReference TypeReference::optionalValueType() const
{
  const auto* value = std::get_if<Optional>(&root().value);
  if (value == nullptr)
    throw std::logic_error("Type reference is not optional");

  return subtree(value->valueNode);
}

TypeReference TypeReference::mapKeyType() const
{
  const auto* value = std::get_if<Map>(&root().value);
  if (value == nullptr)
    throw std::logic_error("Type reference is not a map");

  return subtree(value->keyNode);
}

TypeReference TypeReference::mapValueType() const
{
  const auto* value = std::get_if<Map>(&root().value);
  if (value == nullptr)
    throw std::logic_error("Type reference is not a map");

  return subtree(value->valueNode);
}

std::string TypeReference::toString() const
{
  switch (kind())
  {
    case TypeReferenceKind::Primitive:
      return koda::types::toString(primitiveKind());

    case TypeReferenceKind::Named:
      return namedType().name.toString();

    case TypeReferenceKind::List:
      return "List<" + elementType().toString() + ">";

    case TypeReferenceKind::Optional:
      return "Optional<" + optionalValueType().toString() + ">";

    case TypeReferenceKind::Map:
      return "Map<" + mapKeyType().toString() + ", " + mapValueType().toString() + ">";
  }

  throw std::logic_error("Unknown type reference kind");
}

bool TypeReference::structurallyEquals(const TypeReference& other) const
{
  if (kind() != other.kind())
    return false;

  switch (kind())
  {
    case TypeReferenceKind::Named:
    {
      const Named& left = namedType();
      const Named& right = other.namedType();

      // Prefer stable IDs when both references are resolved.
      if (left.id.has_value() && right.id.has_value())
        return left.id == right.id;

      return left.name == right.name;
    }
    case TypeReferenceKind::Primitive:
      return primitiveKind() == other.primitiveKind();

    case TypeReferenceKind::List:
      return elementType().structurallyEquals(other.elementType());

    case TypeReferenceKind::Optional:
      return optionalValueType().structurallyEquals(other.optionalValueType());

    case TypeReferenceKind::Map:
      return mapKeyType().structurallyEquals(other.mapKeyType()) && mapValueType().structurallyEquals(other.mapValueType());
  }

  return false;
}

bool TypeReference::operator==(const TypeReference& other) const
{
  return structurallyEquals(other);
}

bool TypeReference::operator!=(const TypeReference& other) const
{
  return !(*this == other);
}

std::size_t TypeReference::appendTree(std::vector<Node>& target, const TypeReference& source)
{
  const std::size_t offset = target.size();

  for (const Node& sourceNode : source.mNodes)
  {
    Node copiedNode = sourceNode;
    std::visit([offset](auto& value) {
      using Value = std::decay_t<decltype(value)>;
      if constexpr (std::is_same_v<Value, List>)
      {
        value.elementNode += offset;
      }
      else if constexpr (std::is_same_v<Value, Optional>)
      {
        value.valueNode += offset;
      }
      else if constexpr (std::is_same_v<Value, Map>)
      {
        value.keyNode += offset;
        value.valueNode += offset;
      }
    },
               copiedNode.value);

    target.push_back(std::move(copiedNode));
  }

  return source.mRootNode + offset;
}

const TypeReference::Node& TypeReference::root() const
{
  return mNodes.at(mRootNode);
}

TypeReference TypeReference::subtree(std::size_t rootNode) const
{
  if (rootNode >= mNodes.size())
    throw std::out_of_range("Invalid TypeReference subtree node");

  // Keeping the complete backing tree makes this operation inexpensive and
  // avoids rebuilding nested structures.
  return TypeReference(mNodes, rootNode);
}

// ===========================================================================================================
// TypeDefinition
TypeKind TypeDefinition::kind() const
{
  return std::visit([](const auto& value) -> TypeKind {
    using Value = std::decay_t<decltype(value)>;

    if constexpr (std::is_same_v<Value, PrimitiveTypeDefinition>)
      return TypeKind::Primitive;
    else if constexpr (std::is_same_v<Value, RecordTypeDefinition>)
      return TypeKind::Record;
    else if constexpr (std::is_same_v<Value, EnumTypeDefinition>)
      return TypeKind::Enum;
    else  // if constexpr (std::is_same_v<Value, AliasTypeDefinition>)
      return TypeKind::Alias;
  },
                    data);
}

bool TypeDefinition::isPrimitive() const
{
  return kind() == TypeKind::Primitive;
}

bool TypeDefinition::isRecord() const
{
  return kind() == TypeKind::Record;
}

bool TypeDefinition::isEnum() const
{
  return kind() == TypeKind::Enum;
}

bool TypeDefinition::isAlias() const
{
  return kind() == TypeKind::Alias;
}

const PrimitiveTypeDefinition& TypeDefinition::primitive() const
{
  return getValue<PrimitiveTypeDefinition>(data, "primitive");
}

PrimitiveTypeDefinition& TypeDefinition::primitive()
{
  return getValue<PrimitiveTypeDefinition>(data, "primitive");
}

const RecordTypeDefinition& TypeDefinition::record() const
{
  return getValue<RecordTypeDefinition>(data, "record");
}

RecordTypeDefinition& TypeDefinition::record()
{
  return getValue<RecordTypeDefinition>(data, "record");
}

const EnumTypeDefinition& TypeDefinition::enumeration() const
{
  return getValue<EnumTypeDefinition>(data, "enum");
}

EnumTypeDefinition& TypeDefinition::enumeration()
{
  return getValue<EnumTypeDefinition>(data, "enum");
}

const AliasTypeDefinition& TypeDefinition::alias() const
{
  return getValue<AliasTypeDefinition>(data, "alias");
}

AliasTypeDefinition& TypeDefinition::alias()
{
  return getValue<AliasTypeDefinition>(data, "alias");
}

std::vector<TypeModelDiagnostic> TypeDefinition::validate() const
{
  std::vector<TypeModelDiagnostic> diagnostics;
  const std::string typePath = name.toString();

  if (id.empty())
  {
    addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error,
                  "type.empty_id",
                  "The type has no stable ID.",
                  typePath);
  }

  if (name.name.empty())
  {
    addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error,
                  "type.empty_name",
                  "The type has no name.",
                  typePath);
  }
  else if (containsWhitespace(name.name))
  {
    addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error,
                  "type.invalid_name",
                  "The type name contains whitespace.",
                  typePath);
  }

  for (const auto& namespacePart : name.namespacePath)
  {
    if (namespacePart.empty())
    {
      addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error,
                    "type.empty_namespace_part",
                    "The type namespace contains an empty component.",
                    typePath);
    }
    else if (containsWhitespace(namespacePart))
    {
      addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error,
                    "type.invalid_namespace_part",
                    "A type namespace component contains whitespace.",
                    typePath);
    }
  }

  if (isRecord())
  {
    std::unordered_set<std::string> fieldNames;
    for (const FieldDefinition& field : record().fields)
    {
      const std::string fieldPath = typePath.empty() ? field.name : typePath + "." + field.name;

      if (field.name.empty())
      {
        addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error,
                      "record.empty_field_name",
                      "The record contains a field without a name.",
                      fieldPath);

        continue;
      }

      if (containsWhitespace(field.name))
      {
        addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error,
                      "record.invalid_field_name",
                      "The field name contains whitespace.",
                      fieldPath);
      }

      if (!fieldNames.insert(field.name).second)
      {
        addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error,
                      "record.duplicate_field",
                      "The record contains more than one field named '" + field.name + "'.",
                      fieldPath);
      }

      if (!field.required && field.defaultValue.has_value())
      {
        addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Warning,
                      "record.optional_field_with_default",
                      "The optional field has a default value. Ensure the distinction "
                      "between an absent value and the default value is intentional.",
                      fieldPath);
      }
    }
  }

  if (isEnum())
  {
    std::unordered_set<std::string> enumValueNames;
    std::unordered_set<std::string> explicitValues;

    for (const EnumValueDefinition& enumValue : enumeration().values)
    {
      const std::string valuePath = typePath.empty() ? enumValue.name : typePath + "." + enumValue.name;
      if (enumValue.name.empty())
      {
        addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error,
                      "enum.empty_value_name",
                      "The enum contains a value without a name.",
                      valuePath);

        continue;
      }

      if (!enumValueNames.insert(enumValue.name).second)
      {
        addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Error,
                      "enum.duplicate_value_name",
                      "The enum contains more than one value named '" + enumValue.name + "'.",
                      valuePath);
      }

      if (enumValue.value.has_value() && !explicitValues.insert(*enumValue.value).second)
      {
        addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Warning,
                      "enum.duplicate_explicit_value",
                      "More than one enum item uses the explicit value " + *enumValue.value + ".",
                      valuePath);
      }
    }

    if (enumeration().values.empty())
    {
      addDiagnostic(diagnostics, TypeModelDiagnostic::Severity::Warning,
                    "enum.empty",
                    "The enum contains no values.",
                    typePath);
    }
  }

  return diagnostics;
}

std::string toString(PrimitiveKind kind)
{
  switch (kind)
  {
    case PrimitiveKind::Bool:
      return "Bool";

    case PrimitiveKind::Int8:
      return "Int8";

    case PrimitiveKind::Int16:
      return "Int16";

    case PrimitiveKind::Int32:
      return "Int32";

    case PrimitiveKind::Int64:
      return "Int64";

    case PrimitiveKind::UInt8:
      return "UInt8";

    case PrimitiveKind::UInt16:
      return "UInt16";

    case PrimitiveKind::UInt32:
      return "UInt32";

    case PrimitiveKind::UInt64:
      return "UInt64";

    case PrimitiveKind::Float32:
      return "Float32";

    case PrimitiveKind::Float64:
      return "Float64";

    case PrimitiveKind::String:
      return "String";

    case PrimitiveKind::Bytes:
      return "Bytes";

    case PrimitiveKind::Timestamp:
      return "Timestamp";

    case PrimitiveKind::Duration:
      return "Duration";
  }

  throw std::logic_error("Unknown primitive kind");
}

std::string toString(TypeKind kind)
{
  switch (kind)
  {
    case TypeKind::Primitive:
      return "Primitive";

    case TypeKind::Record:
      return "Record";

    case TypeKind::Enum:
      return "Enum";

    case TypeKind::Alias:
      return "Alias";
  }

  throw std::logic_error("Unknown type kind");
}

std::string toString(TypeReferenceKind kind)
{
  switch (kind)
  {
    case TypeReferenceKind::Primitive:
      return "Primitive";

    case TypeReferenceKind::Named:
      return "Named";

    case TypeReferenceKind::List:
      return "List";

    case TypeReferenceKind::Optional:
      return "Optional";

    case TypeReferenceKind::Map:
      return "Map";
  }

  throw std::logic_error("Unknown type reference kind");
}

std::string toString(EnumUnderlyingKind kind)
{
  switch (kind)
  {
    case EnumUnderlyingKind::Int32:
      return "Int32";

    case EnumUnderlyingKind::UInt32:
      return "UInt32";

    case EnumUnderlyingKind::String:
      return "String";
  }

  throw std::logic_error("Unknown enum underlying kind");
}

}  // namespace koda::types