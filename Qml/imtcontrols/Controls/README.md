# imtcontrols — Qt Quick Controls analogues

This folder provides faithful re-implementations of the most popular
[Qt Quick Controls](https://doc.qt.io/qt-5/qtquickcontrols-index.html) types,
built the same way as the existing `imtcontrols` **Popup** and **Menu**: pure
QML, styled through the application `Style` singleton (module `Acf`), and
API-compatible with their Qt counterparts so existing Qt knowledge and code
transfer directly.

These types form the **imtcontrols 2.0 control system** — a complete,
from-scratch re-implementation of *every* type in the
[Qt Quick Controls](https://doc.qt.io/qt-6/qtquickcontrols-index.html) index
(the **Delegate Controls** section is intentionally excluded). Every type is
provided here even when a same-named control already exists elsewhere in
`imtcontrols` (for example `Button`, `ComboBox`, `Slider`, `Menu`): the
re-implementations that would otherwise collide with a legacy `1.0` type are
registered at **version `2.0`**, so the two sets coexist. Opt in to the new
controls with a versioned import:

```qml
import imtcontrols 2.0
```

A `2.0` import still sees every `1.0` type, so the foundation types
(`Control`, `AbstractButton`, `Container`, `Pane`, ...) and the rest of the
library remain available alongside the new `2.0` controls. Legacy code that
uses `import imtcontrols 1.0` keeps resolving to the original controls and is
unaffected.

> **Why re-implement Qt Quick Controls?**
> These types reproduce Qt's API and visuals while drawing entirely from the
> project's design tokens (`Style.*`). That keeps controls visually consistent
> with the rest of the application, works in environments where the upstream
> `QtQuick.Controls` style is not deployed, and integrates with the
> imtcontrols `Popup`/`Overlay` infrastructure (used by `ToolTip`).

---

## Contents

| Control            | Qt equivalent     | Kind            | Summary                                            |
|--------------------|-------------------|-----------------|----------------------------------------------------|
| [`Control`](#control)            | `Control`         | base            | Padding / content / background / hover foundation. |
| [`AbstractButton`](#abstractbutton) | `AbstractButton`  | base            | Press / click / toggle / exclusivity behaviour.    |
| [`Pane`](#pane)               | `Pane`            | container       | Plain surface with padding.                        |
| [`Frame`](#frame)              | `Frame`           | container       | Bordered surface.                                  |
| [`GroupBox`](#groupbox)           | `GroupBox`        | container       | Titled frame.                                      |
| [`Page`](#page)               | `Page`            | container       | Header / content / footer layout.                  |
| [`ToolBar`](#toolbar)            | `ToolBar`         | container       | Action bar surface.                                |
| [`ToolSeparator`](#toolseparator)      | `ToolSeparator`   | divider         | Vertical / horizontal separator.                   |
| [`Label`](#label)              | `Label`           | text            | Themed `Text`.                                     |
| [`Switch`](#switch)             | `Switch`          | input           | On/off toggle with sliding handle.                 |
| [`RoundButton`](#roundbutton)        | `RoundButton`     | button          | Circular push button.                              |
| [`TabBar`](#tabbar) / [`TabButton`](#tabbutton) | `TabBar` / `TabButton` | navigation | Tabbed navigation header.                       |
| [`PageIndicator`](#pageindicator)      | `PageIndicator`   | navigation      | Row of page dots.                                  |
| [`ToolTip`](#tooltip)            | `ToolTip`         | popup           | Delayed hover tooltip (built on `Popup`).          |
| [`ScrollBar`](#scrollbar)          | `ScrollBar`       | indicator       | Interactive scroll bar for a `Flickable`.          |
| [`ScrollView`](#scrollview)         | `ScrollView`      | container       | Scrollable container with scroll bars.             |
| [`Dial`](#dial)               | `Dial`            | input           | Circular value control.                            |
| [`Container`](#container)          | `Container`        | base            | Item collection with a current-index selection.    |
| [`ApplicationWindow`](#applicationwindow) | `ApplicationWindow` | window     | Window with docked header / footer.                |
| [`DelayButton`](#delaybutton)        | `DelayButton`     | button          | Press-and-hold activation button.                  |
| [`Tumbler`](#tumbler)            | `Tumbler`         | input           | Spinnable wheel of items.                          |
| [`Drawer`](#drawer)             | `Drawer`          | navigation      | Edge panel that slides into view.                  |
| [`Button`](#button)             | `Button`          | button (2.0)    | Standard labelled command button.                  |
| [`ToolButton`](#toolbutton)        | `ToolButton`      | button (2.0)    | Borderless button for tool bars.                   |
| [`CheckBox`](#checkbox)           | `CheckBox`        | button (2.0)    | Checkable (optionally tri-state) option box.       |
| [`RadioButton`](#radiobutton)        | `RadioButton`     | button (2.0)    | Exclusive option button.                           |
| [`BusyIndicator`](#busyindicator)      | `BusyIndicator`   | indicator (2.0) | Animated activity spinner.                         |
| [`ProgressBar`](#progressbar)        | `ProgressBar`     | indicator (2.0) | Determinate / indeterminate progress fill.         |
| [`ScrollIndicator`](#scrollindicator)    | `ScrollIndicator` | indicator (2.0) | Non-interactive scroll position indicator.         |
| [`Slider`](#slider)             | `Slider`          | input (2.0)     | Single-value slider.                               |
| [`RangeSlider`](#rangeslider)        | `RangeSlider`     | input (2.0)     | Two-handle range slider.                           |
| [`SpinBox`](#spinbox)            | `SpinBox`         | input (2.0)     | Numeric up/down spin box.                          |
| [`TextField`](#textfield)          | `TextField`       | input (2.0)     | Single-line text input.                            |
| [`TextArea`](#textarea)           | `TextArea`        | input (2.0)     | Multi-line text input.                             |
| [`ComboBox`](#combobox)           | `ComboBox`        | input (2.0)     | Drop-down selector.                                |
| [`StackView`](#stackview)          | `StackView`       | navigation (2.0)| Push/pop page stack.                               |
| [`SwipeView`](#swipeview)          | `SwipeView`       | navigation (2.0)| Side-by-side paged container.                      |
| [`SplitView`](#splitview)          | `SplitView`       | container (2.0) | Resizable split layout with drag handles.          |

---

## Base types

### Control

Foundation of every visual control. Provides:

- **Padding**: `padding`, `topPadding`, `leftPadding`, `rightPadding`,
  `bottomPadding`.
- **Content / background**: `contentItem` (default content goes here via the
  default `contentData` property) and `background`, both reparented and sized
  automatically. The implicit size is `content + paddings` (or the background
  size, whichever is larger), exactly like Qt's `Control`.
- **Hover**: set `hoverEnabled: true` to track `hovered`.
- **Helpers**: `availableWidth`, `availableHeight`, `spacing`, `font`,
  `mirrored`.

```qml
Control {
    padding: 10
    contentItem: Text { text: "Hello" }
    background: Rectangle { color: "#eee" }
}
```

You normally use one of the concrete subtypes below rather than `Control`
directly.

### AbstractButton

Adds interaction on top of `Control`:

- **State**: `text`, `iconSource`, `enabled`, `checkable`, `checked`,
  `autoExclusive`, read-only `pressed`.
- **Signals**: `clicked()`, `released()`, `canceled()`, `toggled()`,
  `doubleClicked()`.
- **Methods**: `toggle()` — toggles `checked` (respecting `autoExclusive`) and
  emits `clicked()`.

When `autoExclusive` is `true`, checking a button unchecks its sibling
auto-exclusive buttons that share the same parent — the mechanism behind
`TabBar` and radio-style groups.

`Switch`, `RoundButton` and `TabButton` all extend `AbstractButton`.

### Container

Non-visual base that collects a set of child items and exposes a
current-item selection on top of them. It does not lay the items out itself —
concrete containers decide how the current item is presented.

| Member                  | Description                                  |
|-------------------------|----------------------------------------------|
| `count`                 | Number of contained items (read-only).       |
| `currentIndex`          | Selected item index (`-1` when empty).       |
| `currentItem`           | Item at `currentIndex` (read-only).          |
| `itemAt(index)`         | Returns the item at `index`, or `null`.      |
| `incrementCurrentIndex()` | Selects the next item.                     |
| `decrementCurrentIndex()` | Selects the previous item.                 |

```qml
Container {
    id: container
    Item { }
    Item { }
}
```

---

## Containers

### Pane

Plain surface with default padding, matching the application background.

```qml
Pane {
    padding: 12
    Column {
        spacing: 8
        Label { text: "Settings" }
        Switch { text: "Enabled" }
    }
}
```

### Frame

A `Pane` variant with a 1px border and rounded corners.

```qml
Frame {
    Column {
        spacing: 8
        CheckBox { text: "Option A" }
        CheckBox { text: "Option B" }
    }
}
```

### GroupBox

A titled frame. The `title` is rendered at the top-left, breaking the frame
border.

| Property | Type   | Default | Description                       |
|----------|--------|---------|-----------------------------------|
| `title`  | string | `""`    | Heading rendered above the frame. |
| `padding`| real   | `spacingM` | Inset between frame and content. |

```qml
GroupBox {
    title: qsTr("Connection")
    Column {
        spacing: 8
        RadioButton { text: "Automatic" }
        RadioButton { text: "Manual" }
    }
}
```

### Page

Lays out an optional `header` at the top, an optional `footer` at the bottom
and the default content in between. Commonly used with `StackView` /
`SwipeView`.

| Property | Type   | Description                              |
|----------|--------|------------------------------------------|
| `title`  | string | Informational page title.                |
| `header` | Item   | Item docked at the top (e.g. `ToolBar`). |
| `footer` | Item   | Item docked at the bottom.               |
| `padding`| real   | Padding around the central content.      |

```qml
Page {
    title: qsTr("Inbox")
    header: ToolBar {
        Label { text: qsTr("Inbox"); anchors.centerIn: parent }
    }
    Column { /* page body */ }
}
```

### ToolBar

A surface for action controls (usually `ToolButton` + `ToolSeparator` in a
`Row`). Place it as a `Page.header` or `Page.footer`; set `position` to
`ToolBar.Footer` to flip the divider line to the top edge.

```qml
ToolBar {
    Row {
        anchors.fill: parent
        spacing: 4
        ToolButton { text: "Open" }
        ToolButton { text: "Save" }
        ToolSeparator {}
        ToolButton { text: "Cut" }
    }
}
```

### ScrollView

Wraps a single sized child in a `Flickable` and shows `ScrollBar`s when the
content overflows.

```qml
ScrollView {
    width: 200; height: 200
    Column {
        Repeater { model: 50; Label { text: "Row " + index } }
    }
}
```

The internal `Flickable` is exposed as `flickable`; `contentWidth` /
`contentHeight` are aliased for explicit sizing.

### ApplicationWindow

An `Item`-based application surface (no `QtQuick.Window` dependency) that adds
the conventional application layout: an optional `header` docked at the top, an
optional `footer` docked at the bottom and the page content (its default
children) in between. It can be hosted inside any window surface provided by the
application, and its background is themed through `Style`.

| Property | Type | Description                              |
|----------|------|------------------------------------------|
| `header` | Item | Item docked at the top (e.g. `ToolBar`). |
| `footer` | Item | Item docked at the bottom.               |

```qml
ApplicationWindow {
    visible: true
    header: ToolBar { Label { text: qsTr("My App") } }
    Page { }
}
```

---

## Dividers & text

### ToolSeparator

A thin divider for tool bars and rows. `orientation` is `Qt.Vertical` by
default; set `Qt.Horizontal` for menus / vertical layouts.

```qml
Row {
    ToolButton { text: "Cut" }
    ToolButton { text: "Copy" }
    ToolSeparator {}
    ToolButton { text: "Paste" }
}
```

### Label

`Text` with the application's default font family, size and colour. All
standard `Text` properties remain available.

```qml
Label {
    text: qsTr("User name")
    font.bold: true
}
```

---

## Inputs & buttons

### Switch

A checkable button rendered as a sliding handle with an optional label. Click
or tap to toggle.

| Property  | Type | Description                          |
|-----------|------|--------------------------------------|
| `text`    | string | Optional label to the right.       |
| `checked` | bool | On/off state.                        |
| `position`| real (read-only) | `1` when on, `0` when off. |

```qml
Switch {
    text: qsTr("Wi-Fi")
    checked: true
    onToggled: console.log("checked:", checked)
}
```

### RoundButton

A button with a fully rounded background (a circle when square). Set
`highlighted` to render it with the accent colour.

```qml
RoundButton {
    text: "+"
    highlighted: true
    onClicked: model.append({})
}
```

### Dial

A circular control for choosing a value in `[from, to]` by dragging.

| Property   | Type | Description                                  |
|------------|------|----------------------------------------------|
| `from` / `to` | real | Range bounds.                            |
| `value`    | real | Current value (clamped to the range).        |
| `stepSize` | real | Snap step; `0` is continuous.                |
| `position` | real (read-only) | Normalised value in `[0, 1]`.    |
| `moved()`  | signal | Emitted on user-driven value changes.      |

```qml
Dial {
    from: 0; to: 100; value: 25
    onMoved: console.log("value:", value)
}
```

### DelayButton

A checkable button that activates only after being held pressed for `delay`
milliseconds. While held, `progress` fills from `0` to `1`; reaching `1`
toggles `checked` and emits `activated()`. Releasing early drains the
progress back to `0`, guarding against accidental activation.

| Member        | Description                                       |
|---------------|---------------------------------------------------|
| `delay`       | Hold duration before activation, in ms (`3000`).  |
| `progress`    | Normalised hold progress in `[0, 1]` (read-only). |
| `activated()` | Emitted once the button has been held for `delay`.|

```qml
DelayButton {
    text: qsTr("Hold to delete")
    delay: 2000
    onActivated: model.removeSelected()
}
```

### Tumbler

A vertical, scrollable wheel of items that keeps the centred item selected.
Flick or drag the wheel to change `currentIndex`.

| Member            | Description                                   |
|-------------------|-----------------------------------------------|
| `model`           | The model providing the items.                |
| `currentIndex`    | Index of the centred (selected) item.         |
| `count`           | Number of items (read-only).                  |
| `visibleItemCount`| Number of items visible at once (`5`).        |
| `delegate`        | Component rendering each item.                |

```qml
Tumbler {
    model: 12
    onCurrentIndexChanged: console.log("value:", currentIndex)
}
```

---

## Navigation

### TabBar

Lays out `TabButton` children horizontally and keeps exactly one checked.
`currentIndex` is the selected tab and the natural binding source for a
`StackLayout` / `SwipeView`.

```qml
TabBar {
    id: bar
    TabButton { text: qsTr("General") }
    TabButton { text: qsTr("Network") }
    TabButton { text: qsTr("About") }
}

StackLayout {
    currentIndex: bar.currentIndex
    Item { /* General */ }
    Item { /* Network */ }
    Item { /* About */ }
}
```

| Member         | Description                                   |
|----------------|-----------------------------------------------|
| `currentIndex` | Selected tab (read/write).                    |
| `count`        | Number of tabs (read-only).                   |
| `spacing`      | Gap between tabs.                             |
| `tabSelected(index)` | Emitted when the user selects a tab.    |

### TabButton

A checkable, auto-exclusive button for use inside `TabBar`. The active tab is
highlighted with an accent underline. Used as shown above.

### PageIndicator

A row of dots showing `count` pages and the `currentIndex`. Bind it to a
`SwipeView`; set `interactive: true` to let clicks change the page.

```qml
SwipeView { id: view; /* pages */ }
PageIndicator {
    count: view.count
    currentIndex: view.currentIndex
    interactive: true
    onCurrentIndexChanged: view.currentIndex = currentIndex
}
```

### Drawer

A panel that overlays its parent and slides in from one of the four `edge`
values. Open it with `open()` and close it with `close()`; `position`
animates between `0` (closed) and `1` (open). When `modal` is `true` a dim
layer covers the rest of the parent and a click on it closes the drawer.

| Member       | Description                                        |
|--------------|----------------------------------------------------|
| `edge`       | `Qt.LeftEdge` / `RightEdge` / `TopEdge` / `BottomEdge`. |
| `position`   | Open amount in `[0, 1]`.                            |
| `modal`      | Whether the area behind is dimmed and blocked.     |
| `open()` / `close()` | Show / hide the drawer.                    |
| `opened()` / `closed()` | Emitted when fully open / closed.       |

```qml
Item {
    Drawer {
        id: drawer
        edge: Qt.LeftEdge
        Column { Label { text: qsTr("Menu") } }
    }
    Button { text: qsTr("Open"); onClicked: drawer.open() }
}
```

---

## Popups & indicators

### ToolTip

A small non-modal popup (built on `imtcontrols` `Popup`) that appears after
`delay` ms and hides after `timeout` ms.

| Property  | Type | Default | Description                       |
|-----------|------|---------|-----------------------------------|
| `text`    | string | `""`  | Tooltip text.                     |
| `delay`   | int  | `500`   | Show delay in ms.                 |
| `timeout` | int  | `5000`  | Auto-hide timeout (`0` disables). |

Methods: `show(text)` schedules the tooltip; `hide()` cancels/closes it.

```qml
Item {
    ToolTip { id: tip; text: qsTr("Save the document") }
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        onEntered: tip.show(tip.text)
        onExited:  tip.hide()
    }
}
```

### ScrollBar

An interactive scroll bar. `size` is the visible fraction of the content and
`position` the scrolled fraction (both `0..1`). Bind it to a `Flickable`
through the `flickable` convenience property and it both follows and drives the
flickable.

```qml
Flickable {
    id: flick
    contentHeight: column.height
    Column { id: column; /* ... */ }

    ScrollBar {
        flickable: flick
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
    }
}
```

Use `orientation: Qt.Horizontal` for a horizontal bar; `increase()` /
`decrease()` step by `stepSize`.

---

## imtcontrols 2.0 controls

The following types extend the Qt Quick Controls set and are registered at
version `2.0` (use `import imtcontrols 2.0`). The `Popup` and `Menu` families
already exist in the module and are reused as-is.

### Button

Standard command button. Set `highlighted` to mark the default action and
`flat` to drop the permanent fill until hovered/pressed.

```qml
Button {
    text: qsTr("Apply")
    highlighted: true
    onClicked: form.apply()
}
```

### ToolButton

A `Button` variant for `ToolBar`: no permanent border or fill, only a subtle
highlight on hover/press/check.

### CheckBox

Checkable option box with an optional label. Set `tristate: true` to enable
the partially-checked `checkState`.

```qml
CheckBox { text: qsTr("Enable notifications"); checked: true }
```

### RadioButton

Exclusive option button. Radio buttons sharing a parent are auto-exclusive,
so checking one unchecks the others.

```qml
Column {
    RadioButton { text: qsTr("Small"); checked: true }
    RadioButton { text: qsTr("Large") }
}
```

### BusyIndicator

Animated spinner shown while `running` is `true`.

```qml
BusyIndicator { running: loader.status === Loader.Loading }
```

### ProgressBar

Horizontal progress fill between `from` and `to`. Set `indeterminate: true`
for an animated sweep when the duration is unknown.

```qml
ProgressBar { from: 0; to: 100; value: download.percent }
```

### ScrollIndicator

Non-interactive companion to `ScrollBar` that only reports the scrolled
position of a `Flickable` and fades out when idle. Bind it through the
`flickable` property.

### Slider

Single-value slider over `[from, to]`. `stepSize` snaps to discrete values;
`orientation` switches between horizontal and vertical. Emits `moved()` on
user changes.

```qml
Slider { from: 0; to: 100; value: 30; onMoved: brightness = value }
```

### RangeSlider

Two-handle range selector exposing `first` and `second` nodes, each with its
own `value`, `position` and `moved()` signal.

```qml
RangeSlider { from: 0; to: 100; first.value: 20; second.value: 80 }
```

### SpinBox

Integer spin box with up/down buttons and an editable field. `stepSize`
controls the increment; `valueModified()` fires on every change.

```qml
SpinBox { from: 0; to: 100; value: 50; stepSize: 5 }
```

### TextField

Themed single-line editor with `placeholderText` and a focus-aware border.
Emits `accepted()` (Enter) and `editingFinished()`.

```qml
TextField { placeholderText: qsTr("Search..."); onAccepted: model.filter(text) }
```

### TextArea

Themed multi-line editor that wraps by default. Wrap it in a `ScrollView` for
long documents.

```qml
ScrollView {
    TextArea { placeholderText: qsTr("Description...") }
}
```

### ComboBox

Drop-down selector. Clicking opens the `imtcontrols` `Popup` listing the
`model`; selecting an entry updates `currentIndex` / `currentText` and emits
`activated()`. Use `textRole` for object models.

```qml
ComboBox {
    model: [ qsTr("Low"), qsTr("Medium"), qsTr("High") ]
    onActivated: priority = currentText
}
```

### StackView

A last-in/first-out stack of pages. `push()` adds a page, `pop()` removes the
top one, `replace()` swaps it and `clear()` empties the stack; `currentItem`
is the visible page. Pages may be a `Component`, an `Item` or a source URL.

```qml
StackView { id: stack; initialItem: homePage }
Button { text: qsTr("Details"); onClicked: stack.push(detailsPage) }
```

### SwipeView

A `Container` that lays out its pages side by side and shows one at a time via
`currentIndex`. Pair it with a `PageIndicator`.

```qml
SwipeView { id: view; Page { } Page { } }
PageIndicator { count: view.count; currentIndex: view.currentIndex }
```

### SplitView

Arranges its children in a row or column separated by draggable handles that
redistribute space between the adjacent panes.

```qml
SplitView {
    orientation: Qt.Horizontal
    Rectangle { color: "#eee" }
    Rectangle { color: "#ddd" }
}
```

---

## Styling

Every control draws from the `Style` singleton (module `Acf`) — the same
tokens used across `imtcontrols` (e.g. `Style.imaginToolsAccentColor`,
`Style.borderColor`, `Style.textColor`, `Style.spacingM`, `Style.radiusM`,
`Style.controlHeightL`). Switching the application theme automatically
restyles these controls, with no per-control overrides required.

To customise an individual instance, override `background` and/or
`contentItem` exactly as you would with Qt Quick Controls:

```qml
RoundButton {
    text: "x"
    background: Rectangle { radius: width / 2; color: "tomato" }
    contentItem: Text { text: parent.text; color: "white"; anchors.centerIn: parent }
}
```

## Compatibility notes

- The public property and signal names mirror Qt Quick Controls, so most Qt
  snippets work after switching the import from `QtQuick.Controls` to
  `imtcontrols`.
- Sizing follows Qt's `implicitWidth`/`implicitHeight` model
  (`content + paddings`, or the background size when larger).
- `ToolTip` integrates with the imtcontrols `Popup`/`Overlay` stack rather than
  Qt's attached `ToolTip`, so it is created explicitly as shown above.
- Targets `import QtQuick 2.12` (Qt 5), consistent with the rest of the module.
