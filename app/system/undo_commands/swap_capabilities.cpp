#include "swap_capabilities.h"

#include <QObject>

#include "logging.h"
#include "system/canvas.h"

SwapCapabilitiesCommand::SwapCapabilitiesCommand(Canvas* canvas, const QString& parentId, const QString& aId,
                                                 const QString& bId, QUndoCommand* parent)
    : QUndoCommand(parent)
    , mCanvas(canvas)
    , mParentId(parentId)
    , mAId(aId)
    , mBId(bId)
{
  setText(QObject::tr("Swap capabilities"));
}

void SwapCapabilitiesCommand::redo()
{
  if (!mCanvas)
    return;

  mCanvas->swapCapabilities(mParentId, mAId, mBId);

  LOG_TRACE("SwapCapabilitiesCommand redo parent=%s", qPrintable(mParentId));
}

void SwapCapabilitiesCommand::undo()
{
  redo();
}
