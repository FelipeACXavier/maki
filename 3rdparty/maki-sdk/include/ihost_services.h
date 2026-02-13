#pragma once
#include <QString>

class IDocument;

namespace maki
{
class IPipeline;
class ISettings;

class IHostServices
{
public:
  virtual ~IHostServices() = default;

  virtual IDocument* document() const = 0;
  virtual ISettings* settings() const = 0;
  virtual IPipeline* pipeline() const = 0;
  virtual QString projectRoot() const = 0;

  // Optional later: logger(), settings(), uiHooks(), etc.
};

}  // namespace maki
