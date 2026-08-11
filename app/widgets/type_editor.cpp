#include "type_editor.h"

#include <QComboBox>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSplitter>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTreeWidget>
#include <QUuid>
#include <QVBoxLayout>

// #include "type_ref_dialog.h"
#include "app_configs.h"
#include "logging.h"

namespace maki
{

// =============================================================================
// Construction
// =============================================================================

TypeEditor::TypeEditor(QWidget* parent)
    : QWidget(parent)
{
  buildUi();

  auto& registry = TypeRegistry::instance();

  connect(&registry, &TypeRegistry::registryChanged, this, &TypeEditor::reloadTypes);
  connect(mTypeTree, &QTreeWidget::currentItemChanged, this, &TypeEditor::currentTypeChanged);
  connect(mDeleteButton, &QPushButton::clicked, this, &TypeEditor::removeCurrentType);
  connect(mApplyButton, &QPushButton::clicked, this, &TypeEditor::applyChanges);
  connect(mAddFieldButton, &QPushButton::clicked, this, &TypeEditor::addField);
  connect(mRemoveFieldButton, &QPushButton::clicked, this, &TypeEditor::removeField);
  connect(mAddEnumValueButton, &QPushButton::clicked, this, &TypeEditor::addEnumValue);
  connect(mRemoveEnumValueButton, &QPushButton::clicked, this, &TypeEditor::removeEnumValue);

  reloadTypes();
}

// =============================================================================
// Creation
// =============================================================================

void TypeEditor::createRecord()
{
  auto toadd = koda::types::TypeDefinition::createRecord(createUniqueTypeName("Record"), {}, "", QUuid::createUuid().toString().toStdString());
  auto result = TypeRegistry::instance().add(toadd);
  if (!result.IsSuccess())
    LOG_WARNING(result.ErrorMessage());

  showDefinition(toadd);
}

void TypeEditor::createEnum()
{
  auto toadd = koda::types::TypeDefinition::createEnum(createUniqueTypeName("Enum"), koda::types::EnumUnderlyingKind::Int32, {},
                                                       QUuid::createUuid().toString().toStdString());
  auto result = TypeRegistry::instance().add(toadd);
  if (!result.IsSuccess())
    LOG_WARNING(result.ErrorMessage());

  showDefinition(toadd);
}

void TypeEditor::createAlias()
{
  auto toadd =
      koda::types::TypeDefinition::createAlias(createUniqueTypeName("Alias"), koda::types::IntegerType, QUuid::createUuid().toString().toStdString());
  auto result = TypeRegistry::instance().add(toadd);
  if (!result.IsSuccess())
    LOG_WARNING(result.ErrorMessage());

  showDefinition(toadd);
}

void TypeEditor::removeCurrentType()
{
  const auto answer = QMessageBox::question(this, tr("Remove type"), tr("Are you sure you want to remove '%1'?").arg(mSelectedQualifiedName));
  if (answer != QMessageBox::Yes)
    return;

  auto removed = TypeRegistry::instance().remove(getIdFromItem(mSelectedQualifiedName));
  if (!removed.IsSuccess())
  {
    LOG_WARNING(removed.ErrorMessage());
    return;
  }

  clearEditor();
}

void TypeEditor::applyChanges()
{
  LOG_DEBUG("Applying changes: reading from UI {}", mSelectedQualifiedName);
  koda::types::TypeDefinition definition = readDefinitionFromUi();
  if (definition.name.toString().empty())
  {
    LOG_WARNING("{}. {}.", tr("Invalid type"), tr("The type must have a name"));
    return;
  }

  LOG_DEBUG("Applying changes: replacing in registry");
  auto updated = TypeRegistry::instance().replace(definition);
  if (!updated)
  {
    LOG_WARNING(updated.ErrorMessage());
    return;
  }

  LOG_DEBUG("Applying changes: selecting new {}", definition.name.toString());
  selectType(mSelectedQualifiedName);
}

void TypeEditor::currentTypeChanged(QTreeWidgetItem* current, QTreeWidgetItem* /* previous */)
{
  if (mReloading || current == nullptr)
    return;

  // Group headers have no qualified-name role.
  const QString qualifiedName = current->data(0, QualifiedNameRole).toString();
  if (qualifiedName.isEmpty())
  {
    clearEditor();
    return;
  }

  const koda::types::TypeDefinition* definition = TypeRegistry::instance().findByName(qualifiedName.toStdString());
  if (definition == nullptr)
  {
    LOG_DEBUG("No definition found for {}", qualifiedName);
    clearEditor();
    return;
  }

  LOG_INFO("currentTypeChanged: {}", qualifiedName);

  showDefinition(*definition);
}

void TypeEditor::showDefinition(const koda::types::TypeDefinition& definition)
{
  TypeRegistry::instance().print();

  // Since we can only show what we select, it makes sense to update this here
  mSelectedQualifiedName = QString::fromStdString(definition.name.toString());

  if (definition.isRecord())
  {
    mEditorStack->setCurrentIndex(static_cast<int>(EditorPage::Record));

    auto* page = mEditorStack->widget(static_cast<int>(EditorPage::Record));
    auto* nameEdit = page->findChild<QLineEdit*>("recordNameEdit");
    auto* namespaceEdit = page->findChild<QLineEdit*>("recordNamespaceEdit");

    nameEdit->setText(QString::fromStdString(definition.name.name));
    namespaceEdit->setText(QString::fromStdString(definition.name.namespaceString()));

    const auto def = definition.record();
    if (def.baseType.has_value())
    {
      definition.print();
      const int baseIndex = mBaseTypeCombo->findData(QString::fromStdString(def.baseType.value().namedType().name.name));
      if (baseIndex >= 0)
        mBaseTypeCombo->setCurrentIndex(baseIndex);
      else
        mBaseTypeCombo->setCurrentIndex(0);
    }
    else
    {
      mBaseTypeCombo->setCurrentIndex(0);
    }

    mFieldsTable->setRowCount(0);
    for (const auto& field : def.fields)
    {
      const int row = mFieldsTable->rowCount();
      mFieldsTable->insertRow(row);
      auto* nameItem = new QTableWidgetItem(QString::fromStdString(field.name));

      auto* container = new QWidget(mFieldsTable);
      auto* layout = new QHBoxLayout(container);
      layout->setContentsMargins(0, 0, 0, 0);
      layout->setSpacing(0);

      auto* typeCombo = new QComboBox(container);
      typeCombo->setObjectName("typeCombo");
      const auto typeNames = maki::TypeRegistry::instance().allTypeNames();
      typeCombo->addItems(typeNames);

      // Default to String if available.
      const int stringIndex = typeCombo->findText(QString::fromStdString(field.type.toString()));
      if (stringIndex >= 0)
        typeCombo->setCurrentIndex(stringIndex);

      typeCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      layout->addWidget(typeCombo);

      mFieldsTable->setItem(row, 0, nameItem);
      mFieldsTable->setCellWidget(row, 1, container);
    }
  }
  else if (definition.isEnum())
  {
    mEditorStack->setCurrentIndex(static_cast<int>(EditorPage::Enum));

    auto* page = mEditorStack->widget(static_cast<int>(EditorPage::Enum));
    auto* nameEdit = page->findChild<QLineEdit*>("enumNameEdit");
    auto* namespaceEdit = page->findChild<QLineEdit*>("enumNamespaceEdit");

    nameEdit->setText(QString::fromStdString(definition.name.name));
    namespaceEdit->setText(QString::fromStdString(definition.name.namespaceString()));

    const auto def = definition.enumeration();
    mEnumBackingCombo->setCurrentIndex(def.underlyingType == koda::types::EnumUnderlyingKind::String ? 0 : 1);
    mEnumTable->setRowCount(0);
    for (const auto& value : def.values)
    {
      const int row = mEnumTable->rowCount();
      mEnumTable->insertRow(row);
      mEnumTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(value.name)));
      mEnumTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(value.value.value_or(""))));
    }
  }
  else if (definition.isAlias())
  {
    mEditorStack->setCurrentIndex(static_cast<int>(EditorPage::Alias));

    auto* page = mEditorStack->widget(static_cast<int>(EditorPage::Alias));
    auto* nameEdit = page->findChild<QLineEdit*>("aliasNameEdit");
    auto* namespaceEdit = page->findChild<QLineEdit*>("aliasNamespaceEdit");
    auto* aliasEdit = page->findChild<QComboBox*>("aliasTargetEdit");

    nameEdit->setText(QString::fromStdString(definition.name.name));
    namespaceEdit->setText(QString::fromStdString(definition.name.namespaceString()));
    aliasEdit->setCurrentText(QString::fromStdString(definition.alias().target.toString()));
  }
  else if (definition.isPrimitive())
  {
    mEditorStack->setCurrentIndex(static_cast<int>(EditorPage::Builtin));

    auto* page = mEditorStack->widget(static_cast<int>(EditorPage::Builtin));
    auto* nameEdit = page->findChild<QLineEdit*>("builtinNameEdit");
    auto* kindEdit = page->findChild<QLineEdit*>("builtinKindEdit");

    nameEdit->setText(QString::fromStdString(definition.name.toString()));
    kindEdit->setText(QString::fromStdString(koda::types::toString(definition.primitive().primitive)));

    mDeleteButton->setEnabled(false);
    mApplyButton->setEnabled(false);
    return;
  }

  mDeleteButton->setEnabled(true);
  mApplyButton->setEnabled(true);
}

// =============================================================================
// Record fields
void TypeEditor::addField()
{
  const int row = mFieldsTable->rowCount();
  mFieldsTable->insertRow(row);

  // Field name
  auto* nameItem = new QTableWidgetItem(tr("field%1").arg(row + 1));

  // Container fills the entire table cell
  auto* container = new QWidget(mFieldsTable);

  auto* layout = new QHBoxLayout(container);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  // Type combo
  auto* typeCombo = new QComboBox(container);
  typeCombo->setObjectName("typeCombo");
  const auto typeNames = maki::TypeRegistry::instance().allTypeNames();
  typeCombo->addItems(typeNames);

  // Default to String if available.
  const int stringIndex = typeCombo->findText(QStringLiteral("Integer"));

  if (stringIndex >= 0)
    typeCombo->setCurrentIndex(stringIndex);

  typeCombo->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  layout->addWidget(typeCombo);

  mFieldsTable->setItem(row, 0, nameItem);
  mFieldsTable->setCellWidget(row, 1, container);
  mFieldsTable->setCurrentCell(row, 0);
}

void TypeEditor::removeField()
{
  const int row = mFieldsTable->currentRow();

  if (row < 0)
    return;

  mFieldsTable->removeRow(row);
}

// =============================================================================
// Enum values
void TypeEditor::addEnumValue()
{
  const int row = mEnumTable->rowCount();

  mEnumTable->insertRow(row);
  mEnumTable->setItem(row, 0, new QTableWidgetItem(tr("Value%1").arg(row + 1)));
  mEnumTable->setItem(row, 1, new QTableWidgetItem());
  mEnumTable->setCurrentCell(row, 0);
}

void TypeEditor::removeEnumValue()
{
  const int row = mEnumTable->currentRow();

  if (row < 0)
    return;

  mEnumTable->removeRow(row);
}

// =============================================================================
// UI stuff
void TypeEditor::buildUi()
{
  auto* mainLayout = new QVBoxLayout(this);

  auto* splitter = new QSplitter(Qt::Horizontal, this);
  mainLayout->addWidget(splitter, 1);

  // --------------------------------------------------------------------------
  // Left side: type browser
  auto* browserWidget = new QWidget(splitter);
  auto* browserLayout = new QVBoxLayout(browserWidget);
  browserLayout->setContentsMargins(0, 0, 0, 0);

  mTypeTree = new QTreeWidget(browserWidget);
  mTypeTree->setHeaderHidden(true);
  mTypeTree->setRootIsDecorated(true);

  browserLayout->addWidget(mTypeTree, 1);

  auto* browserButtons = new QHBoxLayout();
  mAddButton = new QPushButton(tr("Add"), browserWidget);
  mDeleteButton = new QPushButton(tr("Remove"), browserWidget);

  browserButtons->addWidget(mAddButton);
  browserButtons->addWidget(mDeleteButton);

  browserLayout->addLayout(browserButtons);

  // Add menu
  auto* addMenu = new QMenu(mAddButton);
  auto* recordAction = addMenu->addAction(tr("Record"));
  auto* enumAction = addMenu->addAction(tr("Enum"));
  auto* aliasAction = addMenu->addAction(tr("Alias"));

  mAddButton->setMenu(addMenu);

  connect(recordAction, &QAction::triggered, this, &TypeEditor::createRecord);
  connect(enumAction, &QAction::triggered, this, &TypeEditor::createEnum);
  connect(aliasAction, &QAction::triggered, this, &TypeEditor::createAlias);

  // --------------------------------------------------------------------------
  // Right side: editor
  auto* editorWidget = new QWidget(splitter);
  auto* editorLayout = new QVBoxLayout(editorWidget);
  editorLayout->setContentsMargins(0, 0, 0, 0);

  mEditorStack = new QStackedWidget(editorWidget);

  mEditorStack->addWidget(new QWidget(mEditorStack));  // Empty when nothing is selected
  mEditorStack->addWidget(createBuiltinPage());
  mEditorStack->addWidget(createRecordPage());
  mEditorStack->addWidget(createEnumPage());
  mEditorStack->addWidget(createAliasPage());

  editorLayout->addWidget(mEditorStack);

  auto* editorButtons = new QHBoxLayout();
  editorButtons->addStretch();

  mApplyButton = new QPushButton(tr("Apply"), editorWidget);

  editorButtons->addWidget(mApplyButton);

  editorLayout->addLayout(editorButtons);

  // --------------------------------------------------------------------------
  // Add both to the splitter
  splitter->addWidget(browserWidget);
  splitter->addWidget(editorWidget);

  splitter->setStretchFactor(0, 0);
  splitter->setStretchFactor(1, 1);

  mDeleteButton->setEnabled(false);
  mApplyButton->setEnabled(false);
}

QWidget* TypeEditor::createBuiltinPage()
{
  auto* page = new QWidget(mEditorStack);
  auto* layout = new QFormLayout(page);

  auto* nameEdit = new QLineEdit(page);
  auto* kindEdit = new QLineEdit(page);

  nameEdit->setObjectName("builtinNameEdit");
  kindEdit->setObjectName("builtinKindEdit");

  nameEdit->setReadOnly(true);
  kindEdit->setReadOnly(true);

  layout->addRow(tr("Name"), nameEdit);
  layout->addRow(tr("Kind"), kindEdit);

  auto* note = new QLabel(tr("Built-in types are provided by KODA and cannot be modified."), page);
  note->setWordWrap(true);

  layout->addRow(note);

  return page;
}

QWidget* TypeEditor::createRecordPage()
{
  auto* page = new QWidget(mEditorStack);
  auto* layout = new QVBoxLayout(page);

  auto* properties = new QFormLayout();

  auto* nameEdit = new QLineEdit(page);
  auto* namespaceEdit = new QLineEdit(page);

  nameEdit->setObjectName("recordNameEdit");
  namespaceEdit->setObjectName("recordNamespaceEdit");

  mBaseTypeCombo = new QComboBox(page);
  mBaseTypeCombo->addItem(Constants::EMPTY_COMBO, Constants::EMPTY_COMBO);
  for (const auto& def : TypeRegistry::instance().allTypes())
    if (def->isRecord())
      mBaseTypeCombo->addItem(QString::fromStdString(def->name.toString()), QString::fromStdString(def->name.toString()));

  mBaseTypeCombo->setEditable(false);

  properties->addRow(tr("Name"), nameEdit);
  properties->addRow(tr("Namespace"), namespaceEdit);
  properties->addRow(tr("Extends"), mBaseTypeCombo);

  layout->addLayout(properties);

  auto* fieldsLabel = new QLabel(tr("Fields"), page);
  layout->addWidget(fieldsLabel);

  mFieldsTable = new QTableWidget(page);

  mFieldsTable->setColumnCount(2);
  mFieldsTable->setHorizontalHeaderLabels({tr("Name"), tr("Type")});

  mFieldsTable->horizontalHeader()->setStretchLastSection(true);
  mFieldsTable->verticalHeader()->setVisible(false);

  mFieldsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  mFieldsTable->setSelectionMode(QAbstractItemView::SingleSelection);

  mFieldsTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  mFieldsTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

  layout->addWidget(mFieldsTable, 1);

  auto* fieldButtons = new QHBoxLayout();

  mAddFieldButton = new QPushButton(tr("Add field"), page);
  mRemoveFieldButton = new QPushButton(tr("Remove field"), page);

  fieldButtons->addWidget(mAddFieldButton);
  fieldButtons->addWidget(mRemoveFieldButton);
  fieldButtons->addStretch();

  layout->addLayout(fieldButtons);

  return page;
}

QWidget* TypeEditor::createEnumPage()
{
  auto* page = new QWidget(mEditorStack);
  auto* layout = new QVBoxLayout(page);

  auto* properties = new QFormLayout();

  auto* nameEdit = new QLineEdit(page);
  auto* namespaceEdit = new QLineEdit(page);

  nameEdit->setObjectName("enumNameEdit");
  namespaceEdit->setObjectName("enumNamespaceEdit");

  mEnumBackingCombo = new QComboBox(page);
  mEnumBackingCombo->addItem(Constants::EMPTY_COMBO, Constants::EMPTY_COMBO);
  for (int i = (int)koda::types::EnumUnderlyingKind::Int32; i <= (int)koda::types::EnumUnderlyingKind::String; ++i)
    mEnumBackingCombo->addItem(QString::fromStdString(koda::types::toString((koda::types::EnumUnderlyingKind)i)), i);

  properties->addRow(tr("Name"), nameEdit);
  properties->addRow(tr("Namespace"), namespaceEdit);
  properties->addRow(tr("Backing type"), mEnumBackingCombo);

  layout->addLayout(properties);

  auto* valuesLabel = new QLabel(tr("Values"), page);
  layout->addWidget(valuesLabel);

  mEnumTable = new QTableWidget(page);

  mEnumTable->setColumnCount(2);
  mEnumTable->setHorizontalHeaderLabels({tr("Name"), tr("Value")});

  mEnumTable->horizontalHeader()->setStretchLastSection(true);
  mEnumTable->verticalHeader()->setVisible(false);

  mEnumTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  mEnumTable->setSelectionMode(QAbstractItemView::SingleSelection);

  mEnumTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  mEnumTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

  layout->addWidget(mEnumTable, 1);

  auto* buttons = new QHBoxLayout();

  mAddEnumValueButton = new QPushButton(tr("Add value"), page);
  mRemoveEnumValueButton = new QPushButton(tr("Remove value"), page);

  buttons->addWidget(mAddEnumValueButton);
  buttons->addWidget(mRemoveEnumValueButton);
  buttons->addStretch();

  layout->addLayout(buttons);

  return page;
}

QWidget* TypeEditor::createAliasPage()
{
  auto* page = new QWidget(mEditorStack);
  auto* layout = new QFormLayout(page);

  auto* nameEdit = new QLineEdit(page);
  auto* namespaceEdit = new QLineEdit(page);
  auto* aliasTargetCombo = new QComboBox(page);

  nameEdit->setObjectName("aliasNameEdit");
  namespaceEdit->setObjectName("aliasNamespaceEdit");
  aliasTargetCombo->setObjectName("aliasTargetEdit");

  const auto typeNames = maki::TypeRegistry::instance().allTypeNames();
  aliasTargetCombo->addItem(Constants::EMPTY_COMBO);
  aliasTargetCombo->addItems(typeNames);

  layout->addRow(tr("Name"), nameEdit);
  layout->addRow(tr("Namespace"), namespaceEdit);
  layout->addRow(tr("Target type"), aliasTargetCombo);

  return page;
}

koda::types::TypeDefinition TypeEditor::readDefinitionFromUi() const
{
  koda::types::TypeDefinition definition;

  const int pageIndex = mEditorStack->currentIndex();
  if (pageIndex == static_cast<int>(EditorPage::Record))
  {
    LOG_DEBUG("Reading record from UI");
    std::map<std::string, koda::types::QualifiedName> fields;
    for (int row = 0; row < mFieldsTable->rowCount(); ++row)
    {
      const auto* nameItem = mFieldsTable->item(row, 0);
      const auto* typeContainer = mFieldsTable->cellWidget(row, 1);
      if (nameItem == nullptr || typeContainer == nullptr)
      {
        LOG_DEBUG("Error in record fields, name or type lead to nullptr");
        continue;
      }

      const auto* typeItem = typeContainer->findChild<QComboBox*>("typeCombo");
      LOG_INFO("Reading field: {} {}", nameItem->text(), typeItem->currentText());
      fields.emplace(nameItem->text().toStdString(), koda::types::QualifiedName(typeItem->currentText().toStdString()));
    }

    auto* page = mEditorStack->widget(static_cast<int>(EditorPage::Record));
    auto* nameEdit = page->findChild<QLineEdit*>("recordNameEdit");
    auto* namespaceEdit = page->findChild<QLineEdit*>("recordNamespaceEdit");
    auto baseType = mBaseTypeCombo->currentText();

    return koda::types::TypeDefinition::createRecord(std::format("{}::{}", namespaceEdit->text(), nameEdit->text()), fields,
                                                     baseType == Constants::EMPTY_COMBO ? "" : baseType.toStdString(),
                                                     getIdFromItem(mSelectedQualifiedName));
  }
  else if (pageIndex == static_cast<int>(EditorPage::Enum))
  {
    std::map<std::string, std::optional<std::string>> fields;
    for (int row = 0; row < mEnumTable->rowCount(); ++row)
    {
      const auto* nameItem = mEnumTable->item(row, 0);
      const auto* valueItem = mEnumTable->item(row, 1);

      if (nameItem == nullptr || valueItem == nullptr)
      {
        LOG_DEBUG("Error in enum fields, name or value lead to nullptr");
        continue;
      }

      fields.emplace(nameItem->text().toStdString(),
                     valueItem != nullptr ? std::optional<std::string>(valueItem->text().toStdString()) : std::nullopt);
    }

    auto* page = mEditorStack->widget(static_cast<int>(EditorPage::Enum));
    auto* nameEdit = page->findChild<QLineEdit*>("enumNameEdit");
    if (nameEdit == nullptr)
    {
      LOG_DEBUG("Empty name field in enum editor");
      return definition;
    }
    auto* namespaceEdit = page->findChild<QLineEdit*>("enumNamespaceEdit");
    if (namespaceEdit == nullptr)
    {
      LOG_DEBUG("Empty namespace field in enum editor");
      return definition;
    }
    auto underlyingType = mEnumBackingCombo->currentText();

    LOG_DEBUG("Creating enum with {} {}", nameEdit->text().toStdString(), namespaceEdit->text().toStdString());
    return koda::types::TypeDefinition::createEnum(std::format("{}::{}", namespaceEdit->text(), nameEdit->text()),
                                                   koda::types::enumKindFromString(underlyingType.toStdString()), fields,
                                                   getIdFromItem(mSelectedQualifiedName));
  }
  else if (pageIndex == static_cast<int>(EditorPage::Alias))
  {
    auto* page = mEditorStack->widget(static_cast<int>(EditorPage::Alias));
    auto* nameEdit = page->findChild<QLineEdit*>("aliasNameEdit");
    if (nameEdit == nullptr)
    {
      LOG_DEBUG("Empty name field in alias editor");
      return definition;
    }

    auto* namespaceEdit = page->findChild<QLineEdit*>("aliasNamespaceEdit");
    if (namespaceEdit == nullptr)
    {
      LOG_DEBUG("Empty namespace field in alias editor");
      return definition;
    }
    auto* aliasTargetCombo = page->findChild<QComboBox*>("aliasTargetEdit");
    if (aliasTargetCombo == nullptr)
    {
      LOG_DEBUG("Empty target field in alias editor");
      return definition;
    }

    return koda::types::TypeDefinition::createAlias(std::format("{}::{}", namespaceEdit->text(), nameEdit->text()),
                                                    koda::types::QualifiedName(aliasTargetCombo->currentText().toStdString()),
                                                    getIdFromItem(mSelectedQualifiedName));
  }

  return definition;
}

void TypeEditor::reloadTypes()
{
  const QString previouslySelected = mSelectedQualifiedName;

  QHash<QString, bool> expansionState;
  for (int i = 0; i < mTypeTree->topLevelItemCount(); ++i)
  {
    auto* root = mTypeTree->topLevelItem(i);
    const QString rootId = root->data(0, RootIdRole).toString();
    if (!rootId.isEmpty())
      expansionState[rootId] = root->isExpanded();
  }

  auto createRoot = [this, &expansionState](const QString& id, const QString& title) {
    auto* root = new QTreeWidgetItem(mTypeTree);
    root->setText(0, title);
    root->setData(0, RootIdRole, id);

    // Default is collapsed if this root, e.g., did not exist before.
    root->setExpanded(expansionState.value(id, false));
    return root;
  };

  mReloading = true;

  const QSignalBlocker blocker(mTypeTree);
  mTypeTree->clear();

  auto* builtinRoot = createRoot("builtin", tr("Built-in"));
  auto* projectRoot = createRoot("project", tr("Project"));

  const auto& registry = TypeRegistry::instance();
  for (const auto* type : registry.allTypes())
  {
    if (registry.isBuiltin(*type))
    {
      // Built-ins
      auto* item = new QTreeWidgetItem(builtinRoot);
      item->setText(0, QString::fromStdString(type->name.name));  // Use only the actual name for built-in types
      item->setData(0, QualifiedNameRole, QString::fromStdString(type->name.toString()));
      item->setData(0, IdRole, QString::fromStdString(type->id));
    }
    else
    {
      // User types
      auto* item = new QTreeWidgetItem(projectRoot);
      item->setText(0, QString::fromStdString(type->name.toString()));
      item->setData(0, QualifiedNameRole, QString::fromStdString(type->name.toString()));
      item->setData(0, IdRole, QString::fromStdString(type->id));
    }
  }

  mTypeTree->sortItems(0, Qt::AscendingOrder);

  mReloading = false;

  if (!previouslySelected.isEmpty())
    selectType(previouslySelected);
}

// =============================================================================
// Helpers
void TypeEditor::clearEditor()
{
  // Nothing is selected
  mSelectedQualifiedName.clear();

  mEditorStack->setCurrentIndex(static_cast<int>(EditorPage::None));

  mDeleteButton->setEnabled(false);
  mApplyButton->setEnabled(false);
}

std::string TypeEditor::createUniqueTypeName(const std::string& baseName) const
{
  for (int i = 1;; ++i)
  {
    const auto candidate = std::format("project::{}{}", baseName, i);
    if (TypeRegistry::instance().findByName(candidate) == nullptr)
      return candidate;
  }

  return std::string();
}

QTreeWidgetItem* TypeEditor::findItemByRole(QTreeWidgetItem* parent, int role, const QVariant& value) const
{
  if (!parent)
    return nullptr;

  if (parent->data(0, role) == value)
    return parent;

  for (int i = 0; i < parent->childCount(); ++i)
    if (auto* found = findItemByRole(parent->child(i), role, value))
      return found;

  return nullptr;
}

QTreeWidgetItem* TypeEditor::findTreeItem(const QString& qualifiedName) const
{
  for (int i = 0; i < mTypeTree->topLevelItemCount(); ++i)
    if (auto* found = findItemByRole(mTypeTree->topLevelItem(i), QualifiedNameRole, qualifiedName))
      return found;

  return nullptr;
}

void TypeEditor::selectType(const QString& qualifiedName)
{
  QTreeWidgetItem* item = findTreeItem(qualifiedName);

  if (item == nullptr)
    return;

  mTypeTree->setCurrentItem(item);
  mTypeTree->scrollToItem(item);
}

std::string TypeEditor::getIdFromItem(const QString& qualifiedName) const
{
  QTreeWidgetItem* item = findTreeItem(qualifiedName);
  if (item == nullptr)
  {
    LOG_INFO("Could not find item in tree with name {}", qualifiedName);
    return "";
  }

  return item->data(0, IdRole).toString().toStdString();
}

}  // namespace maki