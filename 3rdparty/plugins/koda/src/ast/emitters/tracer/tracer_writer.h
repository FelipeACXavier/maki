#pragma once

#include <filesystem>
#include <iosfwd>
#include <string>
#include <vector>

#include "result.h"
#include "tracer_json.h"

namespace koda::tracer
{
// Owns output only; serialization and semantic validation live in Serializer.
class Writer
{
public:
  VoidResult write(const std::filesystem::path& path, const Json& document, bool dryRun);
  VoidResult write(const Json& document, std::ostream& stream) const;

  const std::vector<std::string>& generatedFiles() const;
  void clear();

private:
  std::vector<std::string> mGeneratedFiles;
};
}  // namespace koda::tracer
