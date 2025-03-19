#include "behaviour_dialog.h"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include "app_configs.h"
#include "elements/node.h"

BehaviourDialog::BehaviourDialog(const QString& title, QWidget* parent)
    : QDialog(parent)
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

  // Add select widget with list of available nodes
  QComboBox* widget = new QComboBox(this);

  for (const auto& node : nodes)
    widget->addItem(node->nodeName());

  widget->setFont(Fonts::Property);
  layout()->addWidget(widget);

  static_cast<QVBoxLayout*>(layout())->addStretch();

  // Create a button box for OK and Cancel buttons
  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  layout()->addWidget(buttonBox);

  // Connect buttons to appropriate slots
  connect(buttonBox, &QDialogButtonBox::accepted, this, &BehaviourDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &BehaviourDialog::reject);
}
