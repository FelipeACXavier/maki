#include "types.h"

#include <sstream>

namespace koda::types
{
std::vector<std::string> split(const std::string& value, const std::string& separator)
{
  std::vector<std::string> splitList;
  size_t prev = 0;
  for (size_t idx = value.find(separator); idx != std::string::npos; idx = value.find(separator, idx + 1))
  {
    splitList.push_back(value.substr(prev, idx - prev));
    prev = idx + separator.size();
  }

  std::string remainder = value.substr(prev);
  if (!remainder.empty())
    splitList.push_back(remainder);

  return splitList;
}

bool QualifiedName::empty() const
{
  return name.empty();
}

std::string QualifiedName::toId() const
{
  return toString(".");
}

std::string QualifiedName::toString(const std::string& separator) const
{
  std::ostringstream stream;
  for (const auto& namespacePart : namespacePath)
  {
    if (stream.tellp() > 0)
      stream << separator;

    stream << namespacePart;
  }

  if (!name.empty())
  {
    if (stream.tellp() > 0)
      stream << separator;

    stream << name;
  }

  return stream.str();
}

QualifiedName::QualifiedName(const std::string& value, const std::string& separator)
{
  const auto parts = split(value, separator);
  if (parts.empty())
    throw std::invalid_argument("No value passed");

  name = parts.back();
  if (parts.size() > 1)
    namespacePath.assign(parts.begin(), parts.end() - 1);
}
}  // namespace koda::types