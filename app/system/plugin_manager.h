#pragma once

#include <QWidget>

#include "generator_plugin.h"

class QComboBox;

class PluginManager : public QObject
{
  Q_OBJECT
public:
  PluginManager(QObject* parent = nullptr);

  virtual ~PluginManager();

  void start(QMenu* menu, QComboBox* comboBox);

  maki::IGeneratorPlugin* currentPlugin() const;
  maki::IGeneratorPlugin* pluginByLanguage(const QString& language) const;

private:
  maki::IGeneratorPlugin* mPlugin;
  QVector<maki::IGeneratorPlugin*> mPlugins;

  void setPlugin(maki::IGeneratorPlugin* plugin);
};
