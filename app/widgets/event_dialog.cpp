#include "event_dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QStandardItemModel>
#include <QVBoxLayout>

#include "app_configs.h"
#include "event_config.h"
#include "logging.h"
#include "style_helpers.h"

EventDialog::EventDialog(const QString& title, QWidget* parent)
    : QDialog(parent)
    , mName((nullptr))
{
  setWindowTitle(title);

  // Create layout and widgets
  QVBoxLayout* layout = new QVBoxLayout;
  setLayout(layout);

  // TODO(felaze): Make this dynamic
  resize(500, 400);
}

QString EventDialog::getName() const
{
  return mName->text();
}

QString EventDialog::getType() const
{
  return mType->currentText();
}

QString EventDialog::getReturnType() const
{
  return mReturnType->currentText();
}

QStandardItemModel* EventDialog::getArguments() const
{
  return (QStandardItemModel*)mArguments->model();
}

void EventDialog::setup(const EventConfig& event)
{
  QWidget* controls = new QWidget(this);
  QHBoxLayout* controlLayout = new QHBoxLayout(controls);
  controlLayout->setContentsMargins(10, 0, 10, 0);
  layout()->setAlignment(Qt::AlignCenter);

  controls->setLayout(controlLayout);
  layout()->addWidget(controls);

  // -------------------------------------------------------------------------------
  QLabel* nameLabel = new QLabel("Event name:", controls);
  layout()->addWidget(nameLabel);

  mName = new QLineEdit(controls);
  mName->setText(event.id);
  layout()->addWidget(mName);

  // -------------------------------------------------------------------------------
  QLabel* eventTypeLabel = new QLabel("Event type", controls);
  layout()->addWidget(eventTypeLabel);

  mType = new QComboBox(controls);
  for (uint16_t i = 1; i < (uint16_t)Types::ConnectorType::END; ++i)
    mType->addItem(Types::ConnectorTypeToString((Types::ConnectorType)i));

  mType->setCurrentText(Types::ConnectorTypeToString(event.type));
  layout()->addWidget(mType);

  // -------------------------------------------------------------------------------
  QLabel* returnTypeLabel = new QLabel("Return type", controls);
  layout()->addWidget(returnTypeLabel);

  mReturnType = new QComboBox(controls);
  for (uint16_t i = 1; i < (uint16_t)Types::PropertyTypes::END; ++i)
    mReturnType->addItem(Types::PropertyTypesToString((Types::PropertyTypes)i));

  mReturnType->setCurrentText(Types::PropertyTypesToString(event.returnType));
  layout()->addWidget(mReturnType);

  // -------------------------------------------------------------------------------
  QLabel* argumentLabel = new QLabel("Arguments", controls);
  layout()->addWidget(argumentLabel);

  // Create table to hold the arguments
  mArguments = new QTableView(controls);
  QStandardItemModel* model = new QStandardItemModel(0, 2);

  model->setHorizontalHeaderItem(0, new QStandardItem("Name"));
  model->setHorizontalHeaderItem(1, new QStandardItem("Type"));

  mArguments->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  mArguments->setContextMenuPolicy(Qt::CustomContextMenu);

  addDynamicWidget((QVBoxLayout*)layout(), mArguments, controls);

  mArguments->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
  mArguments->setModel(model);

  for (const auto& field : event.arguments)
  {
    int newRow = model->rowCount();
    model->insertRow(newRow);
    model->setItem(newRow, 0, new QStandardItem(field.id));
    model->setItem(newRow, 1, new QStandardItem(field.typeToString()));
  }

  QPushButton* button = new QPushButton(controls);
  connect(button, &QPushButton::pressed, this, [=]() {
    int newRow = model->rowCount();
    model->insertRow(newRow);
    model->setItem(newRow, 0, new QStandardItem(""));
    model->setItem(newRow, 1, new QStandardItem(""));
  });

  button->setText("Add argument");
  button->setMaximumWidth(100);
  layout()->addWidget(button);

  layout()->setContentsMargins(10, 5, 10, 5);

  // -------------------------------------------------------------------------------
  static_cast<QVBoxLayout*>(layout())->addStretch();

  // -------------------------------------------------------------------------------
  // Create a button box for OK and Cancel buttons
  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, controls);
  layout()->addWidget(buttonBox);

  // Connect buttons to appropriate slots
  connect(buttonBox, &QDialogButtonBox::accepted, this, &EventDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &EventDialog::reject);
}
