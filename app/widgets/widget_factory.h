#pragma once

#include <QWidget>

class QLineEdit;

namespace maki
{
class BooleanWidget : public QWidget
{
  Q_OBJECT
public:
  BooleanWidget(const QString& label, bool value, QWidget* parent);

  void addDescription(const QString& label);

signals:
  void valueChanged(const bool value);
};

class IntegerWidget : public QWidget
{
  Q_OBJECT
public:
  IntegerWidget(const QString& label, const QString& placeholder, QWidget* parent, int min = INT32_MIN, int max = INT32_MAX);

  void addDescription(const QString& label);

signals:
  void valueChanged(const int value);

private:
  QLineEdit* mInputField;
};

// TODO
// - String
// - Float
// - Button
// - Color
// - Enum
// - Selection

}  // namespace maki