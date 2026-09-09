#include "typing/ros_data_mapper.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <limits>
#include <ranges>
#include <sstream>
#include <unordered_set>

#include "logging.h"

namespace koda::ros
{

RosDatatypeMapper::RosDatatypeMapper(const koda::types::TypeRegistry& registry)
    : mRegistry(registry)
{
}

std::optional<RosTypeMapping> RosDatatypeMapper::resolve(const koda::types::TypeReference& type)
{
  const auto key = type.toString();

  if (const auto it = mCache.find(key); it != mCache.end())
    return it->second;

  const auto annotation = rosAnnotationFor(type);
  if (!annotation)
    return std::nullopt;

  const auto ref = parseRosInterface(*annotation);
  if (!ref)
    return std::nullopt;

  const auto sourceFields = flattenKodaRecord(type);
  const auto targetFields = flattenRosInterface(*ref);

  RosTypeMapping result{
      .rosName = *annotation,
      .cppType = cppTypeFor(*ref),
      .include = includeFor(*ref),
      .fields = inferMappings(sourceFields, targetFields),
  };

  mCache.emplace(key, result);
  return result;
}

std::optional<std::string> RosDatatypeMapper::cppType(const koda::types::TypeReference& type)
{
  if (const auto mapping = resolve(type))
    return mapping->cppType;

  const auto name = type.toString();

  if (name == "bool")
    return "bool";

  if (name == "byte")
    return "std::uint8_t";

  if (name == "int")
    return "std::int64_t";

  if (name == "real")
    return "double";

  if (name == "string")
    return "std::string";

  if (name == "bytes")
    return "std::vector<std::uint8_t>";

  return std::nullopt;
}

std::optional<std::string> RosDatatypeMapper::includeFor(const koda::types::TypeReference& type)
{
  const auto mapping = resolve(type);

  if (!mapping || mapping->include.empty())
    return std::nullopt;

  return mapping->include;
}

std::optional<std::string> RosDatatypeMapper::targetField(const koda::types::TypeReference& type, const std::string& sourcePath)
{
  const auto mapping = resolve(type);

  if (!mapping)
    return std::nullopt;

  const auto it = std::ranges::find_if(mapping->fields, [&](const RosFieldMapping& field) { return field.sourcePath == sourcePath; });

  if (it == mapping->fields.end())
    return std::nullopt;

  return it->targetPath;
}

std::optional<std::string> RosDatatypeMapper::rosAnnotationFor(const koda::types::TypeReference& type) const
{
  auto definition = mRegistry.resolve(type);
  if (!definition)
    return std::nullopt;

  std::string rosType = "";
  for (const auto& [key, value] : mRegistry.annotationsOf(definition->name))
  {
    if (key != "rosType")
      continue;

    LOG_DEBUG("Using rosType {} for {}", value, type.toString());
    return value;
  }
  /*
   * PROJECT-SPECIFIC ADAPTER.
   *
   * Replace this with your TypeRegistry API.
   *
   * Intended semantics:
   *
   *   robotics::GoalPose
   *       annotation "ros"
   *       = "nav2_msgs/action/NavigateToPose::Goal"
   *
   * Example shape:
   *
   * const auto* definition = mRegistry.get(type);
   * if (!definition)
   *   return std::nullopt;
   *
   * auto it = definition->annotations.find("ros");
   * if (it == definition->annotations.end())
   *   return std::nullopt;
   *
   * return it->second;
   */

  return std::nullopt;
}

std::vector<FlatField> RosDatatypeMapper::flattenKodaRecord(const koda::types::TypeReference& type) const
{
  /*
   * PROJECT-SPECIFIC ADAPTER.
   *
   * This must recursively flatten a KODA record.
   *
   * Example:
   *
   * robotics::GoalPose
   *
   *   position:
   *     x: real
   *     y: real
   *     z: real
   *
   *   orientation:
   *     x: real
   *     y: real
   *     z: real
   *     w: real
   *
   * should produce:
   *
   * {
   *   {"position.x", "real"},
   *   {"position.y", "real"},
   *   {"position.z", "real"},
   *   {"orientation.x", "real"},
   *   {"orientation.y", "real"},
   *   {"orientation.z", "real"},
   *   {"orientation.w", "real"},
   * }
   *
   * I would implement the recursion against TypeRegistry here,
   * because the exact Record/Field API is repository-specific.
   */

  return {};
}

std::optional<RosDatatypeMapper::RosInterfaceRef> RosDatatypeMapper::parseRosInterface(const std::string& annotation) const
{
  // Supported:
  //
  // geometry_msgs/msg/PoseStamped
  // nav2_msgs/action/NavigateToPose::Goal
  // example_interfaces/srv/AddTwoInts::Request

  const auto subtypePos = annotation.find("::");

  std::string base = annotation;
  std::optional<std::string> subtype;

  if (subtypePos != std::string::npos)
  {
    base = annotation.substr(0, subtypePos);
    subtype = annotation.substr(subtypePos + 2);
  }

  const auto parts = split(base, '/');

  if (parts.size() != 3)
    return std::nullopt;

  RosInterfaceRef result;
  result.package = parts[0];
  result.interfaceName = parts[2];
  result.subType = subtype;

  if (parts[1] == "msg")
    result.kind = RosInterfaceRef::Kind::Message;
  else if (parts[1] == "srv")
    result.kind = RosInterfaceRef::Kind::Service;
  else if (parts[1] == "action")
    result.kind = RosInterfaceRef::Kind::Action;
  else
    return std::nullopt;

  if (result.kind == RosInterfaceRef::Kind::Message && result.subType)
    return std::nullopt;

  return result;
}

std::optional<std::filesystem::path> RosDatatypeMapper::interfaceFile(const RosInterfaceRef& ref) const
{
  std::filesystem::path share;

  try
  {
    share = ament_index_cpp::get_package_share_directory(ref.package);
  } catch (...)
  {
    return std::nullopt;
  }

  switch (ref.kind)
  {
    case RosInterfaceRef::Kind::Message:
      return share / "msg" / (ref.interfaceName + ".msg");

    case RosInterfaceRef::Kind::Service:
      return share / "srv" / (ref.interfaceName + ".srv");

    case RosInterfaceRef::Kind::Action:
      return share / "action" / (ref.interfaceName + ".action");
  }

  return std::nullopt;
}

std::vector<FlatField> RosDatatypeMapper::flattenRosInterface(const RosInterfaceRef& ref) const
{
  const auto file = interfaceFile(ref);

  if (!file || !std::filesystem::exists(*file))
    return {};

  if (ref.kind == RosInterfaceRef::Kind::Message)
    return flattenRosFile(*file, ref.package, "", 0);

  std::ifstream in(*file);
  if (!in)
    return {};

  std::vector<std::vector<std::string>> sections(1);

  std::string line;
  while (std::getline(in, line))
  {
    if (trim(line) == "---")
    {
      sections.emplace_back();
      continue;
    }

    sections.back().push_back(line);
  }

  std::size_t index = 0;

  if (ref.kind == RosInterfaceRef::Kind::Action)
  {
    if (!ref.subType || *ref.subType == "Goal")
      index = 0;
    else if (*ref.subType == "Result")
      index = 1;
    else if (*ref.subType == "Feedback")
      index = 2;
    else
      return {};
  }
  else
  {
    if (!ref.subType || *ref.subType == "Request")
      index = 0;
    else if (*ref.subType == "Response")
      index = 1;
    else
      return {};
  }

  if (index >= sections.size())
    return {};

  return flattenRosSection(sections[index], ref.package, "", 0);
}

std::vector<FlatField> RosDatatypeMapper::flattenRosFile(const std::filesystem::path& file, const std::string& package, const std::string& prefix,
                                                         std::size_t depth) const
{
  if (depth > MaximumRecursionDepth)
    return {};

  std::ifstream in(file);
  if (!in)
    return {};

  std::vector<std::string> lines;

  std::string line;
  while (std::getline(in, line))
    lines.push_back(line);

  return flattenRosSection(lines, package, prefix, depth);
}

std::vector<FlatField> RosDatatypeMapper::flattenRosSection(const std::vector<std::string>& lines, const std::string& package,
                                                            const std::string& prefix, std::size_t depth) const
{
  std::vector<FlatField> result;

  for (auto line : lines)
  {
    if (const auto comment = line.find('#'); comment != std::string::npos)
      line.erase(comment);

    line = trim(line);

    if (line.empty())
      continue;

    // Constants:
    //
    // uint8 SOME_VALUE=42
    //
    // They are not runtime fields.
    if (line.find('=') != std::string::npos)
      continue;

    std::istringstream stream(line);

    std::string rawType;
    std::string name;

    stream >> rawType >> name;

    if (rawType.empty() || name.empty())
      continue;

    auto type = stripBound(stripArray(rawType));

    const auto path = prefix.empty() ? name : prefix + "." + name;

    if (isRosPrimitive(type))
    {
      result.push_back({
          .path = path,
          .type = type,
      });

      continue;
    }

    const auto nested = nestedRosMessageFile(package, type);

    if (!nested)
    {
      // Preserve it as an opaque leaf rather than silently
      // deleting the field.
      result.push_back({
          .path = path,
          .type = type,
      });

      continue;
    }

    auto nestedFields = flattenRosFile(*nested, type.find('/') != std::string::npos ? split(type, '/')[0] : package, path, depth + 1);

    result.insert(result.end(), nestedFields.begin(), nestedFields.end());
  }

  return result;
}

std::vector<RosFieldMapping> RosDatatypeMapper::inferMappings(const std::vector<FlatField>& source, const std::vector<FlatField>& target) const
{
  std::vector<RosFieldMapping> result;

  std::unordered_set<std::string> usedTargets;

  for (const auto& sourceField : source)
  {
    Candidate best;
    Candidate second;

    best.score = -std::numeric_limits<double>::infinity();

    second.score = -std::numeric_limits<double>::infinity();

    for (const auto& targetField : target)
    {
      if (usedTargets.contains(targetField.path))
        continue;

      const auto candidateScore = scoreField(sourceField, targetField);

      if (candidateScore > best.score)
      {
        second = best;
        best = {
            .targetPath = targetField.path,
            .score = candidateScore,
        };
      }
      else if (candidateScore > second.score)
      {
        second = {
            .targetPath = targetField.path,
            .score = candidateScore,
        };
      }
    }

    if (best.score < MinimumConfidence)
      continue;

    if (second.score > -std::numeric_limits<double>::infinity() && best.score - second.score < MinimumMargin)
      continue;

    usedTargets.insert(best.targetPath);

    result.push_back({
        .sourcePath = sourceField.path,
        .targetPath = best.targetPath,
        .confidence = best.score,
    });
  }

  return result;
}

double RosDatatypeMapper::scoreField(const FlatField& source, const FlatField& target) const
{
  double score = 0.0;

  const auto sourceLeaf = leafName(source.path);

  const auto targetLeaf = leafName(target.path);

  const auto sourceParent = parentName(source.path);

  const auto targetParent = parentName(target.path);

  if (sourceLeaf == targetLeaf)
    score += 0.40;

  if (!sourceParent.empty() && sourceParent == targetParent)
    score += 0.30;

  if (pathEndsWith(target.path, source.path))
    score += 0.20;

  if (compatibleTypes(source.type, target.type))
    score += 0.10;

  return score;
}

bool RosDatatypeMapper::compatibleTypes(const std::string& source, const std::string& target) const
{
  if (source == target)
    return true;

  if (source == "real")
    return target == "float32" || target == "float64" || target == "double";

  if (source == "int")
  {
    static const std::unordered_set<std::string> integers = {
        "int8", "uint8", "int16", "uint16", "int32", "uint32", "int64", "uint64", "byte", "char",
    };

    return integers.contains(target);
  }

  if (source == "bool")
    return target == "bool";

  if (source == "string")
    return target == "string" || target == "wstring";

  if (source == "bytes")
    return target == "uint8";

  return false;
}

bool RosDatatypeMapper::isRosPrimitive(const std::string& type) const
{
  static const std::unordered_set<std::string> primitives = {
      "bool",  "byte",   "char",  "float32", "float64", "int8",    "uint8", "int16",    "uint16",
      "int32", "uint32", "int64", "uint64",  "string",  "wstring", "time",  "duration",
  };

  return primitives.contains(type);
}

std::optional<std::filesystem::path> RosDatatypeMapper::nestedRosMessageFile(const std::string& currentPackage, const std::string& type) const
{
  std::string package = currentPackage;
  std::string message = type;

  const auto parts = split(type, '/');

  if (parts.size() == 2)
  {
    package = parts[0];
    message = parts[1];
  }
  else if (parts.size() == 3 && parts[1] == "msg")
  {
    package = parts[0];
    message = parts[2];
  }
  else if (parts.size() > 1)
  {
    return std::nullopt;
  }

  std::filesystem::path share;

  try
  {
    share = ament_index_cpp::get_package_share_directory(package);
  } catch (...)
  {
    return std::nullopt;
  }

  const auto file = share / "msg" / (message + ".msg");

  if (!std::filesystem::exists(file))
    return std::nullopt;

  return file;
}

std::string RosDatatypeMapper::cppTypeFor(const RosInterfaceRef& ref) const
{
  std::string result = ref.package + "::";

  switch (ref.kind)
  {
    case RosInterfaceRef::Kind::Message:
      result += "msg::";
      break;

    case RosInterfaceRef::Kind::Service:
      result += "srv::";
      break;

    case RosInterfaceRef::Kind::Action:
      result += "action::";
      break;
  }

  result += ref.interfaceName;

  if (ref.subType)
    result += "::" + *ref.subType;

  return result;
}

std::string RosDatatypeMapper::includeFor(const RosInterfaceRef& ref) const
{
  std::string directory;

  switch (ref.kind)
  {
    case RosInterfaceRef::Kind::Message:
      directory = "msg";
      break;

    case RosInterfaceRef::Kind::Service:
      directory = "srv";
      break;

    case RosInterfaceRef::Kind::Action:
      directory = "action";
      break;
  }

  return ref.package + "/" + directory + "/" + snakeCase(ref.interfaceName) + ".hpp";
}

std::string RosDatatypeMapper::trim(std::string value)
{
  const auto first = value.find_first_not_of(" \t\r\n");

  if (first == std::string::npos)
    return {};

  const auto last = value.find_last_not_of(" \t\r\n");

  return value.substr(first, last - first + 1);
}

std::vector<std::string> RosDatatypeMapper::split(const std::string& value, char delimiter)
{
  std::vector<std::string> result;

  std::stringstream stream(value);
  std::string item;

  while (std::getline(stream, item, delimiter))
    result.push_back(item);

  return result;
}

std::string RosDatatypeMapper::leafName(const std::string& path)
{
  const auto pos = path.find_last_of('.');

  if (pos == std::string::npos)
    return path;

  return path.substr(pos + 1);
}

std::string RosDatatypeMapper::parentName(const std::string& path)
{
  const auto pos = path.find_last_of('.');

  if (pos == std::string::npos)
    return {};

  const auto parentPath = path.substr(0, pos);

  return leafName(parentPath);
}

bool RosDatatypeMapper::pathEndsWith(const std::string& target, const std::string& suffix)
{
  if (suffix.size() > target.size())
    return false;

  if (!target.ends_with(suffix))
    return false;

  if (target.size() == suffix.size())
    return true;

  const auto separator = target.size() - suffix.size() - 1;

  return target[separator] == '.';
}

std::string RosDatatypeMapper::snakeCase(const std::string& name)
{
  std::string result;

  for (std::size_t i = 0; i < name.size(); ++i)
  {
    const auto c = name[i];

    if (std::isupper(static_cast<unsigned char>(c)))
    {
      if (i > 0 && !std::isupper(static_cast<unsigned char>(name[i - 1])))
        result += '_';

      result += static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    else
    {
      result += c;
    }
  }

  return result;
}

std::string RosDatatypeMapper::stripArray(const std::string& type)
{
  const auto pos = type.find('[');

  if (pos == std::string::npos)
    return type;

  return type.substr(0, pos);
}

std::string RosDatatypeMapper::stripBound(const std::string& type)
{
  // bounded strings:
  // string<=32

  const auto pos = type.find("<=");

  if (pos == std::string::npos)
    return type;

  return type.substr(0, pos);
}

}  // namespace koda::ros