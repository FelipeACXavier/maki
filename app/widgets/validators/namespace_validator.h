#pragma once

#include <QRegularExpressionValidator>

class NamespaceValidator : public QRegularExpressionValidator
{
public:
  NamespaceValidator(QObject* parent = nullptr);
};