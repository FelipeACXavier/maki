#include "namespace_validator.h"

NamespaceValidator::NamespaceValidator(QObject* parent)
    : QRegularExpressionValidator(QRegularExpression(R"(^[A-Za-z_][A-Za-z_]*(::[A-Za-z_][A-Za-z_]*)*$)"), parent)
{
}