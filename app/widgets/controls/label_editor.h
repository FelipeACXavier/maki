#pragma once

#include <QString>
#include <QWidget>

#include "control_widget.h"

namespace oclero::qlementine
{
class LineEdit;
}

class LabelEditor : public maki::ControlWidget
{
  Q_OBJECT

public:
  static LabelEditor* create(const QString& placeholder, const QFont& font, QWidget* parent = nullptr);

  explicit LabelEditor(const QString& placeholder, const QFont& font, QWidget* parent = nullptr);

  std::function<void(const QString& text)> finished;

protected:
  bool eventFilter(QObject* watched, QEvent* event) override;

private:
  bool mFinished;
  oclero::qlementine::LineEdit* mEditor;

  void showWarning(const QString& message);
  void hideMenu();
};
