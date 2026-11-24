# TableView and TreeView in JQML v5

JQML v5 has Qt 6.12's table and tree views, ported from the Qt sources and compared with Qt itself.

## What is there

| Module | Types |
|---|---|
| `QtQuick` | `TableView`, `TreeView` |
| `QtQuick.Controls` (the Basic style) | `HorizontalHeaderView`, `VerticalHeaderView`, `TableViewDelegate`, `TreeViewDelegate`, `SelectionRectangle` |
| `QtQml.Models` | `ItemSelectionModel`, `DelegateChooser`, `DelegateChoice`, `DelegateModel` (its enums only) |
| `Qt.labs.qmlmodels` | `TableModel`, `TableModelColumn`, `TreeModel`, `DelegateChooser`, `DelegateChoice` |

These types are visible only through an import of their module, so project types of the same name (the
`SortFilterProxyModel` of imtcontrols) keep working.

`DelegateModel` gives only its enums, which `TableView.delegateModelAccess` uses. A `DelegateModel` as the model of a
view prints a warning.

### A component per column

TableView has no property for the delegate of a column. Use a `DelegateChooser` as the delegate:

```qml
import QtQuick
import Qt.labs.qmlmodels

TableView {
    model: tableModel
    delegate: DelegateChooser {
        DelegateChoice { column: 0; delegate: Text { required property var display; text: display } }
        DelegateChoice { column: 1; delegate: Rectangle { required property var display; color: display ? "green" : "red" } }
        DelegateChoice { delegate: Text { required property var display; text: display } }  // any other column
    }
}
```

The first matching choice is used. A choice can also match a `row` or a role value (`DelegateChooser.role` and
`DelegateChoice.roleValue`), and choosers can be nested. Set the column widths with `columnWidthProvider` or
`setColumnWidth()`.

## How it is checked against Qt

`tests/diff/run.js` runs every case in `tests/diff/cases` in Qt 6.12 and in v5 and compares what they log:

- A case is run with `qml -platform offscreen`.
- A case that imports `QtTest` is run with `qmltestrunner`, so its mouse, key and wheel events are real ones.
- Qt is run with `QT_QUICK_CONTROLS_STYLE=Basic`, the style v5 has, not the style of the platform.

| Case | What it covers |
|---|---|
| `tableview_api` | All 103 properties, methods and signals of TableView and TreeView, with their types and defaults |
| `tableview_context` | The delegates of each kind of model: roles, required properties, `modelData` |
| `tableview_scroll` | Loading and unloading edges, the estimated content size, hidden columns, spacing, the reuse of delegates |
| `tableview_model_changes` | Inserted, removed and moved rows, data changes, resets, a new model or delegate |
| `tableview_features` | The selection model, `current` / `selected`, moved columns and rows, `syncView`, `DelegateChooser`, the editor |
| `tableview_keys` | The arrows, Page Up / Down, Home / End and Tab / Backtab; Shift selection by cells, rows or columns; the selection modes; hidden rows and columns |
| `tableview_mouse` | Taps and modifiers, edit triggers, `pointerNavigationEnabled`, resizing by a grid line, flicks, the wheel |
| `tableview_position` | `positionViewAt*` with every alignment and offset, `Contain`, margins and spacing, animated positioning |
| `tableview_edit` | Return / Enter, any key, Escape, Tab / Backtab while editing, `TableView.commit`, warnings |
| `tableview_delegates` | The pool (`pooled` / `reused`), `reuseItems`, writes from a delegate, `delegateModelAccess`, array and ListModel models |
| `treeview_basic`, `treeview_interaction` | Expanding and collapsing, the keys, the indicator of `TreeViewDelegate`, `rootIndex`, the model changing under expanded nodes |
| `headerview`, `view_delegates`, `selection_rectangle`, `tablemodel_selection`, `flickable_wheel` | The controls, the models and the Flickable wheel |
| `headerview_models` | A header with a model of its own (a JS array, a number, a ListModel), the default `textRole`, the look of the Basic delegate |
| `tablemodel_editrole` | `setData` with `EditRole`: a column without an `edit` role refuses it, and the roles `dataChanged` gives |

Some things qmltestrunner cannot show are covered by `tests/unit/Tables.test.js`:

- QTest spaces its clicks so that no double tap is seen. Double taps on a grid line and with the `DoubleTapped` trigger
  are covered there.
- Waiting test functions.

Some cases in Qt depend on time. The cases are written so that they do not:

- QHash gives the order of the reuse pool, and it changes from run to run. The cases compare counts, not the order.
- Positioning on a row that is not loaded waits for the next polish. The cases call `forceLayout()` before they log.
- Animations would carry over from one test to the next. The cases turn `animate` off.

## Changes made to match Qt

- **Flickable, ListView and GridView:**
  - Setting `contentX` or `contentY` from code stops the animation of that axis and ends the movement, as
    `QQuickFlickable::setContentX` does.
  - A wheel step moves the content by 72 px (`wheelScrollLines` × 24), animated with OutExpo over 300 ms, and is not
    taken at the end of the content.
  - Pixel deltas from a touchpad still move the content directly.
- **TableView:**
  - A resize starts only on a grid line that the pointer hovers before the press.
  - A double tap is detected on a view that does not flick.
  - The modifiers of a press are no longer taken from the buttons.
  - `delegateModelAccess`: `ReadOnly` makes the roles read-only, and with `ReadWrite` a required property writes the
    model. A new value takes effect when items are made or reused.
  - A required `modelData` of a model with more than one role is the model object of the cell.
- **TreeView:**
  - `itemIndex` searches from the item it found last, as Qt does.
  - The set of expanded nodes is looked up by a hash.
  - A data change updates only the loaded rows.
- **HeaderView:**
  - The Basic delegate is a control with a `background` and a `contentItem`, in the Basic palette of Qt 6.12 (`#f6f6f6`,
    a `#e4e4e4` border, `#26282a` text); a selected section looks the same, as in Qt.
  - A model that is a JS array or a number keeps an empty `textRole` and shows its `modelData`. Before, such a header
    was empty.
- **TreeViewDelegate:** the indicator is the chevron of the Basic style (`arrow-indicator.png`, 11 × 18) in
  `windowText`, also on a highlighted row.
- **Rectangle:** the border is a `QQuickPen`: 1 px and black by default, drawn once `border.color` or `border.width` is
  written (the same value too), while the color is visible and the width rounds to 1 or more. A rectangle with only
  `border.color` has a border, as in Qt and in JQML v3.
- **Image:** an image with bindings made in a component took no end of its first load and stayed `Loading`: its load was
  kept in `$pending`, where the engine counts pending bindings.
- **TestCase:**
  - `wait()`, `tryCompare()` and `tryVerify()` block the test function: test methods that call them are compiled as
    async.
  - Keys are delivered without a DOM.
  - The window is activated when the tests start, as qmltestrunner does.

## What a page looks like

The documents of a table (headers, `TableViewDelegate`, a selection and the current cell), of a `DelegateChooser` with a
component per column, a hidden column and `rowHeightProvider`, and of a tree (`TreeViewDelegate`) were rendered in Qt
6.12 (`QT_QUICK_CONTROLS_STYLE=Basic`) and in Chromium. They are the same except for the font: Qt offscreen and the
browser have different default faces, so a text can differ by a pixel.

Not in v5: `ScrollBar`, `ScrollIndicator` and the other controls of `QtQuick.Controls` (`CheckBox`, `Button`...). A
TableView has no scroll bar of its own in Qt either: it is attached with `ScrollBar.vertical: ScrollBar {}`.

`TableModel` sends `dataChanged` with the role that was set only. A delegate bound to `display` does not show an edit
made with `EditRole` (the editor of `TableViewDelegate`), in Qt as in v5. A model whose `display` and `edit` are one
value has to change `display` too.

## Compared with the ImtCore views

The benchmarks are `tests/bench/tables.js` and `tests/bench/trees.js`, run as
`node --expose-gc tests/bench/tables.js` and `node --expose-gc tests/bench/trees.js`. They measure the JavaScript of the
engine in node, without a DOM. Each case runs in a process of its own, and each figure is the median of 3 runs. The row
heights are the same on both sides (36 px in the tables, 28 px in the trees). The window is 1000 × 600.

### Tables: imtcontrols `Table` (TreeItemModel and TableHeaders) and `TableView` (TableModel and HorizontalHeaderView)

The tables have 6 columns.

| Rows | | Create | Heap | Items | Scroll step | Scroll jump | Insert at top |
|---|---|---|---|---|---|---|---|
| 1 000 | ImtCore | 64 ms | 5.4 MB | 821 | 0.57 ms | 1.13 ms | 0.60 ms |
| | TableView | **18 ms** | **1.4 MB** | **221** | 0.70 ms | 2.29 ms | 0.99 ms |
| 10 000 | ImtCore | 97 ms | 7.3 MB | 821 | 0.60 ms | 1.24 ms | 0.63 ms |
| | TableView | **40 ms** | **4.4 MB** | **221** | 0.69 ms | 2.31 ms | 1.02 ms |
| 100 000 | ImtCore | 344 ms | **33.0 MB** | 821 | 0.57 ms | 1.14 ms | 0.71 ms |
| | TableView | **270 ms** | 38.6 MB | **221** | 0.70 ms | 2.36 ms | 1.09 ms |

- TableView makes a table 1.3–3.6 times faster and keeps about 3.7 times fewer items: it loads the visible cells only,
  while the imtcontrols Table keeps the cache buffer of the ListView and rows of several items each.
- The imtcontrols Table scrolls a little faster, and a jump to a far position takes it half the time.
- At 100 000 rows TableModel keeps more memory than TreeItemModel, because it holds the rows as JavaScript objects.
- An edit is not compared: the imtcontrols cells set their text once, when they are made or reused, so
  `TreeItemModel.setData` does not change a visible cell. TableView updates the cell (0.03 ms).

### Trees: imtcontrols `BasicTreeView` and `TreeView` (TreeModel and the Basic TreeViewDelegate)

Each top node has 10 children, and the trees have 4 columns.

| Nodes | | Create | Expand all | Collapse all | Toggle a node | Scroll step | Heap, collapsed / expanded |
|---|---|---|---|---|---|---|---|
| 1 100 | ImtCore | 69 ms | 44 ms | 39 ms | 8.19 ms | 7.46 ms | 9.1 / 7.4 MB |
| | TreeView | **30 ms** | **16 ms** | **6 ms** | **1.23 ms** | **1.01 ms** | **2.5 / 4.5 MB** |
| 11 000 | ImtCore | 115 ms | 56 ms | 43 ms | 8.27 ms | 7.66 ms | 14.8 / 15.1 MB |
| | TreeView | **84 ms** | **39 ms** | **23 ms** | **1.25 ms** | **1.02 ms** | **6.1 / 10.4 MB** |
| 110 000 | ImtCore | **435 ms** | **186 ms** | **77 ms** | 9.43 ms | 7.60 ms | 69.7 / 87.3 MB |
| | TreeView | 550 ms | 270 ms | 546 ms | **1.24 ms** | **0.99 ms** | **41.7 / 67.4 MB** |

- TreeView toggles a node about 7 times faster and scrolls 7.5 times faster. It keeps 1.3–3.6 times less memory.
- At 110 000 nodes BasicTreeView makes the tree and expands or collapses all of it faster: it rebuilds a flat
  ListModel, while TreeView inserts and removes the rows of each node, as Qt does.

### Qt 6.12 itself

The same TreeView document was measured in Qt 6.12. The table gives the time of the operations at 110 000 nodes:

| | Expand all | Collapse all |
|---|---|---|
| Qt 6.12 (C++) | 4.6 s | 485 s |
| JQML v5 | 0.27 s | 0.55 s |

Qt's `collapseRecursively` is quadratic: each collapsed node sends a data change for all the rows below it. v5 limits
that change to the loaded rows, and looks expanded nodes up by a hash.
