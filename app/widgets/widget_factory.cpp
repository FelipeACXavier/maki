#include "widget_factory.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QDoubleValidator>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLineEdit>
#include <QPushButton>
#include <QShortcut>
#include <QSpinBox>
#include <oclero/qlementine.hpp>
#include <oclero/qlementine/widgets/ColorEditor.hpp>
#include <oclero/qlementine/widgets/IconWidget.hpp>
#include <oclero/qlementine/widgets/Label.hpp>
#include <oclero/qlementine/widgets/LineEdit.hpp>

#include "app_configs.h"
#include "oclero/qlementine/Common.hpp"
#include "style_helpers.h"
#include "validators/double_variable.h"
#include "validators/int_variable.h"

static const int WIDGET_SPACING = 2;
static const int WIDGET_PADDING = 24;

namespace maki
{

// =========================================================================================================
GridGroup::GridGroup(const QString& label, int rows, int cols, QWidget* parent)
    : QWidget(parent)
    , mRows(rows)
    , mCols(cols)
    , mCurrentRow(0)
    , mCurrentCol(0)
{
  auto* vLayout = new QVBoxLayout(this);
  vLayout->setContentsMargins(0, 0, 0, 0);
  vLayout->setSpacing(WIDGET_SPACING);
  vLayout->setAlignment(Qt::AlignLeft);

  auto* title = new oclero::qlementine::Label(label, this);
  title->setRole(oclero::qlementine::TextRole::Default);
  title->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

  auto* line = new QFrame(this);
  line->setFrameShape(QFrame::HLine);
  // line->setFrameShadow(QFrame::Sunken);

  mContent = new QWidget(this);
  auto* gridLayout = new QGridLayout(mContent);
  gridLayout->setContentsMargins(Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING);
  gridLayout->setSpacing(Config::CONTENT_PADDING);

  vLayout->addWidget(title);
  vLayout->addWidget(line);
  vLayout->addSpacing(2 * WIDGET_SPACING);
  vLayout->addWidget(mContent);
}

QWidget* GridGroup::widget() const
{
  return mContent;
}

void GridGroup::addWidget(QWidget* widget)
{
  if (mCurrentRow >= mRows)
  {
    LOG_WARNING("Grid full");
    return;
  }

  qobject_cast<QGridLayout*>(mContent->layout())->addWidget(widget, mCurrentRow, mCurrentCol++);
  if (mCurrentCol == mCols)
  {
    mCurrentCol = 0;
    mCurrentRow++;
  }
}

void GridGroup::addLayout(QLayout* layout)
{
  if (mCurrentRow >= mRows)
  {
    LOG_WARNING("Grid full");
    return;
  }

  qobject_cast<QGridLayout*>(mContent->layout())->addLayout(layout, mCurrentRow, mCurrentCol++);
  if (mCurrentCol == mCols)
  {
    mCurrentCol = 0;
    mCurrentRow++;
  }
}

// =========================================================================================================
WidgetGroup::WidgetGroup(const QString& label, QWidget* parent)
    : WidgetGroup(label, oclero::qlementine::TextRole::H4, parent)
{
}

WidgetGroup::WidgetGroup(const QString& label, oclero::qlementine::TextRole role, QWidget* parent)
    : QWidget(parent)
{
  auto* vLayout = new QVBoxLayout(this);
  vLayout->setContentsMargins(0, 0, 0, 0);
  vLayout->setSpacing(WIDGET_SPACING);
  vLayout->setAlignment(Qt::AlignTop);

  auto* title = new oclero::qlementine::Label(label, this);
  title->setRole(role);
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

  qobject_cast<QVBoxLayout*>(this->layout())->addLayout(hlayout);
}

void WidgetGroup::addLayout(QLayout* layout)
{
  qobject_cast<QVBoxLayout*>(this->layout())->addLayout(layout);
}

void WidgetGroup::addSpacing(int spacing)
{
  qobject_cast<QVBoxLayout*>(this->layout())->addSpacing(spacing);
}

void WidgetGroup::addStretch()
{
  qobject_cast<QVBoxLayout*>(this->layout())->addStretch();
}

InputWidget::InputWidget(const QString& label, QWidget* inputField, WidgetAlignment alignment, QWidget* parent)
    : QWidget(parent)
    , mInputField(inputField)
{
  auto* labelWidget = new oclero::qlementine::Label(label, this);
  labelWidget->setRole(oclero::qlementine::TextRole::Default);
  labelWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

  auto* qlementineStyle = oclero::qlementine::appStyle();
  if (qlementineStyle)
  {
    auto metric = labelWidget->fontMetrics();
    const auto theme = qlementineStyle->theme();
    labelWidget->setMinimumWidth(metric.horizontalAdvance(label) + theme.spacing);
  }

  mInputField->setParent(this);

  if (alignment.type == WidgetAlignment::Type::FORM)
  {
    if (alignment.labelWidth > 0)
      labelWidget->setFixedWidth(alignment.labelWidth);

    alignment.group->addWidget(createLayout(labelWidget, alignment));
  }
  else
  {
    auto* vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, WIDGET_SPACING);
    vLayout->setSpacing(WIDGET_SPACING);

    if (alignment.type == WidgetAlignment::Type::INLINE)
    {
      if (alignment.labelWidth > 0)
        labelWidget->setFixedWidth(alignment.labelWidth);

      layout()->addWidget(createLayout(labelWidget, alignment));
    }
    else
    {
      layout()->addWidget(labelWidget);
      layout()->addWidget(mInputField);
    }
  }
}

void InputWidget::addDescription(const QString& text)
{
  auto* description = new oclero::qlementine::Label(text, this);
  description->setRole(oclero::qlementine::TextRole::Caption);

  if (auto* vLayout = qobject_cast<QVBoxLayout*>(layout()))
    vLayout->addWidget(description);
}

QWidget* InputWidget::createLayout(oclero::qlementine::Label* labelWidget, WidgetAlignment alignment)
{
  auto* container = new QWidget(this);
  auto* hLayout = new QHBoxLayout(container);
  hLayout->setContentsMargins(0, 0, 0, 0);
  hLayout->setSpacing(WIDGET_SPACING);

  if (alignment.direction == WidgetAlignment::Direction::RIGHT ||
      alignment.direction == WidgetAlignment::Direction::CENTER)
    hLayout->addStretch();

  hLayout->addWidget(labelWidget);

  if (alignment.direction == WidgetAlignment::Direction::SPREAD)
    hLayout->addStretch();

  hLayout->addWidget(mInputField);

  if (alignment.direction == WidgetAlignment::Direction::LEFT ||
      alignment.direction == WidgetAlignment::Direction::CENTER)
    hLayout->addStretch();

  return container;
}

void InputWidget::setToolTip(const QString& text)
{
  if (!mInputField)
    return;

  mInputField->setToolTip(text);
  mInputField->setToolTipDuration(Constants::TOOLTIP_DURATION);
}

// =========================================================================================================
BooleanWidget::BooleanWidget(const QString& label, bool value, WidgetAlignment alignment, QWidget* parent)
    : TypedInputWidget<bool, QCheckBox>(label, new QCheckBox(), value, alignment, parent)
{
  if (auto* combo = qobject_cast<QCheckBox*>(mInputField))
  {
    combo->setCheckState(value ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    connect(combo, &QCheckBox::checkStateChanged, this, [this](Qt::CheckState state) {
      mValue = (state == Qt::CheckState::Checked);
      emit valueChanged(mValue);
    });
  }

  setValue(value);
}

void maki::BooleanWidget::writeValueToWidget(const bool& value)
{
  mValue = value;
  if (auto* combo = qobject_cast<QCheckBox*>(mInputField))
    combo->setCheckState(value ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
}

// =========================================================================================================
StringWidget::StringWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent)
    : TypedInputWidget<QString, oclero::qlementine::LineEdit>(label, new oclero::qlementine::LineEdit(), placeholder, alignment, parent)
{
  if (auto* edit = qobject_cast<oclero::qlementine::LineEdit*>(mInputField))
  {
    connect(edit, &oclero::qlementine::LineEdit::editingFinished, this, [this, edit]() {
      mValue = edit->text();
      emit valueChanged(mValue);
    });
  }

  setValue(placeholder);
}

void maki::StringWidget::writeValueToWidget(const QString& value)
{
  mValue = value;
  if (auto* edit = qobject_cast<QLineEdit*>(mInputField))
    edit->setText(value);
}

// ========================================================================================================================================
IntegerWidget::IntegerWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent, int min, int max)
    : TypedInputWidget<QString, oclero::qlementine::LineEdit>(label, new oclero::qlementine::LineEdit(), placeholder, alignment, parent)
{
  if (auto* edit = qobject_cast<oclero::qlementine::LineEdit*>(mInputField))
  {
    // Set tooltip
    setToolTip(QString(tr("Input must be an integer value")));

    mValidator = new IntegerOrVariableValidator(min, max, this);
    edit->setValidator(mValidator);

    // Connect the signals
    connect(edit, &QLineEdit::inputRejected, this, [edit]() {
      edit->setStatus(oclero::qlementine::Status::Warning);
    });
    connect(edit, &QLineEdit::editingFinished, this, [this, edit]() {
      if (!edit->hasAcceptableInput())
        return;

      mValue = edit->text();
      edit->setStatus(oclero::qlementine::Status::Default);
      emit valueChanged(mValue);
    });
  }

  setValue(placeholder);
}

void IntegerWidget::setAcceptVariable(bool accept)
{
  if (mValidator)
    mValidator->setAcceptVariable(accept);
}

void IntegerWidget::setValue(int value)
{
  TypedInputWidget::setValue(QString::number(value));
}

int IntegerWidget::getValue() const
{
  bool ok = false;
  int value = mValue.toInt(&ok);
  if (ok)
    return value;

  assert(false && "Tried to get an integer from a field that does not contain an integer");
}

void IntegerWidget::writeValueToWidget(const QString& value)
{
  mValue = value;
  if (auto* edit = qobject_cast<QLineEdit*>(mInputField))
    edit->setText(value);
}

// ========================================================================================================================================
FloatWidget::FloatWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent, qreal min, qreal max)
    : TypedInputWidget<QString, oclero::qlementine::LineEdit>(label, new oclero::qlementine::LineEdit(), placeholder, alignment, parent)
{
  if (auto* edit = qobject_cast<oclero::qlementine::LineEdit*>(mInputField))
  {
    // Set tooltip
    setToolTip(QString(tr("Input must be a real value")));

    mValidator = new DoubleOrVariableValidator(min, max, 6, this);
    mValidator->setNotation(QDoubleValidator::StandardNotation);
    mValidator->setLocale(QLocale::C);  // Use dot instead of comma
    edit->setValidator(mValidator);

    // Connect the signals
    connect(edit, &QLineEdit::inputRejected, this, [edit]() {
      edit->setStatus(oclero::qlementine::Status::Warning);
    });
    connect(edit, &QLineEdit::editingFinished, this, [this, edit]() {
      if (!edit->hasAcceptableInput())
        return;

      mValue = edit->text();
      edit->setStatus(oclero::qlementine::Status::Default);
      emit valueChanged(mValue);
    });
  }

  setValue(placeholder);
}

void FloatWidget::setAcceptVariable(bool accept)
{
  if (mValidator)
    mValidator->setAcceptVariable(accept);
}

void FloatWidget::setValue(qreal value)
{
  TypedInputWidget::setValue(QString::number(value));
}

qreal FloatWidget::getValue() const
{
  bool ok = false;
  int value = mValue.toDouble(&ok);
  if (ok)
    return value;

  assert(false && "Tried to get a real from a field that does not contain a real");
}

void FloatWidget::writeValueToWidget(const QString& value)
{
  mValue = value;
  if (auto* edit = qobject_cast<QLineEdit*>(mInputField))
    edit->setText(value);
}

// ========================================================================================================================================
SpinWidget::SpinWidget(const QString& label, int placeholder, QWidget* parent, WidgetAlignment alignment, int min, int max)
    : TypedInputWidget<int, QSpinBox>(label, new QSpinBox(), placeholder, alignment, parent)
{
  if (auto* edit = qobject_cast<QSpinBox*>(mInputField))
  {
    edit->setRange(min, max);
    edit->setAlignment(Qt::AlignRight);
    edit->setValue(mValue);

    connect(edit, &QSpinBox::valueChanged, this, [this](int value) {
      mValue = value;
      emit valueChanged(mValue);
    });
  }

  setValue(placeholder);
}

void SpinWidget::setSuffix(const QString& suffix)
{
  if (auto* edit = qobject_cast<QSpinBox*>(mInputField))
    edit->setSuffix(suffix);
}

void SpinWidget::writeValueToWidget(const int& value)
{
  mValue = value;
  if (auto* edit = qobject_cast<QSpinBox*>(mInputField))
    edit->setValue(mValue);
}

// ========================================================================================================================================
SelectorWidget::SelectorWidget(const QString& label, WidgetAlignment alignment, QWidget* parent)
    : SelectorWidget(label, new QComboBox(), alignment, parent)
{
}

SelectorWidget::SelectorWidget(const QString& label, QComboBox* comboBox, WidgetAlignment alignment, QWidget* parent)
    : TypedInputWidget<QString, QComboBox>(label, comboBox, Constants::EMPTY_COMBO, alignment, parent)
{
  if (auto* combo = qobject_cast<QComboBox*>(mInputField))
  {
    setValue(Constants::EMPTY_COMBO);
    connect(combo, &QComboBox::currentTextChanged, this, [this, combo](const QString& value) {
      mValue = value;
      mData = combo->currentData();
      emit valueChanged(mValue);
      emit dataChanged(mValue, mData);
    });
  }
}

void SelectorWidget::writeValueToWidget(const QString& text)
{
  if (auto* combo = qobject_cast<QComboBox*>(mInputField))
  {
    if (text == Constants::EMPTY_COMBO)
    {
      combo->setPlaceholderText(Constants::EMPTY_COMBO);
      combo->setCurrentIndex(-1);
      // Clear values
      mValue = Constants::EMPTY_COMBO;
      mData = QVariant();
      return;
    }

    int index = combo->findText(text);
    if (index >= 0)
    {
      combo->setCurrentIndex(index);
      mValue = combo->currentText();
      mData = combo->currentData();
    }
  }
}

void SelectorWidget::setData(const QString& data)
{
  if (auto* combo = qobject_cast<QComboBox*>(mInputField))
  {
    int index = combo->findData(data);
    if (index >= 0)
    {
      combo->setCurrentIndex(index);
      mValue = combo->currentText();
      mData = combo->currentData();
    }
  }
}

QVariant SelectorWidget::getData() const
{
  return mData;
}

void SelectorWidget::addItem(const QString& name, const QVariant& value)
{
  if (auto* combo = qobject_cast<QComboBox*>(mInputField))
  {
    int index = combo->findData(name);
    if (index < 0)
      combo->addItem(name, value);
  }
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
  auto* hint = new oclero::qlementine::Label(label, this);
  hint->setRole(oclero::qlementine::TextRole::Caption);
  layout()->addWidget(hint);
}

// =========================================================================================================
ColorWidget::ColorWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent)
    : TypedInputWidget<QColor, oclero::qlementine::ColorEditor>(label, new oclero::qlementine::ColorEditor(), QColor::fromString(placeholder), alignment, parent)
{
  if (auto* edit = qobject_cast<oclero::qlementine::ColorEditor*>(mInputField))
  {
    edit->setColor(QColor::fromString(placeholder));
    connect(edit, &oclero::qlementine::ColorEditor::colorChanged, this, [this, edit]() {
      mValue = edit->color();
      emit valueChanged(mValue);
    });
  }

  setValue(placeholder);
}

void ColorWidget::writeValueToWidget(const QColor& color)
{
  mValue = color;
  if (auto* edit = qobject_cast<oclero::qlementine::ColorEditor*>(mInputField))
    edit->setColor(color);
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
  searchLayout->setContentsMargins(5, 0, 5, 0);
  searchLayout->setSpacing(5);

  auto* icon = new oclero::qlementine::IconWidget(QIcon(":/icons/search.svg"), QSize(16, 16), this);

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

  layout()->addWidget(icon);
  layout()->addWidget(mInputField);
}

QLineEdit* SearchWidget::widget() const
{
  return mInputField;
}

void SearchWidget::addDescription(const QString& label)
{
  auto* hint = new oclero::qlementine::Label(label, this);
  hint->setRole(oclero::qlementine::TextRole::Caption);
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
