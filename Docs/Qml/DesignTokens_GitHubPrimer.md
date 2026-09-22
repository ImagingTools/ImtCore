# QML design tokens: GitHub (Primer) alignment

This document records how the ImtCore QML style system maps onto GitHub's
[Primer](https://primer.style) design tokens, and where the mapping stops.

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

### Light theme → Primer light

| Primer token | Hex | ImtCore token(s) |
| --- | --- | --- |
| `canvas.default` | `#ffffff` | `baseColor`, `backgroundColor2`, `dialogBackgroundColor`, `tableCellColor` |
| `canvas.subtle` | `#f6f8fa` | `backgroundColor`, `alternateBaseColor`, `menuPanelBackgroundColor`, `buttonColor`, `tableHeaderColor` |
| `border.default` | `#d1d9e0` | `borderColor`, `buttonBorderColor`, `shadowColor`, `tableCellBorderColor` |
| `fg.default` | `#1f2328` | `textColor`, `titleColor`, `buttonTextColor`, `baseColorInverted` |
| `fg.muted` | `#59636e` | `subtitleColor`, `tableHeaderFontColor` |
| `fg.disabled` | `#818b98` | `inactiveTextColor`, `buttonInactiveTextColor` |
| `accent.fg` | `#0969da` | `imaginToolsAccentColor`, `linkColor`, `firstColor`, `iconColorOnSelected`, `buttonBorderFocusColor` |
| `accent.subtle` | `#ddf4ff` | `selectedColor`, `firstColorHighlight`, `popupInfoBackgroundColor` |
| `success.fg` / `success.subtle` | `#1a7f37` / `#dafbe1` | `successColor` / `successSubtleColor` |
| `attention.fg` / `attention.subtle` | `#9a6700` / `#fff8c5` | `attentionColor` / `attentionSubtleColor` |
| `danger.fg` / `danger.subtle` | `#d1242f` / `#ffebe9` | `dangerColor`, `errorTextColor` / `dangerSubtleColor` |
| `done.fg` | `#8250df` | `doneColor` |
| `severe.fg` | `#bc4c00` | `severeColor` |
| `neutral.emphasis` | `#6e7781` | `neutralEmphasisColor` |
| `underlineNav.borderColor.active` | `#fd8c73` | `tabSelectedColor` |
| `button.primary.bgColor` | `#1f883d` | `buttonPrimaryColor` (+ `Hover` / `Pressed` / `Border` / `Text`) |
| `button.danger.bgColor.active` | `#cf222e` | `buttonDangerColor` (+ `Hover` / `Pressed` / `Border` / `Text`) |
| `focus outline` | accent | `focusRingColor`, `focusRingWidth` |

### Dark theme → Primer dark

Same semantic names, Primer dark values: canvas `#0d1117`, elevated surface
`#151b23`, border `#3d444d`, `fg.default` `#f0f6fc`, `fg.muted` `#9198a1`,
accent `#4493f8`, success `#3fb950`, attention `#d29922`, danger `#f85149`,
done `#ab7df8`, severe `#db6d28`.

Note the inversion relative to the old dark theme: `Base` (rows, cards) is now
**lighter** than `Background` (page canvas), matching Primer's elevation model.

## Typography

`fontFamily` / `fontFamilyBold` are `Segoe UI` — the family GitHub's own font
stack resolves to on Windows. `fontFamilyMono` (`Consolas`) was added for
code, IDs and hashes.

Primer's type scale, in `StyleBase.qml`:

| Token | px | Primer role |
| --- | --- | --- |
| `fontSizeS` | 12 | small / meta |
| `fontSizeM` | 14 | body (default) |
| `fontSizeL` | 16 | h4 |
| `fontSizeXL` | 20 | h3 |
| `fontSizeXXL` | 24 | h2 |
| `fontSizeXXXL` | 32 | h1 |

The deprecated aliases (`fontSizeNormal`, `fontSizeSmall`, …) were moved in
lockstep so mixed call sites stay consistent.

## Metrics

| Token | Value | Primer role |
| --- | --- | --- |
| `radiusS` / `radiusM` / `radiusL` | 4 / 6 / 12 | small / default / large |
| `buttonRadius`, `textFieldRadius`, `comboBoxRadius` | 6 | control radius |
| `controlHeightS` / `M` / `L` | 24 / 32 / 40 | xsmall / medium / large control |
| `buttonHeightXS` / `S` / `M` | 24 / 28 / 32 | xsmall / small / medium button |
| `menuRowHeight`, `tableHeaderHeight` | 32, 40 | |

The spacing scale (`marginM` = 10, used in ~450 places) was deliberately left
alone; Primer's 4/8/16/24 grid would be a separate, layout-wide change.

## Button variants

`Button` carries a `variant` property — `"default"`, `"primary"` or `"danger"` —
that `ButtonDecorator` turns into a filled, accent-colored background. Use
`"primary"` for the single affirmative action on a screen and `"danger"` for a
destructive one; everything else stays `"default"`. Dialog rows pass it through
their button model (`variant` on the `ListModel` entry), so a dialog gets a
GitHub-style filled confirm button without a custom decorator.

Focus is shown by a 2 px ring (`focusRingColor` / `focusRingWidth`) drawn inside
the control bounds — the decorator sets `clip: true`, so unlike Primer the ring
cannot sit outside the border.

## Tables

`enableAlternating` defaults to **false**: rows are separated by borders, not
zebra striping, as on GitHub. Screens that want striping opt in per table.

## Not aligned (deliberate)

- **Icons.** Still the in-repo SVG set, not Octicons.
- **Spacing scale.** See above.
- **Font weights.** Primer leans on 600 semibold for emphasis; QML call sites
  use plain `font.bold`. No weight token exists yet.
- **Widget (non-QML) stylesheet.** `Include/imtstyle/Resources/ImtStyle.css`
  still specifies `Barlow`; it styles `QWidget`, not QML.
- **Shadows.** Primer uses translucent shadows; `shadowColor` is an opaque
  border-grey because the token is consumed as a solid color.

## Rules for new screens

1. Never write a hex literal. If no token fits, add one to `StyleBase.qml`,
   give it a `ColorPalette` entry and an `ActiveColors` mapping in **both**
   `.theme` files, and read it back in `parseStyleTheme()`.
2. Use the semantic status tokens (`successColor`, `attentionColor`,
   `dangerColor`, `doneColor`, `severeColor`, `neutralEmphasisColor`) for
   badges and state dots, not raw greens and reds.
3. QML is loaded from `qrc:/qml`, so style edits need an `imtcontrolsqml` /
   `imtguiqml` resource rebuild before they show up in a running app.
