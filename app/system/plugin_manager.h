#pragma once

#include <QWidget>

#include "compiler/generator_plugin.h"

class QComboBox;

class PluginManager : public QObject
{
  Q_OBJECT
public:
  PluginManager();

  void start(QMenu* menu, QComboBox* comboBox);

  GeneratorPlugin* currentPlugin() const;
  GeneratorPlugin* pluginByLanguage(generator::Language language) const;

private:
  GeneratorPlugin* mPlugin;
  QVector<GeneratorPlugin*> mPlugins;

  void setPlugin(GeneratorPlugin* plugin);
};
