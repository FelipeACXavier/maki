#pragma once

#include <QComboBox>
#include <QWidget>

#include "oclero/qlementine/widgets/ColorEditor.hpp"
#include "style_helpers.h"

class QLineEdit;
class QSpinBox;
class QPushButton;
class QLabel;
class QCheckBox;
class QFormLayout;
class IntegerOrVariableValidator;
class DoubleOrVariableValidator;

namespace oclero::qlementine
{
class Label;
class LineEdit;
class ColorEditor;
}  // namespace oclero::qlementine

/**
 * @namespace maki
 * @brief Contains UI helper widgets and layout grouping utilities used by MAKI.
 */
namespace maki
{
class WidgetGroup;

/**
 * @struct WidgetAlignment
 * @brief Describes how a widget should be aligned or inserted into a parent layout.
 */
struct WidgetAlignment
{
  /**
   * @enum Type
   * @brief Supported alignment/layout styles for widgets.
   */
  enum class Type
  {
    /** @brief Alignment type is unknown or unspecified. */
    UNKNOWN = 0,
    /** @brief Widget should be placed inline with other widgets. */
    INLINE,
    /** @brief Widget should be placed vertically. */
    VERTICAL,
    /** @brief Widget should be placed in a form layout. */
    FORM
  } type;  ///< The alignment mode to apply.

  enum class Direction
  {
    /** @brief Alignment type is unknown or unspecified. */
    UNKNOWN = 0,
    LEFT,
    RIGHT,
    CENTER,
    SPREAD,
  } direction;  ///< The alignment mode to apply.

  WidgetGroup* group;  ///< Optional form layout associated with this alignment.
  int labelWidth = 0;

  static WidgetAlignment Inline()
  {
    return WidgetAlignment{WidgetAlignment::Type::INLINE};
  };

  static WidgetAlignment Vertical()
  {
    return WidgetAlignment{WidgetAlignment::Type::VERTICAL};
  };

  static WidgetAlignment Form(maki::WidgetGroup* widgetGroup, int width = 0, Direction dir = Direction::LEFT)
  {
    return maki::WidgetAlignment{
        .type = maki::WidgetAlignment::Type::FORM,
        .direction = dir,
        .group = widgetGroup,
        .labelWidth = width,
    };
  };
};

/**
 * @class GridGroup
 * @brief A labelled container widget that arranges child widgets and layouts in a grid.
 */
class GridGroup : public QWidget
{
public:
  /**
   * @brief Constructs a grid group with a label and fixed grid dimensions.
   * @param label The visible label for the group.
   * @param rows The number of rows in the grid.
   * @param cols The number of columns in the grid.
   * @param parent The parent widget.
   */
  GridGroup(const QString& label, int rows, int cols, QWidget* parent);

  /**
   * @brief Adds a widget to the next available cell in the grid.
   * @param widget The widget to add.
   */
  void addWidget(QWidget* widget);

  /**
   * @brief Adds a layout to the next available cell in the grid.
   * @param layout The layout to add.
   */
  void addLayout(QLayout* layout);

  /**
   * @brief Returns the internal content widget of the group.
   * @return The content widget.
   */
  QWidget* widget() const;

private:
  QWidget* mContent;  ///< Internal content widget that owns the grid contents.
  const int mRows;    ///< Total number of rows in the grid.
  const int mCols;    ///< Total number of columns in the grid.

  int mCurrentRow;  ///< Current row where the next item will be inserted.
  int mCurrentCol;  ///< Current column where the next item will be inserted.
};

/**
 * @class WidgetGroup
 * @brief A labelled container widget that groups widgets in a generic layout.
 */
class WidgetGroup : public QWidget
{
public:
  /**
   * @brief Constructs a widget group with a label.
   * @param label The visible label for the group.
   * @param parent The parent widget.
   */
  WidgetGroup(const QString& label, QWidget* parent);
  WidgetGroup(const QString& label, oclero::qlementine::TextRole role, QWidget* parent);

  /**
   * @brief Adds a widget to the group.
   * @param widget The widget to add.
   */
  void addWidget(QWidget* widget);

  /**
   * @brief Adds a layout to the group.
   * @param layout The layout to add.
   */
  void addLayout(QLayout* layout);

  /**
   * @brief Adds fixed spacing to the group layout.
   * @param spacing The amount of spacing to add, in pixels.
   */
  void addSpacing(int spacing);

  /**
   * @brief Adds a stretchable spacer to the group layout.
   */
  void addStretch();
};

class InputWidget : public QWidget
{
  Q_OBJECT
public:
  InputWidget(const QString& label, QWidget* inputField, WidgetAlignment alignment, QWidget* parent = nullptr);

  void addDescription(const QString& text);
  void setToolTip(const QString& text);

protected:
  QWidget* mInputField = nullptr;

  QWidget* createLayout(oclero::qlementine::Label* label, WidgetAlignment alignment);
};

template <typename T, typename W>
class TypedInputWidget : public InputWidget
{
public:
  TypedInputWidget(const QString& label, W* inputField, const T& value, WidgetAlignment alignment, QWidget* parent = nullptr)
      : InputWidget(label, inputField, alignment, parent)
      , mValue(value)
  {
  }

  void setValue(const T& value)
  {
    mValue = value;
    writeValueToWidget(value);
  }

  T getValue() const
  {
    return mValue;
  }

  W* widget() const
  {
    return qobject_cast<W*>(mInputField);
  }

protected:
  virtual void writeValueToWidget(const T& value) = 0;
  T mValue{};
};

/**
 * @class BooleanWidget
 * @brief A widget for displaying and editing boolean values.
 */
class BooleanWidget : public TypedInputWidget<bool, QCheckBox>
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a boolean input widget.
   * @param label The label shown next to the widget.
   * @param value The initial boolean value.
   * @param alignment The alignment/layout behaviour to use.
   * @param parent The parent widget.
   */
  BooleanWidget(const QString& label, bool value, WidgetAlignment alignment, QWidget* parent);

  void writeValueToWidget(const bool& value) override;

signals:
  /**
   * @brief Emitted when the boolean value changes.
   * @param value The new value.
   */
  void valueChanged(const bool value);
};

/**
 * @class StringWidget
 * @brief A widget for displaying and editing string values.
 */
class StringWidget : public TypedInputWidget<QString, oclero::qlementine::LineEdit>
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a string input widget.
   * @param label The label shown next to the widget.
   * @param placeholder The placeholder text shown in the input field.
   * @param alignment The alignment/layout behaviour to use.
   * @param parent The parent widget.
   */
  StringWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent);
  StringWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, const QString& tooltip, QWidget* parent);

  void writeValueToWidget(const QString& value) override;

signals:
  /**
   * @brief Emitted when the string value changes.
   * @param value The new value.
   */
  void valueChanged(const QString& value);
};

/**
 * @class IntegerWidget
 * @brief A widget for displaying and editing integer values.
 */
class IntegerWidget : public TypedInputWidget<QString, oclero::qlementine::LineEdit>
{
  Q_OBJECT
public:
  /**
   * @brief Constructs an integer input widget.
   * @param label The label shown next to the widget.
   * @param placeholder The placeholder text shown in the input field.
   * @param alignment The alignment/layout behaviour to use.
   * @param parent The parent widget.
   * @param min The minimum allowed value.
   * @param max The maximum allowed value.
   */
  IntegerWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent, int min = INT32_MIN,
                int max = INT32_MAX);

  /**
   * @brief Sets the widget to accept variables
   * @param value The value to set.
   */
  void setAcceptVariable(bool accept);

  void writeValueToWidget(const QString& value) override;

  using TypedInputWidget<QString, oclero::qlementine::LineEdit>::setValue;
  void setValue(int value);

  int getValue() const;

signals:
  /**
   * @brief Emitted when the integer value changes.
   * @param value The new value.
   */
  void valueChanged(const QString& value);

private:
  IntegerOrVariableValidator* mValidator;
};

/**
 * @class FloatWidget
 * @brief A widget for displaying and editing floating-point values.
 */
class FloatWidget : public TypedInputWidget<QString, oclero::qlementine::LineEdit>
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a floating-point input widget.
   * @param label The label shown next to the widget.
   * @param placeholder The placeholder text shown in the input field.
   * @param alignment The alignment/layout behaviour to use.
   * @param parent The parent widget.
   * @param min The minimum allowed value.
   * @param max The maximum allowed value.
   */
  FloatWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent,
              qreal min = std::numeric_limits<qreal>::min(), qreal max = std::numeric_limits<qreal>::max());

  /**
   * @brief Sets the widget to accept variables
   * @param value The value to set.
   */
  void setAcceptVariable(bool accept);

  void writeValueToWidget(const QString& value) override;

  using TypedInputWidget<QString, oclero::qlementine::LineEdit>::setValue;
  void setValue(qreal value);

  qreal getValue() const;

signals:
  /**
   * @brief Emitted when the floating-point value changes.
   * @param value The new value.
   */
  void valueChanged(const QString& value);

private:
  DoubleOrVariableValidator* mValidator;
};

/**
 * @class SpinWidget
 * @brief A widget for displaying and editing integer values using a spin box.
 */
class SpinWidget : public TypedInputWidget<int, QSpinBox>
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a spin box input widget.
   * @param label The label shown next to the widget.
   * @param placeholder The initial or placeholder-like value.
   * @param parent The parent widget.
   * @param min The minimum allowed value.
   * @param max The maximum allowed value.
   */
  SpinWidget(const QString& label, int placeholder, QWidget* parent, WidgetAlignment alignment, int min = INT32_MIN, int max = INT32_MAX);

  void writeValueToWidget(const int& value) override;

  /**
   * @brief Sets the suffix displayed by the spin box.
   * @param suffix The suffix text.
   */
  void setSuffix(const QString& suffix);

signals:
  /**
   * @brief Emitted when the spin box value changes.
   * @param value The new value.
   */
  void valueChanged(const int value);
};

/**
 * @class SelectorWidget
 * @brief A widget for selecting one value from a list of options.
 */
class SelectorWidget : public TypedInputWidget<QString, QComboBox>
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a selector widget with an internally created combo box.
   * @param label The label shown next to the widget.
   * @param parent The parent widget.
   */
  SelectorWidget(const QString& label, WidgetAlignment alignment, QWidget* parent);

  /**
   * @brief Constructs a selector widget using an existing combo box.
   * @param label The label shown next to the widget.
   * @param comboBox The combo box to use as the selector.
   * @param parent The parent widget.
   */
  SelectorWidget(const QString& label, QComboBox* comboBox, WidgetAlignment alignment, QWidget* parent);

  void setData(const QString& value);

  QVariant getData() const;

  /**
   * @brief Adds an item to the selector.
   * @param name The visible display name.
   * @param value The underlying value associated with the item.
   */
  void addItem(const QString& name, const QVariant& value);

  void writeValueToWidget(const QString& value) override;

signals:
  /**
   * @brief Emitted when the selected value changes.
   * @param value The new value.
   */
  void valueChanged(const QString& value);
  void dataChanged(const QString& text, const QVariant& value);

private:
  QVariant mData;
};

/**
 * @class ButtonWidget
 * @brief A widget wrapping a push button with optional description and icon support.
 */
class ButtonWidget : public QWidget
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a button widget.
   * @param label The text shown on the button or alongside it.
   * @param parent The parent widget.
   */
  ButtonWidget(const QString& label, QWidget* parent);

  /**
   * @brief Adds a descriptive label or help text to the widget.
   * @param label The description text.
   */
  void addDescription(const QString& label);

  /**
   * @brief Sets the button icon.
   * @param icon The icon to display.
   */
  void setIcon(const QIcon& icon);

  /**
   * @brief Sets the tooltip shown for the button.
   * @param tooltip The tooltip text.
   */
  void setToolTip(const QString& tooltip);

  /**
   * @brief Sets the text shown in the button.
   * @param text The text to be shown.
   */
  void setText(const QString& text);

signals:
  /**
   * @brief Emitted when the button is activated.
   */
  void valueChanged();

private:
  QPushButton* mInputField;  ///< Push button used by this widget.
};

/**
 * @class ColorWidget
 * @brief A widget for displaying and editing a colour value.
 */
class ColorWidget : public TypedInputWidget<QColor, oclero::qlementine::ColorEditor>
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a colour selection widget.
   * @param label The label shown next to the widget.
   * @param placeholder Additional placeholder or button text.
   * @param parent The parent widget.
   */
  ColorWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent);

  void writeValueToWidget(const QColor& color) override;
signals:
  /**
   * @brief Emitted when the selected colour changes.
   * @param value The new colour value.
   */
  void valueChanged(const QColor& value);
};

/**
 * @class TypeSelectionWidget
 * @brief A specialised combo box for selecting property types.
 */
class TypeSelectionWidget : public QComboBox
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a type selection widget.
   * @param parent The parent widget.
   */
  TypeSelectionWidget(QWidget* parent);

  /**
   * @brief Constructs a type selection widget with an initial value.
   * @param initial The initial selected value.
   * @param parent The parent widget.
   */
  TypeSelectionWidget(const QString& initial, QWidget* parent);

  /**
   * @brief Constructs a type selection widget with an initial value and a last allowed type.
   * @param initial The initial selected value.
   * @param last The last property type to include or allow.
   * @param parent The parent widget.
   */
  TypeSelectionWidget(const QString& initial, Types::PropertyTypes last, QWidget* parent);
};

/**
 * @class SearchWidget
 * @brief A widget providing a search input field with an icon and dismiss support.
 */
class SearchWidget : public QWidget
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a search widget.
   * @param placeholder The placeholder text shown in the search field.
   * @param parent The parent widget.
   */
  SearchWidget(const QString& placeholder, QWidget* parent);

  /**
   * @brief Adds a descriptive label or help text to the widget.
   * @param label The description text.
   */
  void addDescription(const QString& label);

  /**
   * @brief Sets the current search text.
   * @param value The text to set.
   */
  void setValue(const QString& value);

  /**
   * @brief Returns the current search text.
   * @return The current value.
   */
  QString getValue() const;

  /**
   * @brief Returns the underlying line edit used for text input.
   * @return The line edit widget.
   */
  QLineEdit* widget() const;

signals:
  /**
   * @brief Emitted when the search text changes.
   * @param value The new search text.
   */
  void valueChanged(const QString& value);

  /**
   * @brief Emitted when the search widget is dismissed or cleared by the user.
   */
  void dismissed();

private:
  QLineEdit* mInputField;  ///< Line edit used to enter the search text.
  QLabel* mIcon;           ///< Label displaying the search icon.
  QString mValue;          ///< Cached search text.
};

// TODO
// - Enum

}  // namespace maki
