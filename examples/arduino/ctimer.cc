#include "ctimer.hh"

#include <Arduino.h>

ctimer::ctimer(dzn::locator const& locator)
    : skel::ctimer(locator)
{
}

ctimer::~ctimer()
{
}

void ctimer::start_capability()
{
}

Result ctimer::delay_trigger()
{
  // delay(500);
  return Result::Done;
}

Result ctimer::delay_abort()
{
  return Result::Done;
}

Result ctimer::delay_reset()
{
  return Result::Success;
}
