#pragma once

#include <iosfwd>
#include <string>
#include <vector>

#include "nuxmv_model.h"
#include "result.h"

namespace koda::nuxmv
{
class Writer
{
public:
  VoidResult write(const Model& model, bool dryRun);
  VoidResult write(const Model& model, std::ostream& stream) const;

  const std::vector<std::string>& generatedFiles() const;
  void clear();

private:
  void writeConstants(std::ostream& stream) const;
  void writeVariable(std::ostream& stream, const Variable& variable) const;
  void writeNext(std::ostream& stream, const NextAssignment& assignment) const;

  std::vector<std::string> mGeneratedFiles;
};

}  // namespace koda::nuxmv
