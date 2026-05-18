#pragma once

#include "../base_dialog.h"

namespace maki
{
class TextDialog : public BaseDialog
{
  Q_OBJECT

public:
  TextDialog(const QString& title, const QString& prompt, QWidget* parent = nullptr);

  QString Value() const;

private:
  QLineEdit* mNameEdit = nullptr;
};

QString textPrompt(const QString& title, const QString& name, QWidget* parent = nullptr);
}  // namespace maki