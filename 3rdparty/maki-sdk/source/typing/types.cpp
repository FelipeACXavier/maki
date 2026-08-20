#include "typing/types.h"

#include <sstream>

namespace koda::types
{
std::vector<std::string> split(const std::string& value, const std::string& separator)
{
  std::vector<std::string> result;

  size_t start = 0;
  int angleDepth = 0;

  for (size_t i = 0; i < value.size();)
  {
    if (value[i] == '<')
    {
      ++angleDepth;
      ++i;
      continue;
    }

    if (value[i] == '>')
    {
      if (angleDepth > 0)
        --angleDepth;

      ++i;
      continue;
    }

    // Only split when we're not inside <...>
    if (angleDepth == 0 && value.compare(i, separator.size(), separator) == 0)
    {
      result.push_back(value.substr(start, i - start));

      i += separator.size();
      start = i;

      continue;
    }

    ++i;
  }

  const std::string remainder = value.substr(start);

  if (!remainder.empty())
    result.push_back(remainder);

  return result;
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

std::string QualifiedName::namespaceString(const std::string& separator) const
{
  std::ostringstream stream;
  for (const auto& namespacePart : namespacePath)
  {
    if (stream.tellp() > 0)
      stream << separator;

    stream << namespacePart;
  }

  return stream.str();
}

QualifiedName::QualifiedName(const std::string& value, const std::string& separator)
{
  const auto parts = split(value, separator);
  if (parts.empty())
    return;

  name = parts.back();
  if (parts.size() > 1)
    namespacePath.assign(parts.begin(), parts.end() - 1);
}

}  // namespace koda::types