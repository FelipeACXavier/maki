#pragma once

#include <string>
#include <vector>

#include "dezyne_model.h"
#include "result.h"

namespace koda::dezyne
{
class Writer
{
public:
  VoidResult write(const Model& model, bool dryRun);
  const std::vector<std::string>& generatedFiles() const { return mGeneratedFiles; }

private:
  std::vector<std::string> mGeneratedFiles;
};

}  // namespace koda::dezyne
