#include "validate.h"

QValidator::State Validate::variable(QString& input)
{
  if (input.isEmpty())
    return QValidator::State::Intermediate;

  if (input.front().isLetter() || input.front() == '_')
    return QValidator::State::Intermediate;

  static const QRegularExpression variableRe(R"(^[A-Za-z_][A-Za-z0-9_]*$)");
  if (variableRe.match(input).hasMatch())
    return QValidator::State::Acceptable;

  return QValidator::State::Invalid;
}
