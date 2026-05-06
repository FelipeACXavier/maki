#pragma once

#include <QColor>
#include <QFont>
#include <QString>

namespace Config
{
using namespace Qt::StringLiterals;

/** @brief Configuration constants for the application. */
static const QString ORGANIZATION_NAME = QString("Savant"_L1);  /// Organization name.
static const QString APPLICATION_NAME = QString("MAKI"_L1);     /// Application name.
static const QString VERSION = QString("0.0.1"_L1);             /// Application version.

static const QString MAIN_CANVAS = QString("MainSystemCanvas"_L1);  /// Main canvas identifier.

static const int GRID_SIZE = 20;              /// Grid size for the application.
static const int CONNECTOR_RADIUS = 5;        /// Radius of connectors in pixels.
static const qreal MINIMUM_NODE_SIZE = 50;    /// Minimum size of a node.
static const qreal OPACITY_THRESHOLD = 0.25;  /// Opacity threshold for visibility.

static const int CONTENT_PADDING = 5;  /// Padding around content.

extern QColor FOREGROUND;  /// Foreground color.
extern QColor HIGHLIGHT;   /// Highlight color.
extern QColor HOVER;       /// Hover color.
}  // namespace Config

namespace Constants
{
/** @brief Various constants used throughout the application. */
static const QString TMP_CONNECTION_ID = QStringLiteral("tmp_id");                 /// Temporary connection identifier.
static const QString TYPE_NODE = QStringLiteral("application/x-node");             /// MIME type for nodes.
static const QString TYPE_PIXMAP = QStringLiteral("application/x-pixmap");         /// MIME type for pixmaps.
static const QString TYPE_CONFIG = QStringLiteral("application/x-configuration");  /// MIME type for configurations.
static const QString TYPE_NODE_ID = QStringLiteral("application/x-node-id");       /// MIME type for node IDs.

static const qreal CONTROL_POINT_SHIFT = 100;  /// Shift value for control points.
}  // namespace Constants

class Fonts
{
public:
  /** @brief Font constants for the application. */
  static const QFont Main;       /// Main font.
  static const QFont Property;   /// Property font.
  static const QFont Label;      /// Label font.
  static const QFont Hint;       /// Hint font.
  static const QFont SmallTab;   /// Small tab font.
  static const QFont MonoSpace;  /// Monospace font.

  /** @brief Maximum size for fonts. */
  static constexpr float MaxSize = 10000.0;

  /** @brief Base size for fonts. */
  static constexpr float BaseSize = 10.0;

  /** @brief Base factor for font scaling. */
  static constexpr float BaseFactor = 20.0;
};
