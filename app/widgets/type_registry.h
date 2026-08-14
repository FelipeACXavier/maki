#pragma once

#include <QObject>
#include <QString>
#include <QVector>

#include "json.h"
#include "result.h"
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
  QStringList namespaces() const;

  VoidResult loadFromLibrary(const JSON& json);

  koda::types::TypeRegistrationResult add(const koda::types::TypeDefinition& definition) override;
  koda::types::TypeRegistrationResult replace(const koda::types::TypeDefinition& definition) override;
  koda::types::TypeRegistrationResult remove(const std::string& qualifiedName);

signals:
  void registryChanged();
  void typeAdded(const koda::types::TypeDefinition& def);
  void typeRemoved(const koda::types::TypeDefinition& def);
  void typeChanged(const koda::types::TypeDefinition& def);

private:
  TypeRegistry(QObject* parent = nullptr);
};

}  // namespace maki
