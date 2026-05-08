#include "plugin_action_registry.h"

#include "pipeline_action.h"

namespace maki
{
VoidResult PipelineActionRegistry::registerAction(const QString& pluginId, std::shared_ptr<IPipelineAction> action)
{
  if (mActions.contains(action->id()))
    return VoidResult::Failed("Action already registred, " + action->id().toStdString());

  mActions.insert(action->id(), action);
  mActionPluginIds.insert(action->id(), pluginId);

  return VoidResult();
}

std::shared_ptr<IPipelineAction> PipelineActionRegistry::action(const QString& id) const
{
  return mActions.value(id, nullptr);
}

bool PipelineActionRegistry::contains(const QString& id) const
{
  return mActions.contains(id);
}
}  // namespace maki