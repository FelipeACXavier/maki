#include "pipeline_canvas.h"

#include <memory>

#include "canvas/behaviour_canvas.h"
#include "canvas/canvas_view.h"
#include "document/flow_info.h"
#include "widgets/controls/pipeline_data_widget.h"

PipelineCanvas::PipelineCanvas(std::shared_ptr<FlowSaveInfo> storage, std::shared_ptr<EdgeRouter> router, QObject* parent)
    : BehaviourCanvas(storage, "", router, parent)
{
}

PipelineCanvas::~PipelineCanvas()
{
  cleanFlow();
}

Types::LibraryTypes PipelineCanvas::type() const
{
  return Types::LibraryTypes::PIPELINE;
}

QString PipelineCanvas::requestCreateNode(NodeSaveInfo info, NodeCreation creation)
{
  LOG_DEBUG("PipelineCanvas::requestCreateNode: {}", info.getnodeId());

  // The model must know the ID before it is inserted.
  if (info.getid().isEmpty())
    info.setId(QUuid::createUuid().toString());

  const QString id = info.getid();
  mPendingCreations.insert(id, creation);
  auto storage = std::make_shared<NodeSaveInfo>(info);

  SaveInfo::instance().addNodeToPipeline(mFlow->id(), storage);
  return id;
}

void PipelineCanvas::requestRemoveNode(NodeSaveInfo info)
{
  if (auto flow = SaveInfo::instance().getPipelineWithId(mFlow->id()))
    flow->removeNode(info.getid());
}

void PipelineCanvas::requestCreateTransition(TransitionSaveInfo info)
{
  LOG_DEBUG("PipelineCanvas::requestCreateTransition: {}", info.getid());
  // The model must know the ID before it is inserted.
  if (info.getid().isEmpty())
    info.setId(QUuid::createUuid().toString());

  auto storage = std::make_shared<TransitionSaveInfo>(info);
  SaveInfo::instance().addTransitionToPipeline(mFlow->id(), storage);
}

void PipelineCanvas::requestRemoveTransition(TransitionSaveInfo info)
{
  if (auto flow = SaveInfo::instance().getPipelineWithId(mFlow->id()))
    flow->removeTransition(info.getid());
}

void PipelineCanvas::onNodeHovered(NodeItem* node, bool entered)
{
  if (!node)
    return;

  if (entered)
  {
    if (!node || node->hasControl())
      return;

    auto* control = maki::PipelineDataWidget::create(node->id(), mFlow->config(), parentView());
    connect(control, &maki::ControlWidget::activated, this, [this, control] { emit controlCreated(control); });
    connect(control, &maki::ControlWidget::dismissed, this, [this, control] { emit controlDismissed(control); });

    node->showControls(control, ControlProperties{
                                  .fadingMode = ControlProperties::FadingMode::NoFadeOnHover,
                                  .position = Config::ControlPosition::Top,
                                });
  }
  else
  {
    node->hideControlOfType<maki::PipelineDataWidget>();
  }
}
