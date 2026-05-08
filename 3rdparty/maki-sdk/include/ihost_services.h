#pragma once
#include <QString>

class IDocument;

namespace maki
{
class IPipeline;
class ISettings;
class IUI;
class ILogging;

class IHostServices
{
public:
  virtual ~IHostServices() = default;

  virtual IDocument* document() const = 0;
  virtual ISettings* settings() const = 0;
  virtual IPipeline* pipeline() const = 0;
  virtual QString projectRoot() const = 0;
  virtual IUI* ui() const = 0;
  virtual ILogging* logger() const = 0;

  // Optional later: logger(), settings(), uiHooks(), etc.
};

}  // namespace maki
