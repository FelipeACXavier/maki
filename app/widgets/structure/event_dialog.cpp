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
#include <QTimer>
#include <QVBoxLayout>
#include <oclero/qlementine/widgets/LineEdit.hpp>

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
  mStorage = event;

  layout()->setContentsMargins(10, 0, 10, 0);
  layout()->setAlignment(Qt::AlignCenter);

  createNameInput();
  createTypeInput();
  createReturnTypeInput();
  createArgumentInput();

  layout()->setContentsMargins(10, 5, 10, 5);

  qobject_cast<QVBoxLayout*>(layout())->addStretch();

  auto buttonBox = createButtons("Apply", "Cancel");
  connect(buttonBox, &QDialogButtonBox::accepted, this, &EventDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &EventDialog::reject);
}

void EventDialog::createNameInput()
{
  auto name = new maki::StringWidget(tr("Event name"), mStorage->getname(), {maki::WidgetAlignment::Type::VERTICAL}, this);
  name->widget()->setFocusPolicy(mStorage->getmodifiable() ? Qt::StrongFocus : Qt::NoFocus);
  name->widget()->setReadOnly(!mStorage->getmodifiable());

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
  auto typeBox = new maki::TypeSelectionWidget(Types::PropertyTypesToString(Types::PropertyTypes::VOID), this);

  auto returnType = new maki::SelectorWidget(tr("Return type"), typeBox, maki::WidgetAlignment::Vertical(), this);
  returnType->setFocusPolicy(mStorage->getmodifiable() ? Qt::ClickFocus : Qt::NoFocus);
  returnType->setEnabled(mStorage->getmodifiable());

  connect(returnType, &maki::SelectorWidget::valueChanged, this,
          [this](const QString& text) { mStorage->setReturnType(Types::StringToPropertyTypes(text)); });

  if (mStorage->getreturnType() == Types::PropertyTypes::UNKNOWN)
    mStorage->setReturnType(Types::StringToPropertyTypes(returnType->getValue()));
  else
    returnType->setValue(Types::PropertyTypesToString(mStorage->getreturnType()));

  layout()->addWidget(returnType);
}

void EventDialog::createArgumentInput()
{
  QLabel* argumentLabel = new QLabel(tr("Arguments"), this);
  argumentLabel->setObjectName("PropertyLabel");
  layout()->addWidget(argumentLabel);

  // Create table to hold the arguments
  QTableView* args = new QTableView(this);
  QStandardItemModel* model = new QStandardItemModel(0, 2);
  args->verticalHeader()->setVisible(false);
  args->setFocusPolicy(Qt::ClickFocus);

  model->setHorizontalHeaderItem(0, new QStandardItem(tr("Name")));
  model->setHorizontalHeaderItem(1, new QStandardItem(tr("Type")));

  args->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  args->setContextMenuPolicy(Qt::CustomContextMenu);

  addDynamicWidget((QVBoxLayout*)layout(), args, this);

  if (mStorage->getmodifiable())
    args->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
  else
    args->setEditTriggers(QAbstractItemView::NoEditTriggers);

  args->setModel(model);

  for (const auto& field : mStorage->getarguments())
  {
    int newRow = model->rowCount();
    model->insertRow(newRow);

    auto nameItem = new QStandardItem(field->getid());
    nameItem->setEnabled(mStorage->getmodifiable());

    auto box = new maki::TypeSelectionWidget(Types::PropertyTypesToString(field->gettype()), args);
    box->setEnabled(mStorage->getmodifiable());

    model->setItem(newRow, 0, nameItem);
    args->setIndexWidget(model->index(newRow, 1), box);

    if (!mStorage->getmodifiable())
      continue;

    connect(box, &QComboBox::currentTextChanged, this, [this, newRow](const QString& value) { updateArgumentTable(newRow, 1, value); });
  }

  connect(model, &QStandardItemModel::itemChanged, this, [this](QStandardItem* item) {
    if (!item)
      return;

    int row = item->row();
    if (row >= mStorage->getarguments().size())
      return;

    auto text = item->text();
    if (text.isNull() || text.isEmpty())
      return;

    updateArgumentTable(row, item->column(), text);
  });

  QPushButton* button = new QPushButton(this);
  button->setObjectName("TextAndIcon");
  button->setEnabled(mStorage->getmodifiable());
  connect(button, &QPushButton::pressed, this, [this, model, args]() {
    int newRow = model->rowCount();
    model->insertRow(newRow);
    model->setItem(newRow, 0, new QStandardItem(""));

    auto box = new maki::TypeSelectionWidget(args);
    args->setIndexWidget(model->index(newRow, 1), box);
    connect(box, &QComboBox::currentTextChanged, this, [this, newRow](const QString& value) { updateArgumentTable(newRow, 1, value); });

    // Create new argument in the storage as well
    mStorage->addArgument(std::make_shared<PropertyInfo>());
  });

  button->setFocusPolicy(Qt::NoFocus);
  button->setText(" " + tr("Add argument"));
  button->setIcon(addIconWithColor(":/icons/plus.svg", Config::FOREGROUND));
  button->setMaximumWidth(250);

  QPushButton* deletebutton = new QPushButton(this);
  deletebutton->setObjectName("TextAndIcon");
  deletebutton->setEnabled(mStorage->getmodifiable());
  connect(deletebutton, &QPushButton::pressed, this, [this, model, args]() {
    QModelIndex index = args->currentIndex();
    if (!index.isValid())
      return;

    int row = index.row();
    model->removeRow(row);
    if (auto property = mStorage->getArgument(row))
      mStorage->removeArgument(property);
  });

  deletebutton->setFocusPolicy(Qt::NoFocus);
  deletebutton->setText(" " + tr("Remove argument"));
  deletebutton->setIcon(addIconWithColor(":/icons/clear.svg", Config::FOREGROUND));
  deletebutton->setMaximumWidth(250);

  QHBoxLayout* buttonLayout = new QHBoxLayout();
  buttonLayout->addStretch();
  buttonLayout->addWidget(button);
  buttonLayout->addWidget(deletebutton);

  layout()->addWidget(args);
  layout()->addLayout(buttonLayout);
}

void EventDialog::updateArgumentTable(int row, int column, const QString& text)
{
  LOG_INFO("updateArgumentTable: {}", text);
  if (column == 0)
    std::dynamic_pointer_cast<PropertyInfo>(mStorage->getArgument(row))->setId(text);
  else if (column == 1)
    std::dynamic_pointer_cast<PropertyInfo>(mStorage->getArgument(row))->setType(Types::StringToPropertyTypes(text));
}
