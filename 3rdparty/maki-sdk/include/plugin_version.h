#pragma once

#include <QString>

namespace maki
{
struct PluginVersion
{
  QString major = "";
  QString minor = "";
  QString patch = "";

  QString toString() const;

  static PluginVersion fromString(const QString& s);

  bool operator==(const PluginVersion& other) const;
  bool operator!=(const PluginVersion& other) const;
};
}  // namespace maki