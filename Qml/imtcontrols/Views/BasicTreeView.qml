import QtQuick
import QtQuick.Controls
import Acf 1.0
import imtcontrols 1.0

/*
    BasicTreeView — universal tree component with support for:
    - Multi-column display with inline cell editing
    - Tri-state checkboxes with cascading propagation up/down
    - Text filtering with auto-expansion of matched branches
    - Multiple selection
    - Keyboard navigation (arrows, Home/End, F2, Tab, *, Space)
    - writeBack mechanism for propagating changes to the source model

    ═══════════════════════════════════════════════════════════════
    MODEL FORMAT
    ═══════════════════════════════════════════════════════════════

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

    ═══════════════════════════════════════════════════════════════
    COLUMNS FORMAT
    ═══════════════════════════════════════════════════════════════

    columns: [
        {
            name: "name",                       // column identifier
            title: "Name",                      // header title
            display: "text",                    // path to value in node (supports dot-notation: "data.myField")
            type: "string",                     // "string" | "number" | "bool" | "combo" | "auto"
            tree: true,                         // tree column (indent + expand arrow)
            editable: true,                     // editing allowed
            options: [],                        // for combo: array of choices
            editor: null,                       // custom editor Component (overrides built-in editors)
            horizontalAlignment: Text.AlignLeft // text alignment
        }
    ]

    If columns is empty, a single default column is used (display: "text", tree: true, editable: true).

    Custom editor Component contract:
        Required properties:  value, column, nodeIndex
        Required signals:     commit(var value), cancel()
        The component receives focus automatically on activation.

    ═══════════════════════════════════════════════════════════════
    EXAMPLE 1: Simple tree (display only)
    ═══════════════════════════════════════════════════════════════

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

    ═══════════════════════════════════════════════════════════════
    EXAMPLE 2: Checkable tree (tri-state)
    ═══════════════════════════════════════════════════════════════

    BasicTreeView {
        anchors.fill: parent
        tristate: true

        model: [
            { key: "all", text: "All permissions", checkable: true, expanded: true, children: [
                { key: "read",  text: "Read",   checkable: true },
                { key: "write", text: "Write",  checkable: true },
                { key: "admin", text: "Admin",  checkable: true, enabled: false }
            ]}
        ]

        onCheckedItemsChanged: {
            var keys = getCheckedKeys()
            console.log("Checked:", keys.join(", "))
        }
    }

    ═══════════════════════════════════════════════════════════════
    EXAMPLE 3: Editable tree (single column)
    ═══════════════════════════════════════════════════════════════

    BasicTreeView {
        anchors.fill: parent
        editable: true
        editOnDoubleClick: true

        model: myModel

        onNodeTextEdited: {
            console.log("Renamed:", index.key, oldText, "->", text)
        }
    }

    ═══════════════════════════════════════════════════════════════
    EXAMPLE 4: Multi-column tree table with editing
    ═══════════════════════════════════════════════════════════════

    BasicTreeView {
        anchors.fill: parent
        editable: true
        showHeader: true

        columns: [
            { name: "name",  title: "Name",   display: "text",       tree: true, editable: true  },
            { name: "type",  title: "Type",   display: "data.type",  editable: false },
            { name: "value", title: "Value",  display: "data.value", editable: true, type: "number" }
        ]

        model: [
            { key: "p1", text: "Parameter 1", data: { type: "int", value: 42 }, children: [
                { key: "p1a", text: "Sub-parameter", data: { type: "float", value: 3.14 } }
            ]}
        ]

        onCellEdited: {
            console.log("Cell edited:", index.key, column.name, oldValue, "->", value)
        }
    }

    ═══════════════════════════════════════════════════════════════
    EXAMPLE 5: Typed editors (combo, bool, number)
    ═══════════════════════════════════════════════════════════════

    // The editor widget is chosen automatically based on column type:
    //   "string"  → TextInput
    //   "number"  → TextInput with numeric validation
    //   "bool"    → CheckBox (click or Space toggles, Escape cancels)
    //   "combo"   → Popup dropdown with options list

    BasicTreeView {
        anchors.fill: parent
        editable: true
        showHeader: true

        columns: [
            { name: "name",   title: "Name",    display: "text",         tree: true, editable: true },
            { name: "active", title: "Active",  display: "data.active",  editable: true, type: "bool" },
            { name: "mode",   title: "Mode",    display: "data.mode",    editable: true, type: "combo",
              options: ["Auto", "Manual", "Disabled"] },
            { name: "weight", title: "Weight",  display: "data.weight",  editable: true, type: "number" }
        ]

        model: [
            { key: "item1", text: "Item 1", data: { active: true, mode: "Auto", weight: 1.5 } },
            { key: "item2", text: "Item 2", data: { active: false, mode: "Manual", weight: 3.0 } }
        ]
    }

    ═══════════════════════════════════════════════════════════════
    EXAMPLE 6: Custom editor per column
    ═══════════════════════════════════════════════════════════════

    // When built-in editors (TextInput, CheckBox, Popup combo) are not enough,
    // you can provide a custom Component as the editor for any column.
    //
    // HOW TO SET UP:
    //   1. Define a Component containing your editor UI
    //   2. The root item of the Component MUST declare:
    //        property var value      — current cell value (set by the tree on load)
    //        property var column     — column definition object
    //        property var nodeIndex  — index object of the edited node (see INDEX OBJECT section)
    //        signal commit(var value) — emit to save the new value and close the editor
    //        signal cancel()          — emit to discard changes and close the editor
    //   3. Assign the Component to column.editor in your columns array
    //
    // LIFECYCLE:
    //   - The editor is instantiated via Loader when the cell enters edit mode
    //   - It receives focus automatically (forceActiveFocus)
    //   - After commit() or cancel() the Loader unloads the component
    //   - If the delegate scrolls out of view, the editor is canceled automatically
    //
    // WHAT nodeIndex PROVIDES (same object as in signals):
    //   nodeIndex.key        — unique node key
    //   nodeIndex.text       — node text
    //   nodeIndex.data       — user data object (from model item's "data" field)
    //   nodeIndex.level      — nesting depth
    //   nodeIndex.parentKey  — parent node key
    //   nodeIndex.value(path) — read any field by dot-path
    //   (full list: see INDEX OBJECT section above)

    // --- Example: slider editor for a numeric column ---

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
                width: parent.width - valueLabel.width - parent.spacing
                height: parent.height
                from: 0
                to: 100
                value: parent.value || 0
                stepSize: 1

                onPressedChanged: {
                    if (!pressed)
                        parent.commit(slider.value)
                }
            }

            Text {
                id: valueLabel
                width: 30
                anchors.verticalCenter: parent.verticalCenter
                text: Math.round(slider.value)
                horizontalAlignment: Text.AlignRight
            }

            Keys.onEscapePressed: cancel()
            Keys.onReturnPressed: commit(slider.value)
        }
    }

    // --- Example: color picker editor ---

    Component {
        id: colorEditor

        Rectangle {
            property var value
            property var column
            property var nodeIndex

            signal commit(var value)
            signal cancel()

            color: value || "#ffffff"
            border.color: "gray"
            border.width: 1
            radius: 2

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    var colors = ["#ff0000", "#00ff00", "#0000ff", "#ffff00", "#ff00ff"]
                    var idx = colors.indexOf(parent.parent.value)
                    parent.parent.commit(colors[(idx + 1) % colors.length])
                }
            }

            Keys.onEscapePressed: cancel()
        }
    }

    // --- Usage ---

    BasicTreeView {
        anchors.fill: parent
        editable: true
        showHeader: true

        columns: [
            { name: "name",     title: "Name",     display: "text",          tree: true, editable: true },
            { name: "color",    title: "Color",    display: "data.color",    editable: true, editor: colorEditor },
            { name: "priority", title: "Priority", display: "data.priority", editable: true, editor: sliderEditor }
        ]

        model: [
            { key: "item1", text: "Item 1", data: { color: "#ff0000", priority: 75 } },
            { key: "item2", text: "Item 2", data: { color: "#00ff00", priority: 30 } }
        ]

        // onCellEdited fires for ALL editors (built-in and custom) after commit
        onCellEdited: {
            // index   — index object of the edited node
            // column  — column definition: { name, display, editor, ... }
            // value   — new value after editing
            // oldValue — value before editing
            console.log("Edited:", index.key, column.name, oldValue, "->", value)
        }
    }

    ═══════════════════════════════════════════════════════════════
    EXAMPLE 7: writeBack — propagation to source model
    ═══════════════════════════════════════════════════════════════

    // PROBLEM: BasicTreeView works with a JS-array copy of the data.
    // When using TreeModelBuilder.fromListModel() to transform a C++ ListModel
    // (or any QAbstractItemModel) into the tree format, edits only change
    // the local JS copy — the original source model is NOT updated automatically.
    //
    // SOLUTION: Use writeBack callback or onCellEdited signal to propagate
    // changes back to the source model.
    //
    // HOW index.data.sourceIndex WORKS:
    //   When building the tree model, you store the source row index in data:
    //     data: { sourceIndex: i }
    //   Later, in writeBack/onCellEdited, you retrieve it:
    //     var srcIdx = index.data.sourceIndex
    //   This gives you the row number in the ORIGINAL source model, so you can
    //   call sourceModel.setProperty(srcIdx, propertyName, newValue) to write back.
    //
    // NOTE: index.data is the same object you passed as "data" when building the model.
    //   You can store anything there: sourceIndex, sourceId, a reference to the
    //   original item, etc. It's your link between the tree node and the source.
    //
    // onCellEdited SIGNAL PARAMETERS:
    //   index    — index object of the edited node (see INDEX OBJECT section)
    //             index.key       — node key
    //             index.text      — current text
    //             index.data      — your custom data object
    //             index.item      — reference to the original model[] item
    //             index.value(p)  — read any field by dot-path
    //   column   — column definition object that was edited
    //             column.name     — column identifier ("name", "value", etc.)
    //             column.display  — dot-path to the value in the node ("text", "data.myField")
    //             column.type     — "string" | "number" | "bool" | "combo"
    //   value    — the new value AFTER the edit was committed and written to the local node
    //   oldValue — the value BEFORE the edit started

    BasicTreeView {
        anchors.fill: parent
        editable: true
        showHeader: true

        columns: [
            { name: "name",  title: "Name",  display: "text",       tree: true, editable: true },
            { name: "count", title: "Count", display: "data.count", editable: true, type: "number" }
        ]

        model: TreeModelBuilder.fromListModel(sourceModel, function(item, i) {
            return {
                key: item.m_id,          // unique key from source
                text: item.m_name,       // display text
                data: {
                    sourceIndex: i,      // ← ROW INDEX in sourceModel (0, 1, 2, ...)
                    count: item.m_count  // additional fields to display/edit
                }
            }
        })

        // OPTION A: writeBack — called BEFORE writing to local node.
        // Return false to reject the edit (the cell reverts to old value).
        writeBack: function(index, column, value, oldValue) {
            var srcIdx = index.data.sourceIndex   // row in sourceModel
            var prop = column.display             // e.g. "data.count" or "text"

            // For "data.X" paths, extract the property name after "data."
            if (prop.indexOf("data.") === 0)
                prop = prop.substring(5)          // "data.count" -> "count" -> "m_count"

            sourceModel.setProperty(srcIdx, "m_" + prop, value)
            return true  // accept — local node will also be updated
            // return false — reject, local node stays unchanged
        }

        // OPTION B: onCellEdited — called AFTER writing to local node.
        // Use this if you don't need rejection logic.
        onCellEdited: {
            // index.data.sourceIndex — row in sourceModel that corresponds to this node
            var srcIdx = index.data.sourceIndex
            console.log("Node", index.key, "column", column.name,
                        "changed from", oldValue, "to", value,
                        "sourceModel row:", srcIdx)
            // sourceModel.setProperty(srcIdx, "m_" + column.name, value)
        }
    }

    ═══════════════════════════════════════════════════════════════
    EXAMPLE 8: Filtering
    ═══════════════════════════════════════════════════════════════

    Column {
        TextField {
            id: searchField
            placeholderText: "Search..."
        }

        BasicTreeView {
            anchors.left: parent.left
            anchors.right: parent.right
            height: 400

            model: myTreeModel
            filterText: searchField.text    // auto-filter by "text"
            filterRole: "text"              // or "data.description" for searching by another field
        }
    }

    ═══════════════════════════════════════════════════════════════
    EXAMPLE 9: Programmatic control
    ═══════════════════════════════════════════════════════════════

    BasicTreeView {
        id: tree
        model: myModel
    }

    // Navigation
    tree.selectAndEnsureVisible("node-key-123")
    tree.expandAll()
    tree.collapseAll()

    // Reading data
    var idx = tree.indexForKey("node-key-123")
    console.log(idx.text, idx.value("data.myField"))

    // Programmatic modification
    idx.setText("New name")
    idx.setChecked(Qt.Checked)
    idx.setEnabled(false)
    idx.setValue("data.myField", 999)

    // Programmatic edit activation
    idx.edit(0)          // edit column 0
    tree.editCurrent(1)  // edit column 1 of the current node

    // Checkboxes
    tree.checkAll()
    tree.uncheckAll()
    var checked = tree.getCheckedKeys()  // ["key1", "key2"]
    var nodes = tree.getCheckedNodes()   // [index, index, ...]

    ═══════════════════════════════════════════════════════════════
    SIGNALS
    ═══════════════════════════════════════════════════════════════

    nodeClicked(index)                  — row clicked
    nodeDoubleClicked(index)            — row double-clicked
    cellClicked(index, column)          — specific cell clicked
    cellDoubleClicked(index, column)    — specific cell double-clicked
    selectionChanged()                  — selection changed
    checkStateChanged(index, state)     — checkbox state changed
    checkedItemsChanged()               — any checkbox change
    headerClicked(column)               — column header clicked
    cellEditStarted(index, column)      — editing started
    cellEdited(index, column, value, oldValue) — editing committed (after write)
    cellEditCanceled(index, column)     — editing canceled (Escape)
    nodeTextEdited(index, text, oldText) — text changed (convenience shortcut)

    ═══════════════════════════════════════════════════════════════
    INDEX OBJECT (returned by signals and createIndex)
    ═══════════════════════════════════════════════════════════════

    index.key         — node key
    index.text        — text
    index.checked     — checkbox state
    index.checkable   — whether checkbox is shown
    index.enabled     — whether node is active
    index.expanded    — whether node is expanded
    index.level       — nesting level (0 = root)
    index.parentKey   — parent key ("" for roots)
    index.data        — user data
    index.item        — reference to original object from model[]
    index.row         — position among siblings
    index.path        — path in tree ("0/2/1")

    index.value(columnOrPath)           — get value
    index.setValue(columnOrPath, value)  — set value
    index.edit(columnIndex)             — start editing
    index.setText(value)                — set text
    index.setChecked(state)             — set checkbox
    index.setEnabled(value)             — enable/disable
    index.expand()                      — expand
    index.collapse()                    — collapse
    index.select()                      — select

    ═══════════════════════════════════════════════════════════════
    PROPERTIES
    ═══════════════════════════════════════════════════════════════

    model: []                   — tree data
    columns: []                 — column definitions
    rowHeight: 28               — row height
    indentation: 18             — indent per level
    cacheBuffer: 4000           — ListView cache buffer
    headerHeight: 30            — header height
    showHeader: true            — show header row
    multiSelect: false          — multiple selection
    editable: false             — allow editing
    editOnDoubleClick: true     — edit on double-click
    allowDisabledEditing: false — allow editing disabled nodes
    allowDisabledSelection: false — allow selecting disabled nodes
    tristate: false             — tri-state checkboxes
    filterText: ""              — filter text
    filterRole: "text"          — path for filtering
    flickable: null             — external Flickable for scroll sync
    writeBack: null             — function(index, column, value, oldValue) -> bool

    // Readonly
    currentIndex                — currently selected index
    visibleCount                — number of visible rows
    selectedCount               — number of selected items
    editing                     — whether editing is active
    editingKey                  — key of node being edited
    editingColumn               — column index being edited
    contentListView             — access to internal ListView
*/

Item {
    id: root

    property var model: []

    property var columns: []

    property int rowHeight: 28
    property int indentation: 18
    property int cacheBuffer: 4000
    property int headerHeight: 30

    property bool showHeader: true
    property bool multiSelect: false
    property bool allowDisabledSelection: false
    property bool skipDisabledOrNonCheckableOnCheck: true

    property bool editable: false
    property bool editOnDoubleClick: true
    property bool allowDisabledEditing: false

    property var flickable: null

    readonly property var contentListView: listView

    property string filterText: ""
    property string filterRole: "text"

    property int __hoveredRow: -1

    property string selectedBackgroundColor: Style.selectedColor
    property string hoveredBackgroundColor: Style.hover
    property string normalTextColor: Style.textColor
    property string selectedTextColor: Style.textSelectedColor
    property string disabledTextColor: Style.inactiveTextColor
    property string headerBackgroundColor: Style.alternateBaseColor
    property string gridLineColor: Style.borderColor2

    property var currentIndex: null

    readonly property int visibleCount: visibleModel.count
    readonly property int selectedCount: __selectedCount
    readonly property bool editing: __editingKey.length > 0
    readonly property string editingKey: __editingKey
    readonly property int editingColumn: __editingColumn

    signal nodeClicked(var index)
    signal nodeDoubleClicked(var index)
    signal selectionChanged()
    signal checkStateChanged(var index, int state)

    signal headerClicked(var column)
    signal cellClicked(var index, var column)
    signal cellDoubleClicked(var index, var column)
    signal cellEditStarted(var index, var column)
    signal cellEdited(var index, var column, var value, var oldValue)
    signal cellEditCanceled(var index, var column)
    signal nodeTextEdited(var index, string text, string oldText)

    property var __nodes: ({})
    property var __rootKeys: []
    property var __expandedState: ({})
    property var __selectedKeys: ({})
    property var __visibleKeys: []
    property var __visibleRowsByKey: ({})
    property int __selectedCount: 0
    property string __lastAppliedFilter: ""

    property string __editingKey: ""
    property int __editingColumn: -1
    property var __editingOriginalValue: null
    property string __editingPendingText: ""

    // Called on commit with (index, column, value, oldValue). Return false to reject.
    property var writeBack: null

    height: (root.showHeader ? root.headerHeight + 1 : 0) + visibleModel.count * root.rowHeight

    Timer {
        id: filterDebounceTimer
        interval: 10
        repeat: false
        onTriggered: root.buildVisibleTree()
    }

    QtObject {
        id: defaultTreeColumn

        property string name: "name"
        property string title: "Name"
        property int width: 260
        property string display: "text"
        property string type: "string"
        property bool tree: true
        property bool editable: true
        property var options: []
        property int horizontalAlignment: Text.AlignLeft
    }

    ListModel {
        id: visibleModel
    }

    DoubleValidator {
        id: doubleValidator
        locale: "C"
    }

    Column {
        anchors.fill: parent
        spacing: 0

        Row {
            id: headerRow

            width: parent.width
            height: root.showHeader ? root.headerHeight : 0
            visible: root.showHeader

            Repeater {
                model: root.columnCount()

                delegate: Rectangle {
                    property var column: root.columnAt(index)

                    width: root.columnWidth(column)
                    height: parent.height

                    color: root.headerBackgroundColor

                    Text {
                        anchors.fill: parent
                        anchors.leftMargin: Style.marginM
                        anchors.rightMargin: Style.marginM

                        text: root.columnTitle(parent.column)
                        color: root.normalTextColor
                        font.bold: true
                        verticalAlignment: Text.AlignVCenter
                        horizontalAlignment: parent.column && parent.column.horizontalAlignment !== undefined ? parent.column.horizontalAlignment : Text.AlignLeft
                        elide: Text.ElideRight
                    }

                    MouseArea {
                        anchors.fill: parent

                        onClicked: {
                            root.headerClicked(parent.column)
                        }
                    }
                }
            }
        }

        ListView {
            id: listView

            width: parent.width
            height: parent.height - headerRow.height - (root.showHeader ? 1 : 0)

            clip: true
            cacheBuffer: root.cacheBuffer
            boundsBehavior: Flickable.StopAtBounds
            focus: true

            model: visibleModel
            delegate: treeRowDelegate

            Keys.onPressed: {
                if (event.key === Qt.Key_F2) {
                    if (root.currentIndex && root.currentIndex.key) {
                        root.beginEditCell(root.currentIndex.key, 0)
                        event.accepted = true
                    }
                    return
                }
                
                if (event.key === Qt.Key_Escape && root.editing) {
                    root.cancelEdit()
                    event.accepted = true
                    return
                }
                
                if ((event.key === Qt.Key_Return || event.key === Qt.Key_Enter) && root.editing) {
                    root.commitEdit(root.__editingPendingText)
                    event.accepted = true
                    return
                }
                
                if (event.key === Qt.Key_Down) {
                    root.__navigateDown()
                    event.accepted = true
                    return
                }
                
                if (event.key === Qt.Key_Up) {
                    root.__navigateUp()
                    event.accepted = true
                    return
                }
                
                if (event.key === Qt.Key_Right) {
                    if (root.currentIndex && root.currentIndex.key) {
                        var node = root.__nodes[root.currentIndex.key]
                        if (node && node.childrenKeys.length > 0 && !node.expanded)
                            root.expandNode(node.key)
                        else
                            root.__navigateDown()
                    }
                    event.accepted = true
                    return
                }
                
                if (event.key === Qt.Key_Left) {
                    if (root.currentIndex && root.currentIndex.key) {
                        var n = root.__nodes[root.currentIndex.key]
                        if (n && n.expanded && n.childrenKeys.length > 0)
                            root.collapseNode(n.key)
                        else if (n && n.parentKey !== "")
                            root.selectAndEnsureVisible(n.parentKey)
                    }
                    event.accepted = true
                    return
                }
                
                if (event.key === Qt.Key_Space) {
                    if (root.currentIndex && root.currentIndex.key) {
                        root.toggleCheckState(root.currentIndex.key)
                    }
                    event.accepted = true
                    return
                }
                
                if (event.key === Qt.Key_Home) {
                    if (root.__visibleKeys.length > 0)
                        root.selectAndEnsureVisible(root.__visibleKeys[0])
                    event.accepted = true
                    return
                }
                
                if (event.key === Qt.Key_End) {
                    if (root.__visibleKeys.length > 0)
                        root.selectAndEnsureVisible(root.__visibleKeys[root.__visibleKeys.length - 1])
                    event.accepted = true
                    return
                }
                
                if (event.key === Qt.Key_Asterisk) {
                    root.expandAll()
                    event.accepted = true
                    return
                }
            }
        }
    }

    Component {
        id: treeRowDelegate

        Item {
            id: delegateRoot

            property string nodeKey: model.key
            property int nodeLevel: model.level
            property bool nodeExpanded: model.expanded

            property bool nodeHasChildren: model.hasChildren
            property bool nodeSelected: model.selected
            property int nodeChecked: model.checked
            property bool nodeCheckable: model.checkable
            property bool nodeIsEnabled: model.nodeEnabled
            property string nodeText: model.text
            property int nodeVisibleRow: index

            width: listView.width
            height: root.rowHeight

            Rectangle {
                anchors.fill: parent
                color: delegateRoot.nodeSelected ? root.selectedBackgroundColor : delegateRoot.nodeVisibleRow === root.__hoveredRow ? root.hoveredBackgroundColor : "transparent"
            }

            MouseArea {
                id: rowMouseArea

                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton

                onContainsMouseChanged: {
                    root.__hoveredRow = containsMouse ? delegateRoot.nodeVisibleRow : -1
                }

                onClicked: {
                    listView.forceActiveFocus()
                    root.select(delegateRoot.nodeKey)

                    var node = root.__nodes[delegateRoot.nodeKey]
                    if (node) {
                        var indexObject = root.createIndex(node)
                        var col = root.__columnAtX(mouseX)
                        root.nodeClicked(indexObject)
                        root.cellClicked(indexObject, root.columnAt(col))
                    }
                }

                onDoubleClicked: {
                    listView.forceActiveFocus()

                    var node = root.__nodes[delegateRoot.nodeKey]
                    if (!node)
                        return

                    var col = root.__columnAtX(mouseX)
                    var indexObject = root.createIndex(node)

                    if (root.editable && root.editOnDoubleClick) {
                        root.beginEditCell(delegateRoot.nodeKey, col)
                    } else if (delegateRoot.nodeHasChildren) {
                        root.toggleExpanded(delegateRoot.nodeKey)
                    }

                    root.cellDoubleClicked(indexObject, root.columnAt(col))
                    root.nodeDoubleClicked(indexObject)
                }
            }

            Row {
                anchors.fill: parent
                spacing: 0

                Repeater {
                    model: root.columnCount()

                    delegate: Item {
                        id: cellRoot

                        property var column: root.columnAt(index)
                        property bool treeColumn: root.isTreeColumn(column, index)
                        property var value: root.cellValue(delegateRoot.nodeKey, column)
                        property string displayText: cellRoot.value !== undefined && cellRoot.value !== null ? String(cellRoot.value) : ""
                        property bool hasCustomEditor: cellRoot.column && cellRoot.column.editor !== undefined && cellRoot.column.editor !== null
                        property string editorType: root.editing && root.isEditingCell(delegateRoot.nodeKey, index) ? (cellRoot.hasCustomEditor ? "custom" : root.columnType(delegateRoot.nodeKey, column)) : ""

                        width: root.columnWidth(column)
                        height: delegateRoot.height

                        Row {
                            anchors.fill: parent
                            anchors.leftMargin: cellRoot.treeColumn ? delegateRoot.nodeLevel * root.indentation + Style.marginS : Style.marginM
                            anchors.rightMargin: Style.marginM
                            spacing: Style.spacingS

                            Item {
                                width: cellRoot.treeColumn ? 16 : 0
                                height: parent.height
                                visible: cellRoot.treeColumn

                                Button {
                                    anchors.centerIn: parent
                                    width: parent.width
                                    height: width
                                    visible: delegateRoot.nodeHasChildren
                                    iconSource: delegateRoot.nodeExpanded ? "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal) :
                                                                            "../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal)
                                    onClicked: {
                                        root.toggleExpanded(delegateRoot.nodeKey)
                                    }
                                    
                                    decorator: Component {
                                        ButtonDecorator {
                                            color: parent.hovered ? Style.buttonHoverColor : "transparent"
                                            border.width: 0
                                        }}
                                }
                            }

                            CheckBox {
                                visible: cellRoot.treeColumn && delegateRoot.nodeCheckable
                                anchors.verticalCenter: parent.verticalCenter
                                tristate: root.tristate
                                checkState: delegateRoot.nodeChecked

                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor

                                    onClicked: {
                                        root.toggleCheckState(delegateRoot.nodeKey)
                                    }
                                }
                            }

                            Text {
                                id: cellText
                                anchors.verticalCenter: parent.verticalCenter
                                width: Math.max(0, parent.width - x)
                                text: cellRoot.displayText
                                visible: !cellEditor.visible && !comboEditor.visible && !boolEditor.visible && !customEditorLoader.visible
                                color: !delegateRoot.nodeIsEnabled ? root.disabledTextColor : delegateRoot.nodeSelected ? root.selectedTextColor : root.normalTextColor
                                horizontalAlignment: cellRoot.column && cellRoot.column.horizontalAlignment !== undefined ? cellRoot.column.horizontalAlignment : Text.AlignLeft
                                elide: Text.ElideRight
                                verticalAlignment: Text.AlignVCenter
                            }

                            // --- TextInput editor (string, number) ---
                            TextInput {
                                id: cellEditor
                                anchors.verticalCenter: parent.verticalCenter
                                width: Math.max(0, parent.width - x)
                                visible: cellRoot.editorType === "string" || cellRoot.editorType === "number"
                                text: visible ? cellRoot.displayText : ""
                                color: root.normalTextColor
                                selectByMouse: true
                                selectedTextColor: Style.selectedColor
                                selectionColor: Style.imaginToolsAccentColor
                                clip: true
                                validator: cellRoot.editorType === "number" ? doubleValidator : null

                                onTextChanged: {
                                    if (visible)
                                        root.__editingPendingText = text
                                }

                                onVisibleChanged: {
                                    if (visible) {
                                        text = cellRoot.displayText
                                        root.__editingPendingText = text
                                        selectAll()
                                        forceActiveFocus()
                                    } else if (root.editing && root.isEditingCell(delegateRoot.nodeKey, index)) {
                                        // Delegate scrolled out of view — auto-commit
                                        root.commitEdit(root.__editingPendingText)
                                    }
                                }

                                Keys.onEscapePressed: {
                                    root.cancelEdit()
                                    listView.forceActiveFocus()
                                }

                                Keys.onReturnPressed: {
                                    root.commitEdit(cellEditor.text)
                                    listView.forceActiveFocus()
                                }

                                Keys.onEnterPressed: {
                                    root.commitEdit(cellEditor.text)
                                    listView.forceActiveFocus()
                                }

                                Keys.onTabPressed: {
                                    root.commitEdit(cellEditor.text)
                                    root.editNextCell()
                                }

                                Keys.onBacktabPressed: {
                                    root.commitEdit(cellEditor.text)
                                    root.editPreviousCell()
                                }
                            }

                            // --- CheckBox editor (bool, checkState) ---
                            CheckBox {
                                id: boolEditor
                                anchors.verticalCenter: parent.verticalCenter
                                visible: cellRoot.editorType === "bool" || cellRoot.editorType === "checkState"
                                tristate: cellRoot.editorType === "checkState"
                                checkState: {
                                    if (!visible) return Qt.Unchecked
                                    if (cellRoot.editorType === "checkState") return cellRoot.value !== undefined ? cellRoot.value : Qt.Unchecked
                                    return cellRoot.value === true ? Qt.Checked : Qt.Unchecked
                                }
                                text: {
                                    if (!visible) return ""
                                    if (cellRoot.editorType === "checkState") {
                                        if (cellRoot.value === Qt.Checked) return qsTr("Checked")
                                        if (cellRoot.value === Qt.PartiallyChecked) return qsTr("Partial")
                                        return qsTr("Unchecked")
                                    }
                                    return cellRoot.value ? qsTr("true") : qsTr("false")
                                }

                                onVisibleChanged: {
                                    if (visible)
                                        forceActiveFocus()
                                }

                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor

                                    onClicked: {
                                        if (cellRoot.editorType === "checkState") {
                                            var next = cellRoot.value === Qt.Checked ? Qt.Unchecked : Qt.Checked
                                            root.commitEdit(next)
                                        } else {
                                            root.commitEdit(!cellRoot.value)
                                        }
                                        listView.forceActiveFocus()
                                    }
                                }

                                Keys.onEscapePressed: {
                                    root.cancelEdit()
                                    listView.forceActiveFocus()
                                }

                                Keys.onSpacePressed: {
                                    if (cellRoot.editorType === "checkState") {
                                        var next = cellRoot.value === Qt.Checked ? Qt.Unchecked : Qt.Checked
                                        root.commitEdit(next)
                                    } else {
                                        root.commitEdit(!cellRoot.value)
                                    }
                                    listView.forceActiveFocus()
                                }

                                Keys.onReturnPressed: {
                                    if (cellRoot.editorType === "checkState") {
                                        var next = cellRoot.value === Qt.Checked ? Qt.Unchecked : Qt.Checked
                                        root.commitEdit(next)
                                    } else {
                                        root.commitEdit(!cellRoot.value)
                                    }
                                    listView.forceActiveFocus()
                                }
                            }

                            // --- Custom editor (column.editor) ---
                            Loader {
                                id: customEditorLoader
                                anchors.verticalCenter: parent.verticalCenter
                                width: Math.max(0, parent.width - x)
                                height: parent.height
                                visible: cellRoot.editorType === "custom"
                                active: visible
                                sourceComponent: visible && cellRoot.hasCustomEditor ? cellRoot.column.editor : null

                                onLoaded: {
                                    if (item) {
                                        item.value = cellRoot.value
                                        item.column = cellRoot.column
                                        item.nodeIndex = root.createIndex(root.__nodes[delegateRoot.nodeKey])
                                        item.commit.connect(function(val) {
                                            root.commitEdit(val)
                                            listView.forceActiveFocus()
                                        })
                                        item.cancel.connect(function() {
                                            root.cancelEdit()
                                            listView.forceActiveFocus()
                                        })
                                        item.forceActiveFocus()
                                    }
                                }

                                onVisibleChanged: {
                                    if (!visible && root.editing && root.isEditingCell(delegateRoot.nodeKey, index)) {
                                        // Scrolled out — cancel
                                        root.cancelEdit()
                                    }
                                }
                            }

                            // --- Combo editor (combo) ---
                            Item {
                                id: comboEditor
                                anchors.verticalCenter: parent.verticalCenter
                                width: Math.max(0, parent.width - x)
                                height: parent.height
                                visible: cellRoot.editorType === "combo"

                                onVisibleChanged: {
                                    if (visible) {
                                        comboPopup.open()
                                    }
                                }

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: parent.width - comboArrow.width - Style.marginS
                                    text: cellRoot.displayText
                                    color: root.normalTextColor
                                    elide: Text.ElideRight
                                    verticalAlignment: Text.AlignVCenter
                                }

                                Text {
                                    id: comboArrow
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.right: parent.right
                                    text: "\u25BE"
                                    color: root.normalTextColor
                                    font.pixelSize: 10
                                }

                                Keys.onEscapePressed: {
                                    comboPopup.close()
                                    root.cancelEdit()
                                    listView.forceActiveFocus()
                                }

                                Popup {
                                    id: comboPopup
                                    x: 0
                                    y: comboEditor.height
                                    width: comboEditor.width
                                    height: Math.min(comboOptionsList.contentHeight + 4, 200)
                                    padding: 2
                                    closePolicy: Enums.popupCloseOnEscape | Enums.popupCloseOnPressOutside

                                    onClosed: {
                                        if (root.editing && root.isEditingCell(delegateRoot.nodeKey, index))
                                            root.cancelEdit()
                                        listView.forceActiveFocus()
                                    }

                                    background: Rectangle {
                                        color: Style.baseColor
                                        border.color: root.gridLineColor
                                        border.width: 1
                                        radius: 2
                                    }

                                    contentItem: ListView {
                                        id: comboOptionsList
                                        clip: true
                                        model: cellRoot.column && cellRoot.column.options ? cellRoot.column.options : []
                                        currentIndex: cellRoot.column && cellRoot.column.options ? root.comboIndexOf(cellRoot.column.options, cellRoot.value) : -1

                                        delegate: Rectangle {
                                            width: comboOptionsList.width
                                            height: root.rowHeight
                                            color: optionMouse.containsMouse ? root.hoveredBackgroundColor : index === comboOptionsList.currentIndex ? root.selectedBackgroundColor : "transparent"

                                            Text {
                                                anchors.fill: parent
                                                anchors.leftMargin: Style.marginM
                                                anchors.rightMargin: Style.marginM
                                                text: modelData
                                                color: index === comboOptionsList.currentIndex ? root.selectedTextColor : root.normalTextColor
                                                verticalAlignment: Text.AlignVCenter
                                                elide: Text.ElideRight
                                            }

                                            MouseArea {
                                                id: optionMouse
                                                anchors.fill: parent
                                                hoverEnabled: true

                                                onClicked: {
                                                    comboPopup.close()
                                                    root.commitEdit(modelData)
                                                    listView.forceActiveFocus()
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    function rebuildTree() {
        __nodes = ({})
        __rootKeys = []
        __visibleKeys = []
        __visibleRowsByKey = ({})

        visibleModel.clear()

        if (!model) {
            cleanupSelection()
            return
        }

        var stack = []
        var rootCount = itemCount(model)

        for (var i = rootCount - 1; i >= 0; --i) {
            stack.push({ item: itemAt(model, i), row: i, parentKey: "", level: 0, path: String(i) })
        }

        while (stack.length > 0) {
            var entry = stack.pop()

            if (!entry.item)
                continue

            var item = entry.item
            var keyValue = normalizeKey(item.key, entry.path)
            var textValue = normalizeText(item.text, keyValue)
            var children = normalizeChildren(item.children)
            var childCount = itemCount(children)

            var node = {
                key: keyValue,
                text: textValue,
                checked: normalizeChecked(item.checked),
                checkable: normalizeBool(item.checkable, false),
                enabled: normalizeBool(item.enabled, true),
                expanded: __expandedState[keyValue] !== undefined ? __expandedState[keyValue] === true : normalizeBool(item.expanded, false),
                data: item.data !== undefined ? item.data : null,
                parentKey: entry.parentKey,
                childrenKeys: [],
                level: entry.level,
                row: entry.row,
                path: entry.path,
                selected: __selectedKeys[keyValue] === true,
                sourceItem: item
            }

            __nodes[keyValue] = node

            if (entry.parentKey === "")
                __rootKeys.push(keyValue)
            else if (__nodes[entry.parentKey])
                __nodes[entry.parentKey].childrenKeys.push(keyValue)

            for (var c = childCount - 1; c >= 0; --c) {
                stack.push({ item: itemAt(children, c), row: c, parentKey: keyValue, level: entry.level + 1, path: entry.path + "/" + c })
            }
        }

        cleanupSelection()
        buildVisibleTree()
    }

    function rebuildTreePreservingState() {
        captureExpandedState()
        rebuildTree()
    }

    function captureExpandedState() {
        for (var nodeKey in __nodes) {
            var node = __nodes[nodeKey]
            if (node)
                __expandedState[nodeKey] = node.expanded === true
        }
    }

    function buildVisibleTree() {
        var items = []
        __visibleKeys = []
        __visibleRowsByKey = ({})

        var ft = filterText.trim().toLowerCase()

        if (ft.length > 0) {
            var filterChanged = ft !== __lastAppliedFilter
            __lastAppliedFilter = ft
            buildFilteredVisible(ft, items, filterChanged)
        } else {
            __lastAppliedFilter = ""
            for (var i = 0; i < __rootKeys.length; ++i) {
                var rootNode = __nodes[__rootKeys[i]]
                if (rootNode)
                    appendVisibleBranch(rootNode, items)
            }
        }

        visibleModel.clear()
        if (items.length > 0)
            visibleModel.append(items)
    }

    function nodeMatchesFilter(node, ft) {
        var value = valueByPath(node, filterRole)
        return value !== undefined && value !== null && String(value).toLowerCase().indexOf(ft) >= 0
    }

    function buildFilteredVisible(ft, items, autoExpand) {
        var matchMap = {}

        let markMatches = function(nodeKey) {
            var node = __nodes[nodeKey]
            if (!node)
                return false

            var selfMatch = nodeMatchesFilter(node, ft)
            var childMatch = false

            for (var i = 0; i < node.childrenKeys.length; ++i) {
                if (markMatches(node.childrenKeys[i]))
                    childMatch = true
            }

            var matches = selfMatch || childMatch
            if (matches)
                matchMap[nodeKey] = true
            return matches
        }

        for (var r = 0; r < __rootKeys.length; ++r)
            markMatches(__rootKeys[r])

        // When the filter text just changed, auto-expand ancestors of matches
        if (autoExpand) {
            for (var ek in matchMap) {
                var en = __nodes[ek]
                if (!en || en.expanded) continue
                for (var ci = 0; ci < en.childrenKeys.length; ++ci) {
                    if (matchMap[en.childrenKeys[ci]]) {
                        en.expanded = true
                        __expandedState[ek] = true
                        break
                    }
                }
            }
        }

        let appendMarked = function(nodeKey) {
            if (!matchMap[nodeKey])
                return
            var node = __nodes[nodeKey]
            if (!node)
                return
            appendVisibleNode(node, items)
            if (node.expanded) {
                for (var i = 0; i < node.childrenKeys.length; ++i)
                    appendMarked(node.childrenKeys[i])
            }
        }

        for (var j = 0; j < __rootKeys.length; ++j)
            appendMarked(__rootKeys[j])
    }

    function appendVisibleBranch(node, items) {
        appendVisibleNode(node, items)

        if (!node.expanded)
            return

        for (var i = 0; i < node.childrenKeys.length; ++i) {
            var child = __nodes[node.childrenKeys[i]]
            if (child)
                appendVisibleBranch(child, items)
        }
    }

    function appendVisibleNode(node, items) {
        var row = __visibleKeys.length
        __visibleKeys.push(node.key)
        __visibleRowsByKey[node.key] = row
        items.push(toVisibleObject(node))
    }

    function toVisibleObject(node) {
        return {
            key: node.key,
            level: node.level,
            expanded: node.expanded,
            hasChildren: node.childrenKeys.length > 0,
            selected: node.selected,
            checked: node.checked,
            checkable: node.checkable,
            nodeEnabled: node.enabled,
            text: node.text
        }
    }

    function itemCount(items) {
        if (!items)
            return 0
        if (items.count !== undefined)
            return items.count
        if (items.length !== undefined)
            return items.length
        return 0
    }

    function itemAt(items, row) {
        if (!items)
            return null
        if (items.get)
            return items.get(row)
        return items[row]
    }

    function normalizeChildren(children) { return children || [] }
    function normalizeKey(value, path) { return value !== undefined && value !== null && String(value).length > 0 ? String(value) : "path/" + path }
    function normalizeText(value, fallback) { return value !== undefined && value !== null ? String(value) : fallback }
    function normalizeChecked(value) { return value !== undefined && value !== null ? value : Qt.Unchecked }
    function normalizeBool(value, fallback) { return value !== undefined && value !== null ? value : fallback }

    function writeBackNode(node) {
        if (!node || !node.sourceItem)
            return

        node.sourceItem.key = node.key
        node.sourceItem.text = node.text
        node.sourceItem.checked = node.checked
        node.sourceItem.checkable = node.checkable
        node.sourceItem.enabled = node.enabled
        node.sourceItem.expanded = node.expanded
        node.sourceItem.data = node.data
    }

    function createIndex(node) {
        return {
            key: node.key,
            text: node.text,
            checked: node.checked,
            checkable: node.checkable,
            enabled: node.enabled,
            expanded: node.expanded,
            row: node.row,
            path: node.path,
            level: node.level,
            parentKey: node.parentKey,
            data: node.data,
            item: node.sourceItem,

            value: function(columnOrPath) {
                if (typeof columnOrPath === "string")
                    return root.valueByPath(node, columnOrPath)
                return root.cellValue(node.key, columnOrPath)
            },

            setValue: function(columnOrPath, value) {
                if (typeof columnOrPath === "string") {
                    root.setValueByPath(node, columnOrPath, value)
                    root.writeBackNode(node)
                    root.syncVisibleNode(node.key)
                    return
                }
                root.setCellValue(node.key, columnOrPath, value)
            },

            edit: function(columnIndex) { root.beginEditCell(node.key, columnIndex) },
            setText: function(value) { root.setNodeText(node.key, value) },
            setChecked: function(state) { root.setCheckState(node.key, state) },
            setEnabled: function(value) { root.setNodeEnabled(node.key, value) },
            expand: function() { root.expandNode(node.key) },
            collapse: function() { root.collapseNode(node.key) },
            select: function() { root.select(node.key) }
        }
    }

    function columnCount() { return columns.length > 0 ? columns.length : 1 }
    function columnAt(columnIndex) { return columns.length > 0 ? columns[columnIndex] : defaultTreeColumn }

    function columnTitle(column) {
        if (!column)
            return ""
        if (column.title !== undefined && column.title !== null && String(column.title).length > 0)
            return String(column.title)
        if (column.name !== undefined && column.name !== null)
            return String(column.name)
        return ""
    }

    function columnWidth(column) {
        if (columns.length <= 1)
            return root.width
        return root.width / columns.length
    }

    function __columnAtX(mouseX) {
        var count = columnCount()
        if (count <= 1)
            return 0
        var w = root.width / count
        var col = Math.floor(mouseX / w)
        return Math.max(0, Math.min(col, count - 1))
    }

    function isTreeColumn(column, columnIndex) { return column && column.tree === true ? true : columnIndex === 0 && columns.length === 0 }
    function isColumnEditable(column) { return column && column.editable === true }

    function columnDisplayPath(column) {
        if (!column)
            return ""
        if (column.display !== undefined && column.display !== null && String(column.display).length > 0)
            return String(column.display)
        if (column.name !== undefined && column.name !== null && String(column.name).length > 0)
            return String(column.name)
        return ""
    }

    function columnType(keyValue, column) {
        if (!column)
            return "string"

        if (column.type !== undefined && column.type !== null) {
            var explicitType = String(column.type)
            if (explicitType.length > 0 && explicitType !== "auto")
                return explicitType
        }

        if (column.options !== undefined && column.options !== null && column.options.length > 0)
            return "combo"

        var path = columnDisplayPath(column)
        if (path === "checked")
            return "checkState"

        var value = cellValue(keyValue, column)
        if (typeof value === "boolean")
            return "bool"
        if (typeof value === "number")
            return "number"
        return "string"
    }

    function cellValue(keyValue, column) {
        var node = __nodes[keyValue]
        if (!node)
            return undefined
        return valueByPath(node, columnDisplayPath(column))
    }

    function setCellValue(keyValue, column, value) {
        var node = __nodes[keyValue]
        if (!node)
            return

        var path = columnDisplayPath(column)
        setValueByPath(node, path, value)

        if (path === "text")
            node.text = String(value)
        if (path === "checked")
            node.checked = value
        if (path === "enabled")
            node.enabled = value
        if (path === "checkable")
            node.checkable = value
        if (path === "expanded")
            node.expanded = value

        writeBackNode(node)
        syncVisibleNode(keyValue)

        if (path === "checked")
            syncVisibleAncestors(node)
    }

    function displayValue(value, column, keyValue) {
        if (value === undefined || value === null)
            return ""
        return String(value)
    }

    function valueByPath(object, path) {
        if (!object || !path || path.length <= 0)
            return undefined

        var parts = String(path).split(".")
        var current = object

        for (var i = 0; i < parts.length; ++i) {
            if (current === undefined || current === null)
                return undefined
            current = current[parts[i]]
        }

        return current
    }

    function setValueByPath(object, path, value) {
        if (!object || !path || path.length <= 0)
            return

        var parts = String(path).split(".")
        var current = object

        for (var i = 0; i < parts.length - 1; ++i) {
            var part = parts[i]
            if (current[part] === undefined || current[part] === null)
                current[part] = ({})
            current = current[part]
        }

        current[parts[parts.length - 1]] = value
    }

    function comboIndexOf(options, value) {
        if (!options)
            return -1

        for (var i = 0; i < options.length; ++i) {
            if (options[i] === value)
                return i
        }

        return -1
    }

    
    function toggleExpanded(keyValue) {
        var node = __nodes[keyValue]
        if (!node || node.childrenKeys.length <= 0)
            return
        if (node.expanded)
            collapseNode(keyValue)
        else
            expandNode(keyValue)
    }

    function expandNode(keyValue) {
        var node = __nodes[keyValue]
        if (!node || node.expanded)
            return

        node.expanded = true
        __expandedState[keyValue] = true
        if (node.sourceItem) node.sourceItem.expanded = true

        if (filterText.trim().length > 0) {
            buildVisibleTree()
            return
        }

        var row = visibleRowOf(keyValue)
        if (row < 0)
            return

        visibleModel.setProperty(row, "expanded", true)

        var inserted = []
        flattenExpanded(node, inserted)
        if (inserted.length > 0) {
            var insertIndex = row + 1
            var insertKeys = []
            var insertObjects = []
            for (var i = 0; i < inserted.length; ++i) {
                insertKeys.push(inserted[i].key)
                insertObjects.push(toVisibleObject(inserted[i]))
            }
            Array.prototype.splice.apply(__visibleKeys, [insertIndex, 0].concat(insertKeys))
            visibleModel.insert(insertIndex, insertObjects)
            rebuildVisibleRowsFrom(insertIndex)
        }
    }

    function collapseNode(keyValue) {
        var node = __nodes[keyValue]
        if (!node || !node.expanded)
            return

        node.expanded = false
        __expandedState[keyValue] = false
        if (node.sourceItem) node.sourceItem.expanded = false

        if (filterText.trim().length > 0) {
            buildVisibleTree()
            return
        }

        var row = visibleRowOf(keyValue)
        if (row < 0)
            return

        visibleModel.setProperty(row, "expanded", false)

        var removeCount = countVisibleDescendants(node)
        if (removeCount <= 0)
            return

        for (var i = 1; i <= removeCount; ++i)
            delete __visibleRowsByKey[__visibleKeys[row + i]]

        visibleModel.remove(row + 1, removeCount)
        __visibleKeys.splice(row + 1, removeCount)
        rebuildVisibleRowsFrom(row + 1)
    }

    function expandAll() {
        for (var nodeKey in __nodes) {
            var node = __nodes[nodeKey]
            if (node && node.childrenKeys.length > 0 && !node.expanded) {
                node.expanded = true
                __expandedState[nodeKey] = true
                if (node.sourceItem) node.sourceItem.expanded = true
            }
        }
        buildVisibleTree()
    }

    function collapseAll() {
        for (var nodeKey in __nodes) {
            var node = __nodes[nodeKey]
            if (node && node.expanded) {
                node.expanded = false
                __expandedState[nodeKey] = false
                if (node.sourceItem) node.sourceItem.expanded = false
            }
        }
        buildVisibleTree()
    }

    function flattenExpanded(node, out) {
        for (var i = 0; i < node.childrenKeys.length; ++i) {
            var child = __nodes[node.childrenKeys[i]]
            if (!child)
                continue
            out.push(child)
            if (child.expanded)
                flattenExpanded(child, out)
        }
    }

    function countVisibleDescendants(node) {
        var count = 0
        for (var i = 0; i < node.childrenKeys.length; ++i) {
            var child = __nodes[node.childrenKeys[i]]
            if (!child)
                continue
            ++count
            if (child.expanded)
                count += countVisibleDescendants(child)
        }
        return count
    }

    function select(keyValue) {
        var node = __nodes[keyValue]
        if (!node)
            return
        if (!allowDisabledSelection && !node.enabled)
            return

        var changedKeys = []
        if (!multiSelect)
            changedKeys = clearSelectionInternal(false)

        if (!node.selected) {
            node.selected = true
            __selectedKeys[keyValue] = true
            changedKeys.push(keyValue)
        }

        currentIndex = createIndex(node)
        syncVisibleNodes(changedKeys)
        updateSelectedCount()

        if (changedKeys.length > 0)
            selectionChanged()
    }

    function deselect(keyValue) {
        var node = __nodes[keyValue]
        if (!node || !node.selected)
            return

        node.selected = false
        delete __selectedKeys[keyValue]
        syncVisibleNode(keyValue)
        updateSelectedCount()
        selectionChanged()
    }

    function toggleSelection(keyValue) {
        var node = __nodes[keyValue]
        if (!node)
            return
        if (node.selected)
            deselect(keyValue)
        else
            select(keyValue)
    }

    function clearSelection() {
        var changedKeys = clearSelectionInternal(true)
        syncVisibleNodes(changedKeys)
        updateSelectedCount()
        if (changedKeys.length > 0)
            selectionChanged()
    }

    function clearSelectionInternal(clearCurrent) {
        var changedKeys = []
        for (var keyValue in __selectedKeys) {
            var node = __nodes[keyValue]
            if (node && node.selected) {
                node.selected = false
                changedKeys.push(keyValue)
            }
        }
        __selectedKeys = ({})
        if (clearCurrent)
            currentIndex = null
        return changedKeys
    }

    function selectedIndexes() {
        var result = []
        for (var keyValue in __selectedKeys) {
            var node = __nodes[keyValue]
            if (node)
                result.push(createIndex(node))
        }
        return result
    }

    function selectedKeys() { return Object.keys(__selectedKeys) }

    function cleanupSelection() {
        var cleaned = ({})
        for (var keyValue in __selectedKeys) {
            if (__nodes[keyValue]) {
                __nodes[keyValue].selected = true
                cleaned[keyValue] = true
            }
        }
        __selectedKeys = cleaned
        if (currentIndex && currentIndex.key && !__nodes[currentIndex.key])
            currentIndex = null
        updateSelectedCount()
    }

    function updateSelectedCount() { __selectedCount = Object.keys(__selectedKeys).length }

    function toggleCheckState(keyValue) {
        var node = __nodes[keyValue]
        if (!canChangeCheckState(node))
            return
        setCheckState(keyValue, node.checked === Qt.Checked ? Qt.Unchecked : Qt.Checked)
    }

    function setCheckState(keyValue, state) {
        var node = __nodes[keyValue]
        if (!canChangeCheckState(node))
            return

        propagateDown(node, state)
        propagateUp(node)
        syncVisibleSubtree(node)
        syncVisibleAncestors(node)
        checkStateChanged(createIndex(node), state)
        checkedItemsChanged()
    }

    function canChangeCheckState(node) {
        if (!node || !node.checkable)
            return false
        if (skipDisabledOrNonCheckableOnCheck && !node.enabled)
            return false
        return true
    }

    function shouldParticipateInCheck(node) {
        if (!node)
            return false
        if (!skipDisabledOrNonCheckableOnCheck)
            return true
        return node.enabled && node.checkable
    }

    function propagateDown(node, state) {
        if (!shouldParticipateInCheck(node))
            return
        node.checked = state
        writeBackNode(node)
        for (var i = 0; i < node.childrenKeys.length; ++i) {
            var child = __nodes[node.childrenKeys[i]]
            if (child)
                propagateDown(child, state)
        }
    }

    function propagateUp(node) {
        if (!node || node.parentKey === "")
            return

        var parentNode = __nodes[node.parentKey]
        if (!parentNode)
            return

        var hasParticipatingChild = false
        var allChecked = true
        var allUnchecked = true

        for (var i = 0; i < parentNode.childrenKeys.length; ++i) {
            var child = __nodes[parentNode.childrenKeys[i]]
            if (!shouldParticipateInCheck(child))
                continue
            hasParticipatingChild = true
            if (child.checked !== Qt.Checked)
                allChecked = false
            if (child.checked !== Qt.Unchecked)
                allUnchecked = false
        }

        if (hasParticipatingChild && shouldParticipateInCheck(parentNode)) {
            parentNode.checked = allChecked ? Qt.Checked : allUnchecked ? Qt.Unchecked : Qt.PartiallyChecked
            writeBackNode(parentNode)
        }

        propagateUp(parentNode)
    }

    function isEditingCell(keyValue, columnIndex) { return __editingKey === keyValue && __editingColumn === columnIndex }

    function beginEditCell(keyValue, columnIndex) {
        if (!editable)
            return

        var node = __nodes[keyValue]
        if (!node)
            return
        if (!allowDisabledEditing && !node.enabled)
            return
        if (columnIndex < 0 || columnIndex >= columnCount())
            return

        var column = columnAt(columnIndex)
        if (!isColumnEditable(column))
            return

        if (__editingKey.length > 0)
            commitEdit(__editingPendingText)

        select(keyValue)
        __editingKey = keyValue
        __editingColumn = columnIndex
        __editingOriginalValue = cellValue(keyValue, column)
        currentIndex = createIndex(node)
        cellEditStarted(createIndex(node), column)
    }

    function commitEdit(value) {
        if (__editingKey.length <= 0 || __editingColumn < 0)
            return

        var keyValue = __editingKey
        var column = columnAt(__editingColumn)
        var node = __nodes[keyValue]

        if (!node) {
            clearEditState()
            return
        }

        var oldValue = __editingOriginalValue
        if (value === undefined)
            value = __editingPendingText.length > 0 ? __editingPendingText : cellValue(keyValue, column)

        value = normalizeEditorValue(value, column, keyValue)
        clearEditState()

        if (value === oldValue)
            return

        var indexObject = createIndex(node)

        // writeBack callback can reject the edit by returning false
        if (root.writeBack) {
            var accepted = root.writeBack(indexObject, column, value, oldValue)
            if (accepted === false)
                return
        }

        setCellValue(keyValue, column, value)
        node = __nodes[keyValue]
        if (!node)
            return

        indexObject = createIndex(node)
        currentIndex = indexObject
        cellEdited(indexObject, column, value, oldValue)

        if (columnDisplayPath(column) === "text")
            nodeTextEdited(indexObject, String(value), String(oldValue))
    }

    function cancelEdit() {
        if (__editingKey.length <= 0)
            return

        var node = __nodes[__editingKey]
        var column = __editingColumn >= 0 && __editingColumn < columnCount() ? columnAt(__editingColumn) : null
        clearEditState()

        if (node && column)
            cellEditCanceled(createIndex(node), column)
    }

    function clearEditState() {
        __editingKey = ""
        __editingColumn = -1
        __editingOriginalValue = null
        __editingPendingText = ""
    }

    function normalizeEditorValue(value, column, keyValue) {
        var typeName = columnType(keyValue || "", column)
        if (typeName === "number")
            return Number(value)
        if (typeName === "bool")
            return value === true
        if (typeName === "string")
            return String(value)
        return value
    }

    function editCurrent(columnIndex) {
        if (!currentIndex || !currentIndex.key)
            return
        if (columnIndex === undefined || columnIndex === null)
            columnIndex = 0
        beginEditCell(currentIndex.key, columnIndex)
    }

    function editNextCell() {
        if (!currentIndex || !currentIndex.key)
            return
        var colCount = columnCount()
        var nextCol = __editingColumn >= 0 ? __editingColumn + 1 : 0
        var currentKey = currentIndex.key

        // Try next editable column in current row
        for (var c = nextCol; c < colCount; ++c) {
            if (isColumnEditable(columnAt(c))) {
                beginEditCell(currentKey, c)
                return
            }
        }

        // Move to next visible row, first editable column
        var row = visibleRowOf(currentKey)
        if (row >= 0 && row < __visibleKeys.length - 1) {
            var nextKey = __visibleKeys[row + 1]
            var nextNode = __nodes[nextKey]
            if (nextNode && (allowDisabledEditing || nextNode.enabled)) {
                for (var c2 = 0; c2 < colCount; ++c2) {
                    if (isColumnEditable(columnAt(c2))) {
                        beginEditCell(nextKey, c2)
                        return
                    }
                }
            }
        }
    }

    function editPreviousCell() {
        if (!currentIndex || !currentIndex.key)
            return
        var colCount = columnCount()
        var prevCol = __editingColumn >= 0 ? __editingColumn - 1 : colCount - 1
        var currentKey = currentIndex.key

        // Try previous editable column in current row
        for (var c = prevCol; c >= 0; --c) {
            if (isColumnEditable(columnAt(c))) {
                beginEditCell(currentKey, c)
                return
            }
        }

        // Move to previous visible row, last editable column
        var row = visibleRowOf(currentKey)
        if (row > 0) {
            var prevKey = __visibleKeys[row - 1]
            var prevNode = __nodes[prevKey]
            if (prevNode && (allowDisabledEditing || prevNode.enabled)) {
                for (var c2 = colCount - 1; c2 >= 0; --c2) {
                    if (isColumnEditable(columnAt(c2))) {
                        beginEditCell(prevKey, c2)
                        return
                    }
                }
            }
        }
    }

    function setNodeText(keyValue, value) {
        var node = __nodes[keyValue]
        if (!node)
            return
        node.text = String(value)
        writeBackNode(node)
        syncVisibleNode(keyValue)
        if (currentIndex && currentIndex.key === keyValue)
            currentIndex = createIndex(node)
    }

    function setNodeEnabled(keyValue, value) {
        var node = __nodes[keyValue]
        if (!node)
            return
        node.enabled = value
        writeBackNode(node)
        syncVisibleNode(keyValue)
    }

    function syncVisibleNode(keyValue) {
        var row = visibleRowOf(keyValue)
        if (row < 0)
            return
        var node = __nodes[keyValue]
        if (!node)
            return
        visibleModel.set(row, toVisibleObject(node))
    }

    function syncVisibleNodes(keys) {
        for (var i = 0; i < keys.length; ++i)
            syncVisibleNode(keys[i])
    }

    function syncVisibleSubtree(node) {
        if (!node)
            return
        syncVisibleNode(node.key)
        for (var i = 0; i < node.childrenKeys.length; ++i) {
            var child = __nodes[node.childrenKeys[i]]
            if (child)
                syncVisibleSubtree(child)
        }
    }

    function syncVisibleAncestors(node) {
        var current = node
        while (current && current.parentKey !== "") {
            current = __nodes[current.parentKey]
            if (current)
                syncVisibleNode(current.key)
        }
    }

    function rebuildVisibleRowsFrom(startRow) {
        if (startRow < 0)
            startRow = 0
        for (var i = startRow; i < __visibleKeys.length; ++i)
            __visibleRowsByKey[__visibleKeys[i]] = i
    }

    function visibleRowOf(keyValue) {
        var row = __visibleRowsByKey[keyValue]
        return row === undefined ? -1 : row
    }

    function nodeForKey(keyValue) { return __nodes[keyValue] || null }
    function indexForKey(keyValue) { var node = __nodes[keyValue]; return node ? createIndex(node) : null }
    function keyForIndex(indexObject) { return indexObject && indexObject.key ? indexObject.key : "" }
    function isExpanded(keyValue) { var node = __nodes[keyValue]; return node ? node.expanded : false }
    function isSelected(keyValue) { var node = __nodes[keyValue]; return node ? node.selected : false }

    function ensureVisible(keyValue) {
        expandParents(keyValue)
        var row = visibleRowOf(keyValue)
        if (row < 0)
            return

        listView.positionViewAtIndex(row, ListView.Contain)

        // Scroll external flickable if present
        if (root.flickable) {
            var headerOffset = root.showHeader ? root.headerHeight : 0
            var itemY = root.y + headerOffset + row * root.rowHeight
            var itemBottom = itemY + root.rowHeight
            var viewTop = root.flickable.contentY
            var viewBottom = viewTop + root.flickable.height

            if (itemBottom > viewBottom)
                root.flickable.contentY = itemBottom - root.flickable.height
            else if (itemY < viewTop)
                root.flickable.contentY = itemY
        }
    }

    function expandParents(keyValue) {
        var node = __nodes[keyValue]
        if (!node)
            return

        var parents = []
        var current = node

        while (current && current.parentKey !== "") {
            current = __nodes[current.parentKey]
            if (current)
                parents.unshift(current.key)
        }

        for (var i = 0; i < parents.length; ++i) {
            var parentNode = __nodes[parents[i]]
            if (parentNode && !parentNode.expanded)
                expandNode(parentNode.key)
        }
    }

    function selectAndEnsureVisible(keyValue) {
        ensureVisible(keyValue)
        select(keyValue)
    }

    function scrollToCurrent() {
        if (!currentIndex || !currentIndex.key)
            return
        ensureVisible(currentIndex.key)
    }

    function rebuild() {
        rebuildTreePreservingState()
    }

    // --- Convenience API for checkable trees ---

    signal checkedItemsChanged()

    property bool tristate: false

    function checkAll() {
        for (var nodeKey in __nodes) {
            var node = __nodes[nodeKey]
            if (node && node.checkable && node.enabled) {
                node.checked = Qt.Checked
                writeBackNode(node)
            }
        }
        for (var i = 0; i < __visibleKeys.length; ++i)
            syncVisibleNode(__visibleKeys[i])
        checkedItemsChanged()
    }

    function uncheckAll() {
        for (var nodeKey in __nodes) {
            var node = __nodes[nodeKey]
            if (node && node.checkable) {
                node.checked = Qt.Unchecked
                writeBackNode(node)
            }
        }
        for (var i = 0; i < __visibleKeys.length; ++i)
            syncVisibleNode(__visibleKeys[i])
        checkedItemsChanged()
    }

    function checkItem(key) {
        setCheckState(key, Qt.Checked)
    }

    function uncheckItem(key) {
        setCheckState(key, Qt.Unchecked)
    }

    function getCheckedKeys() {
        var result = []
        for (var nodeKey in __nodes) {
            var node = __nodes[nodeKey]
            if (node && node.checked === Qt.Checked)
                result.push(nodeKey)
        }
        return result
    }

    function getCheckedNodes() {
        var result = []
        for (var nodeKey in __nodes) {
            var node = __nodes[nodeKey]
            if (node && node.checked === Qt.Checked)
                result.push(createIndex(node))
        }
        return result
    }

    function allNodes() {
        var result = []
        for (var nodeKey in __nodes) {
            var node = __nodes[nodeKey]
            if (node)
                result.push(createIndex(node))
        }
        return result
    }
    
    function __navigateDown() {
        if (__visibleKeys.length === 0)
            return
        if (!currentIndex || !currentIndex.key) {
            selectAndEnsureVisible(__visibleKeys[0])
            return
        }
        var row = visibleRowOf(currentIndex.key)
        if (row < 0 || row >= __visibleKeys.length - 1)
            return
        selectAndEnsureVisible(__visibleKeys[row + 1])
    }

    function __navigateUp() {
        if (__visibleKeys.length === 0)
            return
        if (!currentIndex || !currentIndex.key) {
            selectAndEnsureVisible(__visibleKeys[0])
            return
        }
        var row = visibleRowOf(currentIndex.key)
        if (row <= 0)
            return
        selectAndEnsureVisible(__visibleKeys[row - 1])
    }

    onModelChanged: {
        rebuildTree()
    }

    onFilterTextChanged: {
        filterDebounceTimer.restart()
    }

    Component.onCompleted: {
        rebuildTree()
    }
}
