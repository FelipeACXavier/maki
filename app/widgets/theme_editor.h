// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <QWidget>
#include <memory>
#include <oclero/qlementine/style/Theme.hpp>

namespace oclero::qlementine
{
class ThemeEditorWidget : public QWidget
{
  Q_OBJECT

public:
  explicit ThemeEditorWidget(QWidget* parent = nullptr);
  ~ThemeEditorWidget() override;

public:
  const Theme& theme() const;
  void setTheme(const Theme& theme);
  void setDefaultPath(const QString& savePath);

Q_SIGNALS:
  void themeChanged(const oclero::qlementine::Theme& theme);
  void themeSaved(const QString& path, const oclero::qlementine::Theme& theme);
  void themeLoaded(const QString& path, const oclero::qlementine::Theme& theme);

private:
  struct Impl;
  std::unique_ptr<Impl> _impl;
};
}  // namespace oclero::qlementine
