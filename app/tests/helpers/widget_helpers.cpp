#include "widget_helpers.h"

namespace maki
{
namespace testing
{
QMenu* submenuForOption(const QMenu* menu, const QString& option)
{
  if (!menu)
    return nullptr;

  for (auto* action : menu->actions())
  {
    if (action->text() == option)
      return action->menu();
  }

  return nullptr;
}

QAction* actionWithText(const QMenu* menu, const QString& text)
{
  if (!menu)
    return nullptr;

  for (auto* action : menu->actions())
  {
    if (action->text() == text)
      return action;
  }

  return nullptr;
}
}  // namespace testing
}  // namespace maki