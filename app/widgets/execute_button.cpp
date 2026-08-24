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
    if (option.isEmpty() || option == DEFAULT_TEXT)
      NOTIFY_WARNING(Config::APPLICATION_NAME.toStdString(), "No option selected, you must first create a pipeline.");
    else
      emit executeRequested(currentOption());
  });

  setupDone();
}

void ExecuteButton::reset()
{
  DropDownButton::reset();
  setupDone();
}

void ExecuteButton::addOption(const QString& name)
{
  auto opts = getPipelineNames();
  opts.append(name);
  rebuildMenu(opts);
}

void ExecuteButton::removeOption(const QString& name)
{
  auto opts = getPipelineNames();
  opts.removeAll(name);
  rebuildMenu(opts);
}

QString ExecuteButton::currentOption()
{
  return mCurrentOption;
}

void ExecuteButton::setCurrentOption(const QString& name)
{
  mCurrentOption = name;
}

void ExecuteButton::setupDone()
{
  menu()->addSeparator();

  auto* addNewPipeline = menu()->addAction(iconFromTheme("document-new"), ADD_NEW_TEXT);
  connect(addNewPipeline, &QAction::triggered, this, [this] { QTimer::singleShot(0, this, [this]() { emit editOptionRequested(""); }); });

  updateButtonText();
}

void ExecuteButton::rebuildMenu(const QStringList& pipelines)
{
  DropDownButton::reset();

  for (const auto& pipeline : pipelines)
  {
    auto* pipelineMenu = menu()->addMenu(pipeline);
    pipelineMenu->setProperty("id", pipeline);
    buildMenu(pipelineMenu, pipeline, true);
  }

  setupDone();
}

void ExecuteButton::updateButtonText()
{
  const auto options = getPipelineNames();
  QString optionName = currentOption();

  // If there are no options, then we just take the default
  if (optionName.isEmpty() || options.isEmpty())
    optionName = DEFAULT_TEXT;
  // If an option was deleted, then we can just take the first option
  else if (!options.contains(optionName))
    optionName = options.first();

  setCurrentOption(optionName);
  setText(optionName);
}

void ExecuteButton::showContextMenu(const QPoint& event)
{
  QMenu menu(this);
  buildMenu(&menu, currentOption(), false);
  menu.exec(mapToGlobal(event));
}

void ExecuteButton::buildMenu(QMenu* menu, const QString& option, bool addSelect)
{
  auto* runAction = menu->addAction(iconFromTheme("run-build"), tr("Run"));
  connect(runAction, &QAction::triggered, this, [this, option] {
    setCurrentOption(option);
    updateButtonText();
    emit executeRequested(option);
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
  connect(editAction, &QAction::triggered, this, [this, option] { emit editOptionRequested(option); });
  auto* deleteAction = menu->addAction(iconFromTheme("edit-delete"), tr("Delete"));
  connect(deleteAction, &QAction::triggered, this, [this, option] { emit deleteOptionRequested(option); });
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

    QPolygonF triangle;
    triangle << QPointF(center.x() - x, center.y() - y) << QPointF(center.x() - x, center.y() + y) << QPointF(center.x() + radius, center.y());

    painter.drawPolygon(triangle);
  }
}

QStringList ExecuteButton::getPipelineNames() const
{
  QStringList pipelineNames;

  for (const QAction* action : menu()->actions())
    if (!action->isSeparator() && action->text() != ADD_NEW_TEXT)
      pipelineNames.append(action->text());

  return pipelineNames;
}