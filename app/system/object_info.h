#include <QDebug>
#include <QEvent>
#include <QObject>
#include <QSet>

static const char* eventName(QEvent::Type type)
{
  switch (type)
  {
    case QEvent::DeferredDelete:
      return "DeferredDelete";
    case QEvent::MouseButtonPress:
      return "MouseButtonPress";
    case QEvent::MouseButtonRelease:
      return "MouseButtonRelease";
    case QEvent::MouseMove:
      return "MouseMove";
    case QEvent::KeyPress:
      return "KeyPress";
    case QEvent::KeyRelease:
      return "KeyRelease";
    case QEvent::Paint:
      return "Paint";
    case QEvent::Resize:
      return "Resize";
    case QEvent::Move:
      return "Move";
    case QEvent::Show:
      return "Show";
    case QEvent::Hide:
      return "Hide";
    case QEvent::Close:
      return "Close";
    case QEvent::Timer:
      return "Timer";
    case QEvent::ChildAdded:
      return "ChildAdded";
    case QEvent::ChildRemoved:
      return "ChildRemoved";
    case QEvent::MetaCall:
      return "MetaCall";
    default:
      return "Other";
  }
}

static QSet<quintptr> liveObjects;
static QString objectInfo(QObject* obj)
{
  if (!obj)
    return "<null>";

  QStringList parts;

  QObject* current = obj;
  while (current)
  {
    parts << QString("%1@0x%2 name='%3'")
                 .arg(current->metaObject()->className())
                 .arg(reinterpret_cast<quintptr>(current), 0, 16)
                 .arg(current->objectName());

    current = current->parent();
  }

  return parts.join(" <- parent ");
}

static void watch(QObject* obj)
{
  if (!obj)
    return;

  const auto addr = reinterpret_cast<quintptr>(obj);

  if (liveObjects.contains(addr))
    return;

  liveObjects.insert(addr);

  QObject::connect(obj, &QObject::destroyed, [addr] {
    liveObjects.remove(addr);
    qDebug().nospace()
        << "Destroyed 0x" << Qt::hex << addr << Qt::dec;
  });
}
static void print(const QString& message, quintptr address, QEvent::Type type)
{
  qWarning().nospace()
      << message
      << " - object 0x" << Qt::hex << address << Qt::dec
      << " event=" << eventName(type)
      << " known=" << liveObjects.contains(address);
}

static void notify_debug(QObject* receiver, QEvent* event)
{
  if (event)
  {
    const auto addr = reinterpret_cast<quintptr>(receiver);
    if (event->type() == QEvent::ChildAdded)
    {
      auto* childEvent = static_cast<QChildEvent*>(event);
      watch(childEvent->child());
      print("ChildAdded", addr, event->type());

      if (QObject* child = childEvent->child())
        qDebug().noquote() << "ChildAdded:" << objectInfo(child);
    }
    else if (event->type() == QEvent::DeferredDelete)
    {
      print("DeleteEvent", addr, event->type());
    }
  }

  if (event && receiver)
  {
    const auto addr = reinterpret_cast<quintptr>(receiver);
    if (!liveObjects.contains(addr))
      print("Event to untracked/dead", addr, event->type());
  }
}
