#pragma once

#include <QBrush>
#include <QFont>
#include <QGraphicsScene>
#include <QHash>
#include <QJsonArray>
#include <QJsonObject>
#include <QPen>
#include <QString>

#include "component_model.h"
#include "itab.h"
#include "simulation_label_item.h"

class TraceSceneBuilder
{
public:
  struct Style
  {
    // Layout
    qreal topMargin = 14;
    qreal leftMargin = 18;
    qreal columnWidth = 150;  // distance between lifelines
    qreal headerWidth = 130;
    qreal headerHeight = 44;
    qreal headerRadius = 10;
    qreal headerToLifelineGap = 20;

    qreal lifelineTopPadding = 8;
    qreal lifelineBottomPadding = 18;

    qreal labelBoxHeight = 22;
    qreal labelBoxHMargin = 10;
    qreal labelBoxRadius = 6;
    qreal labelVSpacing = 8;

    qreal sceneMinHeight = 420;
    qreal labelAreaHeight = 130;  // reserved height at bottom for label pills

    qreal timeStepHeight = 25;  // vertical distance per time unit
    qreal eventVSpacing = 10;   // extra spacing between events at same time
    qreal arrowHeadSize = 7;

    // Groups
    qreal groupTopMargin = 6;
    qreal groupHeight = 18;
    qreal groupVSpacing = 6;
    qreal groupRadius = 6;

    QColor groupFill = QColor("#F7F7F7");
    QColor groupText = QColor("#555555");

    // Fonts
    QColor headerFillDefault = QColor("#FFF7A8");  // warm yellow
    QColor headerBorder = QColor("#8A8A8A");

    QColor lifelineColour = QColor("#D0D0D0");

    QColor labelFill = QColor("#F2F2F2");
    QColor labelBorder = QColor("#BDBDBD");
    QColor labelIllegalFill = QColor("#FAD4D4");
    QColor labelIllegalBorder = QColor("#C44C4C");
    QColor labelIllegalText = QColor("#8B1E1E");
  };

  using LabelClickHandler = std::function<void(const QString& instance, const QString& labelText, bool illegal)>;

  TraceSceneBuilder(const maki::Theme& theme, Style style);

  // Main entry: clears and rebuilds the scene from the given traceUpdate JSON object.
  bool buildScene(const QJsonObject& traceUpdateMsg, QGraphicsScene* scene, LabelClickHandler clickHandler, QString* errorOut = nullptr);

private:
  Style mStyle;
  maki::Theme mTheme;
  TraceLabelItem::Style* mButtonStyle;

  QSet<QString> mCollapsedComponents;
  QJsonObject mLastTraceUpdateMsg;
  QGraphicsScene* mLastScene = nullptr;
  LabelClickHandler mLastClickHandler;

  // Parsing
  bool parseTraceUpdate(const QJsonObject& msg, QVector<RawLifeline>& out, QString* errorOut) const;
  QVector<RawLifeline::State> extractStateForInstance(const QJsonObject& traceData, const QString& instance) const;

  // Rendering
  qreal renderHeader(QGraphicsScene* scene, const ComponentTreeModel& model, ComponentNode* node, QHash<QString, qreal>& outXByLeafInstance);

  void renderBottomLabels(QGraphicsScene* scene, const ComponentTreeModel& model, const QHash<QString, qreal>& xByLeafInstance,
                          qreal lineTopY, qreal diagramBottomY, LabelClickHandler clickHandler) const;

  qreal renderEvents(QGraphicsScene* scene, const ComponentTreeModel& model, const QJsonArray& events,
                     const QJsonArray& lifelinesJson, const QHash<QString, qreal>& xByLeafInstance, qreal diagramTopY) const;

  // Helpers
  QColor headerFillForRole(const QString& role, bool isLeaf) const;

  void addRoundedRect(QGraphicsScene* scene, const QRectF& r, qreal radius, const QPen& pen, const QBrush& brush) const;
  void addCenteredText(QGraphicsScene* scene, const QRectF& within, const QString& text, const QFont& font, const QColor& colour = Qt::black) const;
};
