#include "execute_button.h"

#include <QActionGroup>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionToolButton>
#include <QTimer>
#include <oclero/qlementine.hpp>

#include "app_configs.h"
#include "notifications.h"
#include "style_helpers.h"

static const QString CREATE_NEW = "";

ExecuteButton::ExecuteButton(QWidget* parent)
    : DropDownButton(parent)
    , mCurrentOption("")
    , DEFAULT_TEXT(tr("No pipeline"))
    , ADD_NEW_TEXT(tr("Add new..."))
{
  setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
  connect(this, &QWidget::customContextMenuRequested, this, &ExecuteButton::showContextMenu);
  connect(this, &QToolButton::clicked, this, [this] {
    const auto option = currentOption();
    if (option.name.isEmpty() || option.name == DEFAULT_TEXT)
      emit editOptionRequested(CREATE_NEW);
    else
      emit executeRequested(option.id);
  });

  setupDone();
}

void ExecuteButton::reset()
{
  DropDownButton::reset();
  setupDone();
}

void ExecuteButton::addOption(const QString& name, const QString& id)
{
  auto opts = getPipelineNames();
  auto option = std::find_if(opts.begin(), opts.end(), [id](const OptionInfo& opt) { return opt.id == id; });
  if (option != opts.end())
  {
    // We are editing an existing pipeline
    option->name = name;
  }
  else
  {
    opts.push_back({.id = id, .name = name});
  }
  rebuildMenu(opts);
}

void ExecuteButton::removeOption(const QString& id)
{
  auto opts = getPipelineNames();
  opts.erase(std::remove_if(opts.begin(), opts.end(), [id](const OptionInfo& opt) { return opt.id == id; }));
  rebuildMenu(opts);
}

ExecuteButton::OptionInfo ExecuteButton::currentOption()
{
  return mCurrentOption;
}

void ExecuteButton::setCurrentOption(const OptionInfo& option)
{
  mCurrentOption = option;
}

void ExecuteButton::setupDone()
{
  menu()->addSeparator();

  auto* addNewPipeline = menu()->addAction(iconFromTheme("document-new"), ADD_NEW_TEXT);
  connect(addNewPipeline, &QAction::triggered, this, [this] { QTimer::singleShot(0, this, [this]() { emit editOptionRequested(CREATE_NEW); }); });

  updateButtonText();
}

void ExecuteButton::rebuildMenu(const std::vector<OptionInfo>& pipelines)
{
  DropDownButton::reset();

  for (const auto& pipeline : pipelines)
  {
    auto* pipelineMenu = menu()->addMenu(pipeline.name);
    pipelineMenu->menuAction()->setData(pipeline.id);
    buildMenu(pipelineMenu, pipeline, true);
  }

  setupDone();
}

void ExecuteButton::updateButtonText()
{
  const auto options = getPipelineNames();
  auto option = currentOption();

  // If there are no options, then we just take the default
  if (option.name.isEmpty() || options.empty())
    option.name = DEFAULT_TEXT;
  // If an option was deleted, then we can just take the first option
  else if (std::count_if(options.begin(), options.end(), [option](const OptionInfo& opt) { return opt.name == option.name && opt.id == option.id; }) <
           1)
    option = *options.begin();

  LOG_DEBUG("Setting option: {} {}", option.name, option.id);
  setCurrentOption(option);
  setText(option.name);
}

void ExecuteButton::showContextMenu(const QPoint& event)
{
  QMenu menu(this);
  buildMenu(&menu, currentOption(), false);
  menu.exec(mapToGlobal(event));
}

void ExecuteButton::buildMenu(QMenu* menu, const OptionInfo& option, bool addSelect)
{
  auto* runAction = menu->addAction(iconFromTheme("run-build"), tr("Run"));
  connect(runAction, &QAction::triggered, this, [this, option] {
    setCurrentOption(option);
    updateButtonText();
    emit executeRequested(option.id);
  });

  if (addSelect)
  {
    auto* selectAction = menu->addAction(iconFromTheme("select"), tr("Select"));
    connect(selectAction, &QAction::triggered, this, [this, option] {
      setCurrentOption(option);
      updateButtonText();
    });
  }

  menu->addSeparator();

  auto* editAction = menu->addAction(iconFromTheme("document-edit"), tr("Edit"));
  connect(editAction, &QAction::triggered, this, [this, option] { emit editOptionRequested(option.id); });
  auto* renameAction = menu->addAction(iconFromTheme("insert-text"), tr("Rename"));
  connect(renameAction, &QAction::triggered, this, [this, option] { emit renameOptionRequested(option.id); });
  auto* deleteAction = menu->addAction(iconFromTheme("edit-delete"), tr("Delete"));
  connect(deleteAction, &QAction::triggered, this, [this, option] { emit deleteOptionRequested(option.id); });
}

void ExecuteButton::setRunning(bool running)
{
  mRunning = running;
  update();
}

QSize ExecuteButton::sizeHint() const
{
  return {mWidth, mHeight};
}

int ExecuteButton::leadingContentWidth(const oclero::qlementine::Theme& theme) const
{
  return height() - 2 * theme.spacing;
}

void ExecuteButton::paintLeadingContent(QPainter& painter, const QRect& contentRect, const oclero::qlementine::Theme& theme)
{
  const int circleSize = qMin(contentRect.width(), height() - 2 * theme.spacing);
  const QRectF circleRect(contentRect.left(), contentRect.center().y() - circleSize / 2.0, circleSize, circleSize);

  QColor playBackground(45, 160, 80);

  if (underMouse())
    playBackground = playBackground.darker(108);

  if (!isEnabled())
    playBackground = theme.neutralColorDisabled;

  painter.setPen(Qt::NoPen);
  painter.setBrush(playBackground);
  painter.drawEllipse(circleRect);

  painter.setBrush(Qt::white);
  const QPointF center = circleRect.center();

  if (mRunning)
  {
    constexpr int barWidth = 4;
    constexpr qreal barRadius = 1.5;
    constexpr int gap = 4;
    const int barHeight = static_cast<int>(circleSize / 2.0);

    const QRectF leftBar(center.x() - gap / 2.0 - barWidth, center.y() - barHeight / 2.0, barWidth, barHeight);
    const QRectF rightBar(center.x() + gap / 2.0, center.y() - barHeight / 2.0, barWidth, barHeight);

    painter.drawRoundedRect(leftBar, barRadius, barRadius);
    painter.drawRoundedRect(rightBar, barRadius, barRadius);
  }
  else
  {
    const qreal radius = (circleSize - theme.spacing) / 2.0;
    const qreal x = 0.5 * radius;
    const qreal y = 0.866 * radius;
    const QPointF shift = {0.5, 0};

    QPolygonF triangle;
    triangle << QPointF(center.x() - x, center.y() - y) - shift << QPointF(center.x() - x, center.y() + y) - shift
             << QPointF(center.x() + radius, center.y()) - shift;
    painter.drawPolygon(triangle);
  }
}

std::vector<ExecuteButton::OptionInfo> ExecuteButton::getPipelineNames() const
{
  std::vector<OptionInfo> pipelines;

  for (const QAction* action : menu()->actions())
    if (!action->isSeparator() && action->text() != ADD_NEW_TEXT)
      pipelines.push_back({.id = action->data().toString(), .name = action->text()});

  return pipelines;
}