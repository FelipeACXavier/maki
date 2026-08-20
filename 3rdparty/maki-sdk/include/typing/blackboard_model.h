#pragma once

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "typing/type_reference.h"
#include "typing/type_registry.h"

namespace koda::types
{

using SlotId = std::string;

struct BlackboardSlot
{
  SlotId id;
  std::string name;
  types::TypeReference type;

  // Who created this value, useful for diagnostics/traceability.
  std::optional<std::string> producerId;
};

class Blackboard
{
public:
  // Declare a logical slot. Call once when the slot is created.
  SlotId declare(const std::string& id, const std::string& name, const types::TypeReference& type,
                 std::optional<std::string> producerId = std::nullopt);

  void makeAvailable(const SlotId& id);
  void invalidate(const SlotId& id);

  bool isAvailable(const SlotId& id) const;

  const BlackboardSlot* get(const SlotId& id) const;

  std::vector<const BlackboardSlot*> availableCompatible(const types::TypeReference& requiredType, const types::TypeRegistry& registry) const;

  // Useful when entering a branch.
  Blackboard fork() const;

  // Keep only values guaranteed to be available on both branches.
  void intersectAvailability(const Blackboard& other);

  void print() const;

private:
  std::map<SlotId, BlackboardSlot> mSlots;
  std::map<SlotId, bool> mAvailable;
};

}  // namespace koda::types