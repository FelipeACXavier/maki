#include "type_reference.h"

#include <stdexcept>

#include "helpers.h"

namespace koda::types
{
TypeReference::TypeReference()
    : TypeReference({}, 0)
{
}

TypeReference::TypeReference(std::vector<Node> nodes, std::size_t rootNode)
    : mNodes(std::move(nodes))
    , mRootNode(rootNode)
{
  // if (mNodes.empty() || mRootNode >= mNodes.size())
  // throw std::invalid_argument("Invalid TypeReference node tree");
}

TypeReference TypeReference::primitive(PrimitiveKind kind)
{
  return TypeReference(
      {
          Node{
              .value =
                  Primitive{
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
              .value =
                  Named{
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
      .value =
          List{
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
      .value =
          Optional{
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
      .value =
          Map{
              .keyNode = keyNode,
              .valueNode = valueNode,
          },
  });

  return TypeReference(std::move(nodes), rootNode);
}

TypeReferenceKind TypeReference::kind() const
{
  if (std::holds_alternative<Primitive>(root().value))
    return TypeReferenceKind::Primitive;
  else if (std::holds_alternative<Named>(root().value))
    return TypeReferenceKind::Named;
  else if (std::holds_alternative<List>(root().value))
    return TypeReferenceKind::List;
  else if (std::holds_alternative<Optional>(root().value))
    return TypeReferenceKind::Optional;
  else
    return TypeReferenceKind::Map;
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

    case TypeReferenceKind::Unknown:
      return "Unknown";
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

    case TypeReferenceKind::Unknown:
      return false;
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
    std::visit(
        [offset](auto& value) {
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
}  // namespace koda::types