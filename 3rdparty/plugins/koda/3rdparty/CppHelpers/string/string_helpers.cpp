#include "string_helpers.h"

#include <algorithm>
#include <cstdarg>
#include <cstring>
#include <iterator>
#include <sstream>

void ToUpperCase(std::string& str, uint32_t startPos)
{
  std::transform(str.begin() + startPos, str.end(), str.begin() + startPos, ::toupper);
}

void ToLowerCase(std::string& str, uint32_t startPos)
{
  std::transform(str.begin() + startPos, str.end(), str.begin() + startPos, ::tolower);
}

std::string ToUpperCase(const std::string& str, uint32_t startPos, uint32_t endPos)
{
  std::string cpy = str;
  std::transform(cpy.begin() + startPos, cpy.begin() + endPos, cpy.begin() + startPos, ::toupper);
  return cpy;
}

std::string ToLowerCase(const std::string& str, uint32_t startPos, uint32_t endPos)
{
  std::string cpy = str;
  std::transform(cpy.begin() + startPos, cpy.begin() + endPos, cpy.begin() + startPos, ::tolower);
  return cpy;
}

std::string Format(const std::string& str, ...)
{
  return Format(str.c_str());
}

std::string Format(const char* format, ...)
{
  va_list valist;

  int len = 0;
  va_start(valist, format);
#ifdef _MSC_VER
  // _vscprintf computes the length WITHOUT writing to a buffer
  len = _vscprintf(format, valist);
  va_end(valist);

  if (len < 0)
    return {};

  // +1 for the null terminator
  std::vector<char> buffer(static_cast<size_t>(len) + 1);

  va_start(valist, format);
  // vsnprintf_s is the "safe" MSVC version
  int written = vsnprintf_s(buffer.data(), buffer.size(), _TRUNCATE, format, valist);
  va_end(valist);

  if (written < 0)
    return {};

  // Construct string without the trailing '\0'
  return std::string(buffer.data(), static_cast<size_t>(written));
#else
  len = vsnprintf(0, 0, format, valist);
  va_end(valist);

  if (len < 0)
    return {};

  // Allocate buffer including space for 0 termination
  char buffer[len + 1];

  va_start(valist, format);
  std::vsnprintf(buffer, (uint32_t)(len + 1), format, valist);
  va_end(valist);

  return std::string(buffer);
#endif
}

std::string LTrim(std::string s, const char* t)
{
  s.erase(0, s.find_first_not_of(t));
  return s;
}

std::string RTrim(std::string s, const char* t)
{
  s.erase(s.find_last_not_of(t) + 1);
  return s;
}

std::string Trim(std::string s, const char* t)
{
  return LTrim(RTrim(s, t), t);
}

std::vector<std::string> Split(const std::string& str, char c)
{
  std::vector<std::string> splitList;
  size_t prev = 0;
  for (size_t idx = str.find(c); idx != std::string::npos; idx = str.find(c, idx + 1))
  {
    splitList.push_back(str.substr(prev, idx - prev));
    prev = idx + 1;
  }
  std::string remainder = str.substr(prev);
  if (!remainder.empty())
  {
    splitList.push_back(remainder);
  }

  return splitList;
}

std::map<std::string, std::string> Split(const std::string& str, char c1, char c2)
{
  std::map<std::string, std::string> m;

  std::vector<std::string> v = Split(str, c1);
  for (const std::string& pairs : v)
  {
    auto vInner = Split(pairs, c2);
    if (vInner.size() == 2)
      m[vInner[0]] = vInner[1];
  }

  return m;
}

std::vector<std::string> Keys(const std::map<std::string, uint32_t>& v)
{
  std::vector<std::string> r;

  for (const auto& k : v)
    r.push_back(k.first);

  return r;
}

std::string HexToString(uint32_t hex)
{
  std::stringstream ss;
  ss << "0x" << std::hex << hex;
  return ss.str();
}

std::string VectorToString(const std::vector<uint32_t>& values)
{
  if (values.empty())
    return std::string();

  std::ostringstream toReturn;

  // Convert all but the last element to avoid a trailing ","
  std::copy(values.begin(), values.end() - 1, std::ostream_iterator<uint32_t>(toReturn, ", "));

  // Add the last element with no delimiter
  toReturn << values.back();

  return toReturn.str();
}
