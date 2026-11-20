#include "first_tour.h"

#include <QLineEdit>

#include "canvas/canvas.h"
#include "canvas/canvas_view.h"
#include "canvas/elements/draggable.h"
#include "keys.h"
#include "notifications.h"
#include "pipeline/plugin_pipeline.h"
#include "widgets/controls/capability_call_selector.h"
#include "widgets/dialogs/prompt.h"
#include "widgets/editor/editor_workspace.h"
#include "widgets/execute_button.h"
#include "widgets/library_container.h"
#include "widgets/library_scene.h"
#include "widgets/notification_manager.h"
#include "widgets/properties/properties_menu.h"
#include "widgets/section.h"

FirstTour::FirstTour(MainWindow* window)
    : ITour(window)
{
}

void FirstTour::startTour()
{
  auto [taskLibrary, taskNode] = findDraggableInPalette(ConfigKeys::TASK_NODE);
  if (!taskNode || !taskLibrary)
  {
    LOG_WARNING("Could not find task node in palette");
    return;
  }

  auto [callLibrary, callNode] = findDraggableInPalette(ConfigKeys::CAPABILITY_CALL_NODE);
  if (!callNode || !callLibrary)
  {
    LOG_WARNING("Could not find capability call node in palette");
    return;
  }

  auto [capabilityLibrary, capabilityNode] = findDraggableInPalette("Koda::Siren");
  if (!capabilityNode || !capabilityLibrary)
  {
    LOG_WARNING("Could not find capability call node in palette");
    return;
  }

  auto steps = std::make_shared<Steps>();
  auto canvasView = qobject_cast<CanvasView*>(editorWorkspace()->activeEditor());
  if (!canvasView)
  {
    LOG_DEBUG("Failed to focus on active editor");
    return;
  }

  auto structureCanvas = qobject_cast<Canvas*>(canvasView->scene());

  steps->showWelcome = mTourGuide->addStep(nullptr, tr("Welcome to %1").arg(Config::APPLICATION_NAME),
                                           tr("This quick tour shows you how to create a simple application with %1.").arg(Config::APPLICATION_NAME));
  steps->showPalette = mTourGuide->addStep(
    palette(), tr("Structure Tab"),
    tr("%1 applications revolved around nodes. Structural nodes define the structure of the application. They can be found in the Structure Tab.")
      .arg(Config::APPLICATION_NAME));
  steps->startDragTask = mTourGuide->addItemStep(taskLibrary, taskNode, tr("Task node"), tr("Click and drag the Task node onto the Canvas"),
                                                 TourStep::AdvanceOn::UserAction, steps->showPalette);
  steps->endDragTask = mTourGuide->addStep(canvasView, tr("Drop it on the Canvas"), tr("Release the Task node anywhere on the Canvas."),
                                           TourStep::AdvanceOn::UserAction, steps->startDragTask);

  steps->showProperties = mTourGuide->addStep(
    propertiesMenu(), tr("Properties"),
    tr("Nodes may have properties that are used to modify how they behave. In the \"Property Tab\" you can find the properties of the dropped node"));

  steps->showCapabilities = mTourGuide->addStep(
    capabilityLibrary, tr("Capabilities"),
    tr("Capabilities define the robot functions available to a Task. Capabilities are Structural nodes and can be found in the Structure Tab."));
  steps->startDragCapability = mTourGuide->addItemStep(
    capabilityLibrary, capabilityNode, tr("%1 Capability").arg(tour::QUICK_TOUR_CAPABILTY),
    tr("To add a Capability to a Task. Click on the %1 Capablity and drag in onto a Task node in the Canvas.").arg(tour::QUICK_TOUR_CAPABILTY),
    TourStep::AdvanceOn::UserAction, steps->showCapabilities);

  steps->end = mTourGuide->addStep(
    nullptr, tr("Congratulations!"),
    tr("You should be ready to start creating your application. For more information, explore the other available nodes and the help menu in the toolbar."));

  connect(propertiesMenu(), &PropertiesMenu::nodeFocused, this, [this, steps](const QString& nodeId) {
    if (!mTourGuide->isActive())
      return;

    if (mTourGuide->currentStepId() != steps->highlightTask)
      return;

    if (steps->taskId != nodeId)
      return;

    auto lineEdits = propertiesMenu()->findChildren<QLineEdit*>();
    if (auto lineEdit = lineEdits.isEmpty() ? nullptr : lineEdits.front(); lineEdit)
    {
      steps->updateName = mTourGuide->insertStep(steps->highlightTaskName, lineEdit, tr("Update properties"),
                                                 tr("We need to give our Task a name. Write \"%1\" as the Task name.").arg(tour::QUICK_TOUR_NAME),
                                                 TourStep::AdvanceOn::UserAction, steps->highlightTask, steps->updateName);
    }
  });
  connect(propertiesMenu(), &PropertiesMenu::propertyUpdated, this, [this, steps](const QString& parameterId, const QString& nodeId, const maki::Value& value) {
    if (!mTourGuide->isActive())
      return;

    if (mTourGuide->currentStepId() != steps->updateName)
      return;

    if (steps->taskId != nodeId)
      return;

    if (parameterId == ConfigKeys::NAME && value.isString() && value.toString() == tour::QUICK_TOUR_NAME)
      mTourGuide->advance();
  });
  connect(notifications(), &NotificationManager::showingNotification, this, [this, steps](NotificationWidget* toast) {
    if (!mTourGuide->isActive())
      return;

    if (mTourGuide->currentStepId() != steps->startVerification)
      return;

    if (!toast || toast->disappearing())
      return;

    steps->verifying = mTourGuide->insertStep(steps->end, toast, tr("Verification progress"),
                                              tr("You can monitor the verification progress here. Depending on the size of your "
                                                 "application, the verification may take some time. Let's wait a bit until it is done."),
                                              TourStep::AdvanceOn::UserAction, TourStep::Invalid, steps->verifying);
    steps->verificationDone = mTourGuide->insertStep(steps->end, toast, tr("Verification done"),
                                                     tr("And there we go! Our program has being verified and no errors were found!"),
                                                     TourStep::AdvanceOn::Button, TourStep::Invalid, steps->verificationDone);

    mTourGuide->advance();
  });
  connect(structureCanvas, &Canvas::nodeAdded, this, [this, canvasView, steps](Canvas* /* canvas */, NodeItem* node) {
    if (!mTourGuide->isActive())
      return;

    if (mTourGuide->currentStepId() != steps->endDragTask)
      return;

    steps->taskId = node->id();

    steps->highlightTask = mTourGuide->insertItemStep(steps->showProperties, canvasView, node, tr("Task node"),
                                                      tr("Nice! Now we have a Task node on the Canvas."), TourStep::AdvanceOn::Button, TourStep::Invalid,
                                                      steps->highlightTask);

    steps->highlightTaskName = mTourGuide->insertItemStep(steps->showCapabilities, canvasView, node, tr("Name updated"),
                                                          tr("Good job! You can see that the node label was also updated."), TourStep::AdvanceOn::Button,
                                                          TourStep::Invalid, steps->highlightTaskName);

    steps->endDragCapability = mTourGuide->insertItemStep(steps->end, canvasView, node, tr("Drop Capability"),
                                                          tr("Drop the %1 Capability on the Task node.").arg(tour::QUICK_TOUR_CAPABILTY),
                                                          TourStep::AdvanceOn::UserAction, steps->startDragCapability, steps->endDragCapability);

    steps->highlightCapability = mTourGuide->insertItemStep(steps->end, canvasView, node, tr("Task with Capability"),
                                                            tr("Great! Now, our Task is able to access the %1 Capability.").arg(tour::QUICK_TOUR_CAPABILTY),
                                                            TourStep::AdvanceOn::Button, TourStep::Invalid, steps->highlightCapability);

    steps->preControlHover = mTourGuide->insertItemStep(steps->end, canvasView, node, tr("Modifying behavior"),
                                                        tr("Move the mouse over the Task node to see what else we can do."), TourStep::AdvanceOn::UserAction,
                                                        steps->highlightCapability, steps->preControlHover);
  });
  connect(structureCanvas, &Canvas::controlCreated, this, [this, steps](maki::ControlWidget* control) {
    LOG_DEBUG("controlCreated task - 0");
    if (!mTourGuide->isActive())
      return;

    LOG_DEBUG("controlCreated task - 1");
    if (mTourGuide->currentStepId() != steps->preControlHover)
      return;

    LOG_DEBUG("controlCreated task - 2");
    control->setTouring();

    steps->highlightControl = mTourGuide->insertStep(
      steps->end, control, tr("Task node menu"),
      tr("This is the task node menu. Here, you can open the main flow or add new Flows to Tasks. Click on \"Open main flow\" to start updating it."),
      TourStep::AdvanceOn::UserAction, TourStep::Invalid, steps->highlightControl);

    mTourGuide->advance();
  });
  connect(
    structureCanvas, &Canvas::openFlow, this,
    [this, callLibrary, callNode, steps](std::shared_ptr<FlowSaveInfo> flow, const QString& nodeId, const maki::FocusProperties& /* properties */
    ) {
      if (!mTourGuide->isActive())
        return;

      if (mTourGuide->currentStepId() != steps->highlightControl)
        return;

      const QString tabId = QString::fromStdString(std::format("{}_{}", flow->getid(), nodeId));
      auto behaviourView = qobject_cast<CanvasView*>(editorWorkspace()->findEditor(tabId));
      if (!behaviourView)
      {
        LOG_DEBUG("Could not find editor: {}", tabId);
        return;
      }

      auto behaviorCanvas = qobject_cast<Canvas*>(behaviourView->scene());

      steps->showBehaviourCanvas = mTourGuide->insertStep(steps->end, behaviourView, tr("Behavior Canvas"),
                                                          tr("This is the Behavior Canvas. Here, you can compose the behavior of Task Flows."),
                                                          TourStep::AdvanceOn::Button, TourStep::Invalid, steps->showBehaviourCanvas);
      steps->showBehaviourPalette = mTourGuide->insertStep(steps->end, palette(), tr("Behavior Tab"),
                                                           tr("In the Behavior Tab, you can find the available behavior nodes. Behavior nodes define how "
                                                              "the different Capabilities must be executed in order to complete the Task."),
                                                           TourStep::AdvanceOn::Button, TourStep::Invalid, steps->showBehaviourPalette);

      steps->startDragCall = mTourGuide->insertItemStep(
        steps->end, callLibrary, callNode, tr("%1 node").arg(tour::QUICK_TOUR_CAPABILTY),
        tr("Let's update the Flow. You know the drill by now, click the \"%1\" node and dragging it onto the Canvas.").arg(tour::QUICK_TOUR_CAPABILTY),
        TourStep::AdvanceOn::UserAction, steps->showBehaviourPalette, steps->startDragCall);

      steps->endDragCall =
        mTourGuide->insertStep(steps->end, behaviourView, tr("Node connection feedback"),
                               tr("Nodes can dropped anywhere in the Canvas. If they are dropped on an existing Transition, %1 will automatically "
                                  "connect the new node. Transitions will update their color when a node can be dropped on them")
                                 .arg(Config::APPLICATION_NAME),
                               TourStep::AdvanceOn::UserAction, steps->startDragCall, steps->endDragCall);

      connect(behaviorCanvas, &Canvas::nodeAdded, this, [this, behaviourView, steps](Canvas* /* canvas */, NodeItem* node) {
        if (!mTourGuide->isActive())
          return;

        if (mTourGuide->currentStepId() != steps->endDragCall)
          return;

        steps->highlightCall = mTourGuide->insertItemStep(steps->end, behaviourView, node, tr("Updating the call node"),
                                                          tr("Perfect! Currently, the node doesn't do anything. You need to assign a Capability to be "
                                                             "called by this node. Click on the plus icon in the \"%1\" node.")
                                                            .arg(tour::QUICK_TOUR_CAPABILTY),
                                                          TourStep::AdvanceOn::UserAction, steps->endDragCall, steps->highlightCall);
        steps->completeCall =
          mTourGuide->insertItemStep(steps->end, behaviourView, node, tr("Complete call node"),
                                     tr("That is it! Now, %1 knows which Capability to call during execution.").arg(Config::APPLICATION_NAME),
                                     TourStep::AdvanceOn::Button, TourStep::Invalid, steps->completeCall);

        steps->startVerification = mTourGuide->insertStep(
          steps->end, pipelineButton(), tr("Verification"),
          tr("With the behavior defined, it can be verified. Click the \"Verify\" button. You will then be "
             "prompted to save the application and %1 will handle the rest. A formal model will be automatically generated and verified.")
            .arg(Config::APPLICATION_NAME),
          TourStep::AdvanceOn::UserAction, steps->completeCall, steps->startVerification);
      });
      connect(behaviorCanvas, &Canvas::controlCreated, this, [this, steps](maki::ControlWidget* control) {
        if (!mTourGuide->isActive())
          return;

        if (mTourGuide->currentStepId() != steps->highlightCall)
          return;

        control->setTouring();
        steps->selectCapability =
          mTourGuide->insertStep(steps->completeCall, control, tr("%1 menu").arg(tour::QUICK_TOUR_CAPABILTY),
                                 tr("Here, you can find the Capabilities which can be called. Let's select our %1 capabilty.").arg(tour::QUICK_TOUR_CAPABILTY),
                                 TourStep::AdvanceOn::UserAction, steps->highlightCall, steps->selectCapability);

        if (auto capSelect = qobject_cast<CapabilityCallSelector*>(control))
        {
          connect(capSelect, &CapabilityCallSelector::changedTabs, this, [this, capSelect, steps](const int tab) {
            if (auto eventSelector = capSelect->eventSelector(); eventSelector)
            {
              steps->selectCapabilityEvent = mTourGuide->insertStep(steps->completeCall, eventSelector, tr("%1 event selection").arg(tour::QUICK_TOUR_NODE),
                                                                    tr("After seleting the Capability, you must select the event that should be called by "
                                                                       "the \"%1\" node. Select \"%2\" in the Event drop-down list.")
                                                                      .arg(tour::QUICK_TOUR_NODE, tour::QUICK_TOUR_EVENT),
                                                                    TourStep::AdvanceOn::UserAction, steps->selectCapability, steps->selectCapabilityEvent);
              mTourGuide->advance();
            }
          });

          connect(capSelect, &CapabilityCallSelector::eventSelected, this, [this, capSelect, steps](const QString& event) {
            if (event != tour::QUICK_TOUR_EVENT)
              return;

            if (auto button = capSelect->confirmButton(); button)
            {
              steps->confirmCapability = mTourGuide->insertStep(steps->completeCall, button, tr("%1 menu").arg(tour::QUICK_TOUR_CAPABILTY),
                                                                tr("Fantastic! Now, press the Confirm button to accept your changes."),
                                                                TourStep::AdvanceOn::UserAction, steps->selectCapabilityEvent, steps->confirmCapability);
              mTourGuide->advance();
            }
          });
        }

        mTourGuide->advance();
      });

      mTourGuide->connectTriggerSuccess(behaviorCanvas, &Canvas::nodeAdded,
                                        [steps](const int currentStep) { return (currentStep == steps->endDragCall) ? TourStep::Valid : TourStep::Invalid; });
      mTourGuide->connectTriggerSuccess(behaviorCanvas, &Canvas::controlDismissed, [steps](const int currentStep) {
        return (currentStep == steps->confirmCapability) ? TourStep::Valid : TourStep::Invalid;
      });

      mTourGuide->advance();
    });

  for (const auto* section : palette()->findChildren<SectionWidget*>())
    if (auto* library = qobject_cast<LibraryContainer*>(section->content()))
    {
      if (auto palette = qobject_cast<LibraryScene*>(library->scene()))
      {
        // We use this to automagically move the focus to the Canvas
        mTourGuide->connectTriggerSuccess(palette, &LibraryScene::dragStarted, [steps](const int currentStep) {
          return (currentStep == steps->startDragTask || currentStep == steps->startDragCapability || currentStep == steps->startDragCall) ? TourStep::Valid
                                                                                                                                           : TourStep::Invalid;
        });
        mTourGuide->connectTriggerFailure(palette, &LibraryScene::dragCancelled, [steps](const int currentStep) {
          if (currentStep == steps->endDragTask)
            return steps->startDragTask;
          else if (currentStep == steps->endDragCapability)
            return steps->startDragCapability;
          else if (currentStep == steps->endDragCall)
            return steps->startDragCall;
          else
            return TourStep::Invalid;
        });
      }
    }

  // triggers for the action-driven steps:
  mTourGuide->connectTriggerSuccess(structureCanvas, &Canvas::nodeAdded, [steps](const int currentStep) {
    return (currentStep == steps->endDragTask || currentStep == steps->endDragCapability) ? TourStep::Valid : TourStep::Invalid;
  });
  mTourGuide->connectTriggerSuccess(pluginPipeline(), &maki::PluginPipeline::pipelineFinished,
                                    [steps](const int currentStep) { return (currentStep == steps->verifying) ? TourStep::Valid : TourStep::Invalid; });

  connect(mTourGuide.get(), &TourGuide::finished, this, [this] { emit tourFinished(); });
  connect(mTourGuide.get(), &TourGuide::cancelled, this, [this] { emit tourCancelled(); });

  constexpr std::size_t stepCount = offsetof(Steps, taskId) / sizeof(int);
  mTourGuide->setStepCount(stepCount);
  mTourGuide->start();
}
