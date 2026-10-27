#include "label_editor.h"

#include <QKeyEvent>
#include <QToolTip>
#include <QVBoxLayout>
#include <oclero/qlementine/widgets/LineEdit.hpp>

#include "app_configs.h"
#include "notifications.h"
#include "widgets/validators/label_validator.h"

LabelEditor* LabelEditor::create(const QString& placeholder, const QFont& font, QWidget* parent)
{
  auto* editor = new LabelEditor(placeholder, font, parent);
  return editor;
}

LabelEditor::LabelEditor(const QString& placeholder, const QFont& font, QWidget* parent)
    : maki::ControlWidget(parent)
    , mFinished(false)
{
  setAttribute(Qt::WA_TranslucentBackground);
  setAutoFillBackground(false);

  auto* vlayout = new QVBoxLayout(this);
  vlayout->setContentsMargins(0, 0, 0, 0);

  mEditor = new oclero::qlementine::LineEdit(this);
  mEditor->setText(placeholder);
  mEditor->setFont(font);
  mEditor->selectAll();
  mEditor->installEventFilter(this);

  // Create the illusion of editing the text itself (or at least as close as we can)
  mEditor->setFrame(false);
  mEditor->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
  mEditor->setTextMargins(0, 0, 0, 0);
  mEditor->setAutoFillBackground(false);
  mEditor->setAttribute(Qt::WA_TranslucentBackground);

  auto palette = mEditor->palette();
  palette.setBrush(QPalette::Base, Qt::NoBrush);
  mEditor->setPalette(palette);

  // Validation stuff
  auto* validator = new LabelValidator(this);
  mEditor->setValidator(validator);

  // Invalid characters are rejected by widget
  connect(mEditor, &QLineEdit::inputRejected, this, [this]() { showWarning(tr("Only letters, numbers, spaces, and underscores (_) are allowed.")); });

  // Make sure the status is reset on a valid input
  connect(mEditor, &QLineEdit::textEdited, this, [this](const QString& text) {
    if (mEditor->hasAcceptableInput())
      mEditor->setStatus(oclero::qlementine::Status::Default);
  });
  vlayout->addWidget(mEditor);

  setFocusProxy(mEditor);
  setFocusPolicy(Qt::StrongFocus);
}

void LabelEditor::showWarning(const QString& message)
{
  if (mFinished || !mEditor)
    return;

  mEditor->setStatus(oclero::qlementine::Status::Warning);
  NOTIFY_WARNING(Config::APPLICATION_NAME.toStdString(), message.toStdString());
}

void LabelEditor::hideMenu()
{
  if (mFinished || !mEditor)
    return;

  mFinished = true;
  emit dismissed();
  emit finished(mEditor->text());
}

bool LabelEditor::eventFilter(QObject* watched, QEvent* event)
{
  if (watched == mEditor)
  {
    // TODO: This is very hacky but I dont have time to fix it now. We should not have to handle
    // these actions manually. We should also not handle them in the shortcut override
    if (event->type() == QEvent::ShortcutOverride)
    {
      auto* key = static_cast<QKeyEvent*>(event);
      if (key->matches(QKeySequence::Copy))
      {
        mEditor->copy();
        event->accept();
        return true;
      }

      if (key->matches(QKeySequence::Paste))
      {
        mEditor->paste();
        event->accept();
        return true;
      }

      if (key->matches(QKeySequence::Cut))
      {
        mEditor->cut();
        event->accept();
        return true;
      }
    }

    if (event->type() == QEvent::KeyPress)
    {
      auto* key = static_cast<QKeyEvent*>(event);
      if (key->key() == Qt::Key_Escape)
      {
        hideMenu();
        return true;
      }

      if (key->key() == Qt::Key_Return || key->key() == Qt::Key_Enter)
      {
        if (mEditor->hasAcceptableInput())
          hideMenu();
        else if (mEditor->text().trimmed().isEmpty())
          showWarning(tr("Label cannot be empty, press ESC to cancel"));
        else
          showWarning(tr("Label '%1' is reserved").arg(mEditor->text()));

        return true;
      }
    }
  }

  return maki::ControlWidget::eventFilter(watched, event);
}
