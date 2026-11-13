@page settings Settings

[TOC]

MAKI provides a settings dialog for configuring the editor, its appearance, generated files, and installed plugins. 

# General

The **General** page controls the language, automatic saving, startup and closing behavior, and logging.

## Language

| Setting | Description |
| --- | --- |
| **Set language** | Selects MAKI's interface language from the languages available in the application. A restart is required for the change to take full effect. |

## Autosave

| Setting | Description |
| --- | --- |
| **Enable autosave** | Enables or disables automatic saving. |
| **Autosave interval** | Sets the interval between automatic saves, in minutes. Valid range: **1–120 minutes**. |

## Opening/Closing

| Setting | Description |
| --- | --- |
| **Restore last session on startup** | Controls whether MAKI attempts to restore the previous editing session at startup. |
| **Confirm before closing editor with running execution** | Controls whether MAKI asks for confirmation before closing an editor while an execution is running. |
| **Show welcome message** | Controls whether the welcome message is displayed. |
| **Show Quick Start Tour on startup** | Controls whether the Quick Start Tour is shown at startup. The tour can also be accessed from the Help menu. |
| **Recent History Size** | Sets the number of entries retained in the recent-history list. The dialog uses an integer input and does not specify a practical minimum or maximum beyond the integer type's limits. |

## Logging and Notifications

| Setting | Description |
| --- | --- |
| **Enable debug logs** | Enables or disables debug logging. |
| **Log History Size** | Sets the maximum number of log entries kept in the log history. Minimum: **250**. Larger values may increase memory usage. |

# Appearance

The **Appearance** page controls the application theme and layout, as well as visual settings for the graphical editor.

## UI changes

| Setting | Description |
| --- | --- |
| **Theme** | Selects an available application theme. Selecting a theme also loads it into the embedded Theme Editor. |
| **Use native menubar** | Controls whether MAKI uses the native menu bar provided by the operating system. |
| **UI scale** | Adjusts the user-interface scale as a percentage. Valid range: **80–200%**. |
| **Show canvas grid** | Shows or hides the grid in the graphical canvas. |
| **Show log table filters on start** | Controls whether the log table's filters start expanded. |
| **Number of columns in the node palette** | Sets how many columns are used to display nodes in the node palette. Valid range: **1–4 columns**. **The palette width does not update automatically** when this value changes. |
| **Number of columns in the task node** | Sets the number of columns used in the task node's layout. Valid range: **1–4 columns**. |
| **Shape of the transition edges** | Selects the routing/shape option for edges connecting nodes. The list of available options is provided by MAKI's edge router. |

## Theme editor

The **Theme editor** section provides an embedded editor for modifying the selected theme's colors and metadata. You can load a JSON theme, change its properties, and save it as a JSON file. The theme editor includes color groups for primary, secondary, neutral, semi-transparent, background, border, effects, and status colors.

For the complete list of theme variables, their meanings, and an example theme, see @ref themes "Themes".

# Generation

The **Generation** page configures where MAKI writes files produced by generation plugins.

## General

| Setting | Description |
| --- | --- |
| **Generation output folder** | Base directory used for generated output. MAKI appends `/<plugin name>` to this path, so different plugins can write to their own subdirectories. |
| **...** | Opens a folder selection dialog to choose the generation output folder. |

For example, if the generation output folder is `output`, a plugin named `koda` uses an output path under `output/koda` (the precise path format depends on the selected base directory and operating system).

# Plugins

The **Plugins** page lists registered plugins and provides controls for plugin activation and plugin-related preferences. Each registered plugin also appears as a child page under **Plugins**, where its exposed settings can be configured.

## Installed plugins

The installed-plugins table contains the following columns:

| Column | Description |
| --- | --- |
| **Name** | The registered name of the plugin. |
| **Version** | The plugin's reported version. |
| **Enabled** | A checkbox to enable or disable the plugin. |

The page also displays **Add** and **Remove** buttons. In the supplied Settings dialog implementation, these buttons are constructed, but their click handlers are not connected. They should therefore not yet be documented as a functioning plugin installation or removal workflow.

## Other settings

| Setting | Description |
| --- | --- |
| **Default plugin** | Selects the plugin MAKI should use as the default at startup. Only enabled, registered plugins are included in the list, along with an empty selection. |
| **Merge plugin logs** | When enabled, plugin logs are made available in MAKI's log table. |

## Plugin-specific settings

Each registered plugin can expose its own settings page. Each field uses the label and description supplied by that plugin. Consequently, the available settings differ between plugins and may change with plugin versions. These are, therefore, not listed here.
