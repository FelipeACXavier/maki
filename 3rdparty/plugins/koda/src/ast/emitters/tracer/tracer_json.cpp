#include "tracer_json.h"

#include <string_view>
#include <type_traits>
#include <utility>

namespace koda::tracer
{
namespace
{
void appendString(std::string& out, std::string_view value)
{
  constexpr char digits[] = "0123456789abcdef";
  out.push_back('"');
  for (unsigned char ch : value)
  {
    switch (ch)
    {
      case '"': out += "\\\""; break;
      case '\\': out += "\\\\"; break;
      case '\b': out += "\\b"; break;
      case '\f': out += "\\f"; break;
      case '\n': out += "\\n"; break;
      case '\r': out += "\\r"; break;
      case '\t': out += "\\t"; break;
      default:
        if (ch < 0x20)
        {
          out += "\\u00";
          out.push_back(digits[ch >> 4]);
          out.push_back(digits[ch & 15]);
        }
        else
          out.push_back(static_cast<char>(ch));
    }
  }
  out.push_back('"');
}
void indent(std::string& out, unsigned depth)
{
  out.append(depth * 2, ' ');
}
}  // namespace

Json::Json(std::nullptr_t) : mValue(nullptr) {}
Json::Json(bool value) : mValue(value) {}
Json::Json(std::int64_t value) : mValue(value) {}
Json::Json(int value) : mValue(static_cast<std::int64_t>(value)) {}
Json::Json(const char* value) : mValue(std::string(value)) {}
Json::Json(std::string value) : mValue(std::move(value)) {}
Json::Json(Array value) : mValue(std::move(value)) {}
Json::Json(Object value) : mValue(std::move(value)) {}
Json Json::object(Object value) { return Json(std::move(value)); }
Json Json::array(Array value) { return Json(std::move(value)); }

std::string Json::dump() const
{
  std::string output;
  append(output, 0);
  output.push_back('\n');
  return output;
}

void Json::append(std::string& out, unsigned depth) const
{
  std::visit([&](const auto& value) {
    using T = std::decay_t<decltype(value)>;
    if constexpr (std::is_same_v<T, std::nullptr_t>)
      out += "null";
    else if constexpr (std::is_same_v<T, bool>)
      out += value ? "true" : "false";
    else if constexpr (std::is_same_v<T, std::int64_t>)
      out += std::to_string(value);
    else if constexpr (std::is_same_v<T, std::string>)
      appendString(out, value);
    else if constexpr (std::is_same_v<T, Array>)
    {
      if (value.empty()) { out += "[]"; return; }
      out += "[\n";
      for (std::size_t i = 0; i < value.size(); ++i)
      {
        indent(out, depth + 1);
        value[i].append(out, depth + 1);
        out += i + 1 == value.size() ? "\n" : ",\n";
      }
      indent(out, depth);
      out += ']';
    }
    else if constexpr (std::is_same_v<T, Object>)
    {
      if (value.empty()) { out += "{}"; return; }
      out += "{\n";
      std::size_t i = 0;
      for (const auto& [key, item] : value)
      {
        indent(out, depth + 1);
        appendString(out, key);
        out += ": ";
        item.append(out, depth + 1);
        out += ++i == value.size() ? "\n" : ",\n";
      }
      indent(out, depth);
      out += '}';
    }
  }, mValue);
}
}  // namespace koda::tracer
