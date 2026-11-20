#pragma once

#include <QString>
#include <memory>

#include "system/tutorials/tour_guide.h"

class MainWindow;
class QTabWidget;
class DraggableItem;
class LibraryContainer;
class SettingsManager;
class NotificationManager;
class PropertiesMenu;
class SystemMenu;
class ExecuteButton;

namespace maki
{
class EditorWorkspace;
class PluginPipeline;
}  // namespace maki

class ITour : public QObject
{
  Q_OBJECT
public:
  virtual ~ITour() = default;
  virtual void startTour() = 0;

signals:
  void tourCancelled();
  void tourFinished();

protected:
  MainWindow* mWindow;
  std::shared_ptr<TourGuide> mTourGuide;

  explicit ITour(MainWindow* window);

  QTabWidget* palette() const;
  std::shared_ptr<SettingsManager> settings() const;
  NotificationManager* notifications() const;
  maki::EditorWorkspace* editorWorkspace() const;
  PropertiesMenu* propertiesMenu() const;
  SystemMenu* systemMenu() const;
  ExecuteButton* pipelineButton() const;
  maki::PluginPipeline* pluginPipeline() const;

  std::pair<LibraryContainer*, DraggableItem*> findDraggableInPalette(const QString& nodeType) const;
};
