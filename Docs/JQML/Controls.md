# QtQuick.Controls in JQML v5

JQML v5 (`Tools/JQML/v5`) has the controls of `QtQuick.Controls` (the Basic style) and `QtQuick.Templates` of Qt 6.12.
They are written in JavaScript in `Runtime/QtQuick/Controls`, from the C++ templates (`qquick*.cpp`) and the QML of the
Basic style (`Basic/*.qml`), and every one is checked against Qt 6.12 by a diff test (`tests/diff`).

## Imports

A control is a type only when the document imports its module, as `QQmlImports` of Qt resolves it:

| Import | What the document sees |
| --- | --- |
| none | no control: `Button {}` is not found (a project type `Button.qml` is used) |
| `import QtQuick.Controls` | the Basic controls: the module has no types of its own and imports `QtQuick.Controls.Basic` (`import X auto` of its qmldir) |
| `import QtQuick.Controls.Basic` | the same controls |
| `import QtQuick.Templates as T` | `T.Button` ...: the controls without a style |

- The modules the document imports come first, the last one wins; then the modules they import (transitively); then the
  directory of the document. A `Button.qml` next to the document loses to `QtQuick.Controls`.
- The attached objects of a module are part of it too: `ScrollBar.vertical`, `ToolTip.text`, `ContextMenu.menu`,
  `SplitHandle.pressed` ... without the import are an error, as in Qt ("Non-existent attached object").
- The lists of the types are in `Runtime/Qt/modules.js` (`CONTROLS`, `TEMPLATES`, `IMPORTED_TYPES`).

## The controls

| Group | Types |
| --- | --- |
| base | Control, Label, Pane, Frame, GroupBox, Page, ToolBar, ToolSeparator, MenuSeparator, Container |
| buttons | AbstractButton, Button, ToolButton, RoundButton, CheckBox, RadioButton, Switch, DelayButton, TabButton, ItemDelegate, CheckDelegate, RadioDelegate, SwitchDelegate, SwipeDelegate |
| ranges | Slider, RangeSlider, Dial, ProgressBar, BusyIndicator, PageIndicator, SpinBox, DoubleSpinBox (6.11), Tumbler |
| text | TextField, TextArea, SearchField (6.10) |
| popups | Popup, Overlay, Dialog, DialogButtonBox, Menu, MenuItem, MenuBar, MenuBarItem, Drawer, ToolTip, ComboBox, ContextMenu (6.9) |
| views | ScrollBar, ScrollIndicator, ScrollView, StackView, SplitView, SplitHandle, SwipeView, TabBar |
| actions | Action, ActionGroup, ButtonGroup |
| window | ApplicationWindow |
| calendar | MonthGrid, DayOfWeekRow, WeekNumberColumn, CalendarModel, Calendar (T.AbstractMonthGrid ...) |
| tables | HorizontalHeaderView, VerticalHeaderView, HorizontalHeaderViewDelegate, VerticalHeaderViewDelegate, TableViewDelegate, TreeViewDelegate, SelectionRectangle (see [TablesAndTrees.md](TablesAndTrees.md)) |

`controls_api.qml` (made by `tests/diff/tools/gen_controls_api.js` from `plugins.qmltypes` of Qt 6.12) checks that every
property, signal and method of every control exists and has the default of Qt.

## How a control is made

- **Template** (`Control.js`, `AbstractButton.js`, `Range.js` ...): the logic of the C++ template — the properties, the
  press / move / release, the keys, the focus reasons, the paddings and insets, the implicit sizes.
- **Style** (`Basic.js`): what `Basic/*.qml` declares — the background, the content item, the indicators, the delegates —
  made by `make()` / `apply()`, where a function is a binding. The delegates are made at `componentComplete`, so a
  delegate the document declares replaces the style's one (deferred properties of Qt).
- **Palette** (`Palette.js`): resolved on read — the control's own, then the nearest explicit one above, then the theme
  (Basic for controls, the system one for items); active / inactive / disabled groups.
- **Font** (`ControlFont`): the fields set on a control pass to the controls below it that do not set them.

## Behaviour that changed outside the controls (checked against Qt)

| Change | Test |
| --- | --- |
| `Item.enabled` is the effective value: an item under a disabled parent is disabled | `controls_style.qml` |
| `Rectangle.border`: a pen of width 1 is drawn once its color or width is set (QQuickPen) | `Border.test.js` |
| `Repeater` stacks its delegates before itself (`d0, d1, ..., Repeater`), as `QQuickRepeater::initItem` | `repeater_order.qml` |
| `delegate` / `handle` properties of the controls are `Component` properties (an inline item becomes a component) | `calendar.qml` |
| `QFont`: `bold` sets the weight 700 / 400, a weight above 500 is bold | `font_bold.qml` |
| `ListView` with `StrictlyEnforceRange`: the highlight stays in the range and the item at it becomes current while the view moves; a view made with its component stays at its start | `listview_strict_range.qml` |
| `ListView.model` set again: `currentIndex` 0 (-1 without a delegate); a new delegate keeps it | `listview_strict_range.qml` |
| a `TextInput` / `TextEdit` keeps a binding of `text` while the user types | `textinput_binding_edit.qml` |
| `SpinBox.value` is bound to `from .. to` only once the spin box is complete | `doublespinbox.qml` |
| a right press no item takes is a context menu event (`ContextMenu`) | `contextmenu.qml` |
| `Grid` positioner, `ObjectModel`, `Locale.firstDayOfWeek / dayName / monthName` | `grid.qml`, `objectmodel.qml`, `calendar.qml` |

## Tests

```
cd Tools/JQML/v5
npm run build                          # dist/core.js
npm test                               # unit tests (tests/unit: Controls.test.js - the imports)
node tests/diff/run.js                 # every case against Qt 6.12 (~/Qt/6.12.0)
node tests/diff/run.js tests/diff/cases/tumbler.qml
DIFF_VERBOSE=1 node tests/diff/run.js <case>   # with the message of a failed test function
```

A case with `import QtTest` runs as a `TestCase` in `qmltestrunner` (real pointer and key events), else with `qml`.
The cases of the controls: `controls_api`, `controls_style`, `abstractbutton_signals`, `scrollbar`, `tableview_scrollbar`,
`range_controls`, `range_style`, `containers`, `text_controls`, `popup`, `popups`, `combobox`, `actions`, `scrollview`,
`stackview`, `splitview`, `applicationwindow`, `calendar`, `tumbler`, `swipedelegate`, `doublespinbox`, `searchfield`,
`contextmenu`, `header_delegates`.

## Checked on projects of others

| Project | What it took |
| --- | --- |
| [2048.qml](https://github.com/MartinBriza/2048.qml) (a game, pure QML) | `Item.transform` (Scale, Rotation, Translate, Matrix4x4); `mapToItem` through transforms; an id of the document does not hide a property of an object of a component in it |
| Qt Quick Controls Gallery (the example of Qt 6.12, 38 files) | `as`, type annotations of functions, an object literal binding without parentheses; SortFilterProxyModel; Accessible; a top-level Window of `createObject(null)`; icon themes; default property alias of an object property; `undefined` resets a property; nested layouts, GridLayout spans and auto placement; Container with a Repeater, SwipeView attached; SplitView sizes; Locale month and day numbers of QML; a Menu of Actions; Escape and the shortcuts behind a popup; the overlay of a window only |

| [QmlAppTemplate](https://github.com/emericg/QmlAppTemplate) (an application template with a component library, 200+ files; its C++ helpers replaced by QML stubs of the same API) | `??` and `**`; `Behavior on anchors.margin`; `required name` of an existing property; a Behavior animates changes made in `Component.onCompleted`; `MultiEffect` and `layer.effect` (colorization, shadow, blur, mask), `ShaderEffectSource`; `sourceSize: Qt.size(...)` (a value of a group); a singleton reading its own enums while it is made; `Screen`; `T.Overlay.modal` (an attached property through a namespace); `PlaceholderText`; the corner radii of `Rectangle`; `HoverHandler`, `DragHandler`, `PointerDevice`; an object declared in an alias of `data`; the components of a color (`color.r`, `hsvHue` ...); the position of a window (its place on the screen); `QtQuick.Shapes` and `Path` elements; `layoutDirection` of RowLayout, Row, Grid, Flow; the cells and items of a layout at whole pixels; a layout follows the implicit size of its items; `font: control.font`; `Item.childrenRect`; `topMargin` ... of Flickable and the views; the inherited `locale` of a control; a name in a delegate that is no role (an id of the document around); `Text.linkColor`; the SVG color names (`grey`); `Color` of QtQuick.Controls.impl; `EnterKey`; `Window.activeFocusItem`; `PathView` (and the `Path` geometry: `pointAtPercent`, `PathAttribute`, `PathPercent`); a Tumbler whose content item is a PathView or a ListView of the document; `ColorDialog` (the color input of the browser); `StandardPaths`; `LayoutMirroring` |

The Gallery runs as built for the desktop: a `Main.qml` does what `gallery.cpp` does (the style in the settings, the
`builtInStyles` initial property), the compiler config lists the module (the `qmldir` `qt_add_qml_module` writes) and the
icon theme (`"iconTheme": "gallery"`, `"iconThemePaths": ["src/icons"]`, as `QIcon::setThemeName`). Each of its 33 pages
was compared with the same page in Qt 6.12, and a click through the drawer, a combo box, the dialogs and the options menu.

## Known differences

- **Text metrics.** The default font of Qt is 13 px on macOS, of v5 the page's: sizes that depend on a text are not
  compared.
- **Flick speed.** `qmltestrunner` gives its events timestamps that do not follow `wait()`, so the speed of a drag in a
  Qt test is far above the real one. The cases compare where a view rests after a drag, not how far it flicked
  (Tumbler, SwipeDelegate).
- **Tumbler** of the Basic style draws its wrapping view itself: the positions, the displacement and the animations
  follow `QQuickPathView`; a flick decelerates with `flickDeceleration`, then snaps to an item. A content item of the
  document with a `PathView` or a `ListView` is used as Qt does (QQuickTumblerPrivate::setupViewData).
- **SwipeDelegate:** a click on an item of an open swipe: Qt handles the events of that item twice (through the
  delegate and through its filter); v5 gives the same result (`SwipeDelegate.clicked` of the item, the swipe stays open)
  with one pass. `swipe.transition` is run as its `SmoothedAnimation` (velocity), easing InOutCubic.
- **Images** of the Basic style (`check.png`, `double-arrow.png`, `search-magnifier.png` ...) are vectors of the same
  outline, drawn in the color of the `ColorImage`.
- **The width of a binding against a control's resize** at creation (a `width: parent.width` of the content item of a
  Pane): Qt 6 evaluates the bindings of `width` (bindable properties) in an order of their own; v5 may keep the resize.
- **Native popups and menus** (`popupType: Popup.Native`) are drawn as items.
- **Colors** are kept as their names (`#rrggbb`, `#aarrggbb`): the components of a color made by `Qt.lighter`,
  `Qt.darker`, `Qt.tint` are those of its 8-bit name (Qt keeps 16 bits).
- **LayoutMirroring** mirrors anchors, Row / Grid / Flow, RowLayout / GridLayout and the explicit alignment of a Text;
  not yet the horizontal ListView / GridView.
- **StandardPaths**: a page has no file system of the user, every location is empty.
- **ColorDialog** is the color input of the browser (no alpha in it: the alpha of `selectedColor` is kept).
- **Shapes** are drawn as SVG: the renderer types of Qt are reported, a `ConicalGradient` is a CSS conic gradient
  clipped to the path; `fillItem` and `fillTransform` are kept, not drawn.
