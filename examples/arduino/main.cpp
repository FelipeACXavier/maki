#include <Arduino.h>

#include <iostream>
#include <memory>

#include "blink_task.hh"
#include "cled.hh"
#include "ctimer.hh"

std::ostream nullstream(nullptr);
dzn::runtime runtime;
dzn::locator locator;
std::unique_ptr<cblink> task;

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  task = std::make_unique<cblink>(locator
                                      .set(runtime)
                                      .set(nullstream));

  task->api.out.success = [] {
    Serial.println("System succeeded");
  };
  task->api.out.failure = [] {
    Serial.println("System failed");
  };

  task->led.start_capability();
  task->timer.start_capability();
}

void loop()
{
  task->api.in.trigger();
}
