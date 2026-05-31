# imtcontrols — Qt Quick Controls analogues

This folder provides faithful re-implementations of the most popular
[Qt Quick Controls](https://doc.qt.io/qt-5/qtquickcontrols-index.html) types,
built the same way as the existing `imtcontrols` **Popup** and **Menu**: pure
QML, styled through the application `Style` singleton (module `Acf`), and
API-compatible with their Qt counterparts so existing Qt knowledge and code
transfer directly.

All types live in the `imtcontrols` module and are imported together with the
rest of the library:

```qml
import imtcontrols 1.0
```

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
