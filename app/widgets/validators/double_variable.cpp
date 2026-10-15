#include "double_variable.h"

#include "macros.h"
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

  if (maki::isDouble(input))
    return Acceptable;

  if (mAcceptVariable)
    return Validate::variable(input);

  return Invalid;
}
