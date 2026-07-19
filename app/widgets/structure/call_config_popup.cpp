#include "call_config_popup.h"

#include <QCloseEvent>
#include <QEvent>
#include <QFormLayout>
#include <QFrame>
#include <QHBoxLayout>
#include <QHideEvent>
#include <QIcon>
#include <QJsonArray>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMouseEvent>
#include <QSize>
#include <QSizePolicy>
#include <QToolButton>
#include <QWidgetAction>
#include <QVBoxLayout>

#include <functional>

#include <oclero/qlementine/style/QlementineStyle.hpp>

#include "elements/behaviour/call_capability.h"
#include "elements/behaviour/call_node.h"
#include "keys.h"
#include "save_info.h"
#include "style_helpers.h"
#include "system/config_table.h"
#include "types.h"
#include "widgets/capability_icon_menu.h"

namespace
{
constexpr int kPopupMinWidth = 300;
constexpr int kCapabilityCols = 3;
constexpr int kHelpIconPx = 14;

const char kArgIndexProp[] = "callArgIndex";
const char kArgTypeProp[] = "callArgType";

QIcon helpQuestionIcon()
{
  return QIcon(iconPathFromTheme(QStringLiteral("questionmark_tooltip.svg")));
}

void addModeSectionHeader(QMenu* menu, const QString& title, const QString& tooltip)
{
  auto* host = new QWidget(menu);
  oclero::qlementine::QlementineStyle::setAutoIconColor(host, oclero::qlementine::AutoIconColor::None);

  auto* row = new QHBoxLayout(host);
  row->setContentsMargins(10, 6, 10, 2);
  row->setSpacing(6);

  auto* label = new QLabel(title, host);
  QFont font = label->font();
  font.setBold(true);
  label->setFont(font);

  auto* help = new QLabel(host);
  help->setPixmap(helpQuestionIcon().pixmap(kHelpIconPx, kHelpIconPx));
  help->setFixedSize(kHelpIconPx, kHelpIconPx);
  help->setToolTip(tooltip);
  help->setCursor(Qt::WhatsThisCursor);

  row->addWidget(label, 0, Qt::AlignVCenter);
  row->addWidget(help, 0, Qt::AlignVCenter);
  row->addStretch(1);

  auto* action = new QWidgetAction(menu);
  action->setDefaultWidget(host);
  menu->addAction(action);
}

/** Event row with selected styling and hover feedback. */
class EventMenuRow : public QWidget
{
public:
  std::function<void()> onClicked;

  EventMenuRow(const QString& name, bool selected, QWidget* parent = nullptr)
      : QWidget(parent)
      , mSelected(selected)
  {
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
    setCursor(Qt::PointingHandCursor);

    auto* row = new QHBoxLayout(this);
    row->setContentsMargins(18, 5, 10, 5);
    row->setSpacing(0);

    mLabel = new QLabel(name, this);
    if (selected)
    {
      QFont font = mLabel->font();
      font.setBold(true);
      mLabel->setFont(font);
    }
    row->addWidget(mLabel);
    row->addStretch(1);
    applyStyle(false);
  }

protected:
  bool event(QEvent* event) override
  {
    if (event->type() == QEvent::HoverEnter)
    {
      applyStyle(true);
      return true;
    }
    if (event->type() == QEvent::HoverLeave)
    {
      applyStyle(false);
      return true;
    }
    return QWidget::event(event);
  }

  void mouseReleaseEvent(QMouseEvent* event) override
  {
    if (event->button() == Qt::LeftButton && rect().contains(event->pos()) && onClicked)
      onClicked();
    QWidget::mouseReleaseEvent(event);
  }

private:
  void applyStyle(bool hovered)
  {
    if (hovered)
    {
      setStyleSheet(QStringLiteral(
          "QWidget { background-color: palette(highlight); border-radius: 4px; }"
          "QLabel { color: palette(highlighted-text); background: transparent; }"));
    }
    else if (mSelected)
    {
      setStyleSheet(QStringLiteral(
          "QWidget { background-color: palette(midlight); border-radius: 4px; }"
          "QLabel { background: transparent; }"));
    }
    else
    {
      setStyleSheet(QStringLiteral(
          "QWidget { background-color: transparent; border-radius: 4px; }"
          "QLabel { background: transparent; }"));
    }
  }

  bool mSelected = false;
  QLabel* mLabel = nullptr;
};

void addEventMenuItem(QMenu* menu, const QString& name, bool selected, const std::function<void()>& onChosen)
{
  auto* row = new EventMenuRow(name, selected, menu);
  row->onClicked = [menu, onChosen]() {
    if (onChosen)
      onChosen();
    if (menu)
      menu->hide();
  };

  auto* action = new QWidgetAction(menu);
  action->setDefaultWidget(row);
  menu->addAction(action);
}
}  // namespace

void CallConfigPopup::open(QWidget* parent,
                           CallNode* node,
                           SaveInfo* storage,
                           const ConfigurationTable* configTable,
                           const QPoint& globalAnchor)
{
  if (!node || !storage)
    return;

  auto* popup = new CallConfigPopup(parent);
  popup->setAttribute(Qt::WA_DeleteOnClose);
  popup->setup(node, storage, configTable);
  popup->adjustSize();

  QPoint pos = globalAnchor;
  pos.setX(pos.x() - popup->width() / 2);
  popup->move(pos);
  popup->show();
  popup->raise();
  popup->activateWindow();
}

CallConfigPopup::CallConfigPopup(QWidget* parent)
    : QWidget(parent, Qt::Popup | Qt::FramelessWindowHint)
{
  setObjectName(QStringLiteral("CallConfigPopup"));
  setMinimumWidth(kPopupMinWidth);
  oclero::qlementine::QlementineStyle::setAutoIconColor(this, oclero::qlementine::AutoIconColor::None);

  auto* root = new QVBoxLayout(this);
  root->setContentsMargins(10, 10, 10, 10);
  root->setSpacing(8);

  auto* frame = new QFrame(this);
  frame->setFrameShape(QFrame::StyledPanel);
  auto* frameLayout = new QVBoxLayout(frame);
  frameLayout->setContentsMargins(8, 8, 8, 8);
  frameLayout->setSpacing(8);

  frameLayout->addWidget(new QLabel(tr("Capability"), frame));
  mCapabilityGrid = new CapabilityIconGrid(frame);
  mCapabilityGrid->setColumns(kCapabilityCols);
  connect(mCapabilityGrid, &CapabilityIconGrid::itemSelected, this, [this](const CapabilityIconMenuItem& item) {
    applyCapability(item.name, item.id);
  });
  frameLayout->addWidget(mCapabilityGrid);

  frameLayout->addWidget(new QLabel(tr("Event"), frame));
  mEventButton = new QToolButton(frame);
  mEventButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
  mEventButton->setPopupMode(QToolButton::InstantPopup);
  mEventButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  mEventButton->setEnabled(false);
  mEventButton->setText(tr("Select event"));
  frameLayout->addWidget(mEventButton);

  frameLayout->addWidget(new QLabel(tr("Arguments"), frame));
  mArgsHost = new QWidget(frame);
  mArgsLayout = new QVBoxLayout(mArgsHost);
  mArgsLayout->setContentsMargins(0, 0, 0, 0);
  mArgsLayout->setSpacing(4);
  frameLayout->addWidget(mArgsHost);

  root->addWidget(frame);
}

void CallConfigPopup::setup(CallNode* node, SaveInfo* storage, const ConfigurationTable* configTable)
{
  mNode = node;
  mStorage = storage;
  mConfigTable = configTable;
  mSelectedCapabilityName = call_capability::capabilityName(*node);
  mSelectedCapabilityId = call_capability::resolveCapabilityId(*node, *storage);
  rebuildCapabilityGrid();
  refreshDetails();
}

void CallConfigPopup::rebuildCapabilityGrid()
{
  if (!mCapabilityGrid)
    return;

  if (!mNode || !mStorage)
  {
    mCapabilityGrid->setItems({});
    return;
  }

  mCapabilityGrid->setSelectedId(mSelectedCapabilityId);
  mCapabilityGrid->setSelectedName(mSelectedCapabilityName);
  mCapabilityGrid->setItems(
      CapabilityIconMenu::itemsFromPossibleCallers(*mStorage, mNode->id(), mConfigTable));
}

void CallConfigPopup::applyCapability(const QString& name, const QString& id)
{
  if (!mNode || !mStorage || id.isEmpty())
    return;

  mSelectedCapabilityName = name;
  mSelectedCapabilityId = id;
  mNode->assignCapability(name, id, mStorage);
  rebuildCapabilityGrid();
  refreshDetails();
}

void CallConfigPopup::applyEvent(const QString& mode, const QString& eventName)
{
  if (!mNode || mode.isEmpty() || eventName.isEmpty())
    return;

  mSelectedMode = mode;
  mSelectedEventName = eventName;
  mEventButton->setText(eventName);
  call_capability::applyModeAndEvent(*mNode, mode, eventName);
  rebuildArguments(mSelectedCapabilityId, eventName);
  mNode->update();
  rebuildEventMenu(mSelectedCapabilityId);
}

void CallConfigPopup::rebuildEventMenu(const QString& capabilityId)
{
  auto* menu = new QMenu(mEventButton);
  oclero::qlementine::QlementineStyle::setAutoIconColor(menu, oclero::qlementine::AutoIconColor::None);

  auto addSection = [&](const QString& sectionMode, const QString& title, const QString& tip) {
    const auto events =
        mStorage->getEventsOfTypeFromNode(capabilityId, call_capability::eventTypesForMode(sectionMode));
    if (events.isEmpty())
      return;

    addModeSectionHeader(menu, title, tip);
    for (const auto& event : events)
    {
      const QString name = event->getname();
      const bool selected = (sectionMode == mSelectedMode && name == mSelectedEventName);
      addEventMenuItem(menu, name, selected, [this, sectionMode, name]() { applyEvent(sectionMode, name); });
    }
  };

  if (call_capability::canAsync(*mStorage, capabilityId))
    addSection(call_capability::kModeAsync, tr("Async"), tr("Wait for return"));
  if (call_capability::canSync(*mStorage, capabilityId))
    addSection(call_capability::kModeSync, tr("Sync"), tr("Immediate return"));

  if (QMenu* old = mEventButton->menu())
  {
    mEventButton->setMenu(nullptr);
    old->deleteLater();
  }
  mEventButton->setMenu(menu);
}

void CallConfigPopup::refreshDetails()
{
  if (!mNode || !mStorage)
    return;

  flushArgumentEdits();
  clearLayout(mArgsLayout);
  mEventButton->setEnabled(false);
  mEventButton->setText(tr("Select event"));
  if (QMenu* old = mEventButton->menu())
  {
    mEventButton->setMenu(nullptr);
    old->deleteLater();
  }

  if (mSelectedCapabilityId.isEmpty())
    return;

  const QString capabilityId = mSelectedCapabilityId;
  QString mode = mNode->getProperty(call_capability::kModeProperty).toString();
  QString eventName = call_capability::currentEventName(*mNode);
  call_capability::resolveModeAndEvent(*mStorage, capabilityId, mode, eventName);
  if (mode.isEmpty() || eventName.isEmpty())
    return;

  mSelectedMode = mode;
  mSelectedEventName = eventName;
  call_capability::applyModeAndEvent(*mNode, mode, eventName);
  mEventButton->setText(eventName);
  mEventButton->setEnabled(true);
  rebuildEventMenu(capabilityId);
  rebuildArguments(capabilityId, eventName);
  mNode->update();
}

void CallConfigPopup::rebuildArguments(const QString& capabilityId, const QString& eventName)
{
  flushArgumentEdits();
  clearLayout(mArgsLayout);
  if (!mNode || !mStorage || capabilityId.isEmpty() || eventName.isEmpty())
    return;

  const auto event = mStorage->getEventFromNode(capabilityId, eventName);
  if (!event || event->getarguments().isEmpty())
    return;

  const QVariant propValue = mNode->getProperty(call_capability::kCapabilityProperty);
  if (!propValue.isValid())
    return;

  const QJsonArray argArray = propValue.toJsonObject().value(ConfigKeys::OPTIONS).toArray();
  int index = call_capability::kArgIndex;

  auto* form = new QFormLayout();
  form->setContentsMargins(0, 0, 0, 0);
  form->setSpacing(4);

  for (const auto& arg : event->getarguments())
  {
    const Types::PropertyTypes argType = arg->gettype();
    if (argType != Types::PropertyTypes::INTEGER && argType != Types::PropertyTypes::REAL
        && argType != Types::PropertyTypes::STRING && argType != Types::PropertyTypes::BOOLEAN)
    {
      ++index;
      continue;
    }

    const QJsonObject jsonItem = index < argArray.size() ? argArray.at(index).toObject() : QJsonObject();
    auto* edit = new QLineEdit(mArgsHost);
    edit->setPlaceholderText(arg->getdefaultValue().toString());
    if (jsonItem.contains(ConfigKeys::DATA))
      edit->setText(jsonItem.value(ConfigKeys::DATA).toString());
    else if (!arg->getdefaultValue().toString().isEmpty())
      edit->setText(arg->getdefaultValue().toString());

    edit->setProperty(kArgIndexProp, index);
    edit->setProperty(kArgTypeProp, static_cast<int>(argType));
    form->addRow(arg->getid(), edit);

    connect(edit, &QLineEdit::editingFinished, this, [this, edit]() { commitArgumentEdit(edit); });
    connect(edit, &QLineEdit::textEdited, this, [this, edit](const QString&) { commitArgumentEdit(edit); });
    ++index;
  }

  mArgsLayout->addLayout(form);
}

void CallConfigPopup::commitArgumentEdit(QLineEdit* edit)
{
  if (!edit || !mNode)
    return;

  const QVariant indexVar = edit->property(kArgIndexProp);
  const QVariant typeVar = edit->property(kArgTypeProp);
  if (!indexVar.isValid() || !typeVar.isValid())
    return;

  const int argIndex = indexVar.toInt();
  const auto argType = static_cast<Types::PropertyTypes>(typeVar.toInt());
  call_capability::writeCapabilityOption(*mNode,
                                          argIndex,
                                          edit->text(),
                                          argType,
                                          call_capability::isArgumentVariable(argType, edit->text()));
}

void CallConfigPopup::flushArgumentEdits()
{
  if (!mArgsHost)
    return;
  for (QLineEdit* edit : mArgsHost->findChildren<QLineEdit*>())
    commitArgumentEdit(edit);
}

void CallConfigPopup::hideEvent(QHideEvent* event)
{
  flushArgumentEdits();
  QWidget::hideEvent(event);
}

void CallConfigPopup::closeEvent(QCloseEvent* event)
{
  flushArgumentEdits();
  QWidget::closeEvent(event);
}
