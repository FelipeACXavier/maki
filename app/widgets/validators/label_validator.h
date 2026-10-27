#pragma once

#include <QValidator>

class LabelValidator : public QValidator
{
public:
  LabelValidator(QObject* parent = nullptr);
  State validate(QString& input, int& pos) const override;
};
