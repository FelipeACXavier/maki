#include "style_helpers.h"

#include <QStyle>
#include <QVariant>

static const char* CLASS = "class";

void addClass(QWidget* widget, const QString& className)
{
  // Append the new class if there is already a class, otherwise just add it
  QVariant variant = widget->property(CLASS);
  QString classList = variant.isValid() ? QStringLiteral("%1 %2").arg(variant.toString(), className) : className;

  widget->setProperty(CLASS, QVariant(classList));
}
