#pragma once

#include <QValidator>

class Validate
{
public:
  static QValidator::State variable(QString& input);
};
