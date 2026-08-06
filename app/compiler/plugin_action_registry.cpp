#include "plugin_action_registry.h"

#include "logging.h"
#include "pipeline_action.h"

namespace maki
{
VoidResult PipelineActionRegistry::registerAction(const QString& pluginId, std::shared_ptr<IPipelineAction> action)
{
  if (mActions.contains(action->id()))
    return VoidResult::Failed("Action already registred, " + action->id().toStdString());

  mActions.insert(action->id(), action);
  if (mActionPluginIds.contains(pluginId))
    mActionPluginIds[pluginId] += action->id();
  else
    mActionPluginIds.insert(pluginId, {action->id()});

  LOG_DEBUG("Adding action: {} of plugin: {} to the registry", qPrintable(action->id()), qPrintable(pluginId));

  return VoidResult();
}

std::shared_ptr<IPipelineAction> PipelineActionRegistry::action(const QString& id) const
{
  return mActions.value(id, nullptr);
}

QVector<std::shared_ptr<IPipelineAction>> PipelineActionRegistry::actionsOfPlugin(const QString& pluginId) const
{
  QVector<std::shared_ptr<IPipelineAction>> actions = {};
  for (const auto& a : mActionPluginIds[pluginId])
    actions.push_back(action(a));

  return actions;
}

bool PipelineActionRegistry::contains(const QString& id) const
{
  return mActions.contains(id);
}

void PipelineActionRegistry::printAll() const
{
  LOG_INFO("Available actions:");
  for (const auto& action : mActions)
  {
    LOG_INFO("Id: {} - Action Id: {}", qPrintable(action->id()), qPrintable(action->displayName()));
  }
}
}  // namespace maki