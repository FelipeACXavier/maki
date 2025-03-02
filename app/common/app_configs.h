#pragma once

#include <QString>

namespace Config
{
using namespace Qt::StringLiterals;

static const QString ORGANIZATION_NAME = QString("Savant"_L1);
static const QString APPLICATION_NAME = QString("Low-Code Platform"_L1);
static const QString VERSION = QString("1.0.0"_L1);

static const int GRID_SIZE = 20;
static const int CONNECTOR_RADIUS = 5;
}  // namespace Config

namespace Constants
{
static const QString TMP_CONNECTION_ID = QStringLiteral("tmp_id");
static const QString TYPE_NODE = QStringLiteral("application/x-node");
static const QString TYPE_PIXMAP = QStringLiteral("application/x-pixmap");
static const QString TYPE_CONFIG = QStringLiteral("application/x-configuration");
static const QString TYPE_NODE_ID = QStringLiteral("application/x-node-id");

static const qreal CONTROL_POINT_SHIFT = 100;
}  // namespace Constants
