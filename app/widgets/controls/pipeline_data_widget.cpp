#include "pipeline_data_widget.h"

#include <QHBoxLayout>
#include <QSvgWidget>
#include <QVBoxLayout>
#include <oclero/qlementine/widgets/Label.hpp>

#include "app_configs.h"
#include "document/flow_info.h"
#include "document/node_info.h"
#include "document/save_info.h"
#include "logging.h"
#include "pipeline/plugin_action_registry.h"
#include "pipeline_action.h"
#include "style_helpers.h"
#include "widgets/frame.h"
#include "widgets/widget_factory.h"

namespace maki
{
PipelineDataWidget* PipelineDataWidget::create(const QString& nodeId, std::shared_ptr<FlowSaveInfo> graph, QWidget* parent)
{
  auto* menu = new PipelineDataWidget(nodeId, graph, parent);
  return menu;
}

PipelineDataWidget::PipelineDataWidget(const QString& nodeId, std::shared_ptr<FlowSaveInfo> graph, QWidget* parent)
    : maki::ControlWidget(parent)
{
  setFocusPolicy(Qt::NoFocus);

  const auto* qlementineStyle = oclero::qlementine::appStyle();
  if (!qlementineStyle)
    return;

  const auto theme = qlementineStyle->theme();

  // Since this is an overlay, keep it above the normal canvas widgets.
  raise();

  auto frame = new StyledFrame(this);
  frame->setBackgroundRole(StyledFrame::BackgroundRole::Base);
  frame->setBorderWidth(theme.borderWidth);
  frame->setRadius(theme.borderRadius);
  frame->setCustomBorderColor(theme.primaryColor);

  auto* mainLayout = new QVBoxLayout(frame);
  mainLayout->setContentsMargins(theme.spacing, theme.spacing, theme.spacing, theme.spacing);
  mainLayout->setSpacing(0);

  if (auto nodeInfo = SaveInfo::instance().getPipelineNodeWithId(nodeId))
  {
    const auto actionName = nodeInfo->getnodeId().section("::", 1, 1);
    if (auto action = maki::PipelineActionRegistry::instance().action(actionName))
    {
      auto* group = new maki::WidgetGroup(action->displayName(), oclero::qlementine::TextRole::H5, frame);
      group->setPadding(Config::CONTENT_PADDING / 2);

      if (auto* consumes = createGroup(tr("Consumes"), action->consumes(), [this, graph](const QString& id) { return hasProducer(graph, id); }, group))
        group->addWidget(consumes);

      if (auto* consumes = createGroup(tr("Produces"), action->produces(), [this, graph](const QString& id) { return hasConsumer(graph, id); }, group))
        group->addWidget(consumes);

      mainLayout->addWidget(group);
    }
  }

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(frame);
}

void PipelineDataWidget::hideMenu()
{
  emit dismissed();
}

QWidget* PipelineDataWidget::createGroup(const QString& title, const QStringList& data, HealthCheckFunction check, QWidget* parent) const
{
  if (!check || !parent)
    return nullptr;

  auto* group = new maki::WidgetGroup(title, oclero::qlementine::TextRole::Default, parent);
  group->setPadding(Config::CONTENT_PADDING / 2);

  for (const auto& value : data)
  {
    auto* container = new QWidget(group);
    container->setContentsMargins(0, 0, 0, 0);

    auto labelLayout = new QHBoxLayout(container);
    labelLayout->setContentsMargins(0, 0, 0, 0);
    labelLayout->setSpacing(Config::CONTENT_PADDING);
    labelLayout->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    const auto healthStatus = check(value);
    auto* label = new oclero::qlementine::Label(container);
    label->setRole(oclero::qlementine::TextRole::Default);
    label->setTextFormat(Qt::RichText);
    label->setText(QString("<span style=\"color: %1;\">%2</span>").arg(healthToColor(healthStatus), value.toHtmlEscaped()));

    auto* divider = new QFrame(container);
    divider->setFrameShape(QFrame::VLine);

    // We need to check that the flow is also good
    const auto iconPath = QString(healthStatus == Health::Ok ? ":/icons/accept.svg" : ":/icons/reject.svg");
    auto* icon = new QSvgWidget(container);
    icon->setFixedSize(Config::SMALL_BUTTON_SIZE);

    // Apply the color to all paths that inherit the default fill.
    // This is very hacky (or maybe genius...), but since we are replacing the icons, it is more than enough
    QFile file(iconPath);
    if (file.open(QIODevice::ReadOnly))
    {
      QByteArray svg = file.readAll();
      svg.replace("<svg ", QByteArray("<svg fill=\"") + healthToColor(healthStatus).toUtf8() + "\" ");
      icon->load(svg);
    }

    labelLayout->addWidget(icon);

    // Add everything to the row
    labelLayout->addWidget(icon);
    labelLayout->addWidget(divider);
    labelLayout->addStretch();
    labelLayout->addWidget(label);

    // Add everything to the group
    group->addWidget(container);
  }

  return group;
}

PipelineDataWidget::Health PipelineDataWidget::hasProducer(std::shared_ptr<FlowSaveInfo> graph, const QString& consumableId) const
{
  for (const auto& node : graph->getnodes())
  {
    const auto actionName = node->getnodeId().section("::", 1, 1);
    if (auto action = maki::PipelineActionRegistry::instance().action(actionName))
      if (action->produces().contains(consumableId))
        return Health::Ok;
  }

  return Health::Fail;
}

PipelineDataWidget::Health PipelineDataWidget::hasConsumer(std::shared_ptr<FlowSaveInfo> graph, const QString& productId) const
{
  for (const auto& node : graph->getnodes())
  {
    const auto actionName = node->getnodeId().section("::", 1, 1);
    if (auto action = maki::PipelineActionRegistry::instance().action(actionName))
      if (action->consumes().contains(productId))
        return Health::Ok;
  }

  return Health::Passable;
}

QString PipelineDataWidget::healthToColor(Health state) const
{
  switch (state)
  {
    case Health::Ok:
      return oclero::qlementine::appStyle()->theme().statusColorSuccess.name();
    case Health::Passable:
      return oclero::qlementine::appStyle()->theme().statusColorWarning.name();
    case Health::Fail:
      return oclero::qlementine::appStyle()->theme().statusColorError.name();
  }

  return oclero::qlementine::appStyle()->theme().primaryColor.name();
}

}  // namespace maki
