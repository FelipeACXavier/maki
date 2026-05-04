#pragma once

#include <QPushButton>
#include <oclero/qlementine/widgets/StatusBadgeWidget.hpp>

#include "../base_dialog.h"

namespace maki
{

class PromptDialog : public BaseDialog
{
  Q_OBJECT
public:
  PromptDialog(const QString& title, const QString& accept, const QString& reject,
               const QString& message, QWidget* parent = nullptr);

  void setType(oclero::qlementine::StatusBadge type);
  void setExtraInfo(const QString& info);

private:
  oclero::qlementine::StatusBadgeWidget* mBadge;
};

bool confirmationPrompt(const QString& message, const QString& extraInfo = "", QWidget* parent = nullptr);
bool warningPrompt(const QString& message, const QString& extraInfo = "", QWidget* parent = nullptr);
bool errorPrompt(const QString& message, const QString& extraInfo = "", QWidget* parent = nullptr);

}  // namespace maki
