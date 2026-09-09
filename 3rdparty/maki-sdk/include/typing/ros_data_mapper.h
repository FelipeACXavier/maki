#pragma once

#include <ament_index_cpp/get_package_share_directory.hpp>
#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "type_reference.h"
#include "type_registry.h"

namespace koda::ros
{

struct FlatField
{
  std::string path;
  std::string type;
};

struct RosFieldMapping
{
  std::string sourcePath;
  std::string targetPath;
  double confidence = 0.0;
};

struct RosTypeMapping
{
  // Original annotation, e.g.
  //   nav2_msgs/action/NavigateToPose::Goal
  std::string rosName;

  // Generated C++ type, e.g.
  //   nav2_msgs::action::NavigateToPose::Goal
  std::string cppType;

  // Required generated header, e.g.
  //   nav2_msgs/action/navigate_to_pose.hpp
  std::string include;

  std::vector<RosFieldMapping> fields;
};

class RosDatatypeMapper
{
public:
  RosDatatypeMapper(const koda::types::TypeRegistry& registry);

  std::optional<RosTypeMapping> resolve(const koda::types::TypeReference& type);
  std::optional<std::string> cppType(const koda::types::TypeReference& type);
  std::optional<std::string> includeFor(const koda::types::TypeReference& type);
  std::optional<std::string> targetField(const koda::types::TypeReference& type, const std::string& sourcePath);

private:
  struct RosInterfaceRef
  {
    enum class Kind
    {
      Message,
      Service,
      Action
    };

    std::string package;
    Kind kind = Kind::Message;
    std::string interfaceName;

    // For actions/services:
    // Goal / Result / Feedback / Request / Response.
    std::optional<std::string> subType;
  };

  struct Candidate
  {
    std::string targetPath;
    double score = 0.0;
  };

private:
  [[nodiscard]]
  std::optional<std::string> rosAnnotationFor(const koda::types::TypeReference& type) const;

  [[nodiscard]]
  std::vector<FlatField> flattenKodaRecord(const koda::types::TypeReference& type) const;

  [[nodiscard]]
  std::optional<RosInterfaceRef> parseRosInterface(const std::string& annotation) const;

  [[nodiscard]]
  std::optional<std::filesystem::path> interfaceFile(const RosInterfaceRef& ref) const;

  [[nodiscard]]
  std::vector<FlatField> flattenRosInterface(const RosInterfaceRef& ref) const;

  [[nodiscard]]
  std::vector<FlatField> flattenRosFile(const std::filesystem::path& file, const std::string& package, const std::string& prefix,
                                        std::size_t depth = 0) const;

  [[nodiscard]]
  std::vector<FlatField> flattenRosSection(const std::vector<std::string>& lines, const std::string& package, const std::string& prefix,
                                           std::size_t depth) const;

  [[nodiscard]]
  std::vector<RosFieldMapping> inferMappings(const std::vector<FlatField>& source, const std::vector<FlatField>& target) const;

  [[nodiscard]]
  double scoreField(const FlatField& source, const FlatField& target) const;

  [[nodiscard]]
  bool compatibleTypes(const std::string& source, const std::string& target) const;

  [[nodiscard]]
  bool isRosPrimitive(const std::string& type) const;

  [[nodiscard]]
  std::optional<std::filesystem::path> nestedRosMessageFile(const std::string& currentPackage, const std::string& type) const;

  [[nodiscard]]
  std::string cppTypeFor(const RosInterfaceRef& ref) const;

  [[nodiscard]]
  std::string includeFor(const RosInterfaceRef& ref) const;

private:
  static std::string trim(std::string value);

  static std::vector<std::string> split(const std::string& value, char delimiter);

  static std::string leafName(const std::string& path);

  static std::string parentName(const std::string& path);

  static bool pathEndsWith(const std::string& target, const std::string& suffix);

  static std::string snakeCase(const std::string& name);

  static std::string stripArray(const std::string& type);

  static std::string stripBound(const std::string& type);

private:
  const koda::types::TypeRegistry& mRegistry;

  std::unordered_map<std::string, RosTypeMapping> mCache;

  static constexpr double MinimumConfidence = 0.70;
  static constexpr double MinimumMargin = 0.10;
  static constexpr std::size_t MaximumRecursionDepth = 16;
};

}  // namespace koda::ros