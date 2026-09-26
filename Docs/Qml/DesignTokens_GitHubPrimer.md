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

Three palette entries had not followed that inversion and still pointed at the
page canvas, so the surface they painted came out *darker* than the surface
under it:

| Key | Was | Now | Why |
| --- | --- | --- | --- |
| `Background2` | `#0d1117` | `#151b23` | Cards, table headers, toolbar buttons and the scroll track. Mirrors light, where `Background` is the canvas and `Background2` the surface on top of it. |
| `Hover` | `#0d1117` | `#212830` | Hover has to move *away* from the surface. On light that means one step down from white; on dark one step up from `#151b23`. |
| `OverlayBackdrop` | `#212830` | `#010409` | A modal backdrop has to darken what is behind it. At 40 % over the `#0d1117` canvas the old value lightened it instead. |

`YellowLabel` also had the wrong kind of value: light gives it the subtle
attention *background* `#fff8c5`, dark gave it the attention *foreground*
`#d29922`. It now takes `AttentionSubtle` (`#272115`).

### Tokens that never reached the dark theme

`parseStyleTheme()` only re-reads the tokens it names, so any token left on a
literal keeps its **light** value when the dark scheme loads. Fifteen of them
were in that state. Each is now bound to a token that is theme-driven, chosen
so the light value is unchanged to the digit:

| Token | Bound to | Light (unchanged) | Dark (was) |
| --- | --- | --- | --- |
| `baseColorInverted` | `textColor` | `#1f2328` | `#f0f6fc` (`#1f2328`) |
| `mainColor` | `neutralSubtleColor` | `#eaeef2` | `#212830` (`#eaeef2`) |
| `firstColor` | `linkColor` | `#0969da` | `#4493f8` (`#0969da`) |
| `secondColor` | `attentionColor` | `#9a6700` | `#d29922` (`#9a6700`) |
| `firstColorHighlight` | `selectedColor` | `#ddf4ff` | `#121d2f` (`#ddf4ff`) |
| `positiveAccentColor` | `successColor` | `#1a7f37` | `#3fb950` (`#1a7f37`) |
| `negativeAccentColor` | `dangerColor` | `#d1242f` | `#f85149` (`#d1242f`) |
| `middleAccentColor` | `attentionColor` | `#9a6700` | `#d29922` (`#9a6700`) |
| `selectedLinkFromColor` | `linkColor` | `#0969da` | `#4493f8` (`#0969da`) |
| `selectedLinkToColor` | `successColor` | `#1a7f37` | `#3fb950` (`#1a7f37`) |
| `popupCloseHoverColor` | `menuPanelItemHoverColor` | `#1a818b98` | `#33656c76` (10 % black) |
| `highlightDimmerColor` | `neutralSubtleColor` | `#eaeef2` | `#212830` (`#eaeef2`) |
| `grayColor` | `borderColor2` | `#afb8c1` | `#6e7681` (`#afb8c1`) |
| `lightBlueColor` | `ActiveColors/BlueLabel` | `#54aeff` | `#1f6feb` (`#54aeff`) |

`popupCloseIdleColor`, `highlightBackgroundColor` and `alternatingColor` stay
literal on purpose: they are fully transparent or a black dimmer used under an
`opacity`, and read the same in both themes.

`baseColorInverted` is now a true inversion of `baseColor`, which is what its
name always promised, so it must not be used as a surface color. Three call
sites did exactly that and are fixed: `TableBase.borderColorVertical` takes
`borderColor`, `BarChart.backgroundColor` takes `baseColor` (the patch
rectangles mask a bar outline against the surface behind the chart) and the
`Gallery` lightbox backdrop takes `shadowColor`, which is dark on both themes.

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

### Dialogs and popups carry no border

Primer does put a 1 px `overlay-borderColor` around an overlay, and it was
tried here, but it has to be reverted: that token is translucent, and a
translucent `border.color` cannot survive the JQML web build. `Rectangle.color`
is converted through `Color.getRGBA()`, which reads Qt's `#AARRGGBB`;
`border.color` is interpolated straight into a CSS `outline` string
(`Tools/JQML/v3/QtQml/Border.js`), where the browser reads the same literal as
`#RRGGBBAA`. `#80d1d9e0` — a 50 % grey in Qt — therefore renders as opaque
`rgb(128, 209, 217)`, a pale blue ring. Dialogs and popups are separated from
the page by their shadow and backdrop alone, as before.

**Rule:** never give `border.color` an eight-digit token. Opaque tokens only.

### Dialogs in the dark theme

`dialogBackgroundColor` had no `ActiveColors` entry, so it kept its light
literal `#ffffff` and dialogs stayed white in the dark theme. It is now mapped
to the same surface as `Base` in both themes, which also keeps the dialog body
and the `TopPanelDialog` title strip on one color — needed now that the dialog
is rounded at 12 px, since a title strip in a different color would show its
own corners through the rounding.

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

`fontFamily` and `fontFamilyBold` are the literal `Segoe UI`, the first font in
Primer's system stack on Windows, on desktop and web alike. Mona Sans, GitHub's
brand typeface, was vendored and then removed: the web build rendered its bold
very differently from native Qt, and it has no Cyrillic.

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
| `radiusS` / `radiusM` / `radiusL` | 3 / 6 / 12 | `borderRadius-small` / `-medium` (the default) / `-large` |
| `radiusXS`, `radiusXL`, `radiusXXL` | aliases | deprecated, see below |
| `buttonRadius`, `textFieldRadius`, `comboBoxRadius` | 6 | control radius |
| `controlHeightS` / `M` / `L` | 24 / 32 / 40 | xsmall / medium / large control |
| `buttonHeightXS` / `S` / `M` | 24 / 28 / 32 | xsmall / small / medium button |
| `menuRowHeight`, `tableHeaderHeight` | 32, 40 | |
| `focusRingWidth` | 2 | `focus-outline-width` |
| `buttonBorderWidth` | 1 | `borderWidth-thin` |

The spacing scale (`marginM` = 10, used in ~450 places) was deliberately left
alone; Primer's 4/8/16/24 grid would be a separate, layout-wide change.

### One radius per kind of object

Primer has exactly three radii — 3, 6 and 12 — plus a full round for pills.
`radiusS` was 4 and `radiusXL` / `radiusXXL` were 16 and 30, none of which
appear anywhere in Primer, so the scale now carries the three real values and
the wider names are aliases (`radiusXS` → `radiusS`, `radiusXL` and `radiusXXL`
→ `radiusL`). They are kept rather than deleted because a product `Style.qml`
that reads a missing property fails to load.

Pick by what the object *is*, not by how big it happens to be:

| Kind | Radius | Examples |
| --- | --- | --- |
| Floats above the page | `radiusL` (12) | dialogs, popups, dropdown menus, toasts |
| Interactive control | `radiusM` (6) — or `buttonRadius` / `textFieldRadius` | buttons, inputs, combo boxes, menu and table rows |
| Small decoration | `radiusS` (3) | chips, swatches, check boxes, cell highlights |
| Pill or dot | `width / 2` | avatars, status dots, the menu selection bar |

Closed on this pass:

- **Overlays → `radiusL`.** `DialogDecorator` was 4 and `Dialog` / `DialogNew`
  declared 0; the radius now lives on the dialog, so `MessagePopup` no longer
  restates it. `PopupMenuDialog` and both popup decorators were 6.
- **Buttons → `buttonRadius`.** `TextButton` (5), `TextButtonDecorator` (2),
  `TopButtonDecoratorNew` (4).
- **Inputs → `textFieldRadius`.** `TextInputWithLabel` (3), `CheckBoxMenu` (3),
  and `TextArea` / `TextEditCustom` / `CustomTextEdit`, which were square while
  the single-line fields were rounded.
- **Rows and panels → `radiusM`.** `MenuPanelButtonDecorator` and
  `TableHeaderDelegate` were rounding rows on the `marginS` *spacing* step (8);
  `PopupContainer` was `radiusM + 2`; `TreeViewGql` and `DrawingContainer` 4.
- **Decorations → `radiusS`.** `CheckBox` (square), `Gallery` (2 and 8),
  `BasicTreeView` (2).

Left alone: `radius: 1` on the 2 px grip bars in `ToolbarButton` and
`TableHeaderParamComp` — those are pills, where the radius is half the
thickness.

Radii inside product screens (`imtdeskgui`, `imtgeogui`, …) were left alone
where they compute a circle; they are not part of the shared control set.

## Icons

Icon color is not set in QML. `Style.getIconPath()` picks a pre-rendered file
per state, and those files are produced at build time by
`imtdesign::CDesignTokenIconProcessorComp`, which substitutes the template
color (`#D9D9D9`) in `Include/imtgui/Resources/Icons/Template/*.svg` with the
state color from `IconTemplateList[0].IconColor` in the `.theme`. That map is
the only lever, and it now follows Primer's foreground roles:

| Icon state | Primer role | Light | Dark |
| --- | --- | --- | --- |
| `Normal`, `OffNormal`, `OnNormal` | `fgColor-muted` | `#59636e` | `#9198a1` |
| `OffActive`, `OnActive` | `fgColor-default` | `#1f2328` | `#f0f6fc` |
| `OffSelected`, `OnSelected` | `fgColor-accent` | `#0969da` | `#4493f8` |
| `OffDisabled`, `OnDisabled` | `fgColor-disabled` | `#818b98` | `#656c76` |

Resting icons used to be drawn at `fgColor-default`, which is the near-black
body-text color; GitHub keeps its icons a step back from the text and brings
them forward on hover. That is why `Normal` moved to the muted foreground.

**State is told by color, not opacity.** Three chrome components faded resting
icons with `opacity: Style.opacityHigh`. Stacked on top of the new muted color
that lands lighter than `fgColor-disabled`, so a resting icon would have read
as a disabled one. The fades are gone; `TabPanelDecorator` and the `MenuPanel`
collapse button now swap `Icon.Mode.Normal` ↔ `Icon.Mode.Active` instead. The
generator emits a file for every state in the map, so both variants exist.

`MenuPanelButtonDecorator` cannot do the same swap: its icon URL arrives
pre-built on `baseElement.iconSource`, so a selected row's icon stays muted and
the selection is carried by the row wash, the accent bar and the bold label.

Sizes follow the Octicon grid (16 / 32 / 64, `iconSizeS` / `iconSizeL` /
`iconSizeXXL`); `menuPanelIconSize` moved from 20 to `iconSizeS`.

### The dark theme was showing the light icon set

The recolored files are written to a resource directory named after the style,
`/Light/Icons` and `/Dark/Icons`, and `getIconPath()` picks between them with
`Style.theme`. That property was declared as a constant `"Light"` and nothing
ever wrote to it - `getDesignScheme()` sends the scheme id to the server and
`parseStyleTheme()` applies the colors that come back, but neither touched the
theme name. So the dark scheme repainted every surface and then kept asking for
the icons cut for a white page: `#59636e` glyphs on the `#0d1117` canvas, a
contrast ratio of 1.7, which is why they were invisible rather than merely
dim.

`parseStyleTheme()` now takes the name from the payload itself (`Style/Name`,
which the parser already requires to match the `.theme` file name and therefore
the resource directory) and assigns it to `Style.theme`. Both icon sets are
compiled in - `DesignTokenCreator.cmake` globs every `*.theme` and `rcc`s one
`qrc_<project><style>.cpp` per style - so nothing else had to change.

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

A second pass over the named CSS colors, which the hex sweep had missed, closed
the ones that do not survive a dark background:

| File | Was | Now |
| --- | --- | --- |
| `imtchatgui/ChatPanel.qml`, `ChatView.qml`, `MessageInput.qml`, `ChatNotificationBanner.qml`, `ConversationCreateDialog.qml` | `white` | `highlightedTextColor` |
| `Inputs/Calendar.qml` | `lightgray` | `shadowColor` |
| `Views/ValueLevelIndicator.qml` | `red`, `lightgray` | `dangerColor`, `borderColor2` |
| `Views/CustomScrollbar.qml`, `Views/ScrollIndicator.qml` | `lightgray` fallback | `borderColor2` |
| `GraphicsView/PolyLineShape.qml` | `#000000` | `textColor` |

The chat `white`s were all text on the accent background - an on-emphasis
foreground, which is what `highlightedTextColor` is for, and `#ffffff` in both
themes, so nothing moves on the light theme.

Still holding literals, on purpose: `Qml/imt3dgui/` (3D material, light and
clear colors - a render scene, not UI chrome), `Base/DesignScheme.qml`
(a `QtQuick`-only singleton with no `Acf` import, used by the canvas shape
editors), and the modal scrims in `DialogManagerView.qml` and
`ConversationCreateDialog.qml`, which are a `black` or `gray` wash under an
`opacity` and read the same on both themes.

## Not aligned (deliberate)

- **Icon artwork.** Still the in-repo SVG set, not Octicons. Only the
  *colors and sizes* are aligned — see above.
- **Typeface.** `Segoe UI` instead of Mona Sans — see Typography.
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
3. Never write a radius literal either. Pick from the table above by what the
   object is; a new value on the scale needs a reason Primer agrees with.
4. Keep eight-digit `#AARRGGBB` tokens off `border.color` — they only survive
   on `Rectangle.color`.
5. Express icon state with the icon's own state file
   (`Icon.Mode.Normal` / `Active` / `Selected` / `Disabled`), not with
   `opacity`.
6. Check both themes. A token that is not read back in `parseStyleTheme()`,
   directly or through a binding onto one that is, silently keeps its light
   value under the dark scheme.
7. Do not use `baseColorInverted` as a surface color - it inverts with the
   theme. Backgrounds come from `baseColor`, `backgroundColor`,
   `backgroundColor2` or `shadowColor`.
8. QML is loaded from `qrc:/qml`, so style edits need an `imtcontrolsqml` /
   `imtguiqml` resource rebuild before they show up in a running app.
