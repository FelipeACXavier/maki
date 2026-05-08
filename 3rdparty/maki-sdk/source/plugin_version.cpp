#include "plugin_version.h"

#include <QStringList>

namespace maki
{
QString PluginVersion::toString() const
{
  return QString("%1.%2.%3").arg(major, minor, patch);
}

PluginVersion PluginVersion::fromString(const QString& s)
{
  PluginVersion version;
  const QStringList parts = s.split('.', Qt::KeepEmptyParts);

  if (parts.size() != 3)
    return version;

  version.major = parts.at(0).trimmed();
  version.minor = parts.at(1).trimmed();
  version.patch = parts.at(2).trimmed();

  return version;
}

bool PluginVersion::operator==(const PluginVersion& other) const
{
  return major == other.major &&
         minor == other.minor &&
         patch == other.patch;
}

bool PluginVersion::operator!=(const PluginVersion& other) const
{
  return !(*this == other);
}
}  // namespace maki