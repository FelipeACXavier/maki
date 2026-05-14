#pragma once

#include <QMap>
#include <QString>

#include "result.h"

namespace maki
{
class IPipelineAction;

class PipelineActionRegistry
{
public:
  VoidResult registerAction(const QString& pluginId, std::shared_ptr<IPipelineAction> action);
  std::shared_ptr<IPipelineAction> action(const QString& id) const;
  QVector<std::shared_ptr<IPipelineAction>> actionsOfPlugin(const QString& pluginId) const;

  bool contains(const QString& id) const;

private:
  QMap<QString, std::shared_ptr<IPipelineAction>> mActions;
  QMap<QString, QVector<QString>> mActionPluginIds;
};
}  // namespace maki