#pragma once

#include <QObject>

#include "focus_properties.h"
#include "ihost_services.h"

namespace maki
{
class IPipeline;  /// Interface for the pipeline.
class ISettings;  /// Interface for settings.
class ILogging;   /// Interface for logging.
class IUI;        /// Interface for a tab.
}  // namespace maki

class IDocument;  /// Interface for a document.

/**
 * @brief HostServices class provides services to the host application.
 */
class HostServices : public QObject, public maki::IHostServices
{
  Q_OBJECT
public:
  /**
   * @brief Constructs a new HostServices object.
   *
   * @param document Shared pointer to the IDocument instance.
   * @param pipeline Pointer to the IPipeline instance.
   * @param settings Pointer to the ISettings instance.
   * @param root Project root directory as a QString.
   * @param parent Parent QObject, defaults to nullptr.
   */
  HostServices(std::shared_ptr<IDocument> document, maki::IPipeline* pipeline, maki::ISettings* settings, const QString& root,
               QObject* parent = nullptr);

  /**
   * @brief Sets the plugin tab.
   *
   * @param tab Pointer to the IUI instance.
   */
  void setPluginTab(maki::IUI* tab);

  /**
   * @brief Sets the logger.
   *
   * @param logger Pointer to the ILogging instance.
   */
  void setLogger(maki::ILogging* logger);

  /**
   * @brief Returns the document.
   *
   * @return IDocument* Pointer to the document.
   */
  IDocument* document() const override;

  /**
   * @brief Returns the settings.
   *
   * @return maki::ISettings* Pointer to the settings.
   */
  maki::ISettings* settings() const override;

  /**
   * @brief Returns the pipeline.
   *
   * @return maki::IPipeline* Pointer to the pipeline.
   */
  maki::IPipeline* pipeline() const override;

  /**
   * @brief Returns the project root directory.
   *
   * @return QString Project root directory.
   */
  QString projectRoot() const override;

  void errorOnNode(const QString& nodeId, const QString& flowId, const QString& message) override;
  void simulateOnNode(const QString& nodeId, const QString& flowId, maki::SimulationProperties properties) override;

  /**
   * @brief Returns the plugin tab.
   *
   * @return maki::IUI* Pointer to the plugin tab.
   */
  maki::IUI* ui() const override;

  /**
   * @brief Returns the logger.
   *
   * @return maki::ILogging* Pointer to the logger.
   */
  maki::ILogging* logger() const override;

signals:
  void onFocusOnNode(const QString& nodeId, const QString& flowId, const maki::FocusProperties& properties);

private:
  std::shared_ptr<IDocument> mDocument;  /// Shared pointer to the document.
  maki::IPipeline* mPipeline;            /// Pointer to the pipeline.
  maki::ISettings* mSettings;            /// Pointer to the settings.
  maki::IUI* mPluginTab;                 /// Pointer to the plugin tab.
  maki::ILogging* mLogger;               /// Pointer to the logger.

  QString mRoot;  /// Project root directory.
};
