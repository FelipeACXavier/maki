#pragma once

#include <optional>
#include <variant>

#include "types.h"

namespace koda::types
{
class TypeReference
{
public:
  struct Primitive
  {
    PrimitiveKind kind;
    auto operator<=>(const Primitive&) const = default;
  };

  struct Named
  {
    // The ID is authoritative when available.
    //
    // name is retained as a human-readable fallback and is also useful while
    // parsing unresolved source code.
    std::optional<TypeId> id;
    QualifiedName name;

    auto operator<=>(const Named&) const = default;
  };

  struct List
  {
    // Indices into an internal node store are intentionally avoided. A nested
    // TypeReference is represented recursively through the implementation.
    std::size_t elementNode;

    auto operator<=>(const List&) const = default;
  };

  struct Optional
  {
    std::size_t valueNode;

    auto operator<=>(const Optional&) const = default;
  };

  struct Map
  {
    std::size_t keyNode;
    std::size_t valueNode;

    auto operator<=>(const Map&) const = default;
  };

  TypeReference();

  static TypeReference primitive(PrimitiveKind kind);
  static TypeReference named(QualifiedName name, std::optional<TypeId> id = std::nullopt);
  static TypeReference named(std::string name, std::optional<TypeId> id = std::nullopt);
  static TypeReference list(TypeReference elementType);
  static TypeReference optional(TypeReference valueType);
  static TypeReference map(TypeReference keyType, TypeReference valueType);

  static TypeReference createVoid();
  static TypeReference createBool();
  static TypeReference createInt();
  static TypeReference createUint();
  static TypeReference createReal();
  static TypeReference createString();

  TypeReferenceKind kind() const;

  bool isPrimitive() const;
  bool isNamed() const;
  bool isList() const;
  bool isOptional() const;
  bool isMap() const;
  bool isValid() const;
  bool isNumeric() const;

  PrimitiveKind primitiveKind() const;
  const Named& namedType() const;

  TypeReference elementType() const;
  TypeReference optionalValueType() const;
  TypeReference mapKeyType() const;
  TypeReference mapValueType() const;

  std::string toString() const;

  bool structurallyEquals(const TypeReference& other) const;

  bool operator==(const TypeReference& other) const;
  bool operator!=(const TypeReference& other) const;

private:
  struct Node
  {
    std::variant<Primitive, Named, List, Optional, Map> value;
  };

  std::vector<Node> mNodes;
  std::size_t mRootNode = 0;

  TypeReference(std::vector<Node> nodes, std::size_t rootNode);

  static std::size_t appendTree(std::vector<Node>& target, const TypeReference& source);

  const Node& root() const;
  TypeReference subtree(std::size_t rootNode) const;
};
}  // namespace koda::types