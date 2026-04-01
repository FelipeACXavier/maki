#pragma once

#include <QComboBox>
#include <QWidget>

#include "style_helpers.h"

class QLineEdit;
class QSpinBox;
class QPushButton;
class QLabel;
class QCheckBox;
class QFormLayout;

namespace maki
{
struct WidgetAlignment
{
  enum class Type
  {
    UNKNOWN = 0,
    INLINE,
    VERTICAL,
    FORM
  } type;

  QFormLayout* layout;
};

class GridGroup : public QWidget
{
public:
  GridGroup(const QString& label, int rows, int cols, QWidget* parent);

  void addWidget(QWidget* widget);
  void addLayout(QLayout* layout);

  QWidget* widget() const;

private:
  QWidget* mContent;
  const int mRows;
  const int mCols;

  int mCurrentRow;
  int mCurrentCol;
};

class WidgetGroup : public QWidget
{
public:
  WidgetGroup(const QString& label, QWidget* parent);

  void addWidget(QWidget* widget);
  void addLayout(QLayout* layout);
  void addSpacing(int spacing);
  void addStretch();
};

class BooleanWidget : public QWidget
{
  Q_OBJECT
public:
  BooleanWidget(const QString& label, bool value, WidgetAlignment alignment, QWidget* parent);

  void addDescription(const QString& label);
  void setValue(const bool value);
  bool getValue() const;

signals:
  void valueChanged(const bool value);

private:
  QCheckBox* mInputField;
  bool mValue = false;
};

class StringWidget : public QWidget
{
  Q_OBJECT
public:
  StringWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent);

  void addDescription(const QString& label);
  void setValue(const QString& value);
  QString getValue() const;
  QLineEdit* widget() const;

signals:
  void valueChanged(const QString& value);

private:
  QLineEdit* mInputField;
  QString mValue = "";
};

class IntegerWidget : public QWidget
{
  Q_OBJECT
public:
  IntegerWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent, int min = INT32_MIN, int max = INT32_MAX);

  void addDescription(const QString& label);
  void setValue(const int value);
  int getValue() const;

signals:
  void valueChanged(const int value);

private:
  QLineEdit* mInputField;
  int mValue = 0;
};

class FloatWidget : public QWidget
{
  Q_OBJECT
public:
  FloatWidget(const QString& label, const QString& placeholder, WidgetAlignment alignment, QWidget* parent, qreal min = std::numeric_limits<qreal>::min(), qreal max = std::numeric_limits<qreal>::max());

  void addDescription(const QString& label);
  void setValue(const qreal value);
  qreal getValue() const;

signals:
  void valueChanged(const qreal value);

private:
  QLineEdit* mInputField;
  qreal mValue = 0;
};

class SpinWidget : public QWidget
{
  Q_OBJECT
public:
  SpinWidget(const QString& label, int placeholder, QWidget* parent, int min, int max);

  void addDescription(const QString& label);
  void setValue(const int value);
  int getValue() const;

  void setSuffix(const QString& suffix);

signals:
  void valueChanged(const int value);

private:
  QSpinBox* mInputField;
  int mValue = 0;
};

class SelectorWidget : public QWidget
{
  Q_OBJECT
public:
  SelectorWidget(const QString& label, QWidget* parent);
  SelectorWidget(const QString& label, QComboBox* comboBox, QWidget* parent);

  void addDescription(const QString& label);
  void setValue(const QString& value);
  QString getValue() const;

  QComboBox* widget() const;

  void addItem(const QString& name, const QString& value);

signals:
  void valueChanged(const QString& value);

private:
  QComboBox* mInputField;
  QString mValue = 0;
};

class ButtonWidget : public QWidget
{
  Q_OBJECT
public:
  ButtonWidget(const QString& label, QWidget* parent);

  void addDescription(const QString& label);
  void setIcon(const QIcon& icon);
  void setToolTip(const QString& tooltip);

signals:
  void valueChanged();

private:
  QPushButton* mInputField;
};

class ColorWidget : public QWidget
{
  Q_OBJECT
public:
  ColorWidget(const QString& label, const QString& placeholder, QWidget* parent);

  void addDescription(const QString& label);
  void setIcon(const QIcon& icon);
  void setToolTip(const QString& tooltip);

  QColor getValue() const;
  void setValue(const QColor& color);

  QString getLabel() const;
  QPushButton* widget() const;

signals:
  void valueChanged(const QColor& value);

protected:
  void resizeEvent(QResizeEvent* event) override;

private:
  QLabel* mLabel;
  QString mFullLabel;

  QLabel* mPreview;
  QPushButton* mButton;
  QColor mValue;

  void updateElidedLabel();
};

class TypeSelectionWidget : public QComboBox
{
  Q_OBJECT
public:
  TypeSelectionWidget(QWidget* parent);
  TypeSelectionWidget(const QString& initial, QWidget* parent);
  TypeSelectionWidget(const QString& initial, Types::PropertyTypes last, QWidget* parent);
};

class SearchWidget : public QWidget
{
  Q_OBJECT
public:
  SearchWidget(const QString& placeholder, QWidget* parent);

  void addDescription(const QString& label);
  void setValue(const QString& value);
  QString getValue() const;

  QLineEdit* widget() const;
  WidgetWithIcon icon() const;

signals:
  void valueChanged(const QString& value);
  void dismissed();

private:
  QLineEdit* mInputField;
  QLabel* mIcon;
  QString mValue;
};

// TODO
// - Enum

}  // namespace maki
