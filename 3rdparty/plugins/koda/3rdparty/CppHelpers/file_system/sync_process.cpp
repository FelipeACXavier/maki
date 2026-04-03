#include "sync_process.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <array>

#include "logging.h"

int SyncProcess::Execute(const std::string& cmd, std::string& result)
{
  std::array<char, 128> buffer;

  FILE* p = popen(cmd.c_str(), "r");
  if (!p)
  {
    LOG_ERROR("Failed to execute: %s", cmd.c_str());
    return -errno;
  }

  result.clear();
  while (!feof(p))
  {
    // nullptr can also occur on newline, so continue
    if (fgets(buffer.data(), 128, p) != nullptr)
    {
      try
      {
        result += buffer.data();
      }
      catch (std::exception& e)
      {
        LOG_ERROR("Exception when updating buffer for cmd");
      }
    }
  }

  int ret = pclose(p);
  if (ret < 0)
  {
    ret = -errno;
    LOG_WARNING("Process '%s' exited with code %d ", cmd.c_str(), -ret);
  }
  else if (ret > 0)
  {
    LOG_DEBUG("Process '%s' exited unsuccessfully with code %d", cmd.c_str(), ret);
  }

  return ret;
}

std::string SyncProcess::Error(int ret)
{
  return strerror(ret);
}