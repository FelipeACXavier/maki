#pragma once

#include <QString>

namespace maki
{
struct PlatformIOTargetProfile
{
  QString board = "esp32dev";
  QString platform = "espressif32";
  QString framework = "arduino";
  QString environment = "esp32dev";

  QString uploadPort;
  QString monitorPort;
  int monitorSpeed = 115200;
};
}  // namespace maki