#pragma once

#include "elements/flow.h"
#include "elements/node.h"
#include "ihost_services.h"

namespace maki
{

enum class FocusReason
{
  UNKNOWN = 0,
  SIMULATION,
  MESSAGE,
  INTERNAL
};

struct FocusProperties
{
  maki::ControlWidget* widget;
  QString message;
  QColor color;
  FocusReason reason;

  static FocusProperties internal();
  static FocusProperties fromInfoMessage(const QString& message);
  static FocusProperties fromErrorMessage(const QString& message);
  static FocusProperties fromSimulation(const maki::SimulationProperties& properties);
};

}  // namespace maki
