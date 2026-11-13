@page themes Themes

[TOC]

MAKI uses [qlementine](https://github.com/oclero/qlementine) for its widget styling and provides JSON themes to customize colors, typography, sizing, and interaction effects. Some properties are specific to MAKI (such as the workspace and tab bar colors), while others configure qlementine widgets.

You can edit a theme in MAKI's **Theme Editor**, load an existing JSON file, and export the edited theme. **Changes are not saved to disk unless you export the theme.** You can also edit the JSON directly.

# Available properties

The reference below covers every property in the example. Color descriptions follow the groupings and help text in MAKI's Theme Editor; descriptions of geometry, timing, typography and other options are based on their property names and intended widget roles.

## Value formats and interaction states

- **Colors** use hexadecimal `#RRGGBBAA` values: red, green, blue, and alpha (opacity). For example, `#e95420ff` is fully opaque orange, while `#e954206a` is partially transparent. The final two digits are **alpha**, not another color channel.
- **Sizes and spacing** are numeric UI dimensions in pixels. Font sizes are numeric theme values; their rendering depends on the style/font handling.
- **Durations** are in milliseconds (`192` means 192 ms).
- **Booleans** use JSON `true` or `false` (without quotation marks).
- **Interactive color variants** follow a naming convention: a base color is the default state; `Hovered`, `Pressed`, and `Disabled` specify the corresponding interaction states. Where a `Foreground` property exists, it sets text or icon color drawn over the background color.

## Metadata

| Property | Type | Description |
|---|---|---|
| `meta.name` | Text | Display name of the theme. |
| `meta.author` | Text | Name of the theme author or maintainer. |
| `meta.version` | Text | Theme version identifier (text, e.g. `1.0.0`). |

## Primary colors

| Property | Type | Description |
|---|---|---|
| `primaryColor` | Color | Main accent color used to highlight interactive elements. |
| `primaryColorForeground` | Color | Text or icons drawn on elements using the primary color. |
| `primaryAlternativeColor` | Color | Alternative accent for highlighting elements inside already highlighted elements. |
| `primaryColorHovered` | Color | Main accent color used to highlight interactive elements — color when the pointer hovers over the element. |
| `primaryColorPressed` | Color | Main accent color used to highlight interactive elements — color while the element is pressed. |
| `primaryColorDisabled` | Color | Main accent color used to highlight interactive elements — color when the element is disabled. |
| `primaryColorForegroundHovered` | Color | Text or icons drawn on elements using the primary color — color when the pointer hovers over the element. |
| `primaryColorForegroundPressed` | Color | Text or icons drawn on elements using the primary color — color while the element is pressed. |
| `primaryColorForegroundDisabled` | Color | Text or icons drawn on elements using the primary color — color when the element is disabled. |
| `primaryAlternativeColorHovered` | Color | Alternative accent for highlighting elements inside already highlighted elements — color when the pointer hovers over the element. |
| `primaryAlternativeColorPressed` | Color | Alternative accent for highlighting elements inside already highlighted elements — color while the element is pressed. |
| `primaryAlternativeColorDisabled` | Color | Alternative accent for highlighting elements inside already highlighted elements — color when the element is disabled. |

## Secondary colors

| Property | Type | Description |
|---|---|---|
| `secondaryColor` | Color | More neutral color used for text and non-highlighted elements. |
| `secondaryColorForeground` | Color | Text or icons drawn on secondary-colored elements. |
| `secondaryAlternativeColor` | Color | Lighter alternative to the secondary color. |
| `secondaryColorHovered` | Color | More neutral color used for text and non-highlighted elements — color when the pointer hovers over the element. |
| `secondaryColorPressed` | Color | More neutral color used for text and non-highlighted elements — color while the element is pressed. |
| `secondaryColorDisabled` | Color | More neutral color used for text and non-highlighted elements — color when the element is disabled. |
| `secondaryColorForegroundHovered` | Color | Text or icons drawn on secondary-colored elements — color when the pointer hovers over the element. |
| `secondaryColorForegroundPressed` | Color | Text or icons drawn on secondary-colored elements — color while the element is pressed. |
| `secondaryColorForegroundDisabled` | Color | Text or icons drawn on secondary-colored elements — color when the element is disabled. |
| `secondaryAlternativeColorHovered` | Color | Lighter alternative to the secondary color — color when the pointer hovers over the element. |
| `secondaryAlternativeColorPressed` | Color | Lighter alternative to the secondary color — color while the element is pressed. |
| `secondaryAlternativeColorDisabled` | Color | Lighter alternative to the secondary color — color when the element is disabled. |

## Neutral and translucent colors

| Property | Type | Description |
|---|---|---|
| `neutralColor` | Color | Neutral feedback color for hover, pressed and disabled UI states. |
| `semiTransparentColor1` | Color | First translucent shade for overlays or controls such as scrollbars. |
| `semiTransparentColor2` | Color | Second translucent shade, typically stronger than shade 1. |
| `semiTransparentColor3` | Color | Third translucent shade, typically stronger than shade 2. |
| `semiTransparentColor4` | Color | Fourth translucent shade, typically stronger than shade 3. |
| `neutralColorHovered` | Color | Neutral feedback color for hover, pressed and disabled UI states — color when the pointer hovers over the element. |
| `neutralColorPressed` | Color | Neutral feedback color for hover, pressed and disabled UI states — color while the element is pressed. |
| `neutralColorDisabled` | Color | Neutral feedback color for hover, pressed and disabled UI states — color when the element is disabled. |

## Backgrounds and borders

| Property | Type | Description |
|---|---|---|
| `backgroundColorMain1` | Color | Main background color for windows and containers. |
| `backgroundColorMain2` | Color | Alternative main background shade for layered surfaces. |
| `backgroundColorMain3` | Color | Additional background shade for containers and surfaces. |
| `backgroundColorMain4` | Color | Additional background shade for containers and surfaces. |
| `backgroundColorTabBar` | Color | MAKI tab bar background. |
| `backgroundColorWorkspace` | Color | MAKI workspace or canvas-area background. |
| `borderColor` | Color | Default border color, for example on combo boxes and group boxes. |
| `borderColorHovered` | Color | Default border color, for example on combo boxes and group boxes — color when the pointer hovers over the element. |
| `borderColorPressed` | Color | Default border color, for example on combo boxes and group boxes — color while the element is pressed. |
| `borderColorDisabled` | Color | Default border color, for example on combo boxes and group boxes — color when the element is disabled. |

## Focus, shadows, and status

| Property | Type | Description |
|---|---|---|
| `focusColor` | Color | Color of the keyboard focus border. |
| `shadowColor1` | Color | First shadow shade. |
| `shadowColor2` | Color | Second shadow shade. |
| `shadowColor3` | Color | Third shadow shade. |
| `statusColorError` | Color | Error and failure feedback. |
| `statusColorWarning` | Color | Warning and caution feedback. |
| `statusColorSuccess` | Color | Successful operation feedback. |
| `statusColorInfo` | Color | Informational feedback. |
| `statusColorErrorHovered` | Color | Error and failure feedback — color when the pointer hovers over the element. |
| `statusColorErrorPressed` | Color | Error and failure feedback — color while the element is pressed. |
| `statusColorErrorDisabled` | Color | Error and failure feedback — color when the element is disabled. |
| `statusColorWarningHovered` | Color | Warning and caution feedback — color when the pointer hovers over the element. |
| `statusColorWarningPressed` | Color | Warning and caution feedback — color while the element is pressed. |
| `statusColorWarningDisabled` | Color | Warning and caution feedback — color when the element is disabled. |
| `statusColorSuccessHovered` | Color | Successful operation feedback — color when the pointer hovers over the element. |
| `statusColorSuccessPressed` | Color | Successful operation feedback — color while the element is pressed. |
| `statusColorSuccessDisabled` | Color | Successful operation feedback — color when the element is disabled. |
| `statusColorInfoHovered` | Color | Informational feedback — color when the pointer hovers over the element. |
| `statusColorInfoPressed` | Color | Informational feedback — color while the element is pressed. |
| `statusColorInfoDisabled` | Color | Informational feedback — color when the element is disabled. |

## Typography and icons

| Property | Type | Description |
|---|---|---|
| `fontSize` | Size / spacing | Default UI text size. |
| `fontSizeH1` | Size / spacing | Heading level 1 font size. |
| `fontSizeH2` | Size / spacing | Heading level 2 font size. |
| `fontSizeH3` | Size / spacing | Heading level 3 font size. |
| `fontSizeH4` | Size / spacing | Heading level 4 font size. |
| `fontSizeH5` | Size / spacing | Heading level 5 font size. |
| `fontSizeS1` | Size / spacing | Small supplementary text size. |
| `fontSizeMonospace` | Size / spacing | Monospaced text size. |
| `useSystemFonts` | Boolean | Use fonts chosen by the operating system where supported; `false` uses the theme/application font configuration. |
| `iconSize` | Size / spacing | Default icon size. |
| `iconSizeExtraSmall` | Size / spacing | Extra-small icon size. |
| `iconSizeMedium` | Size / spacing | Medium icon size. |
| `iconSizeLarge` | Size / spacing | Large icon size. |

## Geometry and spacing

| Property | Type | Description |
|---|---|---|
| `spacing` | Size / spacing | Base spacing unit for widget layouts and padding. |
| `borderRadius` | Size / spacing | Default rounding radius for components. |
| `borderWidth` | Size / spacing | Default border stroke width. |
| `checkBoxBorderRadius` | Size / spacing | Corner radius of checkbox indicators. |
| `menuBarItemBorderRadius` | Size / spacing | Corner radius of menu-bar items. |
| `focusBorderWidth` | Size / spacing | Thickness of the keyboard focus border. |
| `controlDefaultWidth` | Size / spacing | Default width of supported controls. |
| `controlHeightSmall` | Size / spacing | Height of small controls. |
| `controlHeightMedium` | Size / spacing | Height of medium controls. |
| `controlHeightLarge` | Size / spacing | Height of large controls. |
| `tabBarPaddingTop` | Size / spacing | Top padding of the tab bar. |
| `tabBarTabMinWidth` | Size / spacing | Minimum tab width (`0` in the example; exact fallback depends on implementation). |
| `tabBarTabMaxWidth` | Size / spacing | Maximum tab width (`0` in the example; exact fallback depends on implementation). |

## Sliders, dials, progress and scrolling

| Property | Type | Description |
|---|---|---|
| `sliderGrooveHeight` | Size / spacing | Thickness of a slider track. |
| `sliderTickSize` | Size / spacing | Size of slider tick marks. |
| `sliderTickSpacing` | Size / spacing | Spacing between slider ticks. |
| `sliderTickThickness` | Size / spacing | Thickness of slider tick marks. |
| `dialGrooveThickness` | Size / spacing | Thickness of the circular dial track. |
| `dialMarkLength` | Size / spacing | Length of the dial marks. |
| `dialMarkThickness` | Size / spacing | Thickness of the dial marks. |
| `dialTickLength` | Size / spacing | Length of dial ticks. |
| `dialTickSpacing` | Size / spacing | Spacing of dial ticks. |
| `progressBarGrooveHeight` | Size / spacing | Thickness of a progress bar track. |
| `scrollBarMargin` | Size / spacing | Margin around the scrollbar. |
| `scrollBarThicknessFull` | Size / spacing | Thickness of a full-size scrollbar. |
| `scrollBarThicknessSmall` | Size / spacing | Thickness of a compact scrollbar. |

## Animation

| Property | Type | Description |
|---|---|---|
| `animationDuration` | Milliseconds | Default animation duration. |
| `focusAnimationDuration` | Milliseconds | Duration of focus transitions. |
| `sliderAnimationDuration` | Milliseconds | Duration of slider transitions. |

## Working with custom themes

1. Start from one of MAKI's bundled theme JSON files rather than creating every property from scratch.
2. Change a small set of related values at a time (for example, `primaryColor` and its hover/pressed/disabled variants).
3. Keep foreground and background colors sufficiently different for readable labels, including the disabled state.
4. Preview your changes in the Theme Editor and **export the theme** to persist them.
5. If changing widget geometry, verify the result in dialogs, tab bars, scrollable panels, and controls at different window sizes.

# Example

The following is the default **Light Yaru** theme, inspired by Ubuntu's Yaru theme:

```json
{
  "meta": {
    "name": "Light Yaru",
    "author": "MAKI",
    "version": "1.0.0"
  },
  "animationDuration": 192,
  "backgroundColorMain1": "#f7f7f7ff",
  "backgroundColorMain2": "#ffffffff",
  "backgroundColorMain3": "#efefefff",
  "backgroundColorMain4": "#e5e5e5ff",
  "backgroundColorTabBar": "#fafafaff",
  "backgroundColorWorkspace": "#eeeeeeff",
  "borderColor": "#d6d1ccff",
  "borderColorDisabled": "#e9e7e5ff",
  "borderColorHovered": "#c9c4c0ff",
  "borderColorPressed": "#bcb8b4ff",
  "borderRadius": 6,
  "borderWidth": 1,
  "checkBoxBorderRadius": 4,
  "controlDefaultWidth": 96,
  "controlHeightLarge": 28,
  "controlHeightMedium": 24,
  "controlHeightSmall": 16,
  "dialGrooveThickness": 4,
  "dialMarkLength": 8,
  "dialMarkThickness": 2,
  "dialTickLength": 4,
  "dialTickSpacing": 4,
  "focusAnimationDuration": 384,
  "focusBorderWidth": 2,
  "focusColor": "#e954206a",
  "fontSize": 12,
  "fontSizeH1": 34,
  "fontSizeH2": 26,
  "fontSizeH3": 22,
  "fontSizeH4": 18,
  "fontSizeH5": 14,
  "fontSizeMonospace": 13,
  "fontSizeS1": 10,
  "iconSize": 16,
  "iconSizeExtraSmall": 12,
  "iconSizeLarge": 32,
  "iconSizeMedium": 24,
  "menuBarItemBorderRadius": 2,
  "neutralColor": "#e6e1dcff",
  "neutralColorDisabled": "#efedebff",
  "neutralColorHovered": "#d8d4cfff",
  "neutralColorPressed": "#cac6c2ff",
  "primaryAlternativeColor": "#c64600ff",
  "primaryAlternativeColorDisabled": "#e3b297ff",
  "primaryAlternativeColorHovered": "#ba4200ff",
  "primaryAlternativeColorPressed": "#ae3e00ff",
  "primaryColor": "#e95420ff",
  "primaryColorDisabled": "#f0b7a3ff",
  "primaryColorForeground": "#ffffffff",
  "primaryColorForegroundDisabled": "#ffffff66",
  "primaryColorForegroundHovered": "#ffffffff",
  "primaryColorForegroundPressed": "#ffffffff",
  "primaryColorHovered": "#db4f1eff",
  "primaryColorPressed": "#cd4a1cff",
  "progressBarGrooveHeight": 6,
  "scrollBarMargin": 0,
  "scrollBarThicknessFull": 12,
  "scrollBarThicknessSmall": 6,
  "secondaryAlternativeColor": "#6e6259ff",
  "secondaryAlternativeColorDisabled": "#6e62593f",
  "secondaryAlternativeColorHovered": "#675c54ff",
  "secondaryAlternativeColorPressed": "#61564eff",
  "secondaryColor": "#2d2926ff",
  "secondaryColorDisabled": "#2d292633",
  "secondaryColorForeground": "#ffffffff",
  "secondaryColorForegroundDisabled": "#ffffff3f",
  "secondaryColorForegroundHovered": "#ffffffff",
  "secondaryColorForegroundPressed": "#ffffffff",
  "secondaryColorHovered": "#2a2724ff",
  "secondaryColorPressed": "#282421ff",
  "semiTransparentColor1": "#e9542018",
  "semiTransparentColor2": "#e9542023",
  "semiTransparentColor3": "#e9542028",
  "semiTransparentColor4": "#e954202d",
  "shadowColor1": "#00000022",
  "shadowColor2": "#00000044",
  "shadowColor3": "#00000066",
  "sliderAnimationDuration": 96,
  "sliderGrooveHeight": 4,
  "sliderTickSize": 3,
  "sliderTickSpacing": 2,
  "sliderTickThickness": 1,
  "spacing": 8,
  "statusColorError": "#c7162bff",
  "statusColorErrorDisabled": "#e3a0a8ff",
  "statusColorErrorHovered": "#bb1528ff",
  "statusColorErrorPressed": "#af1326ff",
  "statusColorInfo": "#3584e4ff",
  "statusColorInfoDisabled": "#abc9eeff",
  "statusColorInfoHovered": "#327cd6ff",
  "statusColorInfoPressed": "#2f74c9ff",
  "statusColorSuccess": "#26a269ff",
  "statusColorSuccessDisabled": "#a6d5bfff",
  "statusColorSuccessHovered": "#249863ff",
  "statusColorSuccessPressed": "#218f5cff",
  "statusColorWarning": "#f99b11ff",
  "statusColorWarningDisabled": "#f6d29eff",
  "statusColorWarningHovered": "#ea9210ff",
  "statusColorWarningPressed": "#db880fff",
  "tabBarPaddingTop": 4,
  "tabBarTabMaxWidth": 0,
  "tabBarTabMinWidth": 0,
  "useSystemFonts": false
}
```
