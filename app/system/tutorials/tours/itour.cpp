#include "itour.h"

#include "canvas/canvas.h"
#include "canvas/canvas_view.h"
#include "canvas/elements/draggable.h"
#include "system/main_window.h"
#include "widgets/editor/editor_workspace.h"
#include "widgets/library_container.h"
#include "widgets/library_scene.h"
#include "widgets/notification_manager.h"
#include "widgets/properties/properties_menu.h"
#include "widgets/section.h"
#include "widgets/structure/system_menu.h"

ITour::ITour(MainWindow* window)
    : QObject(nullptr)
    , mWindow(window)
{
  Q_ASSERT(mWindow);
  mTourGuide = std::make_shared<TourGuide>(mWindow);
}

QTabWidget* ITour::palette() const
{
  return mWindow->mPalette;
}

maki::EditorWorkspace* ITour::editorWorkspace() const
{
  return mWindow->mEditorWorkspace;
}

std::shared_ptr<SettingsManager> ITour::settings() const
{
  return mWindow->mSettingsManager;
}

NotificationManager* ITour::notifications() const
{
  return mWindow->mNotificationManager;
}

PropertiesMenu* ITour::propertiesMenu() const
{
  return mWindow->mPropertiesMenu;
}

SystemMenu* ITour::systemMenu() const
{
  return mWindow->mSystemMenu;
}

ExecuteButton* ITour::pipelineButton() const
{
  return mWindow->mPipelineRun;
}

maki::PluginPipeline* ITour::pluginPipeline() const
{
  return mWindow->mPluginPipeline;
}

std::pair<LibraryContainer*, DraggableItem*> ITour::findDraggableInPalette(const QString& nodeType) const
{
  for (const auto* section : mWindow->mPalette->findChildren<SectionWidget*>())
    if (auto* library = qobject_cast<LibraryContainer*>(section->content()))
      if (auto* nodePalette = qobject_cast<LibraryScene*>(library->scene()))
        for (const auto& item : nodePalette->items())
          if (auto draggable = qgraphicsitem_cast<DraggableItem*>(item); draggable->nodeId() == nodeType)
            return std::make_pair(library, draggable);

  return std::make_pair(nullptr, nullptr);
}
