#include "widget_factory.h"

#include <QCheckBox>
#include <QColorDialog>
#include <QComboBox>
#include <QCompleter>
#include <QDoubleValidator>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QShortcut>
#include <QSpinBox>
#include <QTableWidget>
#include <QTimer>
#include <QToolTip>
#include <oclero/qlementine.hpp>
#include <oclero/qlementine/widgets/ColorEditor.hpp>
#include <oclero/qlementine/widgets/IconWidget.hpp>
#include <oclero/qlementine/widgets/Label.hpp>
#include <oclero/qlementine/widgets/LineEdit.hpp>
#include <variant>

#include "app_configs.h"
#include "oclero/qlementine/Common.hpp"
#include "style_helpers.h"
#include "type_registry.h"
#include "typing/helpers.h"
#include "validators/double_variable.h"
#include "validators/int_variable.h"

static const int WIDGET_SPACING = 2;
static const int WIDGET_PADDING = 24;
static const int TOOLTIP_HEIGHT = 55;

#define GET_VALUE(ALT, DEFAULT)                   \
  do                                              \
  {                                               \
    if (std::holds_alternative<ALT>(mValue.data)) \
      return std::get<ALT>(mValue.data);          \
                                                  \
    return DEFAULT;                               \
  } while (false);

#define SET_VALUE(DATA)         \
  do                            \
  {                             \
    Value val;                  \
    val.data = DATA;            \
    InputWidget::setValue(val); \
  } while (false);

namespace maki
{

void addCompleter(const QStringList& items, QWidget* parent)
{
  auto* completer = new QCompleter(items, parent);
  completer->setFilterMode(Qt::MatchContains);
  completer->setCompletionMode(QCompleter::PopupCompletion);
  completer->setCaseSensitivity(Qt::CaseInsensitive);

  if (auto* underlying = qobject_cast<QComboBox*>(parent))
  {
    if (auto* old = underlying->completer())
      old->deleteLater();

    underlying->setCompleter(completer);
  }
  else if (auto* underlying = qobject_cast<oclero::qlementine::LineEdit*>(parent))
  {
    if (auto* old = underlying->completer())
      old->deleteLater();

    underlying->setCompleter(completer);
  }
  else
  {
    completer->deleteLater();
  }
}

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
    , mPadding(WIDGET_PADDING)
{
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  setMinimumWidth(0);

  auto* rootLayout = new QVBoxLayout(this);
  rootLayout->setContentsMargins(0, 0, 0, 0);
  rootLayout->setSpacing(WIDGET_SPACING);

  auto* title = new oclero::qlementine::Label(label, this);
  title->setRole(role);
  title->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);

  auto* line = new QFrame(this);
  line->setFrameShape(QFrame::HLine);

  rootLayout->addWidget(title);
  rootLayout->addWidget(line);
  rootLayout->addSpacing(2 * WIDGET_SPACING);

  mContentWidget = new QWidget(this);

  mContentLayout = new QVBoxLayout(mContentWidget);
  mContentLayout->setContentsMargins(WIDGET_SPACING, WIDGET_SPACING, WIDGET_SPACING, WIDGET_SPACING);
  mContentLayout->setSpacing(WIDGET_SPACING);
  mContentLayout->setAlignment(Qt::AlignTop);

  rootLayout->addWidget(mContentWidget);

  setFocusPolicy(Qt::ClickFocus);
}

void WidgetGroup::addWidget(QWidget* widget)
{
  auto* hlayout = new QHBoxLayout();
  hlayout->setContentsMargins(mPadding, 0, 0, 0);
  hlayout->addWidget(widget);

  mContentLayout->addLayout(hlayout);
}

void WidgetGroup::addLayout(QLayout* layout)
{
  mContentLayout->addLayout(layout);
}

void WidgetGroup::addSpacing(int spacing)
{
  mContentLayout->addSpacing(spacing);
}

void WidgetGroup::addStretch()
{
  mContentLayout->addStretch();
}

void WidgetGroup::removeWidget(QWidget* widget)
{
  mContentLayout->removeWidget(widget);
}

void WidgetGroup::clear()
{
  // Start after the title + line + spacing
  clearLayout(mContentLayout);
}

void WidgetGroup::setPadding(int padding)
{
  mPadding = padding;
}

// =========================================================================================================
WidgetScrollGroup::WidgetScrollGroup(const QString& label, QWidget* parent)
    : WidgetScrollGroup(label, oclero::qlementine::TextRole::H4, parent)
{
}

WidgetScrollGroup::WidgetScrollGroup(const QString& label, oclero::qlementine::TextRole role, QWidget* parent)
    : WidgetGroup(label, role, parent)
{
  layout()->removeWidget(mContentWidget);

  auto* scrollArea = new QScrollArea(this);
  scrollArea->setWidgetResizable(true);
  scrollArea->setFrameShape(QFrame::NoFrame);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  scrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  mContentWidget->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

  scrollArea->setWidget(mContentWidget);

  layout()->addWidget(scrollArea);
}

// =========================================================================================================
InputWidget::InputWidget(const QString& label, QWidget* inputField, WidgetAlignment alignment, QWidget* parent)
    : QWidget(parent)
    , mInputField(inputField)
{
  oclero::qlementine::Label* labelWidget = nullptr;
  if (!label.isEmpty())
  {
    labelWidget = new oclero::qlementine::Label(label, this);
    labelWidget->setRole(oclero::qlementine::TextRole::Default);
    labelWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    if (auto* qlementineStyle = oclero::qlementine::appStyle())
    {
      auto metric = labelWidget->fontMetrics();
      const auto theme = qlementineStyle->theme();
      labelWidget->setMinimumWidth(metric.horizontalAdvance(label) + theme.spacing);
    }
  }

  mInputField->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  mInputField->setParent(this);
  if (mInputField->focusPolicy() == Qt::NoFocus)
    mInputField->setFocusPolicy(Qt::StrongFocus);

  if (alignment.type == WidgetAlignment::Type::FORM)
  {
    if (labelWidget && alignment.labelWidth > 0)
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
      if (labelWidget && alignment.labelWidth > 0)
        labelWidget->setFixedWidth(alignment.labelWidth);

      layout()->addWidget(createLayout(labelWidget, alignment));
    }
    else
    {
      if (labelWidget)
        layout()->addWidget(labelWidget);

      layout()->addWidget(mInputField);
    }
  }
}

void InputWidget::setValue(const Value& value)
{
  mValue = value;
  writeValueToWidget(value);
}

Value InputWidget::getValue() const
{
  return mValue;
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

  if (alignment.direction == WidgetAlignment::Direction::RIGHT || alignment.direction == WidgetAlignment::Direction::CENTER)
    hLayout->addStretch();

  if (labelWidget)
    hLayout->addWidget(labelWidget);

  if (labelWidget && alignment.direction == WidgetAlignment::Direction::SPREAD)
    hLayout->addStretch();

  hLayout->addWidget(mInputField);

  if (alignment.direction == WidgetAlignment::Direction::LEFT || alignment.direction == WidgetAlignment::Direction::CENTER)
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
    : TypedInputWidget<QCheckBox>(label, new QCheckBox(), alignment, parent)
{
  if (auto* combo = qobject_cast<QCheckBox*>(mInputField))
  {
    combo->setMaximumWidth(20);
    combo->setCheckState(value ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    connect(combo, &QCheckBox::checkStateChanged, this, [this](Qt::CheckState state) {
      mValue.data = (state == Qt::CheckState::Checked);
      emit valueChanged(getValue());
      emit InputWidget::valueChanged();
    });
  }

  setValue(value);
}

bool BooleanWidget::getValue() const
{
  GET_VALUE(bool, false);
}

void BooleanWidget::setValue(bool value)
{
  SET_VALUE(value);
}

void maki::BooleanWidget::writeValueToWidget(const Value& value)
{
  if (!std::holds_alternative<bool>(value.data))
    return;

  auto actualValue = std::get<bool>(value.data);
  if (auto* combo = qobject_cast<QCheckBox*>(mInputField))
    combo->setCheckState(actualValue ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
}

// =========================================================================================================
StringWidget::StringWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent)
    : StringWidget(label, placeholder, alignment, "", parent)
{
}

StringWidget::StringWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, const QString& tooltip, QWidget* parent)
    : TypedInputWidget<oclero::qlementine::LineEdit>(label, new oclero::qlementine::LineEdit(), alignment, parent)
{
  mUpdateTimer = new QTimer(this);
  mUpdateTimer->setSingleShot(true);
  mUpdateTimer->setInterval(300);

  if (auto* edit = qobject_cast<oclero::qlementine::LineEdit*>(mInputField))
  {
    if (!tooltip.isEmpty())
      setToolTip(tooltip);

    connect(edit, &QLineEdit::inputRejected, this, [edit]() {
      edit->setStatus(oclero::qlementine::Status::Warning);
      if (!edit->toolTip().isEmpty())
      {
        const QPoint globalPos = edit->mapToGlobal(QPoint(0, 0)) - QPoint(0, TOOLTIP_HEIGHT);
        QToolTip::showText(globalPos, edit->toolTip(), edit);
      }
    });
    connect(edit, &QLineEdit::textChanged, this, [this]() { mUpdateTimer->start(); });

    connect(mUpdateTimer, &QTimer::timeout, this, [this, edit]() {
      mValue.data = edit->text();
      emit valueChanged(getValue());
      emit InputWidget::valueChanged();
    });

    connect(edit, &QLineEdit::editingFinished, this, [this, edit]() {
      mUpdateTimer->stop();
      mValue.data = edit->text();
      emit valueChanged(getValue());
      emit InputWidget::valueChanged();
    });
  }

  setValue(placeholder);
}

QString StringWidget::getValue() const
{
  GET_VALUE(QString, "");
}

void StringWidget::setValue(const QString& value)
{
  SET_VALUE(value);
}

void maki::StringWidget::writeValueToWidget(const Value& value)
{
  if (!std::holds_alternative<QString>(value.data))
    return;

  auto actualValue = std::get<QString>(value.data);
  if (auto* edit = qobject_cast<QLineEdit*>(mInputField))
    edit->setText(actualValue);
}

// ========================================================================================================================================
IntegerWidget::IntegerWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent, int min, int max)
    : TypedInputWidget<oclero::qlementine::LineEdit>(label, new oclero::qlementine::LineEdit(), alignment, parent)
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
      if (!edit->toolTip().isEmpty())
      {
        const QPoint globalPos = edit->mapToGlobal(QPoint(0, 0)) - QPoint(0, TOOLTIP_HEIGHT);
        QToolTip::showText(globalPos, edit->toolTip(), edit);
      }
    });
    connect(edit, &QLineEdit::editingFinished, this, [this, edit]() {
      if (!edit->hasAcceptableInput())
        return;

      mValue.data = edit->text();
      edit->setStatus(oclero::qlementine::Status::Default);
      emit valueChanged(std::get<QString>(mValue.data));
      emit InputWidget::valueChanged();
    });
  }

  setValue(placeholder);
}

void IntegerWidget::setAcceptVariable(bool accept)
{
  if (mValidator)
    mValidator->setAcceptVariable(accept);
}

int IntegerWidget::getValue() const
{
  if (!std::holds_alternative<QString>(mValue.data))
    assert(false && "Tried to get an integer from a field that does not hold an integer");

  auto actualValue = std::get<QString>(mValue.data);
  bool ok = false;
  int value = actualValue.toInt(&ok);
  if (ok)
    return value;

  assert(false && "Tried to get an integer from a field that does not contain an integer");
}

void IntegerWidget::setValue(const QString& value)
{
  SET_VALUE(value);
}

void IntegerWidget::setValue(int value)
{
  SET_VALUE(QString::number(value));
}

void IntegerWidget::writeValueToWidget(const Value& value)
{
  if (!std::holds_alternative<QString>(value.data))
    return;

  auto actualValue = std::get<QString>(value.data);
  if (auto* edit = qobject_cast<QLineEdit*>(mInputField))
    edit->setText(actualValue);
}

// ========================================================================================================================================
FloatWidget::FloatWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent, qreal min, qreal max)
    : TypedInputWidget<oclero::qlementine::LineEdit>(label, new oclero::qlementine::LineEdit(), alignment, parent)
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
      if (!edit->toolTip().isEmpty())
      {
        const QPoint globalPos = edit->mapToGlobal(QPoint(0, 0)) - QPoint(0, TOOLTIP_HEIGHT);
        QToolTip::showText(globalPos, edit->toolTip(), edit);
      }
    });
    connect(edit, &QLineEdit::editingFinished, this, [this, edit]() {
      if (!edit->hasAcceptableInput())
        return;

      mValue.data = edit->text();
      edit->setStatus(oclero::qlementine::Status::Default);
      emit valueChanged(std::get<QString>(mValue.data));
      emit InputWidget::valueChanged();
    });
  }

  setValue(placeholder);
}

void FloatWidget::setAcceptVariable(bool accept)
{
  if (mValidator)
    mValidator->setAcceptVariable(accept);
}

double FloatWidget::getValue() const
{
  if (!std::holds_alternative<QString>(mValue.data))
    assert(false && "Tried to get an integer from a field that does not hold a real");

  auto actualValue = std::get<QString>(mValue.data);
  bool ok = false;
  double value = actualValue.toDouble(&ok);
  if (ok)
    return value;

  assert(false && "Tried to get an integer from a field that does not contain a real");
}

void FloatWidget::setValue(double value)
{
  SET_VALUE(QString::number(value));
}

void FloatWidget::setValue(const QString& value)
{
  SET_VALUE(value);
}

void FloatWidget::writeValueToWidget(const Value& value)
{
  if (!std::holds_alternative<QString>(value.data))
    return;

  auto actualValue = std::get<QString>(value.data);
  if (auto* edit = qobject_cast<QLineEdit*>(mInputField))
    edit->setText(actualValue);
}

// ========================================================================================================================================
SpinWidget::SpinWidget(const QString& label, int placeholder, QWidget* parent, WidgetAlignment alignment, int min, int max)
    : TypedInputWidget<QSpinBox>(label, new QSpinBox(), alignment, parent)
{
  if (auto* edit = qobject_cast<QSpinBox*>(mInputField))
  {
    edit->setRange(min, max);
    edit->setAlignment(Qt::AlignRight);
    edit->setValue(placeholder);

    connect(edit, &QSpinBox::valueChanged, this, [this](int value) {
      mValue.data = value;
      emit valueChanged(getValue());
      emit InputWidget::valueChanged();
    });
  }

  setValue(placeholder);
}

void SpinWidget::setSuffix(const QString& suffix)
{
  if (auto* edit = qobject_cast<QSpinBox*>(mInputField))
    edit->setSuffix(suffix);
}

int SpinWidget::getValue() const
{
  GET_VALUE(int, 0);
}

void SpinWidget::setValue(int value)
{
  SET_VALUE(value);
}

void SpinWidget::writeValueToWidget(const Value& value)
{
  if (!std::holds_alternative<int>(value.data))
    return;

  auto actualValue = std::get<int>(value.data);
  if (auto* edit = qobject_cast<QSpinBox*>(mInputField))
    edit->setValue(actualValue);
}

// ========================================================================================================================================
SelectorWidget::SelectorWidget(const QString& label, WidgetAlignment alignment, QWidget* parent)
    : SelectorWidget(label, new QComboBox(), alignment, parent)
{
}

SelectorWidget::SelectorWidget(const QString& label, QComboBox* comboBox, WidgetAlignment alignment, QWidget* parent)
    : TypedInputWidget<QComboBox>(label, comboBox, alignment, parent)
{
  if (auto* combo = qobject_cast<QComboBox*>(mInputField))
  {
    combo->view()->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    combo->view()->verticalScrollBar()->setSingleStep(10);

    setValue(Constants::EMPTY_COMBO);
    connect(combo, &QComboBox::currentTextChanged, this, [this, combo](const QString& value) {
      mValue.data = value;
      mData = combo->currentData();
      emit valueChanged(getValue());
      emit dataChanged(getValue(), mData);
      emit InputWidget::valueChanged();
    });
  }
}

void SelectorWidget::writeValueToWidget(const Value& value)
{
  if (auto* combo = qobject_cast<QComboBox*>(mInputField))
  {
    if (!std::holds_alternative<QString>(value.data))
      return;

    auto text = std::get<QString>(value.data);
    if (text == Constants::EMPTY_COMBO)
    {
      combo->setPlaceholderText(Constants::EMPTY_COMBO);
      combo->setCurrentIndex(-1);

      // Clear values
      mValue = {};
      mData = QVariant();
      return;
    }

    int index = combo->findText(text);
    if (index >= 0)
    {
      combo->setCurrentIndex(index);
      mValue.data = combo->currentText();
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
      mValue.data = combo->currentText();
      mData = combo->currentData();
    }
    else
    {
      combo->setCurrentIndex(-1);
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

QString SelectorWidget::getValue() const
{
  GET_VALUE(QString, Constants::EMPTY_COMBO);
}

void SelectorWidget::setValue(const QString& value)
{
  SET_VALUE(value);
}

// =========================================================================================================
ButtonWidget::ButtonWidget(const QString& label, QWidget* parent)
{
  auto* vLayout = new QVBoxLayout(this);
  vLayout->setContentsMargins(0, 0, 0, WIDGET_SPACING);
  vLayout->setSpacing(WIDGET_SPACING);

  mInputField = new QPushButton(this);
  mInputField->setText(label);

  connect(mInputField, &QPushButton::clicked, this, [this]() { emit valueChanged(); });

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

void ButtonWidget::setText(const QString& text)
{
  mInputField->setText(text);
}

void ButtonWidget::addDescription(const QString& label)
{
  auto* hint = new oclero::qlementine::Label(label, this);
  hint->setRole(oclero::qlementine::TextRole::Caption);
  layout()->addWidget(hint);
}

// =========================================================================================================
ColorWidget::ColorWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent)
    : TypedInputWidget<oclero::qlementine::ColorEditor>(label, new oclero::qlementine::ColorEditor(), alignment, parent)
{
  if (auto* edit = qobject_cast<oclero::qlementine::ColorEditor*>(mInputField))
  {
    edit->setColor(QColor::fromString(placeholder));
    connect(edit, &oclero::qlementine::ColorEditor::colorChanged, this, [this, edit]() {
      mValue.data = edit->color();
      emit valueChanged(getValue());
      emit InputWidget::valueChanged();
    });
  }

  setValue(QColor::fromString(placeholder));
}

void ColorWidget::writeValueToWidget(const Value& value)
{
  if (!std::holds_alternative<QColor>(value.data))
    return;

  auto actualValue = std::get<QColor>(value.data);
  if (auto* edit = qobject_cast<oclero::qlementine::ColorEditor*>(mInputField))
    edit->setColor(actualValue);
}

QColor ColorWidget::getValue() const
{
  GET_VALUE(QColor, QColor());
}

void ColorWidget::setValue(const QColor& value)
{
  SET_VALUE(value);
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

ContainerWidget::ContainerWidget(const QString& label, QWidget* widget, WidgetAlignment alignment, QWidget* parent)
    : TypedInputWidget<QWidget>(label, widget, alignment, parent)
{
}

void ContainerWidget::writeValueToWidget(const Value& /* value */)
{
}

// =========================================================================================================
SearchWidget::SearchWidget(const QString& placeholder, QWidget* parent)
{
  setMaximumHeight(40);

  auto* searchLayout = new QHBoxLayout(this);
  searchLayout->setContentsMargins(5, 0, 5, 0);
  searchLayout->setSpacing(5);

  auto* icon = new oclero::qlementine::IconWidget(QIcon(":/icons/search.svg"), Config::SMALL_BUTTON_SIZE, this);

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

// =========================================================================================================
ListWidget::ListWidget(const QString& label, const koda::types::TypeReference& elementType, WidgetAlignment alignment, QWidget* parent)
    : TypedInputWidget<WidgetGroup>("", new maki::WidgetGroup(label, oclero::qlementine::TextRole::H5, nullptr), alignment, parent)
    , mElementType(elementType)
{
  if (auto* group = qobject_cast<maki::WidgetGroup*>(mInputField))
  {
    mItems = new QWidget(group);

    mItemsLayout = new QVBoxLayout(mItems);
    mItemsLayout->setContentsMargins(0, 0, 0, 0);

    auto* addButton = new QPushButton(this);
    addButton->setFixedSize(Config::MEDIUM_BUTTON_SIZE);
    addButton->setIcon(iconFromTheme("plus"));

    auto* browserButtons = new QHBoxLayout();
    browserButtons->addWidget(addButton);
    browserButtons->addStretch();

    connect(addButton, &QPushButton::clicked, this, [this]() { addItem(Value{}); });

    group->addWidget(mItems);
    group->addStretch();
    group->addLayout(browserButtons);
  }
}

ListValue ListWidget::getValue() const
{
  ListValue values;

  for (const auto* editor : mEditors)
    if (editor)
      values.push_back(editor->getValue());

  return values;
}

void ListWidget::setValue(const ListValue& value)
{
  SET_VALUE(value)
}

void ListWidget::writeValueToWidget(const Value& value)
{
  const auto* list = std::get_if<ListValue>(&value.data);
  if (!list)
    return;

  clear();

  for (const auto& item : *list)
    addItem(item);
}

void ListWidget::addItem(const Value& value)
{
  auto* row = new QWidget(mItems);
  auto* rowLayout = new QHBoxLayout(row);
  rowLayout->setAlignment(Qt::AlignVCenter);
  rowLayout->setContentsMargins(0, 0, 0, 0);

  auto alignment = WidgetAlignment::Inline();
  auto* editor = ValueEditorFactory::create("", mElementType, value, alignment, row);
  if (!editor)
  {
    row->deleteLater();
    return;
  }

  editor->setValue(value);

  auto* removeButton = new QPushButton(this);
  removeButton->setFixedSize(Config::MEDIUM_BUTTON_SIZE);
  removeButton->setIcon(iconFromTheme("minus"));

  rowLayout->addWidget(editor);
  rowLayout->addWidget(removeButton);

  mItemsLayout->addWidget(row);

  mEditors.push_back(editor);

  connect(editor, &InputWidget::valueChanged, this, [this] {
    mValue.data = getValue();
    emit InputWidget::valueChanged();
  });
  connect(removeButton, &QPushButton::clicked, this, [this, row, editor]() {
    std::erase(mEditors, editor);
    row->deleteLater();
    mValue.data = getValue();
    emit InputWidget::valueChanged();
  });
}

void ListWidget::clear()
{
  mEditors.clear();
  clearLayout(mItemsLayout);
}

QList<QWidget*> ListWidget::focusWidgets() const
{
  QList<QWidget*> widgets;

  for (const auto& editor : mEditors)
    widgets.append(editor->focusWidgets());

  return widgets;
}

// =========================================================================================================
MapWidget::MapWidget(const QString& label, const koda::types::TypeReference& keyType, const koda::types::TypeReference& valueType,
                     WidgetAlignment alignment, QWidget* parent)
    : TypedInputWidget<WidgetGroup>("", new maki::WidgetGroup(label, oclero::qlementine::TextRole::H5, nullptr), alignment, parent)
    , mKeyType(keyType)
    , mValueType(valueType)
{
  if (auto* group = qobject_cast<maki::WidgetGroup*>(mInputField))
  {
    mItems = new QWidget(group);

    mItemsLayout = new QVBoxLayout(mItems);
    mItemsLayout->setContentsMargins(0, 0, 0, 0);

    auto* addButton = new QPushButton(this);
    addButton->setFixedSize(Config::MEDIUM_BUTTON_SIZE);
    addButton->setIcon(iconFromTheme("plus"));

    auto* browserButtons = new QHBoxLayout();
    browserButtons->addWidget(addButton);
    browserButtons->addStretch();

    connect(addButton, &QPushButton::clicked, this, [this]() { addItem(Value{}, Value{}); });

    group->addWidget(mItems);
    group->addStretch();
    group->addLayout(browserButtons);
  }
}

MapValue MapWidget::getValue() const
{
  MapValue values;

  for (const auto& [key, value] : mEditors)
    if (key && value)
      values.emplace(key->getValue(), value->getValue());

  return values;
}

void MapWidget::setValue(const MapValue& value)
{
  SET_VALUE(value)
}

void MapWidget::writeValueToWidget(const Value& value)
{
  const auto* mapValue = std::get_if<MapValue>(&value.data);
  if (!mapValue)
    return;

  clear();

  for (const auto& [key, item] : *mapValue)
    addItem(key, item);
}

void MapWidget::addItem(const Value& key, const Value& value)
{
  auto* row = new QWidget(mItems);
  auto* rowLayout = new QHBoxLayout(row);
  rowLayout->setAlignment(Qt::AlignVCenter);
  rowLayout->setContentsMargins(0, 0, 0, 0);

  auto* keyEditor = ValueEditorFactory::create("", mKeyType, key, WidgetAlignment::Inline(), row);
  if (!keyEditor)
  {
    row->deleteLater();
    return;
  }
  auto* valueEditor = ValueEditorFactory::create("", mValueType, value, WidgetAlignment::Inline(), row);
  if (!valueEditor)
  {
    keyEditor->deleteLater();
    row->deleteLater();
    return;
  }

  keyEditor->setValue(key);
  valueEditor->setValue(value);

  auto* removeButton = new QPushButton(this);
  removeButton->setFixedSize(Config::MEDIUM_BUTTON_SIZE);
  removeButton->setIcon(iconFromTheme("minus"));

  rowLayout->addWidget(keyEditor);
  rowLayout->addWidget(valueEditor);
  rowLayout->addWidget(removeButton);

  mItemsLayout->addWidget(row);

  mEditors.push_back({keyEditor, valueEditor});

  connect(keyEditor, &InputWidget::valueChanged, this, [this] {
    mValue.data = getValue();
    emit InputWidget::valueChanged();
  });
  connect(valueEditor, &InputWidget::valueChanged, this, [this] {
    mValue.data = getValue();
    emit InputWidget::valueChanged();
  });
  connect(removeButton, &QPushButton::clicked, this, [this, row, keyEditor, valueEditor]() {
    mEditors.erase(std::remove_if(mEditors.begin(), mEditors.end(), [keyEditor, valueEditor](const MapField& field) {
      return field.keyEditor == keyEditor && field.valueEditor == valueEditor;
    }));
    row->deleteLater();
    mValue.data = getValue();
    emit InputWidget::valueChanged();
  });
}

void MapWidget::clear()
{
  mEditors.clear();
  clearLayout(mItemsLayout);
}

QList<QWidget*> MapWidget::focusWidgets() const
{
  QList<QWidget*> widgets;

  for (const auto& [key, value] : mEditors)
  {
    widgets.append(key->focusWidgets());
    widgets.append(value->focusWidgets());
  }

  return widgets;
}

// =========================================================================================================
RecordWidget::RecordWidget(const QString& label, const koda::types::RecordTypeDefinition& definition, WidgetAlignment alignment, QWidget* parent)
    : TypedInputWidget<WidgetGroup>("", new maki::WidgetGroup(label, oclero::qlementine::TextRole::H5, nullptr), alignment, parent)
    , mDefinition(definition)
{
}

RecordValue RecordWidget::getValue() const
{
  RecordValue values;

  for (const auto& field : mEditors)
    values.emplace(field.id.toStdString(), field.editor->getValue());

  return values;
}

void RecordWidget::setValue(const RecordValue& value)
{
  SET_VALUE(value)
}

void RecordWidget::writeValueToWidget(const Value& value)
{
  const auto* record = std::get_if<RecordValue>(&value.data);
  if (!record)
    return;

  clear();

  if (auto* group = qobject_cast<maki::WidgetGroup*>(mInputField))
  {
    for (const auto& field : mDefinition.fields)
    {
      auto it = record->find(field.name);
      Value fieldValue;
      if (it != record->end())
        fieldValue = it->second;

      auto fieldName = QString::fromStdString(field.name);
      auto fieldWidget = ValueEditorFactory::create(fieldName, field.type, fieldValue, WidgetAlignment::Inline(), group);
      if (!fieldWidget)
        continue;

      connect(fieldWidget, &InputWidget::valueChanged, this, [this] {
        mValue.data = getValue();
        emit InputWidget::valueChanged();
      });

      mEditors.push_back({fieldName, fieldWidget});
      group->addWidget(fieldWidget);
    }
  }
}

void RecordWidget::clear()
{
  if (auto* group = qobject_cast<maki::WidgetGroup*>(mInputField))
    group->clear();
}

QList<QWidget*> RecordWidget::focusWidgets() const
{
  QList<QWidget*> widgets;

  for (const auto& field : mEditors)
    widgets.append(field.editor->focusWidgets());

  return widgets;
}

// =========================================================================================================
EnumWidget::EnumWidget(const QString& label, const koda::types::EnumTypeDefinition& definition, WidgetAlignment alignment, QWidget* parent)
    : SelectorWidget(label, alignment, parent)
    , mDefinition(definition)
{
  for (const auto& field : definition.values)
    addItem(QString::fromStdString(field.name), QString::fromStdString(field.value.value_or("None")));
}

// =========================================================================================================
InputWidget* ValueEditorFactory::create(const QString& label, const koda::types::TypeReference& type, const Value& value, WidgetAlignment alignment,
                                        QWidget* parent)
{
  if (type.kind() == koda::types::TypeReferenceKind::List)
  {
    auto* editor = new maki::ListWidget(QString::fromStdString(type.toString()), type.elementType(), alignment, parent);
    editor->setValue(value.toList());
    return editor;
  }
  else if (type.kind() == koda::types::TypeReferenceKind::Map)
  {
    auto* editor = new maki::MapWidget(QString::fromStdString(type.toString()), type.mapKeyType(), type.mapValueType(), alignment, parent);
    editor->setValue(value.toMap());
    return editor;
  }

  const auto* definition = TypeRegistry::instance().resolve(type);
  if (!definition)
    return nullptr;

  if (definition->isPrimitive())
  {
    auto kind = definition->primitive().primitive;

    if (koda::types::isInteger(kind))
      return new maki::IntegerWidget(label, QString::number(value.toInt()), alignment, parent);
    else if (koda::types::isFloatingPoint(kind))
      return new maki::FloatWidget(label, QString::number(value.toDouble()), alignment, parent);
    else if (kind == koda::types::PrimitiveKind::String)
      return new maki::StringWidget(label, value.toString(), alignment, parent);
    else if (kind == koda::types::PrimitiveKind::Bool)
      return new maki::BooleanWidget(label, value.toBool(), alignment, parent);
    else
      return new maki::StringWidget("Other", "-", alignment, parent);
  }
  else if (definition->isRecord())
  {
    auto* editor = new maki::RecordWidget(QString::fromStdString(definition->name.toString()), definition->record(), alignment, parent);
    editor->setValue(value.toRecord());
    return editor;
  }
  else if (definition->isAlias())
  {
    return ValueEditorFactory::create(QString::fromStdString(definition->name.toString()), definition->alias().target, value, alignment, parent);
  }
  else if (definition->isEnum())
  {
    auto* editor = new EnumWidget(QString::fromStdString(definition->name.toString()), definition->enumeration(), alignment, parent);
    editor->setValue(value.toString());
    return editor;
  }

  return nullptr;
}

}  // namespace maki
