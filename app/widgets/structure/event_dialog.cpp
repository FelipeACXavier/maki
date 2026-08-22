#include "event_dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QTableWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <oclero/qlementine/widgets/LineEdit.hpp>

#include "../dialogs/prompt.h"
#include "../properties/type_selector.h"
#include "app_configs.h"
#include "logging.h"
#include "save_info.h"
#include "style_helpers.h"
#include "widgets/widget_factory.h"

EventDialog::EventDialog(const QString& title, QWidget* parent)
    : BaseDialog(title, 0.8, 0.4, parent)
    , mEnterCount(0)
{
}

std::shared_ptr<FlowSaveInfo> EventDialog::getInfo() const
{
  return mStorage;
}

void EventDialog::setup(std::shared_ptr<FlowSaveInfo> event)
{
  layout()->setAlignment(Qt::AlignCenter);
  layout()->setContentsMargins(10, 5, 10, 5);
  mStorage = event;

  createNameInput();
  createTypeInput();
  createReturnTypeInput();
  createArgumentInput();

  qobject_cast<QVBoxLayout*>(layout())->addStretch();

  auto buttonBox = createButtons("Apply", "Cancel");
  connect(buttonBox, &QDialogButtonBox::accepted, this, &EventDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &EventDialog::reject);

  limitWidth(400);
}

void EventDialog::createNameInput()
{
  auto name = new maki::StringWidget(tr("Event name"), mStorage->getname(), maki::WidgetAlignment::Vertical(), this);
  name->widget()->setFocusPolicy(mStorage->getmodifiable() ? Qt::StrongFocus : Qt::NoFocus);
  name->setEnabled(mStorage->getmodifiable());

  connect(name, &maki::StringWidget::valueChanged, this, [this](const QString& text) { mStorage->setName(text); });
  layout()->addWidget(name);
}

void EventDialog::createTypeInput()
{
  auto type = new maki::SelectorWidget(tr("Event type"), maki::WidgetAlignment::Vertical(), this);
  type->setFocusPolicy(mStorage->getmodifiable() ? Qt::ClickFocus : Qt::NoFocus);
  type->setEnabled(mStorage->getmodifiable());

  // Currently, the user can only create USER calls
  auto id = Types::CallTypeToString(Types::CallType::USER);
  type->addItem(id, id);
  type->setValue(id);

  connect(type, &maki::SelectorWidget::valueChanged, this, [this](const QString& text) { mStorage->setType(Types::StringToCallType(text)); });

  if (mStorage->gettype() == Types::CallType::UNKNOWN)
  {
    type->setValue(Types::CallTypeToString((Types::CallType)((uint16_t)Types::CallType::UNKNOWN + 1)));
    mStorage->setType(Types::StringToCallType(type->getValue()));
  }
  else
  {
    type->setValue(Types::CallTypeToString(mStorage->gettype()));
  }

  layout()->addWidget(type);
}

void EventDialog::createReturnTypeInput()
{
  auto returnType = new maki::TypeSelector("typeCombo", this);
  auto container = new maki::ContainerWidget(tr("Return type"), returnType, maki::WidgetAlignment::Vertical(), this);
  returnType->setEnabled(mStorage->getmodifiable());

  connect(returnType, &maki::TypeSelector::typeChanged, this,
          [this](const koda::types::TypeReference& ref) { mStorage->setReturnType(Types::PropertyTypes::VOID); });

  if (mStorage->getreturnType() == Types::PropertyTypes::UNKNOWN)
    mStorage->setReturnType(Types::PropertyTypes::VOID);
  else
    returnType->setReference(koda::types::TypeReference::createVoid());

  layout()->addWidget(container);
}

void EventDialog::createArgumentInput()
{
  auto* argumentGroup = new maki::WidgetGroup(tr("Arguments"), oclero::qlementine::TextRole::H5, this);
  argumentGroup->setPadding(0);

  // Create table to hold the arguments
  QTableWidget* args = new QTableWidget(argumentGroup);
  args->setColumnCount(2);
  args->setHorizontalHeaderLabels({"Name", "Type"});
  args->setEnabled(mStorage->getmodifiable());

  args->verticalHeader()->setVisible(true);
  args->verticalHeader()->setMinimumWidth(30);
  args->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
  args->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

  args->setSelectionBehavior(QAbstractItemView::SelectRows);
  args->setSelectionMode(QAbstractItemView::ExtendedSelection);

  args->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
  args->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
  args->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  if (mStorage->getmodifiable())
    args->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
  else
    args->setEditTriggers(QAbstractItemView::NoEditTriggers);

  for (const auto& field : mStorage->getarguments())
    addArgumentToTable(args, std::dynamic_pointer_cast<PropertyInfo>(field));

  // --------------------------------------------------------------------------------------------------------
  // Buttons
  auto* browserButtons = new QHBoxLayout();
  auto* addButton = new QPushButton(argumentGroup);
  addButton->setFixedSize(Config::MEDIUM_BUTTON_SIZE);
  addButton->setIcon(iconFromTheme("plus"));
  addButton->setEnabled(mStorage->getmodifiable());

  auto* removeButton = new QPushButton(argumentGroup);
  removeButton->setFixedSize(Config::MEDIUM_BUTTON_SIZE);
  removeButton->setIcon(iconFromTheme("minus"));
  removeButton->setEnabled(mStorage->getmodifiable());

  connect(addButton, &QPushButton::pressed, this, [this, args] { addArgumentToTable(args, std::make_shared<PropertyInfo>()); });
  connect(removeButton, &QPushButton::clicked, this, [this, args] { removeArgumentFromTable(args); });

  browserButtons->addWidget(addButton);
  browserButtons->addWidget(removeButton);
  browserButtons->addStretch();

  // --------------------------------------------------------------------------------------------------------
  // Final layout
  argumentGroup->addWidget(args);
  argumentGroup->addStretch();
  argumentGroup->addLayout(browserButtons);

  layout()->addWidget(argumentGroup);
}

void EventDialog::addArgumentToTable(QTableWidget* table, std::shared_ptr<PropertyInfo> field)
{
  int row = table->rowCount();
  table->insertRow(row);

  auto nameItem = new oclero::qlementine::LineEdit(table);
  nameItem->setText(field->getid());

  auto typeItem = new maki::TypeSelector("typeCombo", table);
  typeItem->setReference(field->gettype());

  table->setCellWidget(row, 0, nameItem);
  table->setCellWidget(row, 1, typeItem);

  connect(nameItem, &oclero::qlementine::LineEdit::editingFinished, this, [this, nameItem, row, field]() {
    field->setId(nameItem->text());
    if (row < mStorage->getarguments().size())
      mStorage->setArgument(row, field);
    else
      mStorage->addArgument(field);
  });
  connect(typeItem, &maki::TypeSelector::typeChanged, this, [this, row, field](const koda::types::TypeReference& ref) {
    field->setType(ref);
    if (row < mStorage->getarguments().size())
      mStorage->setArgument(row, field);
    else
      mStorage->addArgument(field);
  });
}

void EventDialog::removeArgumentFromTable(QTableWidget* table)
{
  if (!table)
    return;

  const auto selectedRows = table->selectionModel()->selectedRows();
  if (selectedRows.empty())
    return;

  std::vector<int> rows;
  rows.reserve(selectedRows.size());

  for (const auto& index : selectedRows)
    rows.push_back(index.row());

  std::sort(rows.begin(), rows.end(), std::greater<int>());
  for (const int row : rows)
  {
    if (row < 0 || row >= table->rowCount())
      continue;

    mStorage->removeArgument(*(mStorage->getarguments().begin() + row));
    table->removeRow(row);
  }
}

void EventDialog::accept()
{
  if (mStorage->getname().isEmpty())
  {
    if (maki::warningPrompt(tr("Missing name"), tr("The flow will not be saved.")))
      QDialog::accept();
    return;
  }

  if (mStorage->getreturnType() == Types::PropertyTypes::UNKNOWN)
  {
    if (maki::warningPrompt(tr("Missing return type"), tr("The flow will not be saved.")))
      QDialog::accept();
    return;
  }

  for (const auto& arg : mStorage->getarguments())
  {
    if (arg->getname().isEmpty() || !arg->gettype().isValid())
    {
      if (maki::warningPrompt(tr("Argument has invalid format"), tr("The flow will not be saved.")))
        QDialog::accept();
      return;
    }
  }

  QDialog::accept();
}