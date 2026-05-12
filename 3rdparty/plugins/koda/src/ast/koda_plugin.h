#pragma once

#include "ast.h"
#include "result.h"

namespace koda
{
struct CompilerOptions;

class KodaPlugin
{
public:
  virtual std::string id() const = 0;
  virtual VoidResult generate(const CompilerOptions& options, const koda::System& ast) = 0;

  std::function<std::string(const std::string& name)> toFilename;
  std::function<std::string(const std::string& name)> componentName;
  std::function<std::string(const std::string& name)> flowName;
};
}  // namespace koda