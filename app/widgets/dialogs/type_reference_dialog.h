#pragma once

#include "../base_dialog.h"
#include "typing/type_reference.h"

class QComboBox;
class QDialogButtonBox;
class QLabel;
class QPushButton;
class QStackedWidget;

namespace maki
{

class SelectorWidget;
class ButtonWidget;
class TypeSelector;

class TypeReferenceDialog : public BaseDialog
{
  Q_OBJECT

public:
  TypeReferenceDialog(const QString& title, QWidget* parent = nullptr);

  void setTypeRef(const koda::types::TypeReference& ref);
  koda::types::TypeReference typeRef() const;

private slots:
  void kindChanged(const QString& value);

private:
  QStackedWidget* mStack = nullptr;
  maki::SelectorWidget* mKindCombo = nullptr;

  void buildUi();
  int kindToIndex(koda::types::TypeReferenceKind type) const;

  static koda::types::TypeReference defaultNamedType();
};

}  // namespace maki