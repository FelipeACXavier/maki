#include "dezyne_writer.h"

#include <filesystem>
#include <fstream>

namespace koda::dezyne
{
VoidResult Writer::write(const Model& model, bool dryRun)
{
  mGeneratedFiles.clear();
  if (dryRun)
    return {};

  for (const auto& file : model.mFiles)
  {
    const auto parent = std::filesystem::path(file.path).parent_path();
    if (!parent.empty())
      std::filesystem::create_directories(parent);

    std::ofstream stream(file.path);
    if (!stream.is_open())
      return VoidResult::Failed("Failed to open: " + file.path);

    stream << file.contents;
    stream.close();
    mGeneratedFiles.push_back(file.path);
  }
  return {};
}

}  // namespace koda::dezyne
