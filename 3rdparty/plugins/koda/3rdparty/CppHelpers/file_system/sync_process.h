#pragma once

#include <string>

class SyncProcess
{
public:
  static int Execute(const std::string& cmd, std::string& result);
  static std::string Error(int ret);
};
