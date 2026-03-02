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

#include "app_configs.h"
#include "common/theme.h"
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

  createNameInput(this);
  createTypeInput(this);
  createReturnTypeInput(this);
  createArgumentInput(this);

  layout()->setContentsMargins(10, 5, 10, 5);

  static_cast<QVBoxLayout*>(layout())->addStretch();

  updateIconTheme(mIcons);

  auto buttonBox = createButtons("Apply", "Cancel");
  connect(buttonBox, &QDialogButtonBox::accepted, this, &EventDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &EventDialog::reject);
}

void EventDialog::createNameInput(QWidget* parent)
{
  auto name = new maki::StringWidget(tr("Event name"), mStorage->getname(), parent);
  name->widget()->setFocusPolicy(mStorage->getmodifiable() ? Qt::StrongFocus : Qt::NoFocus);
  name->widget()->setReadOnly(!mStorage->getmodifiable());

  connect(name, &maki::StringWidget::valueChanged, this, [this](const QString& text) { mStorage->setName(text); });
  layout()->addWidget(name);
}

void EventDialog::createTypeInput(QWidget* parent)
{
  auto type = new maki::SelectorWidget(tr("Event type"), parent);
  type->setFocusPolicy(mStorage->getmodifiable() ? Qt::ClickFocus : Qt::NoFocus);
  type->setEnabled(mStorage->getmodifiable());

  for (uint16_t i = (uint16_t)Types::ConnectorType::UNKNOWN + 1; i < (uint16_t)Types::ConnectorType::END; ++i)
  {
    auto id = Types::ConnectorTypeToString((Types::ConnectorType)i);
    type->addItem(id, id);
  }

  connect(type, &maki::SelectorWidget::valueChanged, this, [=](const QString& text) { mStorage->setType(Types::StringToConnectorType(text)); });

  if (mStorage->gettype() == Types::ConnectorType::UNKNOWN)
  {
    type->setValue(Types::ConnectorTypeToString((Types::ConnectorType)((uint16_t)Types::ConnectorType::UNKNOWN + 1)));
    mStorage->setType(Types::StringToConnectorType(type->getValue()));
  }
  else
  {
    type->setValue(Types::ConnectorTypeToString(mStorage->gettype()));
  }

  layout()->addWidget(type);
}

void EventDialog::createReturnTypeInput(QWidget* parent)
{
  auto typeBox = new maki::TypeSelectionWidget(parent);

  auto returnType = new maki::SelectorWidget(tr("Return type"), typeBox, parent);
  returnType->setFocusPolicy(mStorage->getmodifiable() ? Qt::ClickFocus : Qt::NoFocus);
  returnType->setEnabled(mStorage->getmodifiable());

  connect(returnType, &maki::SelectorWidget::valueChanged, this, [this](const QString& text) {
    mStorage->setReturnType(Types::StringToPropertyTypes(text));
  });

  if (mStorage->getreturnType() == Types::PropertyTypes::UNKNOWN)
  {
    returnType->setValue(Types::PropertyTypesToString((Types::PropertyTypes)((uint16_t)Types::PropertyTypes::UNKNOWN + 1)));
    mStorage->setReturnType(Types::StringToPropertyTypes(returnType->getValue()));
  }
  else
  {
    returnType->setValue(Types::PropertyTypesToString(mStorage->getreturnType()));
  }

  layout()->addWidget(returnType);
}

void EventDialog::createArgumentInput(QWidget* parent)
{
  QLabel* argumentLabel = new QLabel(tr("Arguments"), parent);
  argumentLabel->setObjectName("PropertyLabel");
  layout()->addWidget(argumentLabel);

  // Create table to hold the arguments
  QTableView* args = new QTableView(parent);
  QStandardItemModel* model = new QStandardItemModel(0, 2);
  args->verticalHeader()->setVisible(false);
  args->setFocusPolicy(Qt::ClickFocus);

  model->setHorizontalHeaderItem(0, new QStandardItem(tr("Name")));
  model->setHorizontalHeaderItem(1, new QStandardItem(tr("Type")));

  args->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  args->setContextMenuPolicy(Qt::CustomContextMenu);

  addDynamicWidget((QVBoxLayout*)layout(), args, parent);

  if (mStorage->getmodifiable())
    args->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
  else
    args->setEditTriggers(QAbstractItemView::NoEditTriggers);
  args->setModel(model);

  for (const auto& field : mStorage->getarguments())
  {
    int newRow = model->rowCount();
    model->insertRow(newRow);
    model->setItem(newRow, 0, new QStandardItem(field->getid()));
    args->setIndexWidget(model->index(newRow, 1), new maki::TypeSelectionWidget(Types::PropertyTypesToString(field->gettype()), args));
  }

  connect(model, &QStandardItemModel::itemChanged, this, &EventDialog::updateArgumentTable);

  QPushButton* button = new QPushButton(parent);
  button->setObjectName("TextAndIcon");

  button->setEnabled(mStorage->getmodifiable());
  connect(button, &QPushButton::pressed, this, [=]() {
    int newRow = model->rowCount();
    model->insertRow(newRow);
    model->setItem(newRow, 0, new QStandardItem(""));
    args->setIndexWidget(model->index(newRow, 1), new maki::TypeSelectionWidget(args));

    // Create new argument in the storage as well
    mStorage->addArgument(std::make_shared<PropertyInfo>());
  });

  button->setFocusPolicy(Qt::NoFocus);
  button->setText(tr(" Add argument"));
  button->setIcon(addIconWithColor(":/icons/plus.svg", Config::FOREGROUND));
  button->setMaximumWidth(250);

  layout()->addWidget(args);
  layout()->addWidget(button);
}

void EventDialog::updateArgumentTable(QStandardItem* item)
{
  if (!item)
    return;

  int row = item->row();
  if (row >= mStorage->getarguments().size())
  {
    LOG_WARNING("Tried to modify argument that does not exist");
    return;
  }

  int column = item->column();
  auto text = item->text();
  if (text.isNull() || text.isEmpty())
    return;

  if (column == 0)
    std::dynamic_pointer_cast<PropertyInfo>(mStorage->getArgument(row))->setId(text);
  else if (column == 1)
    std::dynamic_pointer_cast<PropertyInfo>(mStorage->getArgument(row))->setType(Types::StringToPropertyTypes(text));
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
