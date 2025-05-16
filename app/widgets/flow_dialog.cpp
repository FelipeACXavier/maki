#include "flow_dialog.h"

#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include "system/canvas.h"

FlowDialog::FlowDialog(const QString& title, QWidget* parent)
    : QDialog(parent)
{
  setWindowTitle(title);

  // Create layout and widgets
  QVBoxLayout* layout = new QVBoxLayout(this);
  setLayout(layout);

  // TODO(felaze): Make this dynamic
  resize(500, 400);

  setWindowFlags(Qt::Window | Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowStaysOnTopHint);
  setAttribute(Qt::WA_DeleteOnClose);
}

void FlowDialog::setup(std::shared_ptr<ConfigurationTable> configTable)
{
  // Flow name
  QWidget* name = new QWidget(this);
  QHBoxLayout* nameLayout = new QHBoxLayout(name);
  nameLayout->setContentsMargins(10, 0, 10, 0);
  layout()->setAlignment(Qt::AlignCenter);

  name->setLayout(nameLayout);
  layout()->addWidget(name);

  // -------------------------------------------------------------------------------
  QLabel* nameLabel = new QLabel("Flow name:", name);
  nameLayout->addWidget(nameLabel);

  QLineEdit* nameField = new QLineEdit(name);
  nameField->setText("");
  nameLayout->addWidget(nameField);

  // Canvas
  mCanvasView = new CanvasView();
  Canvas* canvas = new Canvas(configTable, mCanvasView);
  mCanvasView->setScene(canvas);

  // Add default start and end nodes

  layout()->addWidget(mCanvasView);

  // Button box
  QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
  layout()->addWidget(buttonBox);

  // Connect buttons to appropriate slots
  connect(buttonBox, &QDialogButtonBox::accepted, this, &FlowDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, this, &FlowDialog::reject);
}
