#include "validate.h"

static const QRegularExpression VARIABLE_REGEX(R"(^[A-Za-z_][A-Za-z0-9_]*$)");

QValidator::State Validate::variable(QString& input)
{
  if (input.isEmpty())
    return QValidator::State::Intermediate;

  if (input.front().isLetter() || input.front() == '_')
    return QValidator::State::Intermediate;

  if (VARIABLE_REGEX.match(input).hasMatch())
    return QValidator::State::Acceptable;

  return QValidator::State::Invalid;
}
