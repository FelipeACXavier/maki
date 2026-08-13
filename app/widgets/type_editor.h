#pragma once

#include <QCompleter>
#include <QWidget>

#include "typing/types.h"

class QAction;
class QComboBox;
class QLineEdit;
class QPushButton;
class QStackedWidget;
class QTableWidget;
class QTreeWidget;
class QTreeWidgetItem;
class QMouseEvent;

namespace koda::types
{
struct QualifiedName;
class TypeDefinition;
class TypeReference;
}  // namespace koda::types

namespace maki
{

class StringWidget;
struct WidgetAlignment;

class TypeEditor : public QWidget
{
  Q_OBJECT

public:
  TypeEditor(QWidget* parent = nullptr);

private slots:
  // Type management
  void createRecord();
  void createEnum();
  void createAlias();

  void removeCurrentType();
  void applyChanges();

  // Selection
  void currentTypeChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);

  // Record editor
  void addField(const QString& defaultName, const koda::types::TypeReference& defaultValue);
  void removeField();

  // Enum editor
  void addEnumValue(const QString& defaultName, const QString& defaultValue);
  void removeEnumValue();

  // Registry
  void reloadTypes();

protected:
  bool eventFilter(QObject* object, QEvent* event);

private:
  enum class EditorPage
  {
    None = 0,
    Builtin,
    Record,
    Enum,
    Alias
  };

  enum TreeRole
  {
    QualifiedNameRole = Qt::UserRole,
    IdRole = Qt::UserRole + 1,
    RootIdRole = Qt::UserRole + 2,
  };

  QTreeWidget* mTypeTree = nullptr;

  QPushButton* mAddButton = nullptr;
  QPushButton* mDeleteButton = nullptr;
  QPushButton* mApplyButton = nullptr;

  QStackedWidget* mEditorStack = nullptr;

  QTableWidget* mFieldsTable = nullptr;
  QPushButton* mAddFieldButton = nullptr;
  QPushButton* mRemoveFieldButton = nullptr;

  QComboBox* mEnumBackingCombo = nullptr;
  QTableWidget* mEnumTable = nullptr;
  QPushButton* mAddEnumValueButton = nullptr;
  QPushButton* mRemoveEnumValueButton = nullptr;

  QString mSelectedQualifiedName;
  bool mSelectedBuiltin = false;
  bool mReloading = false;

  QTreeWidgetItem* mBuiltinRoot = nullptr;
  QTreeWidgetItem* mProjectRoot = nullptr;

  void buildUi();
  QWidget* createBuiltinPage();
  QWidget* createRecordPage();
  QWidget* createEnumPage();
  QWidget* createAliasPage();

  void clearEditor();
  void showDefinition(const koda::types::TypeDefinition& definition);
  koda::types::TypeDefinition readDefinitionFromUi() const;

  std::string createUniqueTypeName(const std::string& baseName) const;
  std::string getIdFromItem(const QString& qualifiedName) const;

  QTreeWidgetItem* findTreeItem(const QString& qualifiedName) const;
  QTreeWidgetItem* findItemByRole(QTreeWidgetItem* parent, int role, const QVariant& value) const;

  void selectType(const QString& qualifiedName);

  QTableWidget* createTable(QWidget* parent, const QStringList& headers);
  void createDefinition(const koda::types::TypeDefinition& definition);
  maki::StringWidget* createNamespaceEdit(maki::WidgetAlignment& alignment, QWidget* parent) const;
  QIcon typeToIcon(const koda::types::TypeDefinition& type) const;

  void populateEnumKind(QComboBox* widget) const;
  void populateExtentKind(QComboBox* widget, const koda::types::QualifiedName* currentName = nullptr) const;

  void validateEnumValues();
  bool validateEnumValue(int row);
  bool isValidEnumValue(const QString& value, koda::types::EnumUnderlyingKind type) const;
};

}  // namespace maki