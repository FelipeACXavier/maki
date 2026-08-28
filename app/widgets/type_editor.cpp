#include "type_editor.h"

#include <QApplication>
#include <QCheckBox>
#include <QClipboard>
#include <QComboBox>
#include <QCompleter>
#include <QEvent>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QProxyStyle>
#include <QPushButton>
#include <QScrollBar>
#include <QSignalBlocker>
#include <QSplitter>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTreeWidget>
#include <QUuid>
#include <QVBoxLayout>
#include <oclero/qlementine.hpp>
#include <oclero/qlementine/widgets/Label.hpp>
#include <oclero/qlementine/widgets/LineEdit.hpp>

#include "app_configs.h"
#include "clickable_icon.h"
#include "dialogs/type_reference_dialog.h"
#include "logging.h"
#include "properties/type_selector.h"
#include "type_helpers.h"
#include "type_registry.h"
#include "validators/namespace_validator.h"
#include "widget_factory.h"

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
  connect(mExportButton, &QPushButton::clicked, this, &TypeEditor::exportAsJson);
  connect(mImportButton, &QPushButton::clicked, this, &TypeEditor::importFromJson);
  connect(mAddFieldButton, &QPushButton::clicked, this,
          [this] { addField(tr("field%1").arg(mFieldsTable->rowCount() + 1), koda::types::FieldDefinition{}); });
  connect(mRemoveFieldButton, &QPushButton::clicked, this, &TypeEditor::removeField);
  connect(mAddEnumValueButton, &QPushButton::clicked, this, [this] { addEnumValue(tr("Value%1").arg(mEnumTable->rowCount() + 1), ""); });
  connect(mRemoveEnumValueButton, &QPushButton::clicked, this, &TypeEditor::removeEnumValue);

  reloadTypes();
}

// =============================================================================
// Creation
// =============================================================================
void TypeEditor::createDefinition(const koda::types::TypeDefinition& definition)
{
  auto result = TypeRegistry::instance().add(definition);
  if (!result.IsSuccess())
  {
    LOG_WARNING(result.ErrorMessage());
    return;
  }

  selectType(QString::fromStdString(definition.id));
}

void TypeEditor::createRecord()
{
  createDefinition(koda::types::TypeDefinition::createRecord(createUniqueTypeName("Record"), std::vector<koda::types::FieldDefinition>{}, "",
                                                             QUuid::createUuid().toString().toStdString()));
}

void TypeEditor::createEnum()
{
  createDefinition(koda::types::TypeDefinition::createEnum(createUniqueTypeName("Enum"), koda::types::EnumUnderlyingKind::Int32,
                                                           std::vector<koda::types::EnumValueDefinition>{},
                                                           QUuid::createUuid().toString().toStdString()));
}

void TypeEditor::createAlias()
{
  createDefinition(koda::types::TypeDefinition::createAlias(createUniqueTypeName("Alias"), koda::types::IntegerType,
                                                            QUuid::createUuid().toString().toStdString()));
}

void TypeEditor::removeCurrentType()
{
  auto item = findTreeItemById(mSelectedId);
  if (!item)
    return;

  const QString qualifiedName = item->data(0, QualifiedNameRole).toString();
  const auto answer = QMessageBox::question(this, tr("Remove type"), tr("Are you sure you want to remove '%1'?").arg(qualifiedName));
  if (answer != QMessageBox::Yes)
    return;

  auto removed = TypeRegistry::instance().remove(mSelectedId.toStdString());
  if (!removed.IsSuccess())
  {
    LOG_WARNING(removed.ErrorMessage());
    return;
  }

  clearEditor();
}

void TypeEditor::applyChanges()
{
  koda::types::TypeDefinition definition = readDefinitionFromUi();
  if (definition.name.toString().empty())
  {
    LOG_WARNING("{}. {}.", tr("Invalid type"), tr("The type must have a name"));
    return;
  }

  auto updated = TypeRegistry::instance().replace(definition);
  if (!updated)
  {
    LOG_WARNING(updated.ErrorMessage());
    return;
  }

  selectType(mSelectedId);
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
    LOG_TRACE("No definition found for {}", qualifiedName);
    clearEditor();
    return;
  }

  // Since we can only show what we select, it makes sense to update this here
  mSelectedId = QString::fromStdString(definition->id);

  showDefinition(*definition);
}

// =============================================================================
// Tables (Record and Enum)
void TypeEditor::addField(const QString& defaultName, const koda::types::FieldDefinition& field)
{
  const int row = mFieldsTable->rowCount();
  mFieldsTable->insertRow(row);

  auto* nameItem = new QTableWidgetItem(defaultName);
  auto* selector = new TypeSelector("typeCombo", mFieldsTable);
  if (field.type.isValid())
    selector->setReference(field.type);

  auto* required = new QCheckBox(mFieldsTable);
  required->setTristate(false);
  required->setChecked(field.required);

  auto* requiredContainer = new QWidget(mFieldsTable);
  auto* requiredLayout = new QHBoxLayout(requiredContainer);
  requiredLayout->setContentsMargins(0, 0, 0, 0);
  requiredLayout->setAlignment(Qt::AlignCenter);
  requiredLayout->addWidget(required);

  mFieldsTable->setItem(row, 0, nameItem);
  mFieldsTable->setCellWidget(row, 1, requiredContainer);
  mFieldsTable->setCellWidget(row, 2, selector);

  mFieldsTable->setCurrentCell(row, 0);
}

void TypeEditor::removeField()
{
  const int row = mFieldsTable->currentRow();

  if (row < 0)
    return;

  mFieldsTable->removeRow(row);
}

void TypeEditor::addEnumValue(const QString& defaultName, const QString& defaultValue)
{
  const int row = mEnumTable->rowCount();

  mEnumTable->insertRow(row);
  mEnumTable->setItem(row, 0, new QTableWidgetItem(tr("Value%1").arg(row + 1)));
  mEnumTable->setItem(row, 1, defaultValue.isEmpty() ? new QTableWidgetItem() : new QTableWidgetItem(defaultValue));
  mEnumTable->setCurrentCell(row, 0);
}

void TypeEditor::removeEnumValue()
{
  const int row = mEnumTable->currentRow();

  if (row < 0)
    return;

  mEnumTable->removeRow(row);
}

void TypeEditor::validateEnumValues()
{
  for (int row = 0; row < mEnumTable->rowCount(); ++row)
    validateEnumValue(row);
}

bool TypeEditor::validateEnumValue(int row)
{
  auto* valueItem = mEnumTable->item(row, 1);
  if (!valueItem)
    return true;

  auto* page = mEditorStack->widget(static_cast<int>(EditorPage::Enum));
  auto* baseType = page->findChild<maki::SelectorWidget*>("enumBaseEdit");
  if (!baseType)
    return true;

  const auto type = koda::types::enumKindFromString(baseType->getValue().toStdString());
  const QString value = valueItem->text();
  const bool valid = isValidEnumValue(value, type);
  if (valid)
  {
    valueItem->setToolTip({});
    valueItem->setData(Qt::UserRole, true);

    // Restore normal appearance.
    valueItem->setForeground(QBrush{});
  }
  else
  {
    valueItem->setToolTip(tr("'%1' is not valid for enum type %2.").arg(value, baseType->getValue()));
    valueItem->setData(Qt::UserRole, false);
    valueItem->setForeground(mEnumTable->palette().brush(QPalette::Link));
  }

  return valid;
}

bool TypeEditor::isValidEnumValue(const QString& value, koda::types::EnumUnderlyingKind type) const
{
  if (value.isEmpty())
    return true;

  switch (type)
  {
    case koda::types::EnumUnderlyingKind::Unknown:
    case koda::types::EnumUnderlyingKind::String:
      return true;
    case koda::types::EnumUnderlyingKind::Int32:
    {
      bool ok = false;
      value.toInt(&ok);
      return ok;
    }
    case koda::types::EnumUnderlyingKind::UInt32:
    {
      bool ok = false;
      value.toUInt(&ok);
      return ok;
    }
  }

  return false;
}

// =============================================================================
// UI stuff
void TypeEditor::buildUi()
{
  auto* mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  auto* splitter = new QSplitter(Qt::Horizontal, this);
  mainLayout->addWidget(splitter, 1);

  // --------------------------------------------------------------------------
  // Left side: type browser
  auto* browserWidget = new QWidget();
  browserWidget->setMinimumWidth(200);

  auto* browserLayout = new QVBoxLayout(browserWidget);
  browserLayout->setContentsMargins(0, Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING);

  mTypeTree = new QTreeWidget(browserWidget);
  mTypeTree->setHeaderHidden(true);
  mTypeTree->setRootIsDecorated(true);
  mTypeTree->setSelectionMode(QAbstractItemView::SingleSelection);
  mTypeTree->setSelectionBehavior(QAbstractItemView::SelectRows);

  browserLayout->addWidget(mTypeTree, 1);

  auto* treeLine = new QFrame(browserWidget);
  treeLine->setFrameShape(QFrame::HLine);
  browserLayout->addWidget(treeLine);

  auto* browserButtons = new QHBoxLayout();
  mAddButton = new QPushButton(browserWidget);
  mAddButton->setFixedSize(Config::MEDIUM_BUTTON_SIZE);
  mAddButton->setIcon(iconFromTheme("plus"));

  auto* menu = new QMenu(mAddButton);
  auto* recordAction = menu->addAction(tr("Record"));
  auto* enumAction = menu->addAction(tr("Enum"));
  auto* aliasAction = menu->addAction(tr("Alias"));

  connect(mAddButton, &QPushButton::clicked, this, [this, menu]() {
    const QPoint pos = mAddButton->mapToGlobal(QPoint(0, -menu->sizeHint().height() + 15));
    menu->popup(pos);
  });

  connect(recordAction, &QAction::triggered, this, &TypeEditor::createRecord);
  connect(enumAction, &QAction::triggered, this, &TypeEditor::createEnum);
  connect(aliasAction, &QAction::triggered, this, &TypeEditor::createAlias);

  mDeleteButton = new QPushButton(browserWidget);
  mDeleteButton->setFixedSize(Config::MEDIUM_BUTTON_SIZE);
  mDeleteButton->setIcon(iconFromTheme("minus"));

  browserButtons->addWidget(mAddButton);
  browserButtons->addWidget(mDeleteButton);
  browserButtons->addStretch();

  browserLayout->addLayout(browserButtons);

  // --------------------------------------------------------------------------
  // Right side: editor
  auto* editorWidget = new QWidget();
  auto* editorLayout = new QVBoxLayout(editorWidget);
  editorLayout->setContentsMargins(Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING, Config::CONTENT_PADDING);

  mEditorStack = new QStackedWidget(editorWidget);
  mEditorStack->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Expanding);

  mEditorStack->addWidget(new QWidget(mEditorStack));  // Empty when nothing is selected
  mEditorStack->addWidget(createBuiltinPage());
  mEditorStack->addWidget(createRecordPage());
  mEditorStack->addWidget(createEnumPage());
  mEditorStack->addWidget(createAliasPage());

  editorLayout->addWidget(mEditorStack);

  auto* editorButtons = new QHBoxLayout();
  editorButtons->setContentsMargins(0, 0, 0, 0);

  mImportButton = new QPushButton(editorWidget);
  mImportButton->setIcon(iconFromTheme("document-save"));
  mImportButton->setFixedSize(Config::MEDIUM_BUTTON_SIZE);
  mImportButton->setToolTip(tr("Load type from JSON"));
  editorButtons->addWidget(mImportButton);

  mExportButton = new QPushButton(editorWidget);
  mExportButton->setIcon(iconFromTheme("document-open"));
  mExportButton->setFixedSize(Config::MEDIUM_BUTTON_SIZE);
  mExportButton->setToolTip(tr("Save type to JSON"));
  editorButtons->addWidget(mExportButton);

  editorButtons->addStretch();

  mApplyButton = new QPushButton(tr("Apply"), editorWidget);
  mApplyButton->setFixedHeight(Config::MEDIUM_BUTTON);
  editorButtons->addWidget(mApplyButton);

  auto* stackLine = new QFrame(browserWidget);
  stackLine->setFrameShape(QFrame::HLine);
  editorLayout->addWidget(stackLine);

  editorLayout->addLayout(editorButtons);

  // --------------------------------------------------------------------------
  // Add both to the splitter
  splitter->addWidget(browserWidget);
  splitter->addWidget(editorWidget);

  splitter->setStretchFactor(0, 0);
  splitter->setStretchFactor(1, 1);

  // The second value does not matter as long as it forces the initial tree size of 200
  splitter->setSizes({200, 1000});
  splitter->setCollapsible(0, false);
  splitter->setCollapsible(1, false);

  mDeleteButton->setEnabled(false);
  mApplyButton->setEnabled(false);
  mExportButton->setEnabled(false);
  mImportButton->setEnabled(false);
}

QWidget* TypeEditor::createBuiltinPage()
{
  auto* qlementineStyle = oclero::qlementine::appStyle();
  if (!qlementineStyle)
    return new QWidget();

  auto* page = new QWidget(mEditorStack);
  auto layout = new maki::WidgetGroup(tr("Built-in Type"), page);
  layout->setContentsMargins(0, 0, 0, Config::CONTENT_PADDING);

  auto alignment = maki::WidgetAlignment::Form(layout, Config::LONG_LABEL_WIDTH);

  auto* nameEdit = new maki::StringWidget(tr("Name"), Constants::EMPTY_COMBO, alignment, page);
  auto* namespaceEdit = new maki::StringWidget(tr("Namespace"), Constants::EMPTY_COMBO, alignment, page);
  auto* kindEdit = new maki::StringWidget(tr("Kind"), Constants::EMPTY_COMBO, alignment, page);

  nameEdit->setObjectName("builtinNameEdit");
  namespaceEdit->setObjectName("builtinNamespaceEdit");
  kindEdit->setObjectName("builtinKindEdit");

  nameEdit->widget()->setReadOnly(true);
  nameEdit->widget()->setFocusPolicy(Qt::FocusPolicy::NoFocus);
  namespaceEdit->widget()->setReadOnly(true);
  namespaceEdit->widget()->setFocusPolicy(Qt::FocusPolicy::NoFocus);
  kindEdit->widget()->setReadOnly(true);
  kindEdit->widget()->setFocusPolicy(Qt::FocusPolicy::NoFocus);

  auto* note = new oclero::qlementine::Label(tr("Built-in types are provided by KODA and cannot be modified."), page);
  note->setWordWrap(true);
  note->setRole(oclero::qlementine::TextRole::Caption);
  layout->addWidget(note);

  const auto theme = qlementineStyle->theme();
  auto* pageLayout = new QVBoxLayout(page);
  pageLayout->setContentsMargins(theme.spacing, theme.spacing, theme.spacing, theme.spacing);
  pageLayout->addWidget(layout);
  pageLayout->addStretch();

  return page;
}

QWidget* TypeEditor::createRecordPage()
{
  auto* qlementineStyle = oclero::qlementine::appStyle();
  if (!qlementineStyle)
    return new QWidget();

  const auto theme = qlementineStyle->theme();

  auto* page = new QWidget(mEditorStack);
  auto layout = new maki::WidgetGroup(tr("Record Type"), page);
  auto alignment = maki::WidgetAlignment::Form(layout, Config::LONG_LABEL_WIDTH);

  auto* nameEdit = new maki::StringWidget(tr("Name"), Constants::EMPTY_COMBO, alignment, page);
  auto* namespaceEdit = createNamespaceEdit(alignment, page);
  auto* extendCombo = new maki::SelectorWidget(tr("Extends"), alignment, page);

  nameEdit->setObjectName("recordNameEdit");
  namespaceEdit->setObjectName("recordNamespaceEdit");
  extendCombo->setObjectName("recordExtendsEdit");

  populateExtentKind(extendCombo->widget());
  extendCombo->widget()->setEditable(false);

  auto fieldLayout = new maki::WidgetGroup(tr("Fields"), oclero::qlementine::TextRole::H5, layout);
  mFieldsTable = createTable(page, {tr("Name"), tr("Required"), tr("Type")});
  fieldLayout->addWidget(mFieldsTable);
  fieldLayout->addSpacing(Config::CONTENT_PADDING);

  auto* fieldButtons = new QHBoxLayout();
  mAddFieldButton = new QPushButton(tr("Add field"), page);
  mRemoveFieldButton = new QPushButton(tr("Remove field"), page);

  fieldButtons->addWidget(mAddFieldButton);
  fieldButtons->addWidget(mRemoveFieldButton);
  fieldButtons->addStretch();

  fieldLayout->addStretch();
  fieldLayout->addLayout(fieldButtons);
  layout->addWidget(fieldLayout);

  auto* pageLayout = new QVBoxLayout(page);
  pageLayout->setContentsMargins(theme.spacing, theme.spacing, theme.spacing, theme.spacing);
  pageLayout->addWidget(layout);

  return page;
}

QWidget* TypeEditor::createEnumPage()
{
  auto* qlementineStyle = oclero::qlementine::appStyle();
  if (!qlementineStyle)
    return new QWidget();

  auto* page = new QWidget(mEditorStack);
  auto layout = new maki::WidgetGroup(tr("Enum Type"), page);
  auto alignment = maki::WidgetAlignment::Form(layout, Config::LONG_LABEL_WIDTH);

  auto* nameEdit = new maki::StringWidget(tr("Name"), Constants::EMPTY_COMBO, alignment, page);
  auto* namespaceEdit = createNamespaceEdit(alignment, page);
  auto* extendCombo = new maki::SelectorWidget(tr("Base type"), alignment, page);

  nameEdit->setObjectName("enumNameEdit");
  namespaceEdit->setObjectName("enumNamespaceEdit");
  extendCombo->setObjectName("enumBaseEdit");

  populateEnumKind(extendCombo->widget());
  extendCombo->widget()->setEditable(false);

  // Create table
  auto fieldLayout = new maki::WidgetGroup(tr("Values"), oclero::qlementine::TextRole::H5, layout);
  fieldLayout->addSpacing(Config::CONTENT_PADDING);

  mEnumTable = createTable(page, {tr("Name"), tr("Value")});

  connect(extendCombo, &maki::SelectorWidget::valueChanged, this, [this](const QString&) { validateEnumValues(); });
  connect(mEnumTable, &QTableWidget::itemChanged, this, [this](QTableWidgetItem* item) {
    if (!item)
      return;

    if (item->column() == 1)  // The value columng
      validateEnumValue(item->row());

    mEnumTable->clearFocus();
    mEnumTable->clearSelection();
  });

  fieldLayout->addWidget(mEnumTable);
  layout->addWidget(fieldLayout);
  layout->addStretch();

  // Create buttons
  auto* fieldButtons = new QHBoxLayout();
  mAddEnumValueButton = new QPushButton(tr("Add value"), page);
  mRemoveEnumValueButton = new QPushButton(tr("Remove value"), page);
  fieldButtons->addWidget(mAddEnumValueButton);
  fieldButtons->addWidget(mRemoveEnumValueButton);
  fieldButtons->addStretch();  // Push buttons to the left

  layout->addStretch();
  layout->addLayout(fieldButtons);

  const auto theme = qlementineStyle->theme();
  auto* pageLayout = new QVBoxLayout(page);
  pageLayout->setContentsMargins(theme.spacing, theme.spacing, theme.spacing, theme.spacing);
  pageLayout->addWidget(layout);

  return page;
}

QWidget* TypeEditor::createAliasPage()
{
  auto* qlementineStyle = oclero::qlementine::appStyle();
  if (!qlementineStyle)
    return new QWidget();

  auto* page = new QWidget(mEditorStack);
  auto layout = new maki::WidgetGroup(tr("Alias Type"), page);
  auto alignment = maki::WidgetAlignment::Form(layout, Config::LONG_LABEL_WIDTH);

  auto* nameEdit = new maki::StringWidget(tr("Name"), Constants::EMPTY_COMBO, alignment, page);
  auto* namespaceEdit = createNamespaceEdit(alignment, page);

  auto* selector = new TypeSelector("aliasTargetEdit", page);
  auto* containerLayout = new maki::ContainerWidget(tr("Target type"), selector, alignment, page);
  Q_UNUSED(containerLayout);

  nameEdit->setObjectName("aliasNameEdit");
  namespaceEdit->setObjectName("aliasNamespaceEdit");

  const auto theme = qlementineStyle->theme();
  auto* pageLayout = new QVBoxLayout(page);
  pageLayout->setContentsMargins(theme.spacing, theme.spacing, theme.spacing, theme.spacing);
  pageLayout->addWidget(layout);
  pageLayout->addStretch();

  return page;
}

koda::types::TypeDefinition TypeEditor::readDefinitionFromUi() const
{
  koda::types::TypeDefinition definition;

  const int pageIndex = mEditorStack->currentIndex();
  if (pageIndex == static_cast<int>(EditorPage::Record))
  {
    std::vector<koda::types::FieldDefinition> fields;
    for (int row = 0; row < mFieldsTable->rowCount(); ++row)
    {
      const auto* nameItem = mFieldsTable->item(row, 0);
      const auto* requiredItem = mFieldsTable->cellWidget(row, 1)->findChild<QCheckBox*>();
      const auto* typeItem = qobject_cast<TypeSelector*>(mFieldsTable->cellWidget(row, 2));
      if (nameItem == nullptr || typeItem == nullptr || requiredItem == nullptr)
      {
        LOG_DEBUG("Empty record field: {} {} {}", nameItem != nullptr, typeItem != nullptr, requiredItem != nullptr);
        return definition;
      }

      fields.push_back(koda::types::FieldDefinition{
          .name = nameItem->text().toStdString(),
          .type = typeItem->getReference(),
          .required = requiredItem->isChecked(),
      });
    }

    auto* page = mEditorStack->widget(static_cast<int>(EditorPage::Record));
    auto* nameEdit = page->findChild<maki::StringWidget*>("recordNameEdit");
    auto* namespaceEdit = page->findChild<maki::StringWidget*>("recordNamespaceEdit");
    auto baseType = page->findChild<maki::SelectorWidget*>("recordExtendsEdit");
    if (nameEdit == nullptr || namespaceEdit == nullptr || baseType == nullptr)
    {
      LOG_DEBUG("Empty field in record editor: {} {} {}", nameEdit != nullptr, namespaceEdit != nullptr, baseType != nullptr);
      return definition;
    }

    return koda::types::TypeDefinition::createRecord(std::format("{}::{}", namespaceEdit->getValue(), nameEdit->getValue()), fields,
                                                     baseType->getValue() == Constants::EMPTY_COMBO ? "" : baseType->getValue().toStdString(),
                                                     mSelectedId.toStdString());
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
        LOG_DEBUG("Empty enum field: {} {}", nameItem != nullptr, valueItem != nullptr);
        return definition;
      }

      fields.emplace(nameItem->text().toStdString(),
                     valueItem != nullptr ? std::optional<std::string>(valueItem->text().toStdString()) : std::nullopt);
    }

    auto* page = mEditorStack->widget(static_cast<int>(EditorPage::Enum));
    auto* nameEdit = page->findChild<maki::StringWidget*>("enumNameEdit");
    auto* namespaceEdit = page->findChild<maki::StringWidget*>("enumNamespaceEdit");
    auto* baseType = page->findChild<maki::SelectorWidget*>("enumBaseEdit");
    if (nameEdit == nullptr || namespaceEdit == nullptr || baseType == nullptr)
    {
      LOG_DEBUG("Empty field in enum editor: {} {} {}", nameEdit != nullptr, namespaceEdit != nullptr, baseType != nullptr);
      return definition;
    }

    return koda::types::TypeDefinition::createEnum(std::format("{}::{}", namespaceEdit->getValue(), nameEdit->getValue()),
                                                   koda::types::enumKindFromString(baseType->getValue().toStdString()), fields,
                                                   mSelectedId.toStdString());
  }
  else if (pageIndex == static_cast<int>(EditorPage::Alias))
  {
    auto* page = mEditorStack->widget(static_cast<int>(EditorPage::Alias));

    const auto* nameEdit = page->findChild<maki::StringWidget*>("aliasNameEdit");
    const auto* namespaceEdit = page->findChild<maki::StringWidget*>("aliasNamespaceEdit");
    const auto* aliasTargetCombo = page->findChild<TypeSelector*>("aliasTargetEdit");
    if (nameEdit == nullptr || namespaceEdit == nullptr || aliasTargetCombo == nullptr)
    {
      LOG_DEBUG("Empty field in alias editor: {} {} {}", nameEdit != nullptr, namespaceEdit != nullptr, aliasTargetCombo != nullptr);
      return definition;
    }

    const auto qname = std::format("{}::{}", namespaceEdit->getValue(), nameEdit->getValue());
    return koda::types::TypeDefinition::createAlias(qname, aliasTargetCombo->getReference(), mSelectedId.toStdString());
  }

  return definition;
}

void TypeEditor::showDefinition(const koda::types::TypeDefinition& definition)
{
  if (definition.isRecord())
  {
    mEditorStack->setCurrentIndex(static_cast<int>(EditorPage::Record));

    auto* page = mEditorStack->widget(static_cast<int>(EditorPage::Record));
    auto* nameEdit = page->findChild<maki::StringWidget*>("recordNameEdit");
    nameEdit->setFocus();

    auto* namespaceEdit = page->findChild<maki::StringWidget*>("recordNamespaceEdit");
    auto* baseType = page->findChild<maki::SelectorWidget*>("recordExtendsEdit");

    maki::addCompleter(TypeRegistry::instance().namespaces(), namespaceEdit->widget());
    populateExtentKind(baseType->widget(), &definition.name);

    nameEdit->setValue(QString::fromStdString(definition.name.name));
    namespaceEdit->setValue(QString::fromStdString(definition.name.namespaceString()));

    const auto def = definition.record();
    if (def.baseType.has_value() && def.baseType->isNamed())
      baseType->setValue(QString::fromStdString(def.baseType->namedType().name.toString()));

    mFieldsTable->setRowCount(0);
    for (const auto& field : def.fields)
      addField(QString::fromStdString(field.name), field);
  }
  else if (definition.isEnum())
  {
    mEditorStack->setCurrentIndex(static_cast<int>(EditorPage::Enum));

    auto* page = mEditorStack->widget(static_cast<int>(EditorPage::Enum));
    auto* nameEdit = page->findChild<maki::StringWidget*>("enumNameEdit");
    nameEdit->setFocus();

    auto* namespaceEdit = page->findChild<maki::StringWidget*>("enumNamespaceEdit");
    auto* baseType = page->findChild<maki::SelectorWidget*>("enumBaseEdit");

    maki::addCompleter(TypeRegistry::instance().namespaces(), namespaceEdit->widget());
    populateEnumKind(baseType->widget());

    nameEdit->setValue(QString::fromStdString(definition.name.name));
    namespaceEdit->setValue(QString::fromStdString(definition.name.namespaceString()));

    const auto def = definition.enumeration();
    baseType->setValue(QString::fromStdString(koda::types::toString(def.underlyingType)));

    mEnumTable->setRowCount(0);
    for (const auto& value : def.values)
      addEnumValue(QString::fromStdString(value.name), QString::fromStdString(value.value.value_or("")));
  }
  else if (definition.isAlias())
  {
    mEditorStack->setCurrentIndex(static_cast<int>(EditorPage::Alias));

    auto* page = mEditorStack->widget(static_cast<int>(EditorPage::Alias));
    auto* nameEdit = page->findChild<maki::StringWidget*>("aliasNameEdit");
    nameEdit->setFocus();

    auto* namespaceEdit = page->findChild<maki::StringWidget*>("aliasNamespaceEdit");
    auto* aliasEdit = page->findChild<TypeSelector*>("aliasTargetEdit");

    maki::addCompleter(TypeRegistry::instance().namespaces(), namespaceEdit->widget());
    aliasEdit->populateTypes(&definition.name);

    nameEdit->setValue(QString::fromStdString(definition.name.name));
    namespaceEdit->setValue(QString::fromStdString(definition.name.namespaceString()));
    aliasEdit->setReference(definition.alias().target);
  }
  else if (definition.isPrimitive())
  {
    mEditorStack->setCurrentIndex(static_cast<int>(EditorPage::Builtin));

    auto* page = mEditorStack->widget(static_cast<int>(EditorPage::Builtin));
    auto* nameEdit = page->findChild<maki::StringWidget*>("builtinNameEdit");
    auto* namespaceEdit = page->findChild<maki::StringWidget*>("builtinNamespaceEdit");
    auto* kindEdit = page->findChild<maki::StringWidget*>("builtinKindEdit");

    nameEdit->setValue(QString::fromStdString(definition.name.name));
    namespaceEdit->setValue(QString::fromStdString(definition.name.namespaceString()));
    kindEdit->setValue(QString::fromStdString(koda::types::toString(definition.primitive().primitive)));

    mDeleteButton->setEnabled(false);
    mApplyButton->setEnabled(false);
    mExportButton->setEnabled(true);
    mImportButton->setEnabled(false);
    return;
  }

  mDeleteButton->setEnabled(true);
  mApplyButton->setEnabled(true);
  mExportButton->setEnabled(true);
  mImportButton->setEnabled(true);
}

QIcon TypeEditor::typeToIcon(const koda::types::TypeDefinition& type) const
{
  if (type.isAlias())
    return QIcon(":/icons/alias.svg");
  if (type.isEnum())
    return QIcon(":/icons/enum.svg");
  if (type.isRecord())
    return QIcon(":/icons/record.svg");

  return QIcon(":/icons/primitive.svg");
}

void TypeEditor::reloadTypes()
{
  // Saved it so we can reselect it after the update
  const QString previouslySelected = mSelectedId;

  QHash<QString, bool> expansionState;
  for (int i = 0; i < mTypeTree->topLevelItemCount(); ++i)
  {
    auto* root = mTypeTree->topLevelItem(i);
    const QString rootId = root->data(0, RootIdRole).toString();
    if (!rootId.isEmpty())
      expansionState[rootId] = root->isExpanded();
  }

  auto createRoot = [this, &expansionState](const QString& title) {
    auto* root = new QTreeWidgetItem(mTypeTree);
    root->setText(0, title);
    root->setData(0, RootIdRole, title);

    // Default is collapsed if this root, e.g., did not exist before.
    root->setExpanded(expansionState.value(title, false));
    return root;
  };

  mReloading = true;

  const QSignalBlocker blocker(mTypeTree);
  mTypeTree->clear();

  const auto& registry = TypeRegistry::instance();

  const auto builtInKey = tr("Built-in");
  const auto projectKey = tr("Project");
  auto libs = QStringList{builtInKey, projectKey} + registry.libraries();
  QMap<QString, QTreeWidgetItem*> libWidgets;
  for (const auto& lib : libs)
    libWidgets.insert(lib, createRoot(lib));

  for (const auto* type : registry.allTypes())
  {
    QTreeWidgetItem* item;
    if (registry.isBuiltin(*type))
      item = new QTreeWidgetItem(libWidgets[builtInKey]);
    else if (registry.isFromLibrary(type->name))
      item = new QTreeWidgetItem(libWidgets[QString::fromStdString(type->name.namespaceString())]);
    else
      item = new QTreeWidgetItem(libWidgets[projectKey]);

    item->setText(0, QString::fromStdString(type->name.toString()));
    item->setData(0, QualifiedNameRole, QString::fromStdString(type->name.toString()));
    item->setData(0, IdRole, QString::fromStdString(type->id));
    item->setIcon(0, typeToIcon(*type));
  }

  mTypeTree->sortItems(0, Qt::AscendingOrder);

  mReloading = false;

  if (!previouslySelected.isEmpty())
    selectType(previouslySelected);
}

void TypeEditor::exportAsJson()
{
  if (mSelectedId.isEmpty())
    return;

  LOG_DEBUG("Copying: {}", mSelectedId);
  const koda::types::TypeDefinition* definition = TypeRegistry::instance().findById(mSelectedId.toStdString());
  if (definition == nullptr)
  {
    LOG_DEBUG("No definition found for {}", mSelectedId);
    return;
  }

  QJsonObject json = typeDefinitionToJson(*definition);
  QApplication::clipboard()->setText(QJsonDocument(json).toJson(QJsonDocument::Indented));
  LOG_INFO("Copied {}", mSelectedId);
}

void TypeEditor::importFromJson()
{
  const QString text = QApplication::clipboard()->text();

  QJsonParseError error;
  const QJsonDocument document = QJsonDocument::fromJson(text.toUtf8(), &error);

  if (error.error != QJsonParseError::NoError)
  {
    LOG_WARNING("Invalid JSON: {}", error.errorString());
    return;
  }

  if (!document.isObject())
  {
    LOG_WARNING("Expected a JSON object");
    return;
  }

  auto result = typeDefinitionFromJson(document.object());
  if (!result.IsSuccess())
  {
    LOG_WARNING("Failed to load from json: {}", result.ErrorMessage());
    return;
  }

  showDefinition(result.Value());
  LOG_INFO("Loaded definition for {}", result.Value().name.toString());
}

// =============================================================================
// Helpers
void TypeEditor::clearEditor()
{
  // Nothing is selected
  mSelectedId.clear();

  mEditorStack->setCurrentIndex(static_cast<int>(EditorPage::None));

  mDeleteButton->setEnabled(false);
  mApplyButton->setEnabled(false);
  mExportButton->setEnabled(false);
  mImportButton->setEnabled(true);
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

QTreeWidgetItem* TypeEditor::findTreeItemById(const QString& id) const
{
  for (int i = 0; i < mTypeTree->topLevelItemCount(); ++i)
    if (auto* found = findItemByRole(mTypeTree->topLevelItem(i), IdRole, id))
      return found;

  return nullptr;
}

QTreeWidgetItem* TypeEditor::findTreeItemByName(const QString& qualifiedName) const
{
  for (int i = 0; i < mTypeTree->topLevelItemCount(); ++i)
    if (auto* found = findItemByRole(mTypeTree->topLevelItem(i), QualifiedNameRole, qualifiedName))
      return found;

  return nullptr;
}

void TypeEditor::selectType(const QString& id)
{
  QTreeWidgetItem* item = findTreeItemById(id);
  if (item == nullptr)
    return;

  mTypeTree->setCurrentItem(item);
  mTypeTree->scrollToItem(item);
}

QTableWidget* TypeEditor::createTable(QWidget* parent, const QStringList& headers)
{
  auto* table = new QTableWidget(parent);

  table->setColumnCount(headers.size());
  table->setHorizontalHeaderLabels(headers);

  table->horizontalHeader()->setStretchLastSection(true);
  table->verticalHeader()->setVisible(false);

  table->setSelectionBehavior(QAbstractItemView::SelectRows);
  table->setSelectionMode(QAbstractItemView::SingleSelection);

  table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  for (int i = 1; i < headers.size(); ++i)
    table->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);

  table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  table->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  table->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  table->viewport()->installEventFilter(this);

  return table;
}

maki::StringWidget* TypeEditor::createNamespaceEdit(maki::WidgetAlignment& alignment, QWidget* parent) const
{
  auto* namespaceEdit = new maki::StringWidget(tr("Namespace"), Constants::EMPTY_COMBO, alignment,
                                               tr("Use identifiers separated by '::', for example 'robotics' or 'robotics::geometry'."), parent);

  namespaceEdit->widget()->setValidator(new NamespaceValidator());
  maki::addCompleter(TypeRegistry::instance().namespaces(), namespaceEdit->widget());

  return namespaceEdit;
}

void TypeEditor::populateEnumKind(QComboBox* widget) const
{
  widget->clear();
  for (int i = (int)koda::types::EnumUnderlyingKind::Int32; i <= (int)koda::types::EnumUnderlyingKind::String; ++i)
    widget->addItem(QString::fromStdString(koda::types::toString((koda::types::EnumUnderlyingKind)i)), i);
}

void TypeEditor::populateExtentKind(QComboBox* widget, const koda::types::QualifiedName* currentName) const
{
  widget->clear();
  for (const auto& def : TypeRegistry::instance().allTypes())
    if (def->isRecord() && (currentName == nullptr || def->name != *currentName))
      widget->addItem(QString::fromStdString(def->name.toString()), QVariant::fromValue(def->toReference()));
}

bool TypeEditor::eventFilter(QObject* object, QEvent* event)
{
  if (event->type() == QEvent::MouseButtonPress)
  {
    auto* viewport = qobject_cast<QWidget*>(object);
    auto* table = viewport ? qobject_cast<QTableWidget*>(viewport->parentWidget()) : nullptr;
    if (table && viewport == table->viewport())
    {
      const auto* mouseEvent = static_cast<QMouseEvent*>(event);
      if (!table->indexAt(mouseEvent->position().toPoint()).isValid())
      {
        table->clearFocus();
        table->clearSelection();
        table->setCurrentItem(nullptr);
      }
    }
  }

  return QWidget::eventFilter(object, event);
}

void TypeEditor::focusCurrentEditor()
{
  if (!mEditorStack)
    return;

  QWidget* page = mEditorStack->currentWidget();
  if (!page)
    return;

  const auto edits = page->findChildren<QLineEdit*>();
  for (auto* edit : edits)
  {
    if (edit->isEnabled() && edit->isVisible() && edit->focusPolicy() != Qt::NoFocus)
    {
      edit->setFocus(Qt::ShortcutFocusReason);
      edit->selectAll();
      return;
    }
  }
}

}  // namespace maki