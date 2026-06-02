#include "dropdown_button.h"

#include <QActionGroup>
#include <QMenu>
#include <QPainter>
#include <QStyleOptionToolButton>
#include <oclero/qlementine.hpp>

#include "logging.h"
#include "style_helpers.h"

DropDownButton::DropDownButton(QWidget* parent)
    : QToolButton(parent)
{
  mMenu = new QMenu(this);
  mMenu->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  mMenu->setMinimumWidth(mWidth);

  setMenu(mMenu);
  setEnabled(true);
  setPopupMode(QToolButton::MenuButtonPopup);
  setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
  connect(this, &QWidget::customContextMenuRequested, this, &DropDownButton::showContextMenu);

  connect(this, &QToolButton::clicked, this, [this] {
    if (!mCurrentOption.isEmpty())
      emit executeRequested(mCurrentOption);
  });

  updateButtonText();
}

void DropDownButton::addOption(const QString& name)
{
  mOptions.push_back(name);
  rebuildMenu();
}

void DropDownButton::setCurrentOption(const QString& name)
{
  mCurrentOption = name;
  updateButtonText();
}

QString DropDownButton::currentOption()
{
  return mCurrentOption;
}

void DropDownButton::reset()
{
  mOptions.clear();
}

void DropDownButton::rebuildMenu()
{
  mMenu->clear();

  for (const auto& option : mOptions)
  {
    auto* pipelineMenu = mMenu->addMenu(option);
    pipelineMenu->setProperty("id", option);
    buildMenu(pipelineMenu, option, true);
  }

  updateButtonText();
}

void DropDownButton::updateButtonText()
{
  QString optionName;
  for (const auto& option : mOptions)
  {
    if (option != mCurrentOption)
      continue;

    optionName = option;
    break;
  }

  if (optionName.isEmpty())
  {
    if (mOptions.isEmpty())
      optionName = tr("Add new...");
    else
      // mCurrentOption is not longer in options, so we must override it
      optionName = mOptions.first();
  }

  mCurrentOption = optionName;
  setText(optionName);
}

void DropDownButton::showContextMenu(const QPoint& event)
{
  QMenu menu(this);
  buildMenu(&menu, mCurrentOption, false);
  menu.exec(mapToGlobal(event));
}

void DropDownButton::buildMenu(QMenu* menu, const QString& option, bool addRun)
{
  auto* runAction = menu->addAction(iconFromTheme("run-build"), tr("Run"));
  connect(runAction, &QAction::triggered, this, [this, option] {
    mCurrentOption = option;
    updateButtonText();
    emit executeRequested(option);
  });

  if (addRun)
  {
    auto* selectAction = menu->addAction(iconFromTheme("select"), tr("Select"));
    connect(selectAction, &QAction::triggered, this, [this, option] {
      mCurrentOption = option;
      updateButtonText();
    });
  }

  menu->addSeparator();

  if (!addRun)
  {
    auto* addAction = menu->addAction(iconFromTheme("document-new"), tr("Add"));
    connect(addAction, &QAction::triggered, this, [this, option] {
      emit editOptionRequested("");
    });
  }
  auto* editAction = menu->addAction(iconFromTheme("document-edit"), tr("Edit"));
  connect(editAction, &QAction::triggered, this, [this, option] {
    emit editOptionRequested(option);
  });
  auto* deleteAction = menu->addAction(iconFromTheme("edit-delete"), tr("Delete"));
  connect(deleteAction, &QAction::triggered, this, [this, option] {
    emit deleteOptionRequested(option);
  });
}

void DropDownButton::setSize(int width, int height)
{
  mWidth = width;
  mHeight = height;
}

void DropDownButton::setRunning(bool running)
{
  mRunning = running;
  update();
}

QSize DropDownButton::sizeHint() const
{
  return {mWidth, mHeight};
}

void DropDownButton::paintEvent(QPaintEvent* event)
{
  auto* qlementineStyle = oclero::qlementine::appStyle();
  if (!qlementineStyle)
  {
    QToolButton::paintEvent(event);
    return;
  }
  const auto theme = qlementineStyle->theme();

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing);

  const QRect r = rect().adjusted(theme.borderWidth, theme.borderWidth, -theme.borderWidth, -theme.borderWidth);

  const bool hovered = underMouse();
  const bool pressed = isDown();

  QColor bg = theme.backgroundColorMain2;
  if (hovered || pressed)
    bg = theme.backgroundColorMain3;

  painter.setPen(theme.borderColor);
  painter.setBrush(bg);
  painter.drawRoundedRect(r, 8, 8);

  // Left circular play area.
  const int circleSize = height() - 2 * theme.spacing;
  const QRect circleRect(r.left() + theme.spacing, r.center().y() - circleSize / 2, circleSize, circleSize);

  // A nice medium green
  QColor playBg(45, 160, 80);

  if (hovered)
    playBg = playBg.darker(108);
  if (!isEnabled())
    playBg = theme.neutralColorDisabled;

  painter.setPen(Qt::NoPen);
  painter.setBrush(playBg);
  painter.drawEllipse(circleRect);

  painter.setBrush(Qt::white);
  const QPointF c = circleRect.center() - QPointF{0, -1};
  if (mRunning)
  {
    // Pause icon
    const int barWidth = 4;
    const int barHeight = circleSize / 2;
    const int gap = 4;

    QRectF leftBar(c.x() - gap / 2.0 - barWidth / 2.0, c.y() - barHeight / 2.0, barWidth, barHeight);
    QRectF rightBar(c.x() + gap / 2.0, c.y() - barHeight / 2.0, barWidth, barHeight);

    painter.drawRoundedRect(leftBar, 1.5, 1.5);
    painter.drawRoundedRect(rightBar, 1.5, 1.5);
  }
  else
  {
    // Play triangle.
    QPolygonF triangle;
    const auto z = (circleSize - theme.spacing) / 2;
    const auto x = 0.5 * z;    // cos(60)
    const auto y = 0.866 * z;  // sin(60)
    triangle << QPointF(c.x() - x, c.y() - y)
             << QPointF(c.x() - x, c.y() + y)
             << QPointF(c.x() + z, c.y());

    painter.drawPolygon(triangle);
  }

  // Text.
  const QString label = mCurrentOption.isEmpty() ? text() : mCurrentOption;
  QRect textRect = r.adjusted(circleRect.right() + theme.spacing, 0, -theme.spacing, 0);

  painter.setPen(isEnabled() ? theme.secondaryColor : theme.secondaryColorDisabled);
  painter.drawText(textRect, Qt::AlignVCenter | Qt::AlignLeft, fontMetrics().elidedText(label, Qt::ElideRight, textRect.width()));

  // Dropdown divider.
  const int arrowAreaWidth = 24;
  const int dividerX = r.right() - arrowAreaWidth;

  painter.setPen(theme.secondaryColorDisabled);
  painter.drawLine(dividerX, r.top() + theme.spacing, dividerX, r.bottom() - theme.spacing);

  // Arrow.
  QPolygonF arrow;
  const QPointF ac(dividerX + static_cast<qreal>(arrowAreaWidth) / 2, r.center().y() + 1);

  arrow << QPointF(ac.x() - 4, ac.y() - 2)
        << QPointF(ac.x() + 4, ac.y() - 2)
        << QPointF(ac.x(), ac.y() + 3);

  painter.setPen(Qt::NoPen);
  painter.setBrush(theme.secondaryColor);
  painter.drawPolygon(arrow);
}
