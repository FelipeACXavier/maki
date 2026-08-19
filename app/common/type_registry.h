#pragma once

#include <QObject>
#include <QString>
#include <QVector>

#include "config.h"
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

  QStringList libraries() const;
  VoidResult loadFromLibrary(const JSON& json);

  VoidResult registerNode(const QString& nodeId, const NodeConfig& node);
  QStringList findProducers(const koda::types::TypeReference& type) const;
  QStringList findConsumers(const koda::types::TypeReference& type) const;

  koda::types::TypeRegistrationResult add(const koda::types::TypeDefinition& definition) override;
  koda::types::TypeRegistrationResult replace(const koda::types::TypeDefinition& definition) override;
  koda::types::TypeRegistrationResult remove(const std::string& qualifiedName);

  bool isFromLibrary(const koda::types::QualifiedName& name) const;

signals:
  void registryChanged();
  void typeAdded(const koda::types::TypeDefinition& def);
  void typeRemoved(const koda::types::TypeDefinition& def);
  void typeChanged(const koda::types::TypeDefinition& def);

private:
  QSet<std::string> mLibraries{};

  // TODO: I am not sure this is the place for it
  QMap<QString, QStringList> mConsumers{};
  QMap<QString, QStringList> mProducers{};

  TypeRegistry(QObject* parent = nullptr);
};

}  // namespace maki
