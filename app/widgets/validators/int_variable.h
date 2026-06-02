#pragma once

#include <QValidator>

class IntegerOrVariableValidator : public QIntValidator
{
public:
  IntegerOrVariableValidator(int min, int max, QObject* parent = nullptr);
  void setAcceptVariable(bool accept);
  State validate(QString& input, int& pos) const override;

private:
  bool mAcceptVariable = false;
};