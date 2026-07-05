#pragma once

#include "elements/node.h"

class QPainter;

namespace behaviour
{
/** Paints the selected capability component overlay on async/sync task nodes. */
void paintSelectedComponentOverlay(const NodeItem* node, QPainter* painter);
}  // namespace behaviour
