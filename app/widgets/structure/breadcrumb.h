#pragma once

#include <QWidget>

#include "types.h"
#include "widgets/frame.h"

namespace oclero::qlementine
{
class Label;
class IconWidget;
}  // namespace oclero::qlementine

class QLabel;

class BreadcrumbLabel : public StyledFrame
{
  Q_OBJECT

public:
  BreadcrumbLabel(const QString& text, QWidget* parent = nullptr);

  QString text() const;
  QIcon icon() const;

  void setText(const QString& text);
  void setIcon(const QIcon& icon);

  bool isCollapsed() const;
  void collapse();
  void expand();

private:
  QString mExpandedText;
  QLabel* mLabel;
  oclero::qlementine::IconWidget* mIcon;
};

class BreadcrumbWidget : public QWidget
{
  Q_OBJECT

public:
  explicit BreadcrumbWidget(QWidget* parent = nullptr);

  void setProject(const QString& project);
  void setTab(const QString& tab, Types::LibraryTypes type);
  void setBlock(const QString& block, Types::LibraryTypes type);

private:
  BreadcrumbLabel* mProjectName = nullptr;
  oclero::qlementine::IconWidget* mTabSeparator = nullptr;
  BreadcrumbLabel* mTabName = nullptr;
  oclero::qlementine::IconWidget* mBlockSeparator = nullptr;
  BreadcrumbLabel* mBlockName = nullptr;
  bool mRepositionQueued = false;

  void updateLabel(const QString& text, BreadcrumbLabel* label, const QIcon& labelIcon, oclero::qlementine::IconWidget* icon);
  void setIconOptions(oclero::qlementine::IconWidget* icon);

  QIcon libraryTypeToIcon(Types::LibraryTypes type, bool isBlock) const;

  void reposition();
  void scheduleReposition();
  void applySize();
  bool eventFilter(QObject* watched, QEvent* event);
};
