#pragma once

#include <QColor>
#include <QFont>
#include <QString>

namespace Config
{
using namespace Qt::StringLiterals;

static const QString ORGANIZATION_NAME = QString("Savant"_L1);
static const QString APPLICATION_NAME = QString("MAKI"_L1);
static const QString VERSION = QString("0.0.1"_L1);

static const int GRID_SIZE = 20;
static const int CONNECTOR_RADIUS = 5;
static const qreal MINIMUM_NODE_SIZE = 50;
static const qreal OPACITY_THRESHOLD = 0.25;

// TODO(felaze): move to config
namespace Colours
{
static const QColor BACKGROUND = "#264653";
static const QColor FOREGROUND = "#2a9d8f";
static const QColor MAIN = "#2a9d8f";
static const QColor ACCENT = "#f4a261";
static const QColor TEXT = "#FFFFFF";
static const QColor EXTRA_ACCENT = "#e76f51";
};  // namespace Colours

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

class Fonts
{
public:
  static const QFont Main;
  static const QFont Property;
  static const QFont Label;
  static const QFont SmallTab;
  static const QFont MonoSpace;
  static constexpr float MaxSize = 10000.0;
  static constexpr float BaseSize = 10.0;
  static constexpr float BaseFactor = 20.0;
};
