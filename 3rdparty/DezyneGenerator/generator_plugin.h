#pragma once

#include <QList>
#include <QObject>
#include <QString>

#include "inode.h"

class QGraphicsItem;

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
}  // namespace generator

class GeneratorPlugin
{
public:
  virtual ~GeneratorPlugin() = default;
  virtual QString generateCode(const QList<INode*>& nodes) = 0;
  virtual generator::Language supportedLanguage() const = 0;
  virtual QString languageName() const = 0;
};

#define GeneratorPlugin_iid "com.felipexavier.GeneratorPlugin"

Q_DECLARE_INTERFACE(GeneratorPlugin, GeneratorPlugin_iid)
