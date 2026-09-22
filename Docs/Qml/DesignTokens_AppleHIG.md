# QML design tokens: Apple (HIG) alignment

This document records how the ImtCore QML style system maps onto Apple's
system colors and Human Interface Guidelines, and where the mapping stops.

## Where the tokens live

| Layer | File | Role |
| --- | --- | --- |
| Token declarations + fallback values | `Qml/imtcontrols/Base/StyleBase.qml` | Every `Style.*` property a screen can read. The literal values here are the **light** theme and are what you see before a theme is fetched from the server. |
| Decorator slots | `Qml/imtcontrols/Base/StyleComponents.qml` | `Component` hooks only, no values. |
| Runtime themes | `Include/imtstyle/light.theme`, `Include/imtstyle/dark.theme` | JSON. `ColorPalette` holds raw hex; `Style.ActiveColors` / `InactiveColors` map semantic names onto palette keys. |
| Theme → QML wiring | `StyleBase.parseStyleTheme()` | Copies `ActiveColors` entries into `Style.*` properties after `GetStyleData`. |
| App overrides | `Include/imtstylecontrolsqml/Qml/Acf/Style.qml` | Per-product tweaks on top of `ClientStyle`. |

A token that `parseStyleTheme()` does not read keeps its `StyleBase.qml`
literal in **both** themes — that is the main source of "dark theme still
looks light" defects.

## Color mapping

Apple ships two separate palettes: **system colors** (blue, green, red …)
that shift between light and dark, and a **gray ramp** plus semantic
background and label roles. This maps the app onto the iOS *grouped*
background model, which is the most recognizable Apple surface treatment:
a light gray canvas carrying white content cards.

### Light theme → Apple light

| Apple role | Hex | ImtCore token(s) |
| --- | --- | --- |
| `systemGroupedBackground` | `#f2f2f7` | `backgroundColor`, `alternateBaseColor`, `menuPanelBackgroundColor`, `tableHeaderColor` |
| `secondarySystemGroupedBackground` | `#ffffff` | `baseColor`, `backgroundColor2`, `dialogBackgroundColor`, `buttonColor`, `tableCellColor` |
| `opaqueSeparator` | `#c6c6c8` | `borderColor`, `shadowColor`, `tableCellBorderColor` |
| `systemGray4` | `#d1d1d6` | `buttonBorderColor` |
| `label` | `#000000` | `textColor`, `titleColor`, `buttonTextColor` |
| `secondaryLabel` | `#8a8a8e` | `subtitleColor`, `placeHolderTextColor`, `tableHeaderFontColor` |
| `systemGray2` | `#aeaeb2` | `inactiveTextColor`, `buttonInactiveTextColor`, `borderColor2` |
| `systemGray` | `#8e8e93` | `neutralEmphasisColor` |
| `systemGray5` | `#e5e5ea` | `neutralSubtleColor`, `mainColor` |
| `systemBlue` | `#007aff` | `imaginToolsAccentColor`, `linkColor`, `firstColor`, `iconColorOnSelected`, `tabSelectedColor`, `focusRingColor`, `buttonPrimaryColor` |
| `systemBlue` @15% | `#d9ebff` | `selectedColor`, `firstColorHighlight`, `popupInfoBackgroundColor` |
| `systemGreen` | `#34c759` | `successColor`, `positiveAccentColor`, `greenColor` |
| `systemOrange` | `#ff9500` | `attentionColor`, `secondColor`, `middleAccentColor` |
| `systemRed` | `#ff3b30` | `dangerColor`, `errorTextColor`, `buttonDangerColor` |
| `systemPurple` | `#af52de` | `doneColor` |
| `systemPink` | `#ff2d55` | `severeColor` |
| `systemTeal` | `#5ac8fa` | `lightBlueColor` |

The `*Subtle` background tokens are the matching system color flattened at
15 % over white (`#e0f7e6`, `#ffefd9`, `#ffe2e0`, `#f3e5fa`), since the
tokens are consumed as opaque fills and cannot carry alpha.

### Dark theme → Apple dark

Same roles, Apple's dark values: canvas `#000000`, elevated surface
`#1c1c1e`, secondary surface `#2c2c2e`, separator `#38383a`, label
`#ffffff`, secondary label `#98989f`, blue `#0a84ff`, green `#30d158`,
orange `#ff9f0a`, red `#ff453a`, purple `#bf5af2`, pink `#ff375f`.

Apple's dark canvas is true black with surfaces layered above it, so
`Base` (rows, cards) is lighter than `Background` — the same elevation
direction Apple uses on iOS.

## Typography

Apple's system font is **SF Pro**, and it cannot be used here. Apple's font
license grants use "solely for creating mock-ups of user interfaces to be
used in software products running on Apple's iOS, OS X or tvOS", states
"You may not embed the Apple Font in any software programs or other
products", and explicitly forbids use "for the purpose of creating mock-ups
of user interfaces to be used in software products running on any non-Apple
operating system". ImtCore ships on Windows, Linux and the web, so SF Pro is
out on all three counts — bundling it would be a licence violation, not just
a packaging inconvenience.

**Inter** ([rsms/inter](https://github.com/rsms/inter), SIL OFL 1.1) is used
instead. It is the usual free stand-in for SF Pro: a UI-first grotesque with
similar proportions and a tall x-height. Regular (400) and SemiBold (600) are
vendored under `Include/imtstylecontrolsqml/Qml/Fonts/` with the licence
alongside as `Inter-OFL.txt`, and served from the `/Fonts` resource prefix.

`fontFamily` is **bound to `FontLoader.name`**, not set to a literal. Native
Qt reports the font's own family name (`Inter`), while the JQML web build
derives the `@font-face` family from the file name (`Inter-Regular`) — see
`3rdParty/WebCompiler/qmlcore/core/FontLoader.qml`. No single literal is
correct on both, so the binding resolves per platform, falling back to
`Arial` until the font has loaded.

`fontFamilyMono` remains `Consolas`, standing in for SF Mono.

macOS text sizes, not iOS ones, since this is a dense desktop app:

| Token | px | Apple role |
| --- | --- | --- |
| `fontSizeS` | 11 | caption |
| `fontSizeM` | 13 | body (macOS default) |
| `fontSizeL` | 16 | callout |
| `fontSizeXL` | 20 | title 3 |
| `fontSizeXXL` | 22 | title 2 |
| `fontSizeXXXL` | 28 | title 1 |
| `fontSizeBXL` | 34 | large title |

The deprecated aliases (`fontSizeNormal`, `fontSizeSmall`, …) were moved in
lockstep so mixed call sites stay consistent.

## Metrics

Apple's controls are rounder and roomier than most desktop toolkits.

| Token | Value | Apple role |
| --- | --- | --- |
| `radiusS` / `radiusM` / `radiusL` / `radiusXL` | 6 / 10 / 16 / 22 | control / card / sheet / capsule-ish |
| `buttonRadius`, `textFieldRadius`, `comboBoxRadius` | 10 | control radius |
| `controlHeightS` / `M` / `L` | 28 / 32 / 44 | small / regular / touch-sized row |
| `buttonHeightXS` / `S` / `M` | 28 / 30 / 32 | |
| `menuRowHeight`, `tableHeaderHeight` | 34, 36 | |
| `focusRingWidth` | 3 | Apple's focus ring is thicker than a border |

The spacing scale (`marginM` = 10, used in ~450 places) was deliberately
left alone; Apple's 8-point grid would be a separate, layout-wide change.

## Button variants

`Button` carries a `variant` property — `"default"`, `"primary"` or
`"danger"` — that `ButtonDecorator` turns into a filled background. Apple's
primary action is **filled blue**, and destructive actions are red. Use
`"primary"` for the single affirmative action on a screen and `"danger"`
for a destructive one; everything else stays `"default"`. Dialog rows pass
it through their button model (`variant` on the `ListModel` entry).

Focus is shown by a 3 px ring (`focusRingColor` / `focusRingWidth`) drawn
inside the control bounds — the decorator sets `clip: true`, so unlike
Apple the ring cannot sit outside the border.

## Tables

`enableAlternating` defaults to **false**: rows are separated by hairline
separators, not zebra striping, as in Apple's lists and tables.

## Not aligned (deliberate)

- **SF Pro.** Licence-blocked, see Typography. Inter is a close relative,
  not the same typeface.
- **Vibrancy and translucency.** Apple's sidebars and sheets blur what is
  behind them. QML can do this natively but not in the JQML web build, and
  the token system has no place to express it.
- **Continuous ("squircle") corners.** `Rectangle.radius` is a circular
  arc; Apple's corners are a superellipse. Visible at large radii.
- **Icons.** Still the in-repo SVG set, not SF Symbols.
- **Spacing scale.** See above.
- **Font weights.** Apple leans on semibold for headlines; QML call sites
  use plain `font.bold`. No weight token exists yet.
- **Widget (non-QML) stylesheet.** `Include/imtstyle/Resources/ImtStyle.css`
  still specifies `Barlow`; it styles `QWidget`, not QML.
- **Shadows.** Apple uses soft translucent shadows; `shadowColor` is an
  opaque separator grey because the token is consumed as a solid color.

## Rules for new screens

1. Never write a hex literal. If no token fits, add one to `StyleBase.qml`,
   give it a `ColorPalette` entry and an `ActiveColors` mapping in **both**
   `.theme` files, and read it back in `parseStyleTheme()`.
2. Use the semantic status tokens (`successColor`, `attentionColor`,
   `dangerColor`, `doneColor`, `severeColor`, `neutralEmphasisColor`) for
   badges and state dots, not raw greens and reds.
3. QML is loaded from `qrc:/qml`, so style edits need an `imtcontrolsqml` /
   `imtguiqml` resource rebuild before they show up in a running app.
