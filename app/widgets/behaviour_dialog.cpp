#include "behaviour_dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include "app_configs.h"
#include "elements/node.h"
#include "logging.h"
#include "system/canvas.h"

BehaviourDialog::BehaviourDialog(const QString& title, QWidget* parent)
    : QDialog(parent)
    , mName((nullptr))
    , mListWidget(nullptr)
{
  setWindowTitle(title);

  // Create layout and widgets
  QVBoxLayout* layout = new QVBoxLayout(this);
  setLayout(layout);

  resize(500, 400);
}

QString BehaviourDialog::getName() const
{
  return mName->text();
}

void BehaviourDialog::setup(const QList<NodeItem*>& nodes)
{
  QLabel* nameLabel = new QLabel("Behaviour name:", this);
  layout()->addWidget(nameLabel);

  mName = new QLineEdit(this);
  layout()->addWidget(mName);

  QLabel* startNodeLabel = new QLabel("Starting point:", this);
  layout()->addWidget(startNodeLabel);

  mListWidget = new QListWidget(this);
  layout()->addWidget(mListWidget);

  static_cast<QVBoxLayout*>(layout())->addStretch();

  // Create a button box for OK and Cancel buttons
  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  layout()->addWidget(buttonBox);

  // Connect buttons to appropriate slots
  connect(buttonBox, &QDialogButtonBox::accepted, this, &BehaviourDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &BehaviourDialog::reject);
}

void BehaviourDialog::nodeSelected(NodeItem* node, bool selected)
{
  if (!mListWidget || !node)
    return;

  if (selected)
  {
    QListWidgetItem* listItem = new QListWidgetItem(node->nodeName(), mListWidget);
    listItem->setData(Qt::UserRole, QVariant::fromValue(node->id()));
  }
  else
  {
    for (int i = 0; i < mListWidget->count(); ++i)
    {
      if (mListWidget->item(i)->data(Qt::UserRole).toString() == node->id())
      {
        mListWidget->takeItem(i);
        break;
      }
    }
  }
}
