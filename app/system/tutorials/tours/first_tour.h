#pragma once

#include <QObject>
#include <memory>

#include "itour.h"

class MainWindow;
class DraggableItem;
class LibraryContainer;

class FirstTour : public ITour
{
  Q_OBJECT

public:
  explicit FirstTour(MainWindow* window);
  ~FirstTour() override = default;

  void startTour() override;

private:
  // The steps of this tour
  struct Steps
  {
    int showWelcome = TourStep::Invalid;
    int showPalette = TourStep::Invalid;
    int startDragTask = TourStep::Invalid;
    int endDragTask = TourStep::Invalid;
    int highlightTask = TourStep::Invalid;
    int showProperties = TourStep::Invalid;
    int updateName = TourStep::Invalid;
    int highlightTaskName = TourStep::Invalid;
    int showCapabilities = TourStep::Invalid;
    int startDragCapability = TourStep::Invalid;
    int endDragCapability = TourStep::Invalid;
    int highlightCapability = TourStep::Invalid;
    int preControlHover = TourStep::Invalid;
    int highlightControl = TourStep::Invalid;
    int showBehaviourCanvas = TourStep::Invalid;
    int showBehaviourPalette = TourStep::Invalid;
    int startDragCall = TourStep::Invalid;
    int endDragCall = TourStep::Invalid;
    int highlightCall = TourStep::Invalid;
    int selectCapability = TourStep::Invalid;
    int selectCapabilityEvent = TourStep::Invalid;
    int confirmCapability = TourStep::Invalid;
    int completeCall = TourStep::Invalid;
    int startVerification = TourStep::Invalid;
    int verifying = TourStep::Invalid;
    int verificationDone = TourStep::Invalid;
    int end = TourStep::Invalid;

    QString taskId = "";
  };
};
