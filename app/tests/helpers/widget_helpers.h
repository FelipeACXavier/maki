#pragma once

#include <QAction>
#include <QMenu>

namespace maki
{
namespace testing
{
QMenu* submenuForOption(const QMenu* menu, const QString& option);
QAction* actionWithText(const QMenu* menu, const QString& text);
}  // namespace testing
}  // namespace maki