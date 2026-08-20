#pragma once

#include <QWidget>

#include "typing/type_reference.h"

class QComboBox;

namespace maki
{

class TypeSelector : public QWidget
{
  Q_OBJECT
public:
  TypeSelector(const QString& objectName, QWidget* parent = nullptr);

  koda::types::TypeReference getReference() const;
  void setReference(const koda::types::TypeReference& reference);

  void setPrimitive();
  void setEditorTitle(const QString& title);
  void populateTypes(const koda::types::QualifiedName* currentName = nullptr);

  QList<QWidget*> focusWidgets() const;

signals:
  void typeChanged(const koda::types::TypeReference& reference);

protected:
  void focusInEvent(QFocusEvent* event) override;

private:
  QComboBox* mSelector;
  bool mUsesPrimitives;
  QString mTitle;

  QComboBox* createComboBox();
};

};  // namespace maki
