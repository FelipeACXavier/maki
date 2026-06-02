#include "cled.hh"

#include <Arduino.h>

cled::cled(dzn::locator const& locator)
    : skel::cled(locator)
{
}

cled::~cled()
{
}

void cled::start_capability()
{
  pinMode(LED_BUILTIN, OUTPUT);
}

Result cled::start_trigger()
{
  digitalWrite(LED_BUILTIN, HIGH);
  return Result::Done;
}

Result cled::start_abort()
{
  return Result::Success;
}

Result cled::start_reset()
{
  return Result::Success;
}

Result cled::stop_trigger()
{
  digitalWrite(LED_BUILTIN, LOW);
  return Result::Done;
}

Result cled::stop_abort()
{
  return Result::Success;
}

Result cled::stop_reset()
{
  return Result::Success;
}
