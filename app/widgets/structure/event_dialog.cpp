#include "event_dialog.h"
#include <qabstractitemview.h>
#include <qnamespace.h>

#include <QComboBox>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>
#include <QTimer>
#include <QVBoxLayout>

#include "app_configs.h"
#include "elements/save_info.h"
#include "logging.h"
#include "style_helpers.h"

EventDialog::EventDialog(const QString& title, QWidget* parent)
    : QDialog(parent)
    , mEnterCount(0)
{
  setWindowTitle(title);

  // Create layout and widgets
  QVBoxLayout* layout = new QVBoxLayout;
  setLayout(layout);

  // TODO(felaze): Make this dynamic
  resize(500, 400);
}

std::shared_ptr<FlowSaveInfo> EventDialog::getInfo() const
{
  return mStorage;
}

void EventDialog::setup(std::shared_ptr<FlowSaveInfo> event)
{
  mStorage = event;

  QWidget* controls = new QWidget(this);
  QHBoxLayout* controlLayout = new QHBoxLayout(controls);
  controlLayout->setContentsMargins(10, 0, 10, 0);
  layout()->setAlignment(Qt::AlignCenter);

  controls->setLayout(controlLayout);
  layout()->addWidget(controls);

  createNameInput(controls);
  createTypeInput(controls);
  createReturnTypeInput(controls);
  createArgumentInput(controls);

  layout()->setContentsMargins(10, 5, 10, 5);

  static_cast<QVBoxLayout*>(layout())->addStretch();

  // Create a button box for OK and Cancel buttons
  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, controls);
  layout()->addWidget(buttonBox);

  // Disable enter
  auto okButton = buttonBox->button(QDialogButtonBox::Ok);
  okButton->setDefault(false);
  okButton->setAutoDefault(false);

  // Connect buttons to appropriate slots
  connect(buttonBox, &QDialogButtonBox::accepted, this, &EventDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &EventDialog::reject);
}

void EventDialog::createNameInput(QWidget* parent)
{
  QLabel* nameLabel = new QLabel(tr("Event name"), parent);
  layout()->addWidget(nameLabel);

  QLineEdit* name = new QLineEdit(parent);
  name->setText(mStorage->name);
  name->setFocusPolicy(mStorage->modifiable ? Qt::StrongFocus : Qt::NoFocus);
  name->setReadOnly(!mStorage->modifiable);

  connect(name, &QLineEdit::editingFinished, this, [=]() { mStorage->name = name->text(); });
  layout()->addWidget(name);
}

void EventDialog::createTypeInput(QWidget* parent)
{
  QLabel* eventTypeLabel = new QLabel(tr("Event type"), parent);
  layout()->addWidget(eventTypeLabel);

  QComboBox* type = new QComboBox(parent);
  type->setFocusPolicy(mStorage->modifiable ? Qt::ClickFocus : Qt::NoFocus);
  type->setEnabled(mStorage->modifiable);

  for (uint16_t i = (uint16_t)Types::ConnectorType::UNKNOWN + 1; i < (uint16_t)Types::ConnectorType::END; ++i)
    type->addItem(Types::ConnectorTypeToString((Types::ConnectorType)i));

  if (mStorage->type == Types::ConnectorType::UNKNOWN)
  {
    type->setCurrentIndex(0);
    mStorage->type = Types::StringToConnectorType(type->currentText());
  }
  else
  {
    type->setCurrentText(Types::ConnectorTypeToString(mStorage->type));
  }

  connect(type, &QComboBox::currentTextChanged, this, [=](const QString& text) { mStorage->type = Types::StringToConnectorType(text); });
  layout()->addWidget(type);
}

void EventDialog::createReturnTypeInput(QWidget* parent)
{
  QLabel* returnTypeLabel = new QLabel(tr("Return type"), parent);
  layout()->addWidget(returnTypeLabel);

  QComboBox* returnType = new QComboBox(parent);
  returnType->setFocusPolicy(mStorage->modifiable ? Qt::ClickFocus : Qt::NoFocus);
  returnType->setEnabled(mStorage->modifiable);

  for (uint16_t i = (uint16_t)Types::PropertyTypes::UNKNOWN + 1; i < (uint16_t)Types::PropertyTypes::END; ++i)
    returnType->addItem(Types::PropertyTypesToString((Types::PropertyTypes)i));

  if (mStorage->returnType == Types::PropertyTypes::UNKNOWN)
  {
    returnType->setCurrentIndex(0);
    mStorage->returnType = Types::StringToPropertyTypes(returnType->currentText());
  }
  else
  {
    returnType->setCurrentText(Types::PropertyTypesToString(mStorage->returnType));
  }

  connect(returnType, &QComboBox::currentTextChanged, this, [=](const QString& text) {
    mStorage->returnType = Types::StringToPropertyTypes(text);
  });
  layout()->addWidget(returnType);
}

void EventDialog::createArgumentInput(QWidget* parent)
{
  QLabel* argumentLabel = new QLabel(tr("Arguments"), parent);
  layout()->addWidget(argumentLabel);

  // Create table to hold the arguments
  QTableView* args = new QTableView(parent);
  QStandardItemModel* model = new QStandardItemModel(0, 2);
  args->setFocusPolicy(Qt::ClickFocus);
   
  model->setHorizontalHeaderItem(0, new QStandardItem(tr("Name")));
  model->setHorizontalHeaderItem(1, new QStandardItem(tr("Type")));

  args->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  args->setContextMenuPolicy(Qt::CustomContextMenu);

  addDynamicWidget((QVBoxLayout*)layout(), args, parent);

  if (mStorage->modifiable)
    args->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
  else
    args->setEditTriggers(QAbstractItemView::NoEditTriggers);
  args->setModel(model);

  for (const auto& field : mStorage->arguments)
  {
    int newRow = model->rowCount();
    model->insertRow(newRow);
    model->setItem(newRow, 0, new QStandardItem(field.id));
    model->setItem(newRow, 1, new QStandardItem(Types::PropertyTypesToString(field.type)));
  }

  connect(model, &QStandardItemModel::itemChanged, this, &EventDialog::updateArgumentTable);

  QPushButton* button = new QPushButton(parent);
  button->setEnabled(mStorage->modifiable);
  connect(button, &QPushButton::pressed, this, [=]() {
    int newRow = model->rowCount();
    model->insertRow(newRow);
    model->setItem(newRow, 0, new QStandardItem(""));
    model->setItem(newRow, 1, new QStandardItem(""));

    // Create new argument in the storage as well
    mStorage->arguments.push_back(PropertiesConfig());
  });

  button->setFocusPolicy(Qt::NoFocus);
  button->setText(tr("Add argument"));
  button->setMaximumWidth(100);
  layout()->addWidget(button);
}

void EventDialog::updateArgumentTable(QStandardItem* item)
{
  if (!item)
    return;

  int row = item->row();
  if (row >= mStorage->arguments.size())
  {
    LOG_WARNING("Tried to modify argument that does not exist");
    return;
  }

  int column = item->column();
  auto text = item->text();
  if (text.isNull() || text.isEmpty())
    return;

  if (column == 0)
    mStorage->arguments[row].id = text;
  else if (column == 1)
    mStorage->arguments[row].type = Types::StringToPropertyTypes(text);
}

void EventDialog::keyPressEvent(QKeyEvent* event)
{
  if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
  {
    QWidget* first = focusWidget();
    QWidget* next = first->nextInFocusChain();

    while (next && next != first)
    {
      // TODO: magic number
      if ((next->focusPolicy() & 0x8) && next->isEnabled() && next->isVisible())
      {
        next->setFocus();
        event->accept();
        return;
      }

      next = next->nextInFocusChain();
    }
  }

  QDialog::keyPressEvent(event);
}
