#pragma once

#include <string>

#include "a_timer.hh"
#include "types.hh"

class ctimer : public skel::ctimer
{
public:
  ctimer(dzn::locator const& locator);
  ~ctimer();

  void start_capability();
  Result delay_trigger() override;
  Result delay_abort() override;
  Result delay_reset() override;
};