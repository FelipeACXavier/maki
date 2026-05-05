// SPDX-FileCopyrightText: Olivier Cléro <oclero@hotmail.com>
// SPDX-License-Identifier: MIT

#pragma once

#include <QWidget>
#include <memory>
#include <oclero/qlementine/style/Theme.hpp>

namespace oclero::qlementine
{
/**
 * @brief A widget for editing themes.
 */
class ThemeEditorWidget : public QWidget
{
  Q_OBJECT

public:
  /**
   * @brief Constructs a new ThemeEditorWidget.
   *
   * @param parent The parent widget.
   */
  explicit ThemeEditorWidget(QWidget* parent = nullptr);

  /**
   * @brief Destroys the ThemeEditorWidget.
   */
  ~ThemeEditorWidget() override;

public:
  /**
   * @brief Gets the current theme.
   *
   * @return const Theme& The current theme.
   */
  const Theme& theme() const;

  /**
   * @brief Sets a new theme.
   *
   * @param theme The new theme to set.
   */
  void setTheme(const Theme& theme);

  /**
   * @brief Sets the default save path for themes.
   *
   * @param savePath The default save path.
   */
  void setDefaultPath(const QString& savePath);

Q_SIGNALS:
  /**
   * @brief Emitted when the theme is changed.
   *
   * @param theme The new theme.
   */
  void themeChanged(const oclero::qlementine::Theme& theme);

  /**
   * @brief Emitted when a theme is saved.
   *
   * @param path The save path.
   * @param theme The saved theme.
   */
  void themeSaved(const QString& path, const oclero::qlementine::Theme& theme);

  /**
   * @brief Emitted when a theme is loaded.
   *
   * @param path The load path.
   * @param theme The loaded theme.
   */
  void themeLoaded(const QString& path, const oclero::qlementine::Theme& theme);

private:
  struct Impl;
  std::unique_ptr<Impl> _impl;
};
}  // namespace oclero::qlementine
