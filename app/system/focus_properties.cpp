#include "focus_properties.h"

#include <oclero/qlementine.hpp>

namespace maki
{
FocusProperties FocusProperties::internal()
{
  if (const auto* style = oclero::qlementine::appStyle())
  {
    return FocusProperties{
        .color = style->theme().statusColorInfo,
        .reason = FocusReason::INTERNAL,
    };
  }
  else
  {
    return FocusProperties{.reason = FocusReason::INTERNAL};
  }
}

FocusProperties FocusProperties::fromInfoMessage(const QString& message)
{
  if (const auto* style = oclero::qlementine::appStyle())
  {
    return FocusProperties{
        .message = message,
        .color = style->theme().statusColorInfo,
        .reason = FocusReason::MESSAGE,
    };
  }
  else
  {
    return FocusProperties{.message = message, .reason = FocusReason::MESSAGE};
  }
}

FocusProperties FocusProperties::fromErrorMessage(const QString& message)
{
  if (const auto* style = oclero::qlementine::appStyle())
  {
    return FocusProperties{
        .message = message,
        .color = style->theme().statusColorError,
        .reason = FocusReason::MESSAGE,
    };
  }
  else
  {
    return FocusProperties{.message = message, .reason = FocusReason::MESSAGE};
  }
}

FocusProperties FocusProperties::fromSimulation(const maki::SimulationProperties& properties)
{
  return FocusProperties{
      .widget = properties.widget,
      .color = properties.highlight,
      .reason = FocusReason::SIMULATION,
  };
}

}  // namespace maki