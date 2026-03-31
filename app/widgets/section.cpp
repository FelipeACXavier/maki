#include "section.h"

#include <QLabel>

#include "app_configs.h"

SectionWidget::SectionWidget(QWidget* parent)
    : QWidget(parent)

{
  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);
}

void SectionWidget::addItem(QWidget* container, const QString& title)
{
  QWidget* header = new QWidget(this);

  mContent = container;
  mContent->setParent(this);

  QLabel* label = new QLabel(title, header);
  label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  label->setFont(Fonts::Label);
  auto fm = label->fontMetrics();

  mToggleButton = new QToolButton(header);
  mToggleButton->setCheckable(true);
  mToggleButton->setChecked(true);
  mToggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  mToggleButton->setArrowType(Qt::DownArrow);
  mToggleButton->setFixedWidth(30);

  QHBoxLayout* hLayout = new QHBoxLayout(header);
  hLayout->setContentsMargins(0, 0, 0, 0);
  hLayout->setSpacing(0);

  hLayout->addWidget(mToggleButton);
  hLayout->addWidget(label);

  layout()->addWidget(header);
  layout()->addWidget(mContent);

  connect(mToggleButton, &QToolButton::toggled, this, [this](bool checked) {
    mContent->setVisible(checked);
    mToggleButton->setArrowType(checked ? Qt::DownArrow : Qt::RightArrow);
  });
}

void SectionWidget::setExpanded(bool expanded)
{
  mToggleButton->setChecked(expanded);
}

bool SectionWidget::isExpanded() const
{
  return mToggleButton->isChecked();
}
