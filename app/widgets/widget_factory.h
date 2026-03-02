#pragma once

#include <qcombobox.h>
#include <qpagesize.h>
#include <qpushbutton.h>

#include <QWidget>

class QLineEdit;
class QSpinBox;
class QComboBox;
class QPushButton;
class QLabel;
class QCheckBox;

namespace maki
{
class WidgetGroup : public QWidget
{
public:
  WidgetGroup(const QString& label, QWidget* parent);

  void addWidget(QWidget* widget);
};

class BooleanWidget : public QWidget
{
  Q_OBJECT
public:
  BooleanWidget(const QString& label, bool value, QWidget* parent);

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
  StringWidget(const QString& label, const QString& placeholder, QWidget* parent);

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
  IntegerWidget(const QString& label, const QString& placeholder, QWidget* parent, int min = INT32_MIN, int max = INT32_MAX);

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
  FloatWidget(const QString& label, const QString& placeholder, QWidget* parent, qreal min = std::numeric_limits<qreal>::min(), qreal max = std::numeric_limits<qreal>::max());

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
  QPushButton* widget() const;

signals:
  void valueChanged(const QColor& value);

private:
  QLabel* mPreview;
  QPushButton* mButton;
  QColor mValue;
};

class TypeSelectionWidget : public QComboBox
{
  Q_OBJECT
public:
  TypeSelectionWidget(QWidget* parent);
  TypeSelectionWidget(const QString& initial, QWidget* parent);
};

// TODO
// - Enum

}  // namespace maki
