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
| `canvas.default` | `#ffffff` | `baseColor`, `backgroundColor2`, `dialogBackgroundColor`, `tableCellColor`, `menuPanelBackgroundColor`, `topPanelBackgroundColor` |
| `canvas.subtle` | `#f6f8fa` | `backgroundColor`, `alternateBaseColor`, `buttonColor`, `tableHeaderColor` |
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

## Chrome components

Resolved from `github/primer/primitives` (functional token sources, references
resolved against the base scales) and cross-checked against GitHub's shipped
CSS variables.

| GitHub component | Primer token | Light | Dark | ImtCore token | Consumer |
| --- | --- | --- | --- | --- | --- |
| Left sidebar / NavList surface | `bgColor-default` | `#ffffff` | `#151b23` | `menuPanelBackgroundColor` | `MenuPanel.qml` |
| NavList selected row | `control-transparent-bgColor-selected` | `#818b9826` | `#656c761a` | `menuPanelItemSelectedColor` | `MenuPanelButtonDecorator.qml` |
| NavList hovered row | `control-transparent-bgColor-hover` | `#818b981a` | `#656c7633` | `menuPanelItemHoverColor` | `MenuPanelButtonDecorator.qml` |
| NavList selected indicator | `borderColor-accent-emphasis` | `#0969da` | `#1f6feb` | `iconColorOnSelected` | `MenuPanelButtonDecorator.qml` |
| Top bar surface | `bgColor-default` | `#ffffff` | `#151b23` | `topPanelBackgroundColor` | `TopPanelDecorator.qml` |
| Top bar divider | `borderColor-default` | `#d1d9e0` | `#3d444d` | `topPanelBorderColor` | `TopPanelDecorator.qml` |
| Dialog / popup / dropdown surface | `overlay-bgColor` | `#ffffff` | `#151b23` | `dialogBackgroundColor`, `baseColor` | `DialogDecorator.qml`, `PopupDecorator*.qml` |
| Dialog / popup border | `overlay-borderColor` | `#d1d9e080` | `#3d444db3` | `overlayBorderColor` | `DialogDecorator.qml`, `PopupDecorator*.qml` |
| Modal backdrop | `overlay-backdrop-bgColor` | `#c8d1da` at 40% | `#212830` at 40% | `overlayBackgroundColor` | `OverlayManager.qml` |
| Table header | `bgColor-muted` + `fgColor-muted` | `#f6f8fa` / `#59636e` | `#151b23` / `#9198a1` | `tableHeaderColor`, `tableHeaderFontColor` | `TableHeaderDelegate.qml` |
| Table row border | `borderColor-default` | `#d1d9e0` | `#3d444d` | `tableCellBorderColor` | `TableRowDelegateBase.qml` |
| Table row hover | `control-bgColor-hover` | `#eff2f5` | `#262c36` | `hover` | `TableRowDelegateBase.qml` |
| UnderlineNav active tab | `underlineNav-borderColor-active` | `#fd8c73` | `#f78166` | `tabSelectedColor` | tab decorators |

Alpha is written Qt-style (`#AARRGGBB`), so Primer's `#818b9826` becomes
`#26818b98`.

### Top panel: on canvas, not a dark bar

GitHub's global header is a dark bar in **both** themes
(`header-bgColor` = `#25292e` light, `#151b23f2` dark). ImtCore's top panel
deliberately does **not** follow it: the panel hosts theme-colored SVG icons
resolved through `Style.getIconPath()`, drawn near-black in the light theme,
which would disappear on a dark bar. The panel therefore stays on the page
surface and is separated by a `borderColor-default` divider. The two tokens
exist so a product that ships its own light-on-dark icon set can opt into the
dark header from its `.theme` file alone, without touching QML.

### `colorMenuPanel`

`colorMenuPanel` used to default to `baseColor` while
`menuPanelBackgroundColor` held a muted value and was never connected to
anything - two tokens for one role, disagreeing. `colorMenuPanel` is now an
alias of `menuPanelBackgroundColor`, kept only so styles that still assign it
keep loading. New code should use `menuPanelBackgroundColor`.

The surviving token takes `bgColor-default`, not the `bgColor-muted` that
GitHub gives a full-bleed sidebar: `ThumbnailDecorator` insets the panel with a
margin all round, so it is a card on the page canvas, and on GitHub an inset
card sits on the elevated surface with the page muted behind it. In the light
theme this is the value the panel already had; in the dark theme it moves the
panel from the page canvas up onto the elevated surface.

## Typography

GitHub's own brand typeface is **Mona Sans**
([github/mona-sans](https://github.com/github/mona-sans), SIL OFL 1.1), so it
is vendored rather than approximated. Regular (400) and SemiBold (600) live
under `Include/imtstylecontrolsqml/Qml/Fonts/` with the licence alongside as
`MonaSans-OFL.txt`, served from the `/Fonts` resource prefix.

`fontFamily` is **bound to `FontLoader.name`**, not set to a literal. Native
Qt reports the font's own family name (`Mona Sans`), while the JQML web build
derives the `@font-face` family from the file name (`MonaSans-Regular`) — see
`3rdParty/WebCompiler/qmlcore/core/FontLoader.qml`. No single literal is
correct on both, so the binding resolves per platform, falling back to
`Segoe UI` until the font has loaded.

`fontFamilyMono` (`Consolas`) was added for code, IDs and hashes.

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

## Closed hardcoded-color leaks

Hex literals that survived the first pass and now read from tokens:

| File | Was | Now |
| --- | --- | --- |
| `Views/Piechart.qml` | `#999`, `#ccc` | `borderColor2`, `inactiveTextColor`, `neutralSubtleColor` |
| `Views/StackedBarChart.qml` | `#999`, `#ddd`, `#ccc` | `borderColor2`, `borderColor`, `neutralSubtleColor` |
| `Views/CustomScrollbar.qml`, `Views/ScrollIndicator.qml` | fallback `#efefef` | `backgroundColor` |
| `Views/DrawingContainer.qml` | `#ffe4c4` | `borderColor` |
| `Views/ProgressBar.qml`, `Views/BarChart*.qml` | `#ffffff`, `#000000`, `lightgrey` | `baseColor`, `textColor`, `highlightedTextColor`, `borderColor` |
| `Views/BasicTreeView.qml` | `#d04848` | `dangerColor` |
| `Views/TreeViewGql.qml` | `lightsteelblue` | `hover` |
| `GraphicsView/PolyLineShape.qml` | `#ff0000` | `dangerColor` |
| `Inputs/CalendarView.qml`, `Inputs/CheckBoxMenu.qml` | `#ffffff`, `#000000` | `baseColor`, `textColor` |
| `Popup/Internal/OverlayManager.qml` | `#80000000` | `overlayBackgroundColor` |
| `imtgui/Application/AlertMessage.qml` | `#f9dedc` | `dangerSubtleColor` |
| `imtgui/Application/ApplicationMain.qml` | `#bbbbbb`, `#d4edda`, `#d6eaf8`, `#666666`, `#999999` | `borderColor`, `successSubtleColor`, `popupInfoBackgroundColor`, `subtitleColor`, `inactiveTextColor` |
| `imtauthgui/Tenant*.qml` | `#FFFFFF` | `highlightedTextColor` |
| `imtcolgui/ItemSelectElementView.qml` | `#5b8fd6`, `#DFECF9`, `#B4D3F2` | `linkColor`, `popupInfoBackgroundColor`, `borderColor` |
| `imtcolgui/SchemeView.qml` | `#bcd2e8`, `#1167b1` | `firstColorHighlight`, `imaginToolsAccentColor` |
| `imtcolgui/GridCollectionViewBase.qml` | `#ffffff`, `gray` | `baseColor`, `borderColor` |
| `imtdeskgui/TicketCreateDialog.qml` | `#FAFBFC` | `backgroundColor` |
| `imtgeogui/AddressTreeView.qml` | `#ffffff`, `lightsteelblue` | `scrollBackgroundColor`, `selectedColor` |

The ticket-notification popup in `ApplicationMain.qml` keeps its split by
notification type - `assignee` stays green (`successSubtleColor`), everything
else stays blue (`popupInfoBackgroundColor`, Primer accent-subtle) - so the
meaning of the color is unchanged for the ten applications that inherit it.

Still holding literals, on purpose: `Qml/imt3dgui/` (3D material, light and
clear colors - a render scene, not UI chrome) and `Base/DesignScheme.qml`
(a `QtQuick`-only singleton with no `Acf` import, used by the canvas shape
editors).

## Not aligned (deliberate)

- **Icons.** Still the in-repo SVG set, not Octicons.
- **Mona Sans weights.** Only Regular and SemiBold are vendored; Mona Sans
  ships a much wider range, and the variable font is not used because Qt 6.2
  support for variable axes is limited.
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
