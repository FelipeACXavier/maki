#pragma once

#include <qvalidator.h>

#include <QValidator>

class DoubleOrVariableValidator : public QDoubleValidator
{
public:
  DoubleOrVariableValidator(double bottom, double top, int decimals, QObject* parent = nullptr);
  void setAcceptVariable(bool accept);
  State validate(QString& input, int& pos) const override;

private:
  bool mAcceptVariable = false;
};