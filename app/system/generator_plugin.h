#pragma once

#include <QGraphicsItem>
#include <QList>
#include <QObject>
#include <QString>

// Forward declaration to avoid exposing full NodeItem definition
class NodeItem;

namespace generator
{
enum class Language
{
  Cpp,
  ROS,
  Python,
  JSON,
  Custom
};
}

class GeneratorPlugin
{
public:
  virtual ~GeneratorPlugin() = default;
  virtual QString generateCode(const QList<QGraphicsItem*>& nodes) = 0;
  virtual generator::Language supportedLanguage() const = 0;
  virtual QString languageName() const = 0;
};

#define GeneratorPlugin_iid "com.felipexavier.GeneratorPlugin"

Q_DECLARE_INTERFACE(GeneratorPlugin, GeneratorPlugin_iid)
