#include "widgets/validators/label_validator.h"

static const QStringList RESERVED_NAMES = {"Task", "Capability"};
static const QRegularExpression LABEL_REGEX(R"(^[\p{L}\p{N} _]+$)");

LabelValidator::LabelValidator(QObject* parent)
    : QValidator(parent)
{
}

QValidator::State LabelValidator::validate(QString& input, int& pos) const
{
  Q_UNUSED(pos);

  if (input.trimmed().isEmpty())
    return Intermediate;

  if (!LABEL_REGEX.match(input).hasMatch())
    return Invalid;

  if (RESERVED_NAMES.contains(input.trimmed(), Qt::CaseInsensitive))
    return Intermediate;

  return Acceptable;
}
