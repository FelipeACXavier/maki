#include "widget_factory.h"

#include <qobject.h>
#include <qvariant.h>

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
    : QWidget(parent)
{
  auto* vLayout = new QVBoxLayout(this);
  vLayout->setContentsMargins(0, 0, 0, 0);
  vLayout->setSpacing(WIDGET_SPACING);
  vLayout->setAlignment(Qt::AlignTop);

  auto* title = new oclero::qlementine::Label(label, this);
  title->setRole(oclero::qlementine::TextRole::H4);
  ;
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

    auto* title = new oclero::qlementine::Label(label, this);
    title->setRole(oclero::qlementine::TextRole::Default);
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
  auto* hint = new oclero::qlementine::Label(label, this);
  hint->setRole(oclero::qlementine::TextRole::Caption);
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

    auto* labelWidget = new oclero::qlementine::Label(label, this);
    labelWidget->setRole(oclero::qlementine::TextRole::Default);

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
  auto* hint = new oclero::qlementine::Label(label, this);
  hint->setRole(oclero::qlementine::TextRole::Caption);
  ;
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
  mInputField = new oclero::qlementine::LineEdit(this);
  mInputField->setPlaceholderText(placeholder);
  mInputField->setToolTip(QString(tr("Input must be an integer value")));
  mInputField->setToolTipDuration(Constants::TOOLTIP_DURATION);

  bool valid = false;
  auto intPlaceholder = placeholder.toInt(&valid);
  if (valid)
    mValue = intPlaceholder;

  mValidator = new IntegerOrVariableValidator(min, max, this);
  mInputField->setValidator(mValidator);

  connect(mInputField, &QLineEdit::inputRejected, this, [this]() {
    mInputField->setStatus(oclero::qlementine::Status::Warning);
  });
  connect(mInputField, &QLineEdit::editingFinished, this, [this]() {
    if (!mInputField->hasAcceptableInput())
      return;

    mValue = mInputField->text().toInt();
    mInputField->setStatus(oclero::qlementine::Status::Default);
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

    auto* labelWidget = new oclero::qlementine::Label(label, this);
    labelWidget->setRole(oclero::qlementine::TextRole::Default);

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
  auto* hint = new oclero::qlementine::Label(label, this);
  hint->setRole(oclero::qlementine::TextRole::Caption);
  layout()->addWidget(hint);
}

void IntegerWidget::setAcceptVariable(bool accept)
{
  if (mValidator)
    mValidator->setAcceptVariable(accept);
}

QLineEdit* IntegerWidget::widget() const
{
  return mInputField;
}

// ========================================================================================================================================
FloatWidget::FloatWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent, qreal min, qreal max)
    : QWidget(parent)
{
  mInputField = new oclero::qlementine::LineEdit(this);
  mInputField->setPlaceholderText(placeholder);
  mInputField->setToolTip(QString(tr("Input must be a real value")));
  mInputField->setToolTipDuration(Constants::TOOLTIP_DURATION);

  bool valid = false;
  auto intPlaceholder = placeholder.toInt(&valid);
  if (valid)
    mValue = intPlaceholder;

  mValidator = new DoubleOrVariableValidator(min, max, 6, this);
  mValidator->setNotation(QDoubleValidator::StandardNotation);
  mValidator->setLocale(QLocale::C);  // Use dot instead of comma
  mInputField->setValidator(mValidator);

  connect(mInputField, &QLineEdit::inputRejected, this, [this]() {
    mInputField->setStatus(oclero::qlementine::Status::Warning);
  });
  connect(mInputField, &QLineEdit::editingFinished, this, [this]() {
    if (!mInputField->hasAcceptableInput())
      return;

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

    auto* labelWidget = new oclero::qlementine::Label(label, this);
    labelWidget->setRole(oclero::qlementine::TextRole::Default);

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
  auto* hint = new oclero::qlementine::Label(label, this);
  hint->setRole(oclero::qlementine::TextRole::Caption);
  layout()->addWidget(hint);
}

void FloatWidget::setAcceptVariable(bool accept)
{
  if (mValidator)
    mValidator->setAcceptVariable(accept);
}

QLineEdit* FloatWidget::widget() const
{
  return mInputField;
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

  auto* labelWidget = new oclero::qlementine::Label(label, this);
  labelWidget->setRole(oclero::qlementine::TextRole::Default);

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
  auto* hint = new oclero::qlementine::Label(label, this);
  hint->setRole(oclero::qlementine::TextRole::Caption);
  layout()->addWidget(hint);
}

// ========================================================================================================================================
SelectorWidget::SelectorWidget(const QString& label, QComboBox* comboBox, WidgetAlignment alignment, QWidget* parent)
{
  if (comboBox)
    mInputField = comboBox;
  else
    mInputField = new QComboBox(this);

  setValue(Constants::EMPTY_COMBO);
  connect(mInputField, &QComboBox::currentTextChanged, this, [this](const QString& value) {
    mValue = value;
    emit valueChanged(mValue);
    mData = mInputField->currentData();
    emit dataChanged(mValue, mData);
  });

  mValue = mInputField->currentText();

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

    auto* labelWidget = new oclero::qlementine::Label(label, this);
    labelWidget->setRole(oclero::qlementine::TextRole::Default);

    layout()->addWidget(labelWidget);
    layout()->addWidget(mInputField);
  }
}

SelectorWidget::SelectorWidget(const QString& label, WidgetAlignment alignment, QWidget* parent)
    : SelectorWidget(label, nullptr, alignment, parent)
{
}

void SelectorWidget::addDescription(const QString& label)
{
  auto* hint = new oclero::qlementine::Label(label, this);
  hint->setRole(oclero::qlementine::TextRole::Caption);
  layout()->addWidget(hint);
}

void SelectorWidget::setData(const QString& data)
{
  int index = mInputField->findData(data);
  if (index >= 0)
  {
    mInputField->setCurrentIndex(index);
    mValue = mInputField->currentText();
    mData = mInputField->currentData();
  }
}

void SelectorWidget::setValue(const QString& text)
{
  if (text == Constants::EMPTY_COMBO)
  {
    mInputField->setPlaceholderText(Constants::EMPTY_COMBO);
    mInputField->setCurrentIndex(-1);
    mValue = Constants::EMPTY_COMBO;
    mData = QVariant();
    return;
  }

  int index = mInputField->findText(text);
  if (index >= 0)
  {
    mInputField->setCurrentIndex(index);
    mValue = mInputField->currentText();
    mData = mInputField->currentData();
  }
}

QString SelectorWidget::getValue() const
{
  return mValue;
}

QVariant SelectorWidget::getData() const
{
  return mData;
}

void SelectorWidget::addItem(const QString& name, const QString& value)
{
  int index = mInputField->findData(name);
  if (index < 0)
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
  auto* hint = new oclero::qlementine::Label(label, this);
  hint->setRole(oclero::qlementine::TextRole::Caption);
  layout()->addWidget(hint);
}

// =========================================================================================================
ColorWidget::ColorWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent)
    : QWidget(parent)
{
  QColor placeholderColor = QColor::fromString(placeholder);
  mInputField = new oclero::qlementine::ColorEditor(placeholderColor, this);
  connect(mInputField, &oclero::qlementine::ColorEditor::colorChanged, this, [this]() {
    mValue = mInputField->color();
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

    auto* labelWidget = new oclero::qlementine::Label(label, this);
    labelWidget->setRole(oclero::qlementine::TextRole::Default);

    layout()->addWidget(labelWidget);
    layout()->addWidget(mInputField);
  }
}

oclero::qlementine::ColorEditor* ColorWidget::widget() const
{
  return mInputField;
}

void ColorWidget::addDescription(const QString& label)
{
  auto* hint = new oclero::qlementine::Label(label, this);
  hint->setRole(oclero::qlementine::TextRole::Caption);
  layout()->addWidget(hint);
}

QColor ColorWidget::getValue() const
{
  return mValue;
}

void ColorWidget::setValue(const QColor& color)
{
  mValue = color;
  mInputField->setColor(color);
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
