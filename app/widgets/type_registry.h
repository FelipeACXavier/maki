#pragma once

#include <QObject>
#include <QString>
#include <QVector>
#include <memory>

#include "typing/type_registry.h"

namespace maki
{

class TypeRegistry : public QObject, public koda::types::TypeRegistry
{
  Q_OBJECT

public:
  static TypeRegistry& instance();

  TypeRegistry(const TypeRegistry&) = delete;
  TypeRegistry& operator=(const TypeRegistry&) = delete;

  QStringList allTypeNames() const;
  QStringList userTypeNames() const;
  QStringList builtinTypeNames() const;

  koda::types::TypeRegistrationResult add(const koda::types::TypeDefinition& definition) override;
  koda::types::TypeRegistrationResult replace(const koda::types::TypeDefinition& definition) override;
  koda::types::TypeRegistrationResult remove(const std::string& qualifiedName);

signals:
  void registryChanged();
  void typeAdded(const QString& qualifiedName);
  void typeRemoved(const QString& qualifiedName);
  void typeChanged(const QString& qualifiedName);

private:
  TypeRegistry(QObject* parent = nullptr);
};

}  // namespace maki