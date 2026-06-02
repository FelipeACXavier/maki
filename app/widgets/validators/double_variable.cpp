#include "double_variable.h"

#include "validate.h"

DoubleOrVariableValidator::DoubleOrVariableValidator(double bottom, double top, int decimals, QObject* parent)
    : QDoubleValidator(bottom, top, decimals, parent)
{
}

void DoubleOrVariableValidator::setAcceptVariable(bool accept)
{
  mAcceptVariable = accept;
}

QValidator::State DoubleOrVariableValidator::validate(QString& input, int& pos) const
{
  Q_UNUSED(pos);

  if (input.isEmpty())
    return Intermediate;

  bool ok = false;
  input.toDouble(&ok);
  if (ok)
    return Acceptable;

  if (mAcceptVariable)
    return Validate::variable(input);

  return Invalid;
}
