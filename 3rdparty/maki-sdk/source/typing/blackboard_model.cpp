#include "typing/blackboard_model.h"

#include "logging.h"

namespace koda::types
{

void Blackboard::print() const
{
  LOG_DEBUG("Slots:");
  for (const auto& [id, slot] : mSlots)
    LOG_DEBUG("  Id: {} Slot: {} {}", id, slot.name, slot.type.toString());

  LOG_DEBUG("Available:");
  for (const auto& [id, availability] : mAvailable)
    LOG_DEBUG("  Id: {} Available? {} ", id, availability);
}

SlotId Blackboard::declare(const std::string& id, const std::string& name, const types::TypeReference& type, std::optional<std::string> producerId)
{
  mSlots.insert_or_assign(id, BlackboardSlot{
                                  .id = id,
                                  .name = name,
                                  .type = type,
                                  .producerId = std::move(producerId),
                              });

  return id;
}

// Mark a slot as definitely containing a valid value.
void Blackboard::makeAvailable(const SlotId& id)
{
  mAvailable[id] = true;
}

// Mark the value as no longer guaranteed to exist.
void Blackboard::invalidate(const SlotId& id)
{
  mAvailable[id] = false;
}

bool Blackboard::isAvailable(const SlotId& id) const
{
  const auto it = mAvailable.find(id);
  return it != mAvailable.end() && it->second;
}

const BlackboardSlot* Blackboard::get(const SlotId& id) const
{
  const auto it = mSlots.find(id);
  return it == mSlots.end() ? nullptr : &it->second;
}

std::vector<const BlackboardSlot*> Blackboard::availableCompatible(const types::TypeReference& requiredType,
                                                                   const types::TypeRegistry& registry) const
{
  std::vector<const BlackboardSlot*> result;
  for (const auto& [id, available] : mAvailable)
  {
    if (!available)
      continue;

    const auto* slot = get(id);
    if (!slot)
      continue;

    if (registry.isAssignable(slot->type, requiredType))
    {
      LOG_DEBUG("    Found assignable {} for {}", slot->id, requiredType.toString());
      result.push_back(slot);
    }
  }

  return result;
}

// Useful when entering a branch.
Blackboard Blackboard::fork() const
{
  return *this;
}

// Keep only values guaranteed to be available on both branches.
void Blackboard::intersectAvailability(const Blackboard& other)
{
  for (auto& [id, available] : mAvailable)
    available = available && other.isAvailable(id);
}
}  // namespace koda::types