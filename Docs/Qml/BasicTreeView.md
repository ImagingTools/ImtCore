# BasicTreeView

`imtcontrols.BasicTreeView` is a universal QML tree component with optional
multi-column layout, inline cell editing, tri-state checkboxes, text filtering
and keyboard navigation.

Source: `Qml/imtcontrols/Views/BasicTreeView.qml`.

---

## Model format

```qml
model: [
    {
        key: "unique-key",       // unique identifier (required)
        text: "Display text",    // displayed text
        checked: Qt.Unchecked,   // Qt.Unchecked | Qt.Checked | Qt.PartiallyChecked
        checkable: false,        // show checkbox
        enabled: true,           // whether the node is active
        expanded: false,         // whether the node is expanded
        children: [],            // child items (same format)
        data: null               // arbitrary user data
    }
]
```

## Columns format

```qml
columns: [
    {
        name: "name",                       // column identifier
        title: "Name",                      // header title
        display: "text",                    // path to value in node (supports dot-notation: "data.myField")
        type: "string",                     // "string" | "number" | "bool" | "checkState" | "combo" | "auto"
        tree: true,                         // tree column (indent + expand arrow)
        editable: true,                     // editing allowed
        options: [],                        // for combo: array of strings OR [{value, text}]
        editor: null,                       // custom editor Component (overrides built-in editors)
        validator: null,                    // QValidator OR function(value, index) -> true | "error text"
        placeholder: "",                    // hint for the text editor
        width: 0,                           // fixed width in px (0 = stretch share of remaining width)
        minWidth: 0,                        // lower bound when stretching
        horizontalAlignment: Text.AlignLeft // text alignment
    }
]
```

If `columns` is empty, a single default column is used (`display: "text"`,
`tree: true`, `editable: true`).

### Custom editor contract

The component assigned to `column.editor` MUST declare:

* `property var value` — current cell value, set by the tree on load
* `property var column` — column definition object
* `property var nodeIndex` — index object of the edited node
* `signal commit(var value)` — emit to save the new value and close the editor
* `signal cancel()` — emit to discard changes and close the editor

Optionally:

* `function validate(value) -> true | "error text"` — called before commit
* `property bool autoFocus: true` — disable to suppress auto `forceActiveFocus`

The editor is instantiated via `Loader` when the cell enters edit mode and
unloaded after `commit()` / `cancel()`. If the delegate scrolls out of view
the editor is canceled.

---

## Properties

| Property | Default | Description |
| --- | --- | --- |
| `model` | `[]` | Tree data (JS array or `QAbstractItemModel` / `ListModel`). |
| `columns` | `[]` | Column definitions; empty → single default tree column. |
| `rowHeight` | `28` | Row height (px). |
| `indentation` | `18` | Indent per level (px). |
| `cacheBuffer` | `4000` | `ListView` cache buffer. |
| `headerHeight` | `30` | Header row height (px). |
| `showHeader` | `true` | Show header row. |
| `multiSelect` | `false` | Allow multiple selection. |
| `editable` | `false` | Allow cell editing. |
| `editOnDoubleClick` | `true` | Enter edit mode on double-click. |
| `allowDisabledEditing` | `false` | Allow editing of disabled nodes. |
| `allowDisabledSelection` | `false` | Allow selecting of disabled nodes. |
| `tristate` | `false` | Tri-state checkboxes. |
| `skipDisabledOrNonCheckableOnCheck` | `true` | Exclude disabled / non-checkable nodes from check propagation. |
| `filterText` | `""` | Filter text (case-insensitive substring). |
| `filterRole` | `"text"` | Dot-path to the value used for filtering. |
| `filterDebounceMs` | `150` | Debounce delay for `filterText` rebuilds. |
| `flickable` | `null` | External `Flickable` for scroll sync. |
| `writeBack` | `null` | `function(index, column, value, oldValue) -> bool`. Return `false` to reject the edit. |
| `editingFocusOutPolicy` | `"commit"` | What to do when the tree loses active focus while editing: `"commit"`, `"cancel"`, `"keep"`. |
| `currentIndex` (RO) | `null` | Currently selected index object. |
| `editingIndex` (RO) | `null` | Index object of the cell currently being edited (or `null`). |
| `visibleCount` (RO) | | Number of visible rows. |
| `selectedCount` (RO) | | Number of selected items. |
| `editing` (RO) | | `true` when a cell is being edited. |
| `editingKey` (RO) | | Key of node being edited (`""` when not editing). |
| `editingColumn` (RO) | | Column index being edited (`-1` when not editing). |
| `contentListView` (RO) | | Internal `ListView` (e.g. for external scrollbars). |

## Signals

| Signal | Fired |
| --- | --- |
| `nodeClicked(index)` | Row clicked. |
| `nodeDoubleClicked(index)` | Row double-clicked. |
| `cellClicked(index, column)` | A specific cell clicked. |
| `cellDoubleClicked(index, column)` | A specific cell double-clicked. |
| `selectionChanged()` | Selection changed. |
| `checkStateChanged(index, state)` | Checkbox state changed for one node. |
| `checkedItemsChanged()` | Any checkbox change (incl. batch). |
| `headerClicked(column)` | A column header clicked. |
| `cellEditStarted(index, column)` | Editing started. |
| `cellEdited(index, column, value, oldValue)` | Editing committed (after write to the local node). |
| `cellEditCanceled(index, column)` | Editing canceled (Escape / focus-out / scroll-out). |
| `cellEditCommitFailed(index, column, value, reason)` | `writeBack` rejected the edit or `validator` failed. |
| `nodeTextEdited(index, text, oldText)` | Convenience shortcut for `cellEdited` on the `"text"` path. |
| `editingChanged()` | Edit mode entered / left (use with `editing`). |

## Index object

Returned by signals and by `indexForKey()`, `createIndex()`:

| Field | Description |
| --- | --- |
| `key` | Node key. |
| `text` | Node text. |
| `checked` | Checkbox state. |
| `checkable` | Whether the checkbox is shown. |
| `enabled` | Whether the node is active. |
| `expanded` | Whether the node is expanded. |
| `level` | Nesting level (`0` = root). |
| `row` | Position among siblings. |
| `path` | Path in the tree (`"0/2/1"`). |
| `parentKey` | Parent key (`""` for roots). |
| `data` | User data. |
| `item` | Reference to the original object from `model[]`. |
| `value(columnOrPath)` | Read value. |
| `setValue(columnOrPath, value)` | Set value (writes back to the local node and source item). |
| `edit(columnIndex)` | Start editing. |
| `setText(value)` | Set node text. |
| `setChecked(state)` | Set checkbox. |
| `setEnabled(value)` | Enable/disable node. |
| `expand()` / `collapse()` / `select()` | Self-explanatory. |

## Keyboard shortcuts

| Key | Action |
| --- | --- |
| Arrow Up / Down | Move selection. |
| Arrow Right | Expand / move down. |
| Arrow Left | Collapse / move to parent. |
| Home / End | First / last visible row. |
| Space | Toggle checkbox of current node. |
| `*` | Expand all. |
| F2 | Start editing the first editable column of the current row. |
| Enter | Commit current editor. |
| Escape | Cancel current editor. |
| Tab / Shift+Tab | Commit and move to next / previous editable cell. |

---

## Examples

### 1. Simple read-only tree

```qml
BasicTreeView {
    anchors.fill: parent
    model: [
        { key: "root1", text: "Folder 1", children: [
            { key: "child1", text: "File A" },
            { key: "child2", text: "File B" }
        ]},
        { key: "root2", text: "Folder 2", expanded: true, children: [
            { key: "child3", text: "File C" }
        ]}
    ]

    onNodeClicked: console.log("Clicked:", index.key, index.text)
}
```

### 2. Tri-state checkable tree

```qml
BasicTreeView {
    anchors.fill: parent
    tristate: true

    model: [
        { key: "all", text: "All permissions", checkable: true, expanded: true, children: [
            { key: "read",  text: "Read",  checkable: true },
            { key: "write", text: "Write", checkable: true },
            { key: "admin", text: "Admin", checkable: true, enabled: false }
        ]}
    ]

    onCheckedItemsChanged: console.log("Checked:", getCheckedKeys().join(", "))
}
```

### 3. Editable single-column tree

```qml
BasicTreeView {
    anchors.fill: parent
    editable: true
    editOnDoubleClick: true

    model: myModel

    onNodeTextEdited: console.log("Renamed:", index.key, oldText, "->", text)
}
```

### 4. Multi-column tree table with editing

```qml
BasicTreeView {
    anchors.fill: parent
    editable: true
    showHeader: true

    columns: [
        { name: "name",  title: "Name",  display: "text",       tree: true, editable: true, width: 240 },
        { name: "type",  title: "Type",  display: "data.type",  editable: false, width: 120 },
        { name: "value", title: "Value", display: "data.value", editable: true, type: "number" }
    ]

    model: [
        { key: "p1", text: "Parameter 1", data: { type: "int", value: 42 }, children: [
            { key: "p1a", text: "Sub-parameter", data: { type: "float", value: 3.14 } }
        ]}
    ]

    onCellEdited: console.log("Cell edited:", index.key, column.name, oldValue, "->", value)
}
```

### 5. Typed editors

The editor is chosen automatically based on the column's `type`:

| `type` | Editor |
| --- | --- |
| `"string"` | TextInput |
| `"number"` | TextInput with numeric validation |
| `"bool"` | CheckBox (Space / click toggles, Escape cancels) |
| `"checkState"` | Tri-state CheckBox |
| `"combo"` | Popup dropdown with `options` |
| `"auto"` *(or missing)* | Inferred from value or `options` |

### 6. Custom editor per column

```qml
Component {
    id: sliderEditor

    Row {
        property var value
        property var column
        property var nodeIndex

        signal commit(var value)
        signal cancel()

        spacing: 8

        Slider {
            id: slider
            from: 0; to: 100; stepSize: 1
            value: parent.value || 0
            width: parent.width - label.width - parent.spacing
            onPressedChanged: if (!pressed) parent.commit(value)
        }

        Text {
            id: label
            width: 30
            text: Math.round(slider.value)
        }

        Keys.onEscapePressed: cancel()
        Keys.onReturnPressed: commit(slider.value)
    }
}

BasicTreeView {
    columns: [
        { name: "name",     title: "Name",     display: "text",          tree: true, editable: true },
        { name: "priority", title: "Priority", display: "data.priority", editable: true, editor: sliderEditor }
    ]
    model: [
        { key: "i1", text: "Item 1", data: { priority: 75 } }
    ]
    editable: true
}
```

### 7. writeBack — propagating edits to a source model

```qml
BasicTreeView {
    editable: true
    showHeader: true

    columns: [
        { name: "name",  title: "Name",  display: "text",       tree: true, editable: true },
        { name: "count", title: "Count", display: "data.count", editable: true, type: "number" }
    ]

    model: TreeModelBuilder.fromListModel(sourceModel, function(item, i) {
        return {
            key: item.m_id,
            text: item.m_name,
            data: { sourceIndex: i, count: item.m_count }
        }
    })

    // Synchronous; return false to reject the edit (the cell reverts to oldValue)
    writeBack: function(index, column, value, oldValue) {
        var srcIdx = index.data.sourceIndex
        var prop = column.display
        if (prop.indexOf("data.") === 0)
            prop = prop.substring(5)
        return sourceModel.setProperty(srcIdx, "m_" + prop, value)
    }

    onCellEditCommitFailed: console.warn("Rejected:", index.key, column.name, value, reason)
}
```

`writeBack` MUST be synchronous (Promises are not supported in this codebase).
If the source model needs asynchronous validation, accept the edit
optimistically, perform the async operation outside the tree, and on failure
call `treeView.revertCell(index, column)` to restore the previous value.

### 8. Filtering

```qml
Column {
    TextField { id: searchField; placeholderText: "Search..." }

    BasicTreeView {
        model: myTreeModel
        filterText: searchField.text   // case-insensitive substring match
        filterRole: "text"             // or "data.description"
        filterDebounceMs: 200
    }
}
```

### 9. Programmatic control

```qml
BasicTreeView { id: tree; model: myModel }

tree.selectAndEnsureVisible("node-key-123")
tree.expandAll()
tree.collapseAll()

var idx = tree.indexForKey("node-key-123")
idx.setText("New name")
idx.setChecked(Qt.Checked)
idx.setValue("data.myField", 999)

// Editing
tree.startEdit("node-key-123", 0)   // by key + column index
tree.commitCurrentEdit()
tree.cancelCurrentEdit()
tree.editCurrent(1)
```

---

## Notes

* `BasicTreeView` works on a JS copy of the data. Edits do NOT propagate to a
  `QAbstractItemModel` source automatically — use `writeBack` or
  `onCellEdited` to do that.
* `writeBack` is called BEFORE the local node is updated. `onCellEdited` is
  emitted AFTER the local node is updated.
* `cellEditCommitFailed` is emitted both when `writeBack` returns `false` and
  when `column.validator` rejects the value.
