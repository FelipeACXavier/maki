#pragma once

#include <QCompleter>
#include <QWidget>

#include "type_registry.h"

class QAction;
class QComboBox;
class QLineEdit;
class QPushButton;
class QStackedWidget;
class QTableWidget;
class QTreeWidget;
class QTreeWidgetItem;

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
  void addField(const QString& defaultName, const QString& defaultValue);
  void removeField();

  // Enum editor
  void addEnumValue(const QString& defaultName, const QString& defaultValue);
  void removeEnumValue();

  // Registry
  void reloadTypes();

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

  // --------------------------------------------------------------------------
  // General UI
  // --------------------------------------------------------------------------

  QTreeWidget* mTypeTree = nullptr;

  QPushButton* mAddButton = nullptr;
  QPushButton* mDeleteButton = nullptr;
  QPushButton* mApplyButton = nullptr;

  QStackedWidget* mEditorStack = nullptr;

  // --------------------------------------------------------------------------
  // Record page
  // --------------------------------------------------------------------------

  QTableWidget* mFieldsTable = nullptr;
  QPushButton* mAddFieldButton = nullptr;
  QPushButton* mRemoveFieldButton = nullptr;

  // --------------------------------------------------------------------------
  // Enum page
  // --------------------------------------------------------------------------

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

  void showDefinition(const koda::types::TypeDefinition& definition);

  void clearEditor();

  koda::types::TypeDefinition readDefinitionFromUi() const;

  std::string createUniqueTypeName(const std::string& baseName) const;
  std::string getIdFromItem(const QString& qualifiedName) const;

  QTreeWidgetItem* findTreeItem(const QString& qualifiedName) const;
  QTreeWidgetItem* findItemByRole(QTreeWidgetItem* parent, int role, const QVariant& value) const;

  void selectType(const QString& qualifiedName);

  QComboBox* createComboBox(QWidget* parent, const QString& defaultValue) const;
  QTableWidget* createTable(QWidget* parent) const;
  void createDefinition(const koda::types::TypeDefinition& definition);
  maki::StringWidget* createNamespaceEdit(maki::WidgetAlignment& alignment, QWidget* parent) const;
  void addCompleter(const QStringList& items, QWidget* parent) const;
  QIcon typeToIcon(const koda::types::TypeDefinition& type) const;

  void populateEnumKind(QComboBox* widget) const;
  void populateExtentKind(QComboBox* widget, const koda::types::QualifiedName* currentName = nullptr) const;
  void populateTypes(QComboBox* widget, const koda::types::QualifiedName* currentName = nullptr) const;
};

}  // namespace maki