#pragma once

#include <QWidget>

#include "widgets/frame.h"

class QPushButton;
class QLabel;

namespace oclero::qlementine
{
class Label;
}

class TourBubble : public StyledFrame
{
  Q_OBJECT
public:
  explicit TourBubble(QWidget* parent = nullptr);

  void setContent(const QString& title, const QString& text, int currentStep, int totalSteps, bool lastStep, bool showBack, bool showNext);

  QPushButton* nextButton() const;
  QPushButton* backButton() const;
  QPushButton* cancelButton() const;

private:
  oclero::qlementine::Label* m_title;
  oclero::qlementine::Label* m_text;
  oclero::qlementine::Label* m_stepLabel;
  QPushButton* m_back;
  QPushButton* m_next;
  QPushButton* m_cancel;
};
