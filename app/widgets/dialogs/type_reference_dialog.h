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

class TypeReferenceDialog : public BaseDialog
{
  Q_OBJECT

public:
  TypeReferenceDialog(QWidget* parent = nullptr);

  void setTypeRef(const koda::types::TypeReference& ref);
  koda::types::TypeReference typeRef() const;

private slots:
  void kindChanged(const QString& value);

private:
  enum class Page
  {
    Primitive = 0,
    Named,
    List,
    Map,
    Optional,
  };

  maki::SelectorWidget* mKindCombo = nullptr;
  QStackedWidget* mStack = nullptr;

  QDialogButtonBox* mButtons = nullptr;

  maki::SelectorWidget* mPrimitiveTypeCombo = nullptr;
  koda::types::TypeReference mPrimitiveValue;

  maki::SelectorWidget* mNamedTypeCombo = nullptr;
  koda::types::TypeReference mNamedValue;

  maki::SelectorWidget* mListElementCombo = nullptr;
  koda::types::TypeReference mListElement;

  maki::SelectorWidget* mMapKeyCombo = nullptr;
  maki::SelectorWidget* mMapValueCombo = nullptr;

  koda::types::TypeReference mMapKey;
  koda::types::TypeReference mMapValue;

  maki::SelectorWidget* mOptionalElementCombo = nullptr;
  koda::types::TypeReference mOptionalElement;

  void buildUi();
  void updateText(const koda::types::TypeReference& ref, maki::SelectorWidget* widget);

  struct SelectorButton
  {
    QWidget* container;
    maki::SelectorWidget* selector;
    maki::ButtonWidget* button;
  };

  SelectorButton createTypeSelector(const QString& text, const QString& title, QWidget* parent, koda::types::TypeReference* refValue);
  koda::types::TypeReference editNestedType(const koda::types::TypeReference& current, const QString& title);

  static koda::types::TypeReference defaultNamedType();
};

}  // namespace maki