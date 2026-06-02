#include "int_variable.h"

#include "validate.h"

IntegerOrVariableValidator::IntegerOrVariableValidator(int min, int max, QObject* parent)
    : QIntValidator(min, max, parent)
{
}

void IntegerOrVariableValidator::setAcceptVariable(bool accept)
{
  mAcceptVariable = accept;
}

QValidator::State IntegerOrVariableValidator::validate(QString& input, int& pos) const
{
  Q_UNUSED(pos);

  if (input.isEmpty())
    return Intermediate;

  bool ok = false;
  input.toInt(&ok);
  if (ok)
    return Acceptable;

  if (mAcceptVariable)
    return Validate::variable(input);

  return Invalid;
}
