#include "widget_factory.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>

#include "app_configs.h"
#include "style_helpers.h"
#include "theme.h"

static const int WIDGET_SPACING = 2;

namespace maki
{

BooleanWidget::BooleanWidget(const QString& label, bool value, QWidget* parent)
    : QWidget(parent)
{
  auto* hlayout = new QHBoxLayout();
  setLayout(hlayout);

  layout()->setSpacing(WIDGET_SPACING);
  layout()->setAlignment(Qt::AlignLeft);

  auto checkBox = new QCheckBox(label, this);
  checkBox->setChecked(value);
  checkBox->setFont(Fonts::Label);
  checkBox->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

  connect(checkBox, &QCheckBox::checkStateChanged, this, [this](Qt::CheckState state) {
    emit valueChanged(state == Qt::CheckState::Checked);
  });

  layout()->addWidget(checkBox);
  hlayout->addStretch();
}

void maki::BooleanWidget::addDescription(const QString& label)
{
  auto* hint = new QLabel(label, this);
  hint->setFont(Fonts::Hint);
  hint->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
  layout()->addWidget(hint);
}

IntegerWidget::IntegerWidget(const QString& label, const QString& placeholder, QWidget* parent, int min, int max)
    : QWidget(parent)
{
  auto* colLayout = new QVBoxLayout();
  setLayout(colLayout);
  layout()->setSpacing(WIDGET_SPACING);

  auto* labelWidget = new QLabel(label, this);
  labelWidget->setFont(Fonts::Label);

  mInputField = new QLineEdit(this);
  mInputField->setPlaceholderText(placeholder);

  QIntValidator* validator = new QIntValidator(min, max, this);
  mInputField->setValidator(validator);

  connect(mInputField, &QLineEdit::textEdited, this, [this](const QString& text) {
    int pos = 0;
    QString t = text;
    QValidator::State state = mInputField->validator()->validate(t, pos);
    updateProperty(mInputField, Config::INVALID, (state != QValidator::Acceptable));
  });
  connect(mInputField, &QLineEdit::editingFinished, this, [this]() {
    emit valueChanged(mInputField->text().toInt());
  });

  layout()->addWidget(labelWidget);
  layout()->addWidget(mInputField);
}

void IntegerWidget::addDescription(const QString& label)
{
  auto* hint = new QLabel(label, this);
  hint->setFont(Fonts::Hint);
  layout()->addWidget(hint);
}

}  // namespace maki