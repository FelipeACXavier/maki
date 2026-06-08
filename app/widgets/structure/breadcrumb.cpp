#include "breadcrumb.h"

#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>

#include "app_configs.h"
#include "oclero/qlementine/widgets/IconWidget.hpp"
#include "types.h"
#include "widgets/frame.h"

static const QString EMPTY_LABEL = "...";

BreadcrumbLabel::BreadcrumbLabel(const QString& text, QWidget* parent)
    : StyledFrame(parent)
{
  setBackgroundRole(StyledFrame::BackgroundRole::Window);
  setBorderRole(StyledFrame::BorderRole::Mid);
  setBorderWidth(0);
  setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);

  mLabel = new QLabel("", this);
  auto labelFont = font();
  labelFont.setItalic(true);
  mLabel->setFont(labelFont);

  mIcon = new oclero::qlementine::IconWidget(this);

  auto* layout = new QHBoxLayout(this);
  const auto* qlementineStyle = oclero::qlementine::appStyle();
  if (qlementineStyle)
  {
    const int vpadding = qlementineStyle->theme().tabBarPaddingTop;
    const int hpadding = qlementineStyle->theme().spacing;
    layout->setContentsMargins(hpadding, vpadding, hpadding, vpadding);
    layout->setSpacing(hpadding);
    setRadius(hpadding);
  }
  else
  {
    const int vpadding = 4;
    const int hpadding = 6;
    layout->setContentsMargins(hpadding, vpadding, hpadding, vpadding);
    layout->setSpacing(hpadding);
    setRadius(hpadding);
  }

  layout->addWidget(mIcon);
  layout->addWidget(mLabel);
}

bool BreadcrumbLabel::isCollapsed() const
{
  return !mLabel || mLabel->text() == EMPTY_LABEL;
}

QString BreadcrumbLabel::text() const
{
  return mExpandedText;
}

QIcon BreadcrumbLabel::icon() const
{
  return mIcon ? mIcon->icon() : QIcon();
}

void BreadcrumbLabel::setText(const QString& text)
{
  mExpandedText = text;
  if (mLabel)
    mLabel->setText(text);
}

void BreadcrumbLabel::setIcon(const QIcon& icon)
{
  if (mIcon)
    mIcon->setIcon(icon);
}

void BreadcrumbLabel::collapse()
{
  if (mLabel)
    mLabel->setText(EMPTY_LABEL);
}

void BreadcrumbLabel::expand()
{
  if (mLabel)
    mLabel->setText(mExpandedText);
}

BreadcrumbWidget::BreadcrumbWidget(QWidget* parent)
    : QWidget(parent)
{
  if (parent)
    parent->installEventFilter(this);

  auto* layout = new QHBoxLayout(this);
  layout->setContentsMargins(8, 0, 0, 0);
  layout->setSpacing(0);

  setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

  mProjectName = new BreadcrumbLabel("", this);
  mTabSeparator = new oclero::qlementine::IconWidget(QIcon(":/icons/separator.svg"), this);
  setIconOptions(mTabSeparator);

  mTabName = new BreadcrumbLabel("", this);
  mBlockSeparator = new oclero::qlementine::IconWidget(QIcon(":/icons/separator.svg"), this);
  setIconOptions(mBlockSeparator);

  mBlockName = new BreadcrumbLabel("", this);

  layout->addWidget(mProjectName);
  layout->addWidget(mTabSeparator);
  layout->addWidget(mTabName);
  layout->addWidget(mBlockSeparator);
  layout->addWidget(mBlockName);

  // Start closed
  setProject("Unnamed");
  reposition();
  applySize();
}

void BreadcrumbWidget::scheduleApplySize()
{
  if (mApplySizeQueued)
    return;

  mApplySizeQueued = true;

  QMetaObject::invokeMethod(this, [this] {
        layout()->invalidate();
        layout()->activate();
        applySize();
        update();
        mApplySizeQueued = false; }, Qt::QueuedConnection);
}

void BreadcrumbWidget::applySize()
{
  const QSize wantedSize = sizeHint();
  if (size() != wantedSize)
    resize(wantedSize);
}

void BreadcrumbWidget::reposition()
{
  if (!parentWidget())
    return;

  const QRect r = parentWidget()->contentsRect();
  const QPoint wantedPos(r.left() + Config::CANVAS_OVERLAY_X_MARGIN,
                         r.top() + Config::CANVAS_OVERLAY_Y_MARGIN);
  if (pos() != wantedPos)
    move(wantedPos);

  raise();
}

void BreadcrumbWidget::setProject(const QString& project)
{
  setUpdatesEnabled(false);
  updateLabel(project, mProjectName, QIcon(":/icons/home.svg"), nullptr);
  updateLabel("", mTabName, QIcon(), mTabSeparator);
  updateLabel("", mBlockName, QIcon(), mBlockSeparator);
  setUpdatesEnabled(true);
  scheduleApplySize();
}

void BreadcrumbWidget::setTab(const QString& tab, Types::LibraryTypes type)
{
  setUpdatesEnabled(false);
  updateLabel(type == Types::LibraryTypes::STRUCTURAL ? "" : tab, mTabName, libraryTypeToIcon(type, false), mTabSeparator);
  updateLabel("", mBlockName, QIcon(), mBlockSeparator);
  applySize();
  setUpdatesEnabled(true);
  update();
}

void BreadcrumbWidget::setBlock(const QString& block, Types::LibraryTypes type)
{
  setUpdatesEnabled(false);
  updateLabel(block, mBlockName, libraryTypeToIcon(type, true), mBlockSeparator);
  applySize();
  setUpdatesEnabled(true);
  update();
}

void BreadcrumbWidget::updateLabel(const QString& text, BreadcrumbLabel* label, const QIcon& labelIcon, oclero::qlementine::IconWidget* icon)
{
  label->setText(text);
  label->setIcon(labelIcon);

  if (text.isEmpty())
  {
    label->hide();
    if (icon)
      icon->hide();
  }
  else
  {
    label->show();
    if (icon)
      icon->show();
  }
}

void BreadcrumbWidget::setIconOptions(oclero::qlementine::IconWidget* icon)
{
  icon->setIconSize({10, 10});
  icon->hide();
}

QIcon BreadcrumbWidget::libraryTypeToIcon(Types::LibraryTypes type, bool isBlock) const
{
  if (type == Types::LibraryTypes::STRUCTURAL)
    return isBlock ? QIcon(":/icons/capability.svg") : QIcon(":/icons/structure.svg");
  else if (type == Types::LibraryTypes::BEHAVIOUR)
    return isBlock ? QIcon(":/icons/flow_block.svg") : QIcon(":/icons/behaviour.svg");
  else if (type == Types::LibraryTypes::PIPELINE)
    return isBlock ? QIcon(":/icons/pipeline.svg") : QIcon(":/icons/pipeline.svg");

  return QIcon();
}

bool BreadcrumbWidget::eventFilter(QObject* watched, QEvent* event)
{
  if (watched == parentWidget())
  {
    switch (event->type())
    {
      case QEvent::Resize:
      case QEvent::Move:
        reposition();
        break;
      case QEvent::Show:
      case QEvent::Hide:
      case QEvent::LayoutRequest:
      default:
        break;
    }
  }

  return QObject::eventFilter(watched, event);
}
