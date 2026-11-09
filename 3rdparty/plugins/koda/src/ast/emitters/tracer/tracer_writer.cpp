#include "tracer_writer.h"

#include <exception>
#include <fstream>
#include <ostream>
#include <system_error>

#include "logging.h"

namespace koda::tracer
{
VoidResult Writer::write(const std::filesystem::path& path, const Json& document, bool dryRun)
{
  if (dryRun)
    return {};
  if (path.empty())
    return VoidResult::Failed("tracer: no output path provided");

  std::error_code ec;
  if (!path.parent_path().empty())
  {
    std::filesystem::create_directories(path.parent_path(), ec);
    if (ec)
      return VoidResult::Failed("tracer: cannot create output directory '" + path.parent_path().string() + "': " + ec.message());
  }

  std::ofstream output(path, std::ios::out | std::ios::binary | std::ios::trunc);
  if (!output.is_open())
    return VoidResult::Failed("tracer: cannot open '" + path.string() + "'");

  auto result = write(document, output);
  output.flush();
  if (!result.IsSuccess() || !output.good())
    return VoidResult::Failed("tracer: failed writing '" + path.string() + "'");

  LOG_DEBUG("Generated reference IR JSON: {}", path.string());
  mGeneratedFiles.push_back(path.string());
  return {};
}

VoidResult Writer::write(const Json& document, std::ostream& stream) const
{
  stream << document.dump();
  if (!stream.good())
    return VoidResult::Failed("tracer: JSON output stream failure");
  return {};
}

const std::vector<std::string>& Writer::generatedFiles() const
{
  return mGeneratedFiles;
}

void Writer::clear()
{
  mGeneratedFiles.clear();
}
}  // namespace koda::tracer
