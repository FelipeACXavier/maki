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
  OPEN_BROWSER
};

class IPipeline
{
public:
  virtual ~IPipeline() = default;
  virtual VoidResult add(QProcess* process, OnFail onFail, const QString& options = {}) = 0;
};

}  // namespace maki
