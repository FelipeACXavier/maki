#pragma once

#include <string>

#include "a_led.hh"
#include "types.hh"

class cled : public skel::cled
{
public:
  cled(dzn::locator const& locator);
  ~cled();

  void start_capability();

  Result start_trigger() override;
  Result start_abort() override;
  Result start_reset() override;
  Result stop_trigger() override;
  Result stop_abort() override;
  Result stop_reset() override;
};