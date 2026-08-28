#include "suggestion_menu.h"

#include <QCheckBox>
#include <QEnterEvent>
#include <QGraphicsOpacityEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QSvgRenderer>
#include <QTimer>
#include <QVBoxLayout>
#include <oclero/qlementine.hpp>
#include <oclero/qlementine/widgets/Label.hpp>

#include "../widget_factory.h"
#include "elements/node.h"
#include "system/canvas_view.h"

SuggestionRow::SuggestionRow(const QString& svgPath, const QString& labelText, QWidget* parent)
    : QWidget(parent)
    , mText(labelText)
{
  const auto* qlementineStyle = oclero::qlementine::appStyle();
  if (!qlementineStyle)
    return;

  const auto theme = qlementineStyle->theme();
  setCursor(Qt::PointingHandCursor);

  auto* layout = new QHBoxLayout(this);
  layout->setContentsMargins(Config::CONTENT_PADDING, 0, Config::CONTENT_PADDING, 0);
  layout->setSpacing(theme.spacing);
  layout->setAlignment(Qt::AlignCenter);

  mCheckBox = new QCheckBox(this);
  mCheckBox->setFocusPolicy(Qt::NoFocus);
  layout->addWidget(mCheckBox, 0, Qt::AlignVCenter);

  if (!svgPath.isEmpty())
  {
    auto* icon = new QLabel(this);
    icon->setFixedSize(18, 18);
    icon->setPixmap(QPixmap(svgPath).scaled(18, 18, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    layout->addWidget(icon, 0, Qt::AlignVCenter);
  }

  mLabel = new oclero::qlementine::Label(labelText, this);
  mLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  layout->addWidget(mLabel);
}

const QString& SuggestionRow::text() const
{
  return mText;
}

bool SuggestionRow::isSelected() const
{
  return mCheckBox->isChecked();
}

void SuggestionRow::setSelected(bool selected)
{
  mCheckBox->setChecked(selected);
}

void SuggestionRow::enterEvent(QEnterEvent* event)
{
  setHovered(true);
  QWidget::enterEvent(event);
}

void SuggestionRow::leaveEvent(QEvent* event)
{
  setHovered(false);
  QWidget::leaveEvent(event);
}

void SuggestionRow::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton)
  {
    mCheckBox->toggle();
    event->accept();
    return;
  }

  QWidget::mousePressEvent(event);
}

void SuggestionRow::setHovered(bool hovered)
{
  if (mHovered == hovered)
    return;

  mHovered = hovered;
  update();
}

void SuggestionRow::paintEvent(QPaintEvent* event)
{
  QPainter painter(this);

  if (const auto* qlementineStyle = oclero::qlementine::appStyle())
  {
    painter.setPen(Qt::NoPen);
    painter.setBrush(mHovered ? qlementineStyle->theme().neutralColorHovered : qlementineStyle->theme().backgroundColorMain1);
    painter.drawRect(rect());
  }

  QWidget::paintEvent(event);
}

// ============================================================================
// SuggestionMenu
// ============================================================================
SuggestionMenu* SuggestionMenu::create(QWidget* parent)
{
  auto* menu = new SuggestionMenu(parent);
  return menu;
}

SuggestionMenu::SuggestionMenu(QWidget* parent)
    : maki::ControlWidget(parent)
{
  const auto* qlementineStyle = oclero::qlementine::appStyle();
  if (!qlementineStyle)
    return;

  const auto theme = qlementineStyle->theme();

  // Since this is an overlay, keep it above the normal canvas widgets.
  raise();

  setFixedWidth(Config::MENU_WIDTH);

  auto frame = new StyledFrame(this);
  frame->setBackgroundRole(StyledFrame::BackgroundRole::Base);
  frame->setBorderWidth(theme.borderWidth);
  frame->setRadius(theme.borderRadius);
  frame->setCustomBorderColor(theme.primaryColor);

  auto* mainLayout = new QVBoxLayout(frame);
  mainLayout->setContentsMargins(theme.spacing, theme.spacing, theme.spacing, theme.spacing);
  mainLayout->setSpacing(theme.spacing);

  // --------------------------------------------------------------------------
  // Header
  auto* title = new oclero::qlementine::Label(tr("Suggested connections"), this);
  title->setRole(oclero::qlementine::TextRole::H4);

  auto* treeLine = new QFrame(this);
  treeLine->setFrameShape(QFrame::HLine);

  auto* description = new oclero::qlementine::Label(tr("Select capabilities you would like to add."), this);
  description->setRole(oclero::qlementine::TextRole::Default);
  description->setWordWrap(true);

  mainLayout->addWidget(title);
  mainLayout->addWidget(treeLine);
  mainLayout->addWidget(description);

  // --------------------------------------------------------------------------
  // Body
  mConsumersGroup = new maki::WidgetGroup(tr("Consumers"), oclero::qlementine::TextRole::Default, this);
  mProducersGroup = new maki::WidgetGroup(tr("Producers"), oclero::qlementine::TextRole::Default, this);

  mainLayout->addWidget(mConsumersGroup);
  mainLayout->addWidget(mProducersGroup);

  // --------------------------------------------------------------------------
  // Buttons
  auto buttons = new QWidget(this);
  auto* buttonLayout = new QHBoxLayout(buttons);
  buttonLayout->setContentsMargins(0, Config::CONTENT_PADDING, 0, 0);
  buttonLayout->setSpacing(theme.spacing);

  buttonLayout->addStretch();
  auto* skipButton = new QPushButton(tr("Skip"), this);
  auto* addButton = new QPushButton(tr("Add selected"), this);
  addButton->setDefault(true);

  buttonLayout->addWidget(skipButton);
  buttonLayout->addWidget(addButton);
  mainLayout->addWidget(buttons);

  connect(skipButton, &QPushButton::clicked, this, [this]() { emit dismissed(); });

  connect(addButton, &QPushButton::clicked, this, [this]() {
    const auto selected = selectedSuggestions();
    if (selected.empty())
      return;

    emit accepted(selected);
  });

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
  layout->addWidget(frame);
}

void SuggestionMenu::setSuggestions(const QStringList& consumers, const QStringList& producers)
{
  clearSuggestions();

  if (!consumers.isEmpty())
  {
    for (const auto& consumer : consumers)
    {
      auto* row = new SuggestionRow(QString{}, consumer, this);
      mConsumerRows.push_back(row);
      mConsumersGroup->addWidget(row);
    }
    mConsumersGroup->show();
  }
  else
  {
    mConsumersGroup->hide();
  }

  if (!producers.isEmpty())
  {
    for (const auto& producer : producers)
    {
      auto* row = new SuggestionRow(QString{}, producer, this);
      mProducerRows.push_back(row);
      mProducersGroup->addWidget(row);
    }
    mProducersGroup->show();
  }
  else
  {
    mProducersGroup->hide();
  }

  adjustSize();
}

void SuggestionMenu::clearSuggestions()
{
  for (auto* row : mConsumerRows)
  {
    mConsumersGroup->removeWidget(row);
    row->deleteLater();
  }

  for (auto* row : mProducerRows)
  {
    mProducersGroup->removeWidget(row);
    row->deleteLater();
  }

  mConsumerRows.clear();
  mProducerRows.clear();

  mConsumersGroup->clear();
  mProducersGroup->clear();
}

QStringList SuggestionMenu::selectedSuggestions() const
{
  QStringList result;
  for (const auto* row : mConsumerRows)
    if (row->isSelected())
      result << row->text();

  for (const auto* row : mProducerRows)
    if (row->isSelected())
      result << row->text();

  return result;
}
