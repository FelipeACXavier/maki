#pragma once
#include <QProcess>
#include <QString>
#include <QStringList>

#include "result.h"

namespace maki
{

enum class OnFail
{
  STOP = 0,
  CONTINUE,
  EXECUTE,
  ALWAYS_EXECUTE
};

class IPipeline
{
public:
  virtual ~IPipeline() = default;
  virtual VoidResult add(QProcess* process, OnFail onFail, std::function<void(int& exitCode, QProcess::ExitStatus& status)> callback = nullptr) = 0;
  virtual void startGroup(const QString& groupName) = 0;
  virtual void endGroup() = 0;
};

}  // namespace maki
