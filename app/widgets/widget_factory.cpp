#include "widget_factory.h"

#include <qboxlayout.h>
#include <qnamespace.h>
#include <qpushbutton.h>

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDoubleValidator>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QShortcut>
#include <QSpinBox>

#include "app_configs.h"
#include "style_helpers.h"
#include "theme.h"

static const int WIDGET_SPACING = 2;
static const int WIDGET_PADDING = 24;

namespace maki
{

// =========================================================================================================
WidgetGroup::WidgetGroup(const QString& label, QWidget* parent)
    : QWidget(parent)
{
  auto* vLayout = new QVBoxLayout(this);
  vLayout->setContentsMargins(0, 0, 0, 0);
  vLayout->setSpacing(WIDGET_SPACING);
  vLayout->setAlignment(Qt::AlignLeft);

  auto* title = new QLabel(label, this);
  title->setFont(Fonts::Label);
  title->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

  auto* line = new QFrame(this);
  line->setFrameShape(QFrame::HLine);
  // line->setFrameShadow(QFrame::Sunken);

  vLayout->addWidget(title);
  vLayout->addWidget(line);
  vLayout->addSpacing(2 * WIDGET_SPACING);
}

void WidgetGroup::addWidget(QWidget* widget)
{
  auto* hlayout = new QHBoxLayout();
  hlayout->setContentsMargins(WIDGET_PADDING, 0, 0, 0);
  hlayout->addWidget(widget);

  static_cast<QVBoxLayout*>(layout())->addLayout(hlayout);
}

// =========================================================================================================
BooleanWidget::BooleanWidget(const QString& label, bool value, WidgetAlignment alignment, QWidget* parent)
    : QWidget(parent)
{
  mInputField = new QCheckBox(this);
  mInputField->setCheckState(value ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
  connect(mInputField, &QCheckBox::checkStateChanged, this, [this](Qt::CheckState state) {
    mValue = (state == Qt::CheckState::Checked);
    emit valueChanged(mValue);
  });

  mValue = value;

  if (alignment.type == WidgetAlignment::Type::FORM)
  {
    alignment.layout->addRow("&" + label, mInputField);
  }
  else
  {
    auto* vlayout = new QVBoxLayout(this);
    vlayout->setContentsMargins(0, 0, 0, WIDGET_SPACING);
    vlayout->setSpacing(WIDGET_SPACING);

    auto* hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(0, 0, WIDGET_SPACING, 0);
    hlayout->setSpacing(WIDGET_SPACING);
    hlayout->setAlignment(Qt::AlignLeft);

    auto* title = new QLabel(label, this);
    title->setFont(Fonts::Main);
    title->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

    hlayout->addWidget(title);
    hlayout->addStretch();
    hlayout->addWidget(mInputField);
    vlayout->addLayout(hlayout);
  }
}

void maki::BooleanWidget::setValue(const bool value)
{
  mValue = value;
  mInputField->setCheckState(value ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
}

bool maki::BooleanWidget::getValue() const
{
  return mValue;
}

void maki::BooleanWidget::addDescription(const QString& label)
{
  auto* hint = new QLabel(label, this);
  hint->setFont(Fonts::Hint);
  hint->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
  layout()->addWidget(hint);
}

// =========================================================================================================
StringWidget::StringWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent)
{
  mInputField = new QLineEdit(this);
  mInputField->setPlaceholderText(placeholder);

  mValue = placeholder;

  connect(mInputField, &QLineEdit::editingFinished, this, [this]() {
    mValue = mInputField->text();
    emit valueChanged(mValue);
  });

  if (alignment.type == WidgetAlignment::Type::FORM)
  {
    alignment.layout->addRow("&" + label, mInputField);
  }
  else
  {
    if (alignment.type == WidgetAlignment::Type::INLINE)
    {
      auto* hLayout = new QHBoxLayout(this);
      hLayout->setContentsMargins(0, 0, 0, WIDGET_SPACING);
      hLayout->setSpacing(WIDGET_SPACING);
    }
    else if (alignment.type == WidgetAlignment::Type::VERTICAL)
    {
      auto* vLayout = new QVBoxLayout(this);
      vLayout->setContentsMargins(0, 0, 0, WIDGET_SPACING);
      vLayout->setSpacing(WIDGET_SPACING);
    }

    auto* labelWidget = new QLabel(label, this);
    labelWidget->setFont(Fonts::Main);

    layout()->addWidget(labelWidget);
    layout()->addWidget(mInputField);
  }
}

QLineEdit* StringWidget::widget() const
{
  return mInputField;
}

void StringWidget::addDescription(const QString& label)
{
  auto* hint = new QLabel(label, this);
  hint->setFont(Fonts::Hint);
  layout()->addWidget(hint);
}

QString StringWidget::getValue() const
{
  return mValue;
}

void StringWidget::setValue(const QString& value)
{
  mValue = value;
  mInputField->setText(value);
}

// ========================================================================================================================================
IntegerWidget::IntegerWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent, int min, int max)
    : QWidget(parent)
{
  mInputField = new QLineEdit(this);
  mInputField->setPlaceholderText(placeholder);

  bool valid = false;
  auto intPlaceholder = placeholder.toInt(&valid);
  if (valid)
    mValue = intPlaceholder;

  QIntValidator* validator = new QIntValidator(min, max, this);
  mInputField->setValidator(validator);

  connect(mInputField, &QLineEdit::textEdited, this, [this](const QString& text) {
    int pos = 0;
    QString t = text;
    QValidator::State state = mInputField->validator()->validate(t, pos);
    updateProperty(mInputField, Config::INVALID, (state != QValidator::Acceptable));
  });
  connect(mInputField, &QLineEdit::editingFinished, this, [this]() {
    mValue = mInputField->text().toInt();
    emit valueChanged(mValue);
  });

  if (alignment.type == WidgetAlignment::Type::FORM)
  {
    alignment.layout->addRow("&" + label, mInputField);
  }
  else
  {
    if (alignment.type == WidgetAlignment::Type::INLINE)
    {
      auto* hLayout = new QHBoxLayout(this);
      hLayout->setContentsMargins(0, 0, 0, WIDGET_SPACING);
      hLayout->setSpacing(WIDGET_SPACING);
    }
    else if (alignment.type == WidgetAlignment::Type::VERTICAL)
    {
      auto* vLayout = new QVBoxLayout(this);
      vLayout->setContentsMargins(0, 0, 0, WIDGET_SPACING);
      vLayout->setSpacing(WIDGET_SPACING);
    }

    auto* labelWidget = new QLabel(label, this);
    labelWidget->setFont(Fonts::Main);

    layout()->addWidget(labelWidget);
    layout()->addWidget(mInputField);
  }
}

void maki::IntegerWidget::setValue(const int value)
{
  mValue = value;
  mInputField->setText(QString("%1").arg(mValue));
}

int IntegerWidget::getValue() const
{
  return mValue;
}

void IntegerWidget::addDescription(const QString& label)
{
  auto* hint = new QLabel(label, this);
  hint->setFont(Fonts::Hint);
  layout()->addWidget(hint);
}

// ========================================================================================================================================
FloatWidget::FloatWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent, qreal min, qreal max)
    : QWidget(parent)
{
  mInputField = new QLineEdit(this);
  mInputField->setPlaceholderText(placeholder);

  bool valid = false;
  auto intPlaceholder = placeholder.toInt(&valid);
  if (valid)
    mValue = intPlaceholder;

  QDoubleValidator* validator = new QDoubleValidator(min, max, 6, this);
  validator->setNotation(QDoubleValidator::StandardNotation);
  validator->setLocale(QLocale::C);  // Use dot instead of comma

  mInputField->setValidator(validator);

  connect(mInputField, &QLineEdit::textEdited, this, [this](const QString& text) {
    int pos = 0;
    QString t = text;
    QValidator::State state = mInputField->validator()->validate(t, pos);
    updateProperty(mInputField, Config::INVALID, (state != QValidator::Acceptable));
  });
  connect(mInputField, &QLineEdit::editingFinished, this, [this]() {
    mValue = mInputField->text().toDouble();
    emit valueChanged(mValue);
  });

  if (alignment.type == WidgetAlignment::Type::FORM)
  {
    alignment.layout->addRow("&" + label, mInputField);
  }
  else
  {
    if (alignment.type == WidgetAlignment::Type::INLINE)
    {
      auto* hLayout = new QHBoxLayout(this);
      hLayout->setContentsMargins(0, 0, 0, WIDGET_SPACING);
      hLayout->setSpacing(WIDGET_SPACING);
    }
    else if (alignment.type == WidgetAlignment::Type::VERTICAL)
    {
      auto* vLayout = new QVBoxLayout(this);
      vLayout->setContentsMargins(0, 0, 0, WIDGET_SPACING);
      vLayout->setSpacing(WIDGET_SPACING);
    }

    auto* labelWidget = new QLabel(label, this);
    labelWidget->setFont(Fonts::Main);

    layout()->addWidget(labelWidget);
    layout()->addWidget(mInputField);
  }
}

void maki::FloatWidget::setValue(const qreal value)
{
  mValue = value;
  mInputField->setText(QString("%1").arg(mValue));
}

qreal FloatWidget::getValue() const
{
  return mValue;
}

void FloatWidget::addDescription(const QString& label)
{
  auto* hint = new QLabel(label, this);
  hint->setFont(Fonts::Hint);
  layout()->addWidget(hint);
}

// ========================================================================================================================================
SpinWidget::SpinWidget(const QString& label, int placeholder, QWidget* parent, int min, int max)
    : QWidget(parent)
{
  auto* vlayout = new QVBoxLayout(this);
  vlayout->setContentsMargins(0, 0, 0, WIDGET_SPACING);
  vlayout->setSpacing(WIDGET_SPACING);

  auto* hlayout = new QHBoxLayout();
  hlayout->setContentsMargins(0, 0, 0, 0);
  hlayout->setSpacing(WIDGET_SPACING);

  auto* labelWidget = new QLabel(label, this);
  labelWidget->setFont(Fonts::Main);

  mInputField = new QSpinBox(this);
  mInputField->setRange(min, max);
  mInputField->setAlignment(Qt::AlignRight);

  mValue = placeholder;
  mInputField->setValue(mValue);

  connect(mInputField, &QSpinBox::valueChanged, this, [this](int value) {
    mValue = value;
    emit valueChanged(mValue);
  });

  hlayout->addWidget(labelWidget);
  hlayout->addStretch();
  hlayout->addWidget(mInputField);

  vlayout->addLayout(hlayout);
}

void SpinWidget::setValue(const int value)
{
  mValue = value;
  mInputField->setValue(mValue);
}

int SpinWidget::getValue() const
{
  return mValue;
}

void SpinWidget::setSuffix(const QString& suffix)
{
  mInputField->setSuffix(suffix);
}

void SpinWidget::addDescription(const QString& label)
{
  auto* hint = new QLabel(label, this);
  hint->setFont(Fonts::Hint);
  layout()->addWidget(hint);
}

// ========================================================================================================================================
SelectorWidget::SelectorWidget(const QString& label, QComboBox* comboBox, QWidget* parent)
{
  auto* vlayout = new QVBoxLayout(this);
  vlayout->setContentsMargins(0, 0, 0, WIDGET_SPACING);
  vlayout->setSpacing(WIDGET_SPACING);

  auto* hlayout = new QHBoxLayout();
  hlayout->setContentsMargins(0, 0, 0, 0);
  hlayout->setSpacing(WIDGET_SPACING);

  auto* labelWidget = new QLabel(label, this);
  labelWidget->setFont(Fonts::Main);

  if (comboBox)
    mInputField = comboBox;
  else
    mInputField = new QComboBox(this);

  connect(mInputField, &QComboBox::currentTextChanged, this, [this](const QString& value) {
    mValue = value;
    emit valueChanged(mValue);
  });

  mValue = mInputField->currentText();

  hlayout->addWidget(labelWidget);
  hlayout->addStretch();
  hlayout->addWidget(mInputField);

  vlayout->addLayout(hlayout);
}

SelectorWidget::SelectorWidget(const QString& label, QWidget* parent)
    : SelectorWidget(label, nullptr, parent)
{
}

void SelectorWidget::addDescription(const QString& label)
{
  auto* hint = new QLabel(label, this);
  hint->setFont(Fonts::Hint);
  layout()->addWidget(hint);
}

void SelectorWidget::setValue(const QString& data)
{
  int index = mInputField->findData(data);
  if (index >= 0)
    mInputField->setCurrentIndex(index);
}

QString SelectorWidget::getValue() const
{
  return mValue;
}

void SelectorWidget::addItem(const QString& name, const QString& value)
{
  mInputField->addItem(name, value);
}

// =========================================================================================================
ButtonWidget::ButtonWidget(const QString& label, QWidget* parent)
{
  auto* vLayout = new QVBoxLayout(this);
  vLayout->setContentsMargins(0, 0, 0, WIDGET_SPACING);
  vLayout->setSpacing(WIDGET_SPACING);

  mInputField = new QPushButton(this);
  mInputField->setText(label);

  connect(mInputField, &QPushButton::clicked, this, [this]() {
    emit valueChanged();
  });

  layout()->addWidget(mInputField);
}

void ButtonWidget::setIcon(const QIcon& icon)
{
  mInputField->setIcon(icon);
}

void ButtonWidget::setToolTip(const QString& tooltip)
{
  mInputField->setToolTip(tooltip);
}

void ButtonWidget::addDescription(const QString& label)
{
  auto* hint = new QLabel(label, this);
  hint->setFont(Fonts::Hint);
  layout()->addWidget(hint);
}

// =========================================================================================================
ColorWidget::ColorWidget(const QString& label, const QString& placeholder, QWidget* parent)
    : QWidget(parent)
{
  auto* vlayout = new QVBoxLayout(this);
  vlayout->setContentsMargins(0, 0, 0, WIDGET_SPACING);
  vlayout->setSpacing(WIDGET_SPACING);

  auto* hlayout = new QHBoxLayout();
  hlayout->setContentsMargins(0, 0, 0, 0);
  hlayout->setSpacing(WIDGET_SPACING);

  auto* labelWidget = new QLabel(label, this);
  labelWidget->setFont(Fonts::Main);

  mValue = QColor::fromString(placeholder);

  mPreview = new QLabel(this);
  mPreview->setFixedSize({16, 16});
  mPreview->setObjectName("PropertyColorPreview");

  applyStyle(mPreview, QStringLiteral(
                           "QLabel#PropertyColorPreview { background-color: %1; }")
                           .arg(placeholder));

  mButton = new QPushButton(this);
  connect(mButton, &QPushButton::pressed, [this, label]() {
    QColor color = QColorDialog::getColor(mValue, this, label);
    if (!color.isValid())
      return;

    applyStyle(mPreview, QStringLiteral(
                             "QLabel#PropertyColorPreview { background-color: %1; }")
                             .arg(color.name()));

    mValue = color;
    mPreview->update();
    emit valueChanged(color);
  });

  hlayout->addWidget(labelWidget);
  hlayout->addStretch();
  hlayout->addWidget(mPreview);
  hlayout->addWidget(mButton);

  vlayout->addLayout(hlayout);
}

QPushButton* ColorWidget::widget() const
{
  return mButton;
}

void ColorWidget::addDescription(const QString& label)
{
  auto* hint = new QLabel(label, this);
  hint->setFont(Fonts::Hint);
  layout()->addWidget(hint);
}

QColor ColorWidget::getValue() const
{
  return mValue;
}

// =========================================================================================================
TypeSelectionWidget::TypeSelectionWidget(QWidget* parent)
    : TypeSelectionWidget("", Types::PropertyTypes::VOID, parent)
{
}

TypeSelectionWidget::TypeSelectionWidget(const QString& initial, QWidget* parent)
    : TypeSelectionWidget(initial, Types::PropertyTypes::VOID, parent)
{
}

TypeSelectionWidget::TypeSelectionWidget(const QString& initial, Types::PropertyTypes last, QWidget* parent)
    : QComboBox(parent)
{
  uint16_t start = (uint16_t)Types::PropertyTypes::UNKNOWN + 1;
  uint16_t end = (uint16_t)last + 1;

  for (uint16_t i = start; i < end; ++i)
  {
    auto id = Types::PropertyTypesToString((Types::PropertyTypes)i);
    addItem(id, id);
  }

  if (initial.isEmpty())
    return;

  int index = findData(initial);
  if (index >= 0)
    setCurrentIndex(index);
}

// =========================================================================================================
SearchWidget::SearchWidget(const QString& placeholder, QWidget* parent)
{
  setMaximumHeight(40);

  auto* searchLayout = new QHBoxLayout(this);
  searchLayout->setContentsMargins(0, 0, 0, 0);
  searchLayout->setSpacing(0);

  mIcon = new QLabel(this);
  mIcon->setAlignment(Qt::AlignCenter);
  mIcon->setFixedSize(16, 16);

  mInputField = new QLineEdit(this);
  mInputField->setPlaceholderText(placeholder);
  mInputField->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
  mInputField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  mValue = placeholder;

  connect(mInputField, &QLineEdit::textChanged, this, [this]() {
    mValue = mInputField->text();
    emit valueChanged(mValue);
  });

  auto* esc = new QShortcut(QKeySequence(Qt::Key_Escape), this);
  connect(esc, &QShortcut::activated, this, [this]() { emit dismissed(); });

  layout()->addWidget(mIcon);
  layout()->addWidget(mInputField);
}

WidgetWithIcon SearchWidget::icon() const
{
  return {mIcon, ":/icons/magnifying-glass.svg"};
}

QLineEdit* SearchWidget::widget() const
{
  return mInputField;
}

void SearchWidget::addDescription(const QString& label)
{
  auto* hint = new QLabel(label, this);
  hint->setFont(Fonts::Hint);
  layout()->addWidget(hint);
}

QString SearchWidget::getValue() const
{
  return mValue;
}

void SearchWidget::setValue(const QString& value)
{
  mValue = value;
  mInputField->setText(value);
}

}  // namespace maki
