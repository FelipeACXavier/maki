#pragma once

#include <QRegularExpressionValidator>

class NamespaceValidator : public QRegularExpressionValidator
{
public:
  explicit NamespaceValidator(QObject* parent = nullptr);
};
