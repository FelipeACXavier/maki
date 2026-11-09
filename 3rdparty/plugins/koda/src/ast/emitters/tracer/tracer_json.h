#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace koda::tracer
{
// Self-contained JSON value: no additional JSON library dependency for the emitter.
// Map keys are sorted, making generated documents stable across runs.
class Json
{
public:
  using Array = std::vector<Json>;
  using Object = std::map<std::string, Json>;

  Json() = default;
  Json(std::nullptr_t);
  Json(bool value);
  Json(std::int64_t value);
  Json(int value);
  Json(const char* value);
  Json(std::string value);
  Json(Array value);
  Json(Object value);

  static Json object(Object value);
  static Json array(Array value);

  // Produces UTF-8 JSON with two-space indentation and a final newline.
  std::string dump() const;

private:
  using Value = std::variant<std::nullptr_t, bool, std::int64_t, std::string, Array, Object>;
  Value mValue = nullptr;
  void append(std::string& out, unsigned depth) const;
};
}  // namespace koda::tracer
