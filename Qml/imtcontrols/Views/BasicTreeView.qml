import QtQuick
import QtQuick.Controls
import Acf 1.0
import imtcontrols 1.0

/*!
    \qmltype BasicTreeView
    \inqmlmodule imtcontrols

    Universal tree component with multi-column display, inline cell editing,
    tri-state checkboxes, text filtering and keyboard navigation.

    Full documentation, examples and the editor contract live in
    \c Docs/Qml/BasicTreeView.md. Keep this file lean — only the public API
    is briefly summarised below; for anything else consult the markdown.

    Public properties (defaults in brackets):
        model[[]], columns[[]], rowHeight[28], indentation[18], cacheBuffer[4000],
        headerHeight[30], showHeader[true], multiSelect[false], editable[false],
        editOnDoubleClick[true], allowDisabledEditing[false],
        allowDisabledSelection[false], skipDisabledOrNonCheckableOnCheck[true],
        tristate[false], filterText[""], filterRole["text"],
        filterDebounceMs[150], flickable[null], writeBack[null],
        editingFocusOutPolicy["commit"].

    Public read-only:
        currentIndex, editingIndex, visibleCount, selectedCount,
        editing, editingKey, editingColumn, contentListView.

    Public signals:
        nodeClicked, nodeDoubleClicked, cellClicked, cellDoubleClicked,
        selectionChanged, checkStateChanged, checkedItemsChanged,
        headerClicked, cellEditStarted, cellEdited, cellEditCanceled,
        cellEditCommitFailed, nodeTextEdited, editingChanged.

    Public functions:
        rebuild(), expandAll(), collapseAll(), checkAll(), uncheckAll(),
        getCheckedKeys(), getCheckedNodes(), allNodes(), indexForKey(key),
        nodeForKey(key), isExpanded(key), isSelected(key),
        select(key), deselect(key), clearSelection(), selectedKeys(),
        selectedIndexes(), toggleSelection(key), selectAndEnsureVisible(key),
        ensureVisible(key), scrollToCurrent(),
        expandNode(key), collapseNode(key), toggleExpanded(key), expandParents(key),
        setCheckState(key, state), toggleCheckState(key),
        checkItem(key), uncheckItem(key),
        setNodeText(key, value), setNodeEnabled(key, value),
        setCellValue(key, column, value), cellValue(key, column),
        editCurrent(col), startEdit(key, col), beginEditCell(key, col),
        commitCurrentEdit(), cancelCurrentEdit(),
        commitEdit(value), cancelEdit(),
        editNextCell(), editPreviousCell(),
        revertCell(index, column),
        columnAt(i), columnCount(), columnWidth(column), columnTitle(column),
        columnType(key, column), columnDisplayPath(column),
        isTreeColumn(column, i), isColumnEditable(column),
        valueByPath(obj, path), setValueByPath(obj, path, value),
        comboIndexOf(options, value).
*/

Item {
    id: root

    // ─── Public configuration ──────────────────────────────────────────────

    property var    model: []
    property var    columns: []

    property int    rowHeight: 28
    property int    indentation: 18
    property int    cacheBuffer: 4000
    property int    headerHeight: 30

    property bool   showHeader: true
    property bool   multiSelect: false
    property bool   allowDisabledSelection: false
    property bool   skipDisabledOrNonCheckableOnCheck: true

    property bool   editable: false
    property bool   editOnDoubleClick: true
    property bool   allowDisabledEditing: false

    property bool   tristate: false

    property string filterText: ""
    property string filterRole: "text"
    property int    filterDebounceMs: 150

    property var    flickable: null

    // Synchronous: function(index, column, value, oldValue) -> bool.
    // Return false to reject the edit.
    property var    writeBack: null

    // What to do when the tree loses active focus while editing:
    // "commit" (default), "cancel", "keep".
    property string editingFocusOutPolicy: "commit"

    // ─── Theming ───────────────────────────────────────────────────────────

    property string selectedBackgroundColor: Style.selectedColor
    property string hoveredBackgroundColor:  Style.hover
    property string normalTextColor:         Style.textColor
    property string selectedTextColor:       Style.textSelectedColor
    property string disabledTextColor:       Style.inactiveTextColor
    property string headerBackgroundColor:   Style.alternateBaseColor
    property string gridLineColor:           Style.borderColor2
    property string editorErrorColor:        "#d04848"

    // ─── Public readonly state ─────────────────────────────────────────────

    readonly property var  contentListView: listView
    readonly property int  visibleCount: visibleModel.count
    readonly property int  selectedCount: __selectedCount
    readonly property bool editing: __editingKey.length > 0
    readonly property string editingKey: __editingKey
    readonly property int  editingColumn: __editingColumn

    property var currentIndex: null
    property var editingIndex: null

    // ─── Signals ───────────────────────────────────────────────────────────

    signal nodeClicked(var index)
    signal nodeDoubleClicked(var index)
    signal selectionChanged()
    signal checkStateChanged(var index, int state)
    signal checkedItemsChanged()

    signal headerClicked(var column)
    signal cellClicked(var index, var column)
    signal cellDoubleClicked(var index, var column)

    signal cellEditStarted(var index, var column)
    signal cellEdited(var index, var column, var value, var oldValue)
    signal cellEditCanceled(var index, var column)
    signal cellEditCommitFailed(var index, var column, var value, string reason)
    signal nodeTextEdited(var index, string text, string oldText)
    signal editingChanged()

    // ─── Internal state (prefix __) ────────────────────────────────────────

    // Node store: key -> node {key,text,checked,checkable,enabled,expanded,
    //   data,parentKey,childrenKeys[],level,row,path,selected,sourceItem}.
    property var    __nodes: ({})
    property var    __rootKeys: []
    // Only stores explicitly-expanded keys (true/false) for nodes that the
    // user toggled or that initial model marked. Pruned in cleanupExpanded().
    property var    __expandedState: ({})
    property var    __selectedKeys: ({})
    property var    __visibleKeys: []
    property var    __visibleRowsByKey: ({})
    property int    __selectedCount: 0
    property string __lastAppliedFilter: ""

    // Edit state. Pending value is var (not string) to also cover bool/combo/custom.
    property string __editingKey: ""
    property int    __editingColumn: -1
    property var    __editingOriginalValue: null
    property var    __editingPendingValue: null
    property bool   __editingIsDirty: false
    property string __editingError: ""

    // Column layout cache (recomputed on width/columns changes)
    property var    __columnWidths: []

    // ─── Sizing ────────────────────────────────────────────────────────────

    height: (root.showHeader ? root.headerHeight + 1 : 0) + visibleModel.count * root.rowHeight

    // ─── Default column ────────────────────────────────────────────────────

    readonly property var __defaultColumn: ({
        name: "name",
        title: "Name",
        display: "text",
        type: "string",
        tree: true,
        editable: true,
        options: [],
        horizontalAlignment: Text.AlignLeft,
        width: 0,
        minWidth: 0
    })

    // ─── Helpers ───────────────────────────────────────────────────────────

    QtObject {
        id: priv
        readonly property string ckPathText: "text"
        readonly property string ckPathChecked: "checked"
        readonly property string ckPathEnabled: "enabled"
        readonly property string ckPathCheckable: "checkable"
        readonly property string ckPathExpanded: "expanded"
    }

    // Debounce filterText rebuilds so fast typing doesn't trigger a full
    // tree pass on every keystroke (interval is configurable via filterDebounceMs).
    Timer {
        id: filterDebounceTimer
        interval: root.filterDebounceMs
        repeat: false
        onTriggered: root.buildVisibleTree()
    }

    DoubleValidator {
        id: doubleValidator
        locale: "C"
    }

    ListModel {
        id: visibleModel
    }

    // ─── Visual structure ──────────────────────────────────────────────────

    Column {
        anchors.fill: parent
        spacing: 0

        // Header row
        Row {
            id: headerRow

            width: parent.width
            height: root.showHeader ? root.headerHeight : 0
            visible: root.showHeader

            Repeater {
                model: root.columnCount()

                delegate: Rectangle {
                    property var column: root.columnAt(index)

                    width: root.__columnWidths[index] !== undefined ? root.__columnWidths[index] : 0
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
                        horizontalAlignment: parent.column && parent.column.horizontalAlignment !== undefined
                            ? parent.column.horizontalAlignment : Text.AlignLeft
                        elide: Text.ElideRight
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: root.headerClicked(parent.column)
                    }
                }
            }
        }

        // Data area
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

            Keys.onPressed: function (event) {
                if (root.__handleKey(event))
                    event.accepted = true
            }
        }
    }

    // ─── Row delegate ──────────────────────────────────────────────────────

    Component {
        id: treeRowDelegate

        Item {
            id: delegateRoot

            property string nodeKey: model.key
            property int    nodeLevel: model.level
            property bool   nodeExpanded: model.expanded
            property bool   nodeHasChildren: model.hasChildren
            property bool   nodeSelected: model.selected
            property int    nodeChecked: model.checked
            property bool   nodeCheckable: model.checkable
            property bool   nodeIsEnabled: model.nodeEnabled
            property string nodeText: model.text
            property int    nodeVisibleRow: index
            property bool   rowHovered: false

            width: listView.width
            height: root.rowHeight

            Rectangle {
                anchors.fill: parent
                color: delegateRoot.nodeSelected ? root.selectedBackgroundColor
                     : delegateRoot.rowHovered  ? root.hoveredBackgroundColor
                     : "transparent"
            }

            MouseArea {
                id: rowMouseArea
                anchors.fill: parent
                hoverEnabled: true
                acceptedButtons: Qt.LeftButton

                onContainsMouseChanged: delegateRoot.rowHovered = containsMouse

                onClicked: function (mouse) {
                    // If we're editing another row, honour focus-out policy
                    if (root.editing && root.__editingKey !== delegateRoot.nodeKey)
                        root.__autoLeaveEdit()

                    listView.forceActiveFocus()
                    root.select(delegateRoot.nodeKey)

                    var node = root.__nodes[delegateRoot.nodeKey]
                    if (node) {
                        var col = root.__columnAtX(mouse.x)
                        var idx = root.createIndex(node)
                        root.nodeClicked(idx)
                        root.cellClicked(idx, root.columnAt(col))
                    }
                }

                onDoubleClicked: function (mouse) {
                    listView.forceActiveFocus()

                    var node = root.__nodes[delegateRoot.nodeKey]
                    if (!node)
                        return

                    var col = root.__columnAtX(mouse.x)
                    var column = root.columnAt(col)
                    var idx = root.createIndex(node)

                    // Tree column on a node with children → toggle expansion;
                    // otherwise — try to enter edit mode.
                    var clickedTree = root.isTreeColumn(column, col)
                    if (clickedTree && delegateRoot.nodeHasChildren && !root.editable) {
                        root.toggleExpanded(delegateRoot.nodeKey)
                    } else if (root.editable && root.editOnDoubleClick) {
                        if (!root.beginEditCell(delegateRoot.nodeKey, col) && delegateRoot.nodeHasChildren && clickedTree)
                            root.toggleExpanded(delegateRoot.nodeKey)
                    } else if (delegateRoot.nodeHasChildren && clickedTree) {
                        root.toggleExpanded(delegateRoot.nodeKey)
                    }

                    root.cellDoubleClicked(idx, column)
                    root.nodeDoubleClicked(idx)
                }
            }

            Row {
                anchors.fill: parent
                spacing: 0

                Repeater {
                    model: root.columnCount()

                    delegate: Item {
                        id: cellRoot

                        property var    column: root.columnAt(index)
                        property bool   treeColumn: root.isTreeColumn(column, index)
                        property var    cellRawValue: root.cellValue(delegateRoot.nodeKey, column)
                        property string displayText: cellRawValue !== undefined && cellRawValue !== null ? String(cellRawValue) : ""
                        property bool   isEditingHere: root.editing && root.__editingKey === delegateRoot.nodeKey && root.__editingColumn === index
                        property string editorType: isEditingHere ? root.__pickEditorType(column, cellRawValue, delegateRoot.nodeKey) : ""

                        width: root.__columnWidths[index] !== undefined ? root.__columnWidths[index] : 0
                        height: delegateRoot.height

                        Row {
                            id: cellContent
                            anchors.fill: parent
                            anchors.leftMargin: cellRoot.treeColumn ? delegateRoot.nodeLevel * root.indentation + Style.marginS : Style.marginM
                            anchors.rightMargin: Style.marginM
                            spacing: Style.spacingS

                            // Expand / collapse arrow (only in tree column)
                            Item {
                                width: cellRoot.treeColumn ? 16 : 0
                                height: parent.height
                                visible: cellRoot.treeColumn

                                Button {
                                    anchors.centerIn: parent
                                    width: parent.width
                                    height: width
                                    visible: delegateRoot.nodeHasChildren
                                    iconSource: delegateRoot.nodeExpanded
                                        ? "../../../" + Style.getIconPath("Icons/Down",  Icon.State.On, Icon.Mode.Normal)
                                        : "../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal)
                                    onClicked: root.toggleExpanded(delegateRoot.nodeKey)

                                    decorator: Component {
                                        ButtonDecorator {
                                            color: parent.hovered ? Style.buttonHoverColor : "transparent"
                                            border.width: 0
                                        }
                                    }
                                }
                            }

                            // Tri-state checkbox (tree column, checkable nodes)
                            CheckBox {
                                visible: cellRoot.treeColumn && delegateRoot.nodeCheckable
                                anchors.verticalCenter: parent.verticalCenter
                                tristate: root.tristate
                                checkState: delegateRoot.nodeChecked

                                MouseArea {
                                    anchors.fill: parent
                                    cursorShape: Qt.PointingHandCursor
                                    onClicked: root.toggleCheckState(delegateRoot.nodeKey)
                                }
                            }

                            // Cell text (only when not editing this cell)
                            Text {
                                anchors.verticalCenter: parent.verticalCenter
                                width: Math.max(0, parent.width - x)
                                text: cellRoot.displayText
                                visible: !cellRoot.isEditingHere
                                color: !delegateRoot.nodeIsEnabled ? root.disabledTextColor
                                     : delegateRoot.nodeSelected   ? root.selectedTextColor
                                                                   : root.normalTextColor
                                horizontalAlignment: cellRoot.column && cellRoot.column.horizontalAlignment !== undefined
                                    ? cellRoot.column.horizontalAlignment : Text.AlignLeft
                                elide: Text.ElideRight
                                verticalAlignment: Text.AlignVCenter
                            }

                            // Single editor Loader per row (active only for the edited cell)
                            Loader {
                                id: editorLoader
                                anchors.verticalCenter: parent.verticalCenter
                                width: Math.max(0, parent.width - x)
                                height: parent.height
                                visible: cellRoot.isEditingHere
                                active: cellRoot.isEditingHere

                                sourceComponent: {
                                    switch (cellRoot.editorType) {
                                    case "string":     return textEditorComponent
                                    case "number":     return textEditorComponent
                                    case "bool":       return boolEditorComponent
                                    case "checkState": return boolEditorComponent
                                    case "combo":      return comboEditorComponent
                                    case "custom":     return cellRoot.column.editor
                                    }
                                    return null
                                }

                                property var __commitHandler: null
                                property var __cancelHandler: null

                                onLoaded: {
                                    if (!item)
                                        return

                                    item.value = cellRoot.cellRawValue
                                    if (item.column !== undefined)
                                        item.column = cellRoot.column
                                    if (item.nodeIndex !== undefined)
                                        item.nodeIndex = root.createIndex(root.__nodes[delegateRoot.nodeKey])
                                    if (item.editorType !== undefined)
                                        item.editorType = cellRoot.editorType

                                    if (item.commit) {
                                        __commitHandler = function (val) {
                                            root.commitEdit(val)
                                            listView.forceActiveFocus()
                                        }
                                        item.commit.connect(__commitHandler)
                                    }
                                    if (item.cancel) {
                                        __cancelHandler = function () {
                                            root.cancelEdit()
                                            listView.forceActiveFocus()
                                        }
                                        item.cancel.connect(__cancelHandler)
                                    }

                                    if (item.autoFocus === undefined || item.autoFocus === true)
                                        item.forceActiveFocus()
                                }

                                onVisibleChanged: {
                                    if (!visible && root.editing && root.__editingKey === delegateRoot.nodeKey && root.__editingColumn === index) {
                                        // Delegate scrolled out — cancel to avoid losing typed text
                                        root.cancelEdit()
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // ─── Built-in editor components (instantiated once per row when editing) ──

    Component {
        id: textEditorComponent

        Rectangle {
            id: textEditorRoot
            property var    value
            property var    column
            property var    nodeIndex
            property string editorType: "string"
            property bool   autoFocus: true

            signal commit(var value)
            signal cancel()

            color: "transparent"
            border.width: root.__editingError.length > 0 ? 1 : 0
            border.color: root.editorErrorColor
            radius: 2

            TextInput {
                id: textEditor
                anchors.fill: parent
                anchors.leftMargin: 2
                anchors.rightMargin: 2
                verticalAlignment: TextInput.AlignVCenter
                color: root.normalTextColor
                selectByMouse: true
                selectedTextColor: Style.selectedColor
                selectionColor: Style.imaginToolsAccentColor
                clip: true
                text: textEditorRoot.value !== undefined && textEditorRoot.value !== null ? String(textEditorRoot.value) : ""
                validator: textEditorRoot.editorType === "number" ? doubleValidator
                          : (textEditorRoot.column && textEditorRoot.column.validator && textEditorRoot.column.validator.validate)
                              ? textEditorRoot.column.validator : null

                Component.onCompleted: {
                    root.__editingPendingValue = text
                    if (textEditorRoot.autoFocus) {
                        selectAll()
                        forceActiveFocus()
                    }
                }

                onTextChanged: root.__editingPendingValue = text

                Keys.onEscapePressed: textEditorRoot.cancel()
                Keys.onReturnPressed: textEditorRoot.commit(text)
                Keys.onEnterPressed:  textEditorRoot.commit(text)
                Keys.onTabPressed: {
                    var c = root.__editingColumn
                    textEditorRoot.commit(text)
                    root.editNextCell(c)
                }
                Keys.onBacktabPressed: {
                    var c = root.__editingColumn
                    textEditorRoot.commit(text)
                    root.editPreviousCell(c)
                }
            }

            Text {
                anchors.left: parent.left
                anchors.bottom: parent.top
                visible: root.__editingError.length > 0
                text: root.__editingError
                color: root.editorErrorColor
                font.pixelSize: 10
            }
        }
    }

    Component {
        id: boolEditorComponent

        CheckBox {
            id: boolEditor
            property var    value
            property var    column
            property var    nodeIndex
            property string editorType: "bool"
            property bool   autoFocus: true

            signal commit(var value)
            signal cancel()

            tristate: editorType === "checkState"
            checkState: {
                if (editorType === "checkState")
                    return value !== undefined && value !== null ? value : Qt.Unchecked
                return value === true ? Qt.Checked : Qt.Unchecked
            }
            text: {
                if (editorType === "checkState") {
                    if (value === Qt.Checked)          return qsTr("Checked")
                    if (value === Qt.PartiallyChecked) return qsTr("Partial")
                    return qsTr("Unchecked")
                }
                return value ? qsTr("true") : qsTr("false")
            }

            function __toggle() {
                if (editorType === "checkState") {
                    var next = value === Qt.Checked ? Qt.Unchecked : Qt.Checked
                    commit(next)
                } else {
                    commit(!value)
                }
            }

            Component.onCompleted: if (autoFocus) forceActiveFocus()

            MouseArea {
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: boolEditor.__toggle()
            }

            Keys.onEscapePressed: boolEditor.cancel()
            Keys.onSpacePressed:  boolEditor.__toggle()
            Keys.onReturnPressed: boolEditor.__toggle()
            Keys.onEnterPressed:  boolEditor.__toggle()
        }
    }

    Component {
        id: comboEditorComponent

        Item {
            id: comboEditor
            property var    value
            property var    column
            property var    nodeIndex
            property string editorType: "combo"
            property bool   autoFocus: true

            signal commit(var value)
            signal cancel()

            Text {
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width - comboArrow.width - Style.marginS
                text: root.__comboLabel(comboEditor.column, comboEditor.value)
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

            Component.onCompleted: {
                if (autoFocus) forceActiveFocus()
                comboPopup.open()
            }

            Keys.onEscapePressed: { comboPopup.close(); comboEditor.cancel() }
            Keys.onReturnPressed: {
                if (comboList.currentIndex >= 0) {
                    var opt = comboList.model[comboList.currentIndex]
                    comboPopup.close()
                    comboEditor.commit(root.__comboValue(opt))
                }
            }
            Keys.onUpPressed:   if (comboList.currentIndex > 0) comboList.currentIndex -= 1
            Keys.onDownPressed: if (comboList.currentIndex < comboList.count - 1) comboList.currentIndex += 1

            Popup {
                id: comboPopup
                x: 0
                y: comboEditor.height
                width: comboEditor.width
                height: Math.min(comboList.contentHeight + 4, 200)
                padding: 2
                closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

                onClosed: {
                    if (root.editing && root.__editingKey === comboEditor.nodeIndex.key)
                        comboEditor.cancel()
                }

                background: Rectangle {
                    color: Style.baseColor
                    border.color: root.gridLineColor
                    border.width: 1
                    radius: 2
                }

                contentItem: ListView {
                    id: comboList
                    clip: true
                    model: comboEditor.column && comboEditor.column.options ? comboEditor.column.options : []
                    currentIndex: comboEditor.column && comboEditor.column.options
                        ? root.comboIndexOf(comboEditor.column.options, comboEditor.value) : -1

                    delegate: Rectangle {
                        width: comboList.width
                        height: root.rowHeight
                        color: optMouse.containsMouse           ? root.hoveredBackgroundColor
                             : index === comboList.currentIndex ? root.selectedBackgroundColor
                                                                : "transparent"

                        Text {
                            anchors.fill: parent
                            anchors.leftMargin: Style.marginM
                            anchors.rightMargin: Style.marginM
                            text: root.__comboLabel(comboEditor.column, modelData)
                            color: index === comboList.currentIndex ? root.selectedTextColor : root.normalTextColor
                            verticalAlignment: Text.AlignVCenter
                            elide: Text.ElideRight
                        }

                        MouseArea {
                            id: optMouse
                            anchors.fill: parent
                            hoverEnabled: true
                            onClicked: {
                                comboPopup.close()
                                comboEditor.commit(root.__comboValue(modelData))
                            }
                        }
                    }
                }
            }
        }
    }

    // ─── Focus / external triggers ─────────────────────────────────────────

    onActiveFocusChanged: {
        if (!activeFocus)
            __autoLeaveEdit()
    }

    onModelChanged: rebuildTree()
    onColumnsChanged: __recomputeColumnWidths()
    onWidthChanged:  __recomputeColumnWidths()
    onFilterTextChanged: filterDebounceTimer.restart()
    onFilterDebounceMsChanged: filterDebounceTimer.interval = filterDebounceMs

    Component.onCompleted: {
        __recomputeColumnWidths()
        rebuildTree()
    }

    // ═══════════════════════════════════════════════════════════════════════
    //  Tree building
    // ═══════════════════════════════════════════════════════════════════════

    function rebuild() { rebuildTreePreservingState() }

    function rebuildTreePreservingState() {
        captureExpandedState()
        rebuildTree()
    }

    function captureExpandedState() {
        for (var k in __nodes) {
            var n = __nodes[k]
            if (n && n.expanded)
                __expandedState[k] = true
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

        // Iterative DFS building. We push children in reverse so the
        // pop order is left-to-right.
        var stack = []
        var rootCount = __itemCount(model)
        for (var i = rootCount - 1; i >= 0; --i)
            stack.push({ item: __itemAt(model, i), row: i, parentKey: "", level: 0, path: String(i) })

        while (stack.length > 0) {
            var entry = stack.pop()
            if (!entry.item)
                continue

            var item = entry.item
            var keyValue = __normKey(item.key, entry.path)
            var children = item.children || []
            var childCount = __itemCount(children)

            var explicitExpanded = __normBool(item.expanded, false)
            var rememberedExpanded = __expandedState[keyValue] === true
            var node = {
                key:          keyValue,
                text:         __normText(item.text, keyValue),
                checked:      item.checked !== undefined && item.checked !== null ? item.checked : Qt.Unchecked,
                checkable:    __normBool(item.checkable, false),
                enabled:      __normBool(item.enabled, true),
                expanded:     explicitExpanded || rememberedExpanded,
                data:         item.data !== undefined ? item.data : null,
                parentKey:    entry.parentKey,
                childrenKeys: [],
                level:        entry.level,
                row:          entry.row,
                path:         entry.path,
                selected:     __selectedKeys[keyValue] === true,
                sourceItem:   item
            }

            __nodes[keyValue] = node

            if (entry.parentKey === "")
                __rootKeys.push(keyValue)
            else if (__nodes[entry.parentKey])
                __nodes[entry.parentKey].childrenKeys.push(keyValue)

            for (var c = childCount - 1; c >= 0; --c)
                stack.push({ item: __itemAt(children, c), row: c, parentKey: keyValue, level: entry.level + 1, path: entry.path + "/" + c })
        }

        __pruneExpandedState()
        cleanupSelection()
        buildVisibleTree()
    }

    function __pruneExpandedState() {
        var fresh = ({})
        for (var k in __expandedState) {
            if (__nodes[k] && __expandedState[k] === true)
                fresh[k] = true
        }
        __expandedState = fresh
    }

    // ═══════════════════════════════════════════════════════════════════════
    //  Visible flattening (filtering aware)
    // ═══════════════════════════════════════════════════════════════════════

    function buildVisibleTree() {
        var items = []
        __visibleKeys = []
        __visibleRowsByKey = ({})

        var ft = filterText.trim().toLowerCase()

        if (ft.length > 0) {
            var filterChanged = ft !== __lastAppliedFilter
            __lastAppliedFilter = ft
            __buildFilteredVisible(ft, items, filterChanged)
        } else {
            __lastAppliedFilter = ""
            for (var i = 0; i < __rootKeys.length; ++i)
                __appendBranchIterative(__rootKeys[i], items)
        }

        visibleModel.clear()
        if (items.length > 0)
            visibleModel.append(items)
    }

    function __appendBranchIterative(rootKey, items) {
        // Iterative DFS over expanded subtree.
        var stack = [rootKey]
        while (stack.length > 0) {
            var key = stack.pop()
            var node = __nodes[key]
            if (!node)
                continue

            __appendVisibleNode(node, items)
            if (!node.expanded)
                continue

            for (var i = node.childrenKeys.length - 1; i >= 0; --i)
                stack.push(node.childrenKeys[i])
        }
    }

    function __nodeMatchesFilter(node, ft) {
        var value = valueByPath(node, filterRole)
        return value !== undefined && value !== null && String(value).toLowerCase().indexOf(ft) >= 0
    }

    function __buildFilteredVisible(ft, items, autoExpand) {
        // Pass 1 (iterative, post-order): mark matches.
        var matchMap = {}
        var stack = []
        for (var r = 0; r < __rootKeys.length; ++r)
            stack.push({ key: __rootKeys[r], visited: false })

        var postOrder = []
        while (stack.length > 0) {
            var top = stack[stack.length - 1]
            var node = __nodes[top.key]
            if (!node) { stack.pop(); continue }
            if (!top.visited) {
                top.visited = true
                for (var i = node.childrenKeys.length - 1; i >= 0; --i)
                    stack.push({ key: node.childrenKeys[i], visited: false })
            } else {
                stack.pop()
                postOrder.push(top.key)
            }
        }

        for (var p = 0; p < postOrder.length; ++p) {
            var k = postOrder[p]
            var n = __nodes[k]
            if (!n) continue
            var selfMatch = __nodeMatchesFilter(n, ft)
            var childMatch = false
            for (var ci = 0; ci < n.childrenKeys.length; ++ci) {
                if (matchMap[n.childrenKeys[ci]]) { childMatch = true; break }
            }
            if (selfMatch || childMatch)
                matchMap[k] = true
        }

        // Pass 2: when filter just changed, auto-expand ancestors of matches.
        if (autoExpand) {
            for (var ek in matchMap) {
                var en = __nodes[ek]
                if (!en || en.expanded) continue
                for (var c2 = 0; c2 < en.childrenKeys.length; ++c2) {
                    if (matchMap[en.childrenKeys[c2]]) {
                        en.expanded = true
                        __expandedState[ek] = true
                        break
                    }
                }
            }
        }

        // Pass 3: append in tree order.
        var appendStack = []
        for (var rr = __rootKeys.length - 1; rr >= 0; --rr) {
            if (matchMap[__rootKeys[rr]])
                appendStack.push(__rootKeys[rr])
        }
        while (appendStack.length > 0) {
            var ak = appendStack.pop()
            var an = __nodes[ak]
            if (!an) continue
            __appendVisibleNode(an, items)
            if (an.expanded) {
                for (var ci2 = an.childrenKeys.length - 1; ci2 >= 0; --ci2) {
                    if (matchMap[an.childrenKeys[ci2]])
                        appendStack.push(an.childrenKeys[ci2])
                }
            }
        }
    }

    function __appendVisibleNode(node, items) {
        var row = __visibleKeys.length
        __visibleKeys.push(node.key)
        __visibleRowsByKey[node.key] = row
        items.push(__toVisibleObject(node))
    }

    function __toVisibleObject(node) {
        return {
            key:          node.key,
            level:        node.level,
            expanded:     node.expanded,
            hasChildren:  node.childrenKeys.length > 0,
            selected:     node.selected,
            checked:      node.checked,
            checkable:    node.checkable,
            nodeEnabled:  node.enabled,
            text:         node.text
        }
    }

    function __itemCount(items) {
        if (!items) return 0
        if (items.count !== undefined)  return items.count
        if (items.length !== undefined) return items.length
        return 0
    }

    function __itemAt(items, row) {
        if (!items) return null
        if (items.get) return items.get(row)
        return items[row]
    }

    function __normKey(value, path)  { return value !== undefined && value !== null && String(value).length > 0 ? String(value) : "path/" + path }
    function __normText(value, fb)   { return value !== undefined && value !== null ? String(value) : fb }
    function __normBool(value, fb)   { return value !== undefined && value !== null ? value === true || value === "true" || value === 1 : fb }

    // ═══════════════════════════════════════════════════════════════════════
    //  Source write-back
    // ═══════════════════════════════════════════════════════════════════════

    function writeBackNode(node) {
        // Mirror scalar fields to the original model item so that JS-array
        // models stay in sync without surprising the user with aliased "data".
        if (!node || !node.sourceItem)
            return
        node.sourceItem.text     = node.text
        node.sourceItem.checked  = node.checked
        node.sourceItem.checkable = node.checkable
        node.sourceItem.enabled  = node.enabled
        node.sourceItem.expanded = node.expanded
        // "data" is left untouched on purpose — setCellValue for paths inside
        // "data" mutates the same object, which is intentional. Replacing the
        // reference here would surprise callers who keep their own pointers.
    }

    // ═══════════════════════════════════════════════════════════════════════
    //  Index object
    // ═══════════════════════════════════════════════════════════════════════

    function createIndex(node) {
        if (!node) return null
        return {
            key:        node.key,
            text:       node.text,
            checked:    node.checked,
            checkable:  node.checkable,
            enabled:    node.enabled,
            expanded:   node.expanded,
            row:        node.row,
            path:       node.path,
            level:      node.level,
            parentKey:  node.parentKey,
            data:       node.data,
            item:       node.sourceItem,

            value: function (columnOrPath) {
                if (typeof columnOrPath === "string")
                    return root.valueByPath(node, columnOrPath)
                return root.cellValue(node.key, columnOrPath)
            },
            setValue: function (columnOrPath, value) {
                if (typeof columnOrPath === "string") {
                    root.setValueByPath(node, columnOrPath, value)
                    root.writeBackNode(node)
                    root.syncVisibleNode(node.key)
                    return
                }
                root.setCellValue(node.key, columnOrPath, value)
            },
            edit:       function (columnIndex) { root.beginEditCell(node.key, columnIndex) },
            setText:    function (value)       { root.setNodeText(node.key, value) },
            setChecked: function (state)       { root.setCheckState(node.key, state) },
            setEnabled: function (value)       { root.setNodeEnabled(node.key, value) },
            expand:     function ()            { root.expandNode(node.key) },
            collapse:   function ()            { root.collapseNode(node.key) },
            select:     function ()            { root.select(node.key) }
        }
    }

    // ═══════════════════════════════════════════════════════════════════════
    //  Columns
    // ═══════════════════════════════════════════════════════════════════════

    function columnCount() { return columns.length > 0 ? columns.length : 1 }
    function columnAt(i)   { return columns.length > 0 ? columns[i] : __defaultColumn }

    function columnTitle(column) {
        if (!column) return ""
        if (column.title !== undefined && column.title !== null && String(column.title).length > 0)
            return String(column.title)
        if (column.name !== undefined && column.name !== null)
            return String(column.name)
        return ""
    }

    function columnWidth(column) {
        var idx = -1
        for (var i = 0; i < columnCount(); ++i) {
            if (columnAt(i) === column) { idx = i; break }
        }
        if (idx < 0) return root.width
        return __columnWidths[idx] !== undefined ? __columnWidths[idx] : 0
    }

    function __recomputeColumnWidths() {
        var n = columnCount()
        if (n <= 0) { __columnWidths = []; return }

        var widths = new Array(n)
        var stretchIndices = []
        var fixedTotal = 0
        var i, col, w

        for (i = 0; i < n; ++i) {
            col = columnAt(i)
            w = (col && col.width !== undefined && col.width > 0) ? Number(col.width) : 0
            if (w > 0) {
                widths[i] = w
                fixedTotal += w
            } else {
                widths[i] = 0
                stretchIndices.push(i)
            }
        }

        if (stretchIndices.length > 0) {
            var remaining = Math.max(0, root.width - fixedTotal)
            var share = Math.floor(remaining / stretchIndices.length)
            var leftover = remaining - share * stretchIndices.length
            for (i = 0; i < stretchIndices.length; ++i) {
                var idx = stretchIndices[i]
                var minW = 0
                col = columnAt(idx)
                if (col && col.minWidth !== undefined && col.minWidth > 0)
                    minW = Number(col.minWidth)
                widths[idx] = Math.max(minW, share + (i === stretchIndices.length - 1 ? leftover : 0))
            }
        } else if (fixedTotal < root.width && n > 0) {
            // Distribute leftover proportionally so the rightmost column fills the row.
            widths[n - 1] += root.width - fixedTotal
        }

        __columnWidths = widths
    }

    function __columnAtX(x) {
        var n = columnCount()
        if (n <= 1) return 0
        var acc = 0
        for (var i = 0; i < n; ++i) {
            acc += __columnWidths[i] || 0
            if (x < acc) return i
        }
        return n - 1
    }

    function isTreeColumn(column, columnIndex) {
        if (column && column.tree === true)
            return true
        return columnIndex === 0 && columns.length === 0
    }

    function isColumnEditable(column) { return column && column.editable === true }

    function columnDisplayPath(column) {
        if (!column) return ""
        if (column.display !== undefined && column.display !== null && String(column.display).length > 0)
            return String(column.display)
        if (column.name !== undefined && column.name !== null && String(column.name).length > 0)
            return String(column.name)
        return ""
    }

    function columnType(keyValue, column) {
        if (!column) return "string"
        if (column.type !== undefined && column.type !== null) {
            var explicit = String(column.type)
            if (explicit.length > 0 && explicit !== "auto")
                return explicit
        }
        if (column.options && column.options.length > 0)
            return "combo"
        if (columnDisplayPath(column) === priv.ckPathChecked)
            return "checkState"
        var v = cellValue(keyValue, column)
        if (typeof v === "boolean") return "bool"
        if (typeof v === "number")  return "number"
        return "string"
    }

    function __pickEditorType(column, value, keyValue) {
        if (column && column.editor)
            return "custom"
        return columnType(keyValue || "", column)
    }

    function cellValue(keyValue, column) {
        var node = __nodes[keyValue]
        if (!node) return undefined
        return valueByPath(node, columnDisplayPath(column))
    }

    function setCellValue(keyValue, column, value) {
        var node = __nodes[keyValue]
        if (!node) return

        var path = columnDisplayPath(column)
        setValueByPath(node, path, value)

        switch (path) {
            case "text":      node.text      = String(value); break
            case "checked":   node.checked   = value; break
            case "enabled":   node.enabled   = value; break
            case "checkable": node.checkable = value; break
            case "expanded":  node.expanded  = value; break
        }

        writeBackNode(node)
        syncVisibleNode(keyValue)
        if (path === "checked")
            syncVisibleAncestors(node)
    }

    function valueByPath(object, path) {
        if (!object || !path || path.length <= 0) return undefined
        var parts = String(path).split(".")
        var current = object
        for (var i = 0; i < parts.length; ++i) {
            if (current === undefined || current === null) return undefined
            current = current[parts[i]]
        }
        return current
    }

    function setValueByPath(object, path, value) {
        if (!object || !path || path.length <= 0) return
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
        if (!options) return -1
        for (var i = 0; i < options.length; ++i) {
            var opt = options[i]
            if (opt === value) return i
            if (opt && typeof opt === "object" && opt.value === value) return i
        }
        return -1
    }

    function __comboLabel(column, option) {
        if (option === undefined || option === null) return ""
        if (typeof option === "object") return option.text !== undefined ? String(option.text) : String(option.value)
        // Plain string option — but if value is a {value,text} from column.options,
        // try to resolve label by matching value list.
        if (column && column.options) {
            for (var i = 0; i < column.options.length; ++i) {
                var o = column.options[i]
                if (o && typeof o === "object" && o.value === option)
                    return o.text !== undefined ? String(o.text) : String(o.value)
            }
        }
        return String(option)
    }

    function __comboValue(option) {
        if (option && typeof option === "object" && option.value !== undefined)
            return option.value
        return option
    }

    // ═══════════════════════════════════════════════════════════════════════
    //  Expand / collapse
    // ═══════════════════════════════════════════════════════════════════════

    function toggleExpanded(keyValue) {
        var node = __nodes[keyValue]
        if (!node || node.childrenKeys.length <= 0) return
        if (node.expanded) collapseNode(keyValue)
        else               expandNode(keyValue)
    }

    function expandNode(keyValue) {
        var node = __nodes[keyValue]
        if (!node || node.expanded) return

        node.expanded = true
        __expandedState[keyValue] = true
        if (node.sourceItem) node.sourceItem.expanded = true

        if (filterText.trim().length > 0) {
            buildVisibleTree()
            return
        }

        var row = visibleRowOf(keyValue)
        if (row < 0) return

        visibleModel.setProperty(row, "expanded", true)

        var inserted = []
        __flattenExpandedIterative(node, inserted)
        if (inserted.length > 0) {
            var insertIndex = row + 1
            var insertObjects = new Array(inserted.length)
            var insertKeys = new Array(inserted.length)
            for (var i = 0; i < inserted.length; ++i) {
                insertKeys[i] = inserted[i].key
                insertObjects[i] = __toVisibleObject(inserted[i])
            }
            __visibleKeys.splice.apply(__visibleKeys, [insertIndex, 0].concat(insertKeys))
            visibleModel.insert(insertIndex, insertObjects)
            __rebuildVisibleRowsFrom(insertIndex)
        }
    }

    function collapseNode(keyValue) {
        var node = __nodes[keyValue]
        if (!node || !node.expanded) return

        node.expanded = false
        // Only explicitly-expanded nodes are tracked; deleting on collapse
        // keeps __expandedState small and avoids stale entries after rebuild.
        delete __expandedState[keyValue]
        if (node.sourceItem) node.sourceItem.expanded = false

        if (filterText.trim().length > 0) {
            buildVisibleTree()
            return
        }

        var row = visibleRowOf(keyValue)
        if (row < 0) return

        visibleModel.setProperty(row, "expanded", false)

        var removeCount = __countVisibleDescendantsIterative(node)
        if (removeCount <= 0) return

        for (var i = 1; i <= removeCount; ++i)
            delete __visibleRowsByKey[__visibleKeys[row + i]]

        visibleModel.remove(row + 1, removeCount)
        __visibleKeys.splice(row + 1, removeCount)
        __rebuildVisibleRowsFrom(row + 1)
    }

    function expandAll() {
        for (var key in __nodes) {
            var n = __nodes[key]
            if (n && n.childrenKeys.length > 0 && !n.expanded) {
                n.expanded = true
                __expandedState[key] = true
                if (n.sourceItem) n.sourceItem.expanded = true
            }
        }
        buildVisibleTree()
    }

    function collapseAll() {
        for (var key in __nodes) {
            var n = __nodes[key]
            if (n && n.expanded) {
                n.expanded = false
                if (n.sourceItem) n.sourceItem.expanded = false
            }
        }
        __expandedState = ({})
        buildVisibleTree()
    }

    function __flattenExpandedIterative(node, out) {
        // Pre-order DFS that follows children only when the node is expanded.
        var stack = []
        for (var i = node.childrenKeys.length - 1; i >= 0; --i)
            stack.push(node.childrenKeys[i])
        while (stack.length > 0) {
            var k = stack.pop()
            var child = __nodes[k]
            if (!child) continue
            out.push(child)
            if (child.expanded) {
                for (var j = child.childrenKeys.length - 1; j >= 0; --j)
                    stack.push(child.childrenKeys[j])
            }
        }
    }

    function __countVisibleDescendantsIterative(node) {
        var count = 0
        var stack = []
        for (var i = 0; i < node.childrenKeys.length; ++i)
            stack.push(node.childrenKeys[i])
        while (stack.length > 0) {
            var k = stack.pop()
            var child = __nodes[k]
            if (!child) continue
            ++count
            if (child.expanded) {
                for (var j = 0; j < child.childrenKeys.length; ++j)
                    stack.push(child.childrenKeys[j])
            }
        }
        return count
    }

    // ═══════════════════════════════════════════════════════════════════════
    //  Selection
    // ═══════════════════════════════════════════════════════════════════════

    function select(keyValue) {
        var node = __nodes[keyValue]
        if (!node) return
        if (!allowDisabledSelection && !node.enabled) return

        var changed = []
        if (!multiSelect)
            changed = __clearSelectionInternal(false)

        if (!node.selected) {
            node.selected = true
            __selectedKeys[keyValue] = true
            changed.push(keyValue)
        }

        currentIndex = createIndex(node)
        __syncVisibleNodes(changed)
        __updateSelectedCount()
        if (changed.length > 0)
            selectionChanged()
    }

    function deselect(keyValue) {
        var node = __nodes[keyValue]
        if (!node || !node.selected) return
        node.selected = false
        delete __selectedKeys[keyValue]
        syncVisibleNode(keyValue)
        __updateSelectedCount()
        selectionChanged()
    }

    function toggleSelection(keyValue) {
        var node = __nodes[keyValue]
        if (!node) return
        if (node.selected) deselect(keyValue)
        else               select(keyValue)
    }

    function clearSelection() {
        var changed = __clearSelectionInternal(true)
        __syncVisibleNodes(changed)
        __updateSelectedCount()
        if (changed.length > 0)
            selectionChanged()
    }

    function __clearSelectionInternal(clearCurrent) {
        var changed = []
        for (var k in __selectedKeys) {
            var n = __nodes[k]
            if (n && n.selected) {
                n.selected = false
                changed.push(k)
            }
        }
        __selectedKeys = ({})
        if (clearCurrent)
            currentIndex = null
        return changed
    }

    function selectedKeys()    { return Object.keys(__selectedKeys) }
    function selectedIndexes() {
        var result = []
        for (var k in __selectedKeys) {
            var n = __nodes[k]
            if (n) result.push(createIndex(n))
        }
        return result
    }

    function cleanupSelection() {
        var cleaned = ({})
        for (var k in __selectedKeys) {
            if (__nodes[k]) {
                __nodes[k].selected = true
                cleaned[k] = true
            }
        }
        __selectedKeys = cleaned
        if (currentIndex && currentIndex.key && !__nodes[currentIndex.key])
            currentIndex = null
        __updateSelectedCount()
    }

    function __updateSelectedCount() { __selectedCount = Object.keys(__selectedKeys).length }

    // ═══════════════════════════════════════════════════════════════════════
    //  Check states
    // ═══════════════════════════════════════════════════════════════════════

    function toggleCheckState(keyValue) {
        var node = __nodes[keyValue]
        if (!__canChangeCheckState(node)) return
        setCheckState(keyValue, node.checked === Qt.Checked ? Qt.Unchecked : Qt.Checked)
    }

    function setCheckState(keyValue, state) {
        var node = __nodes[keyValue]
        if (!__canChangeCheckState(node)) return

        __propagateDownIterative(node, state)
        __propagateUpIterative(node)
        __syncVisibleSubtreeIterative(node)
        syncVisibleAncestors(node)
        checkStateChanged(createIndex(node), state)
        checkedItemsChanged()
    }

    function checkItem(key)   { setCheckState(key, Qt.Checked) }
    function uncheckItem(key) { setCheckState(key, Qt.Unchecked) }

    function checkAll() {
        for (var k in __nodes) {
            var n = __nodes[k]
            if (n && n.checkable && n.enabled) {
                n.checked = Qt.Checked
                writeBackNode(n)
            }
        }
        for (var i = 0; i < __visibleKeys.length; ++i)
            syncVisibleNode(__visibleKeys[i])
        checkedItemsChanged()
    }

    function uncheckAll() {
        for (var k in __nodes) {
            var n = __nodes[k]
            if (n && n.checkable) {
                n.checked = Qt.Unchecked
                writeBackNode(n)
            }
        }
        for (var i = 0; i < __visibleKeys.length; ++i)
            syncVisibleNode(__visibleKeys[i])
        checkedItemsChanged()
    }

    function getCheckedKeys() {
        var result = []
        for (var k in __nodes) {
            var n = __nodes[k]
            if (n && n.checked === Qt.Checked)
                result.push(k)
        }
        return result
    }

    function getCheckedNodes() {
        var result = []
        for (var k in __nodes) {
            var n = __nodes[k]
            if (n && n.checked === Qt.Checked)
                result.push(createIndex(n))
        }
        return result
    }

    function allNodes() {
        var result = []
        for (var k in __nodes) {
            var n = __nodes[k]
            if (n) result.push(createIndex(n))
        }
        return result
    }

    function __canChangeCheckState(node) {
        if (!node || !node.checkable) return false
        if (skipDisabledOrNonCheckableOnCheck && !node.enabled) return false
        return true
    }

    function __shouldParticipateInCheck(node) {
        if (!node) return false
        if (!skipDisabledOrNonCheckableOnCheck) return true
        return node.enabled && node.checkable
    }

    function __propagateDownIterative(rootNode, state) {
        var stack = [rootNode]
        while (stack.length > 0) {
            var n = stack.pop()
            if (!__shouldParticipateInCheck(n)) continue
            n.checked = state
            writeBackNode(n)
            for (var i = 0; i < n.childrenKeys.length; ++i) {
                var c = __nodes[n.childrenKeys[i]]
                if (c) stack.push(c)
            }
        }
    }

    function __propagateUpIterative(startNode) {
        var current = startNode
        while (current && current.parentKey !== "") {
            var parent = __nodes[current.parentKey]
            if (!parent) return

            var hasParticipating = false
            var allChecked = true
            var allUnchecked = true
            for (var i = 0; i < parent.childrenKeys.length; ++i) {
                var child = __nodes[parent.childrenKeys[i]]
                if (!__shouldParticipateInCheck(child)) continue
                hasParticipating = true
                if (child.checked !== Qt.Checked)   allChecked = false
                if (child.checked !== Qt.Unchecked) allUnchecked = false
            }
            if (hasParticipating && __shouldParticipateInCheck(parent)) {
                parent.checked = allChecked ? Qt.Checked : allUnchecked ? Qt.Unchecked : Qt.PartiallyChecked
                writeBackNode(parent)
            }
            current = parent
        }
    }

    function __syncVisibleSubtreeIterative(rootNode) {
        var stack = [rootNode]
        while (stack.length > 0) {
            var n = stack.pop()
            if (!n) continue
            syncVisibleNode(n.key)
            for (var i = 0; i < n.childrenKeys.length; ++i)
                stack.push(__nodes[n.childrenKeys[i]])
        }
    }

    // ═══════════════════════════════════════════════════════════════════════
    //  Editing
    // ═══════════════════════════════════════════════════════════════════════

    function isEditingCell(keyValue, columnIndex) {
        return __editingKey === keyValue && __editingColumn === columnIndex
    }

    function editCurrent(columnIndex) {
        if (!currentIndex || !currentIndex.key) return false
        if (columnIndex === undefined || columnIndex === null)
            columnIndex = __firstEditableColumn()
        return beginEditCell(currentIndex.key, columnIndex)
    }

    function startEdit(keyValue, columnOrName) {
        var colIndex = typeof columnOrName === "number"
            ? columnOrName : __columnIndexByName(String(columnOrName))
        return beginEditCell(keyValue, colIndex)
    }

    function commitCurrentEdit() { return commitEdit(__editingPendingValue) }
    function cancelCurrentEdit() { cancelEdit() }

    // Returns true on success.
    function beginEditCell(keyValue, columnIndex) {
        if (!editable) return false

        var node = __nodes[keyValue]
        if (!node) return false
        if (!allowDisabledEditing && !node.enabled) return false
        if (columnIndex < 0 || columnIndex >= columnCount()) return false

        var column = columnAt(columnIndex)
        if (!isColumnEditable(column)) return false

        // Finish any previous edit gracefully.
        if (__editingKey.length > 0)
            commitEdit(__editingPendingValue)

        select(keyValue)

        __editingKey = keyValue
        __editingColumn = columnIndex
        __editingOriginalValue = cellValue(keyValue, column)
        __editingPendingValue = __editingOriginalValue
        __editingIsDirty = false
        __editingError = ""

        currentIndex = createIndex(node)
        editingIndex = createIndex(node)
        editingChanged()
        cellEditStarted(editingIndex, column)
        return true
    }

    function commitEdit(value) {
        if (__editingKey.length <= 0 || __editingColumn < 0) return false

        var keyValue = __editingKey
        var columnIndex = __editingColumn
        var column = columnAt(columnIndex)
        var node = __nodes[keyValue]

        if (!node) {
            __clearEditState()
            return false
        }

        var oldValue = __editingOriginalValue
        if (value === undefined)
            value = __editingPendingValue !== null && __editingPendingValue !== undefined
                ? __editingPendingValue : cellValue(keyValue, column)

        value = __normalizeEditorValue(value, column, keyValue)

        // Optional column-level validation. Returns true on success or an
        // error string otherwise.
        var validatorMsg = __runColumnValidator(value, column, createIndex(node))
        if (validatorMsg !== true) {
            __editingError = String(validatorMsg)
            cellEditCommitFailed(createIndex(node), column, value, __editingError)
            return false
        }

        // Equality short-circuit avoids spurious writes / events.
        if (__valuesEqual(value, oldValue)) {
            __clearEditState()
            return true
        }

        var indexObject = createIndex(node)

        // writeBack veto.
        if (root.writeBack) {
            var accepted = root.writeBack(indexObject, column, value, oldValue)
            if (accepted === false) {
                __editingError = ""
                cellEditCommitFailed(indexObject, column, value, "writeBack rejected")
                return false
            }
        }

        __clearEditState()
        setCellValue(keyValue, column, value)
        node = __nodes[keyValue]
        if (!node) return true

        indexObject = createIndex(node)
        currentIndex = indexObject
        cellEdited(indexObject, column, value, oldValue)

        if (columnDisplayPath(column) === priv.ckPathText)
            nodeTextEdited(indexObject, String(value), String(oldValue))

        return true
    }

    function cancelEdit() {
        if (__editingKey.length <= 0) return

        var node = __nodes[__editingKey]
        var column = __editingColumn >= 0 && __editingColumn < columnCount() ? columnAt(__editingColumn) : null

        __clearEditState()

        if (node && column)
            cellEditCanceled(createIndex(node), column)
    }

    function revertCell(indexObject, column) {
        if (!indexObject || !indexObject.key) return
        var node = __nodes[indexObject.key]
        if (!node) return
        // Re-read from the source item if available, otherwise re-emit the
        // current local value to give consumers a chance to refresh.
        var path = columnDisplayPath(column)
        if (node.sourceItem) {
            var sourceVal = valueByPath(node.sourceItem, path)
            if (sourceVal !== undefined)
                setValueByPath(node, path, sourceVal)
        }
        syncVisibleNode(node.key)
    }

    function editNextCell(fromColumn) {
        var anchorKey = __editingKey.length > 0 ? __editingKey : (currentIndex ? currentIndex.key : "")
        var baseCol   = fromColumn !== undefined && fromColumn !== null && fromColumn >= 0
            ? fromColumn : __editingColumn
        var startCol  = baseCol >= 0 ? baseCol + 1 : 0
        if (anchorKey.length <= 0) return

        // Try further editable columns in the same row.
        for (var c = startCol; c < columnCount(); ++c) {
            if (isColumnEditable(columnAt(c))) {
                beginEditCell(anchorKey, c)
                return
            }
        }
        // Next visible row, first editable.
        var row = visibleRowOf(anchorKey)
        if (row >= 0 && row < __visibleKeys.length - 1) {
            var nextKey = __visibleKeys[row + 1]
            var nextNode = __nodes[nextKey]
            if (nextNode && (allowDisabledEditing || nextNode.enabled)) {
                for (var c2 = 0; c2 < columnCount(); ++c2) {
                    if (isColumnEditable(columnAt(c2))) {
                        beginEditCell(nextKey, c2)
                        return
                    }
                }
            }
        }
    }

    function editPreviousCell(fromColumn) {
        var anchorKey = __editingKey.length > 0 ? __editingKey : (currentIndex ? currentIndex.key : "")
        var baseCol   = fromColumn !== undefined && fromColumn !== null && fromColumn >= 0
            ? fromColumn : __editingColumn
        var startCol  = baseCol >= 0 ? baseCol - 1 : columnCount() - 1
        if (anchorKey.length <= 0) return

        for (var c = startCol; c >= 0; --c) {
            if (isColumnEditable(columnAt(c))) {
                beginEditCell(anchorKey, c)
                return
            }
        }
        var row = visibleRowOf(anchorKey)
        if (row > 0) {
            var prevKey = __visibleKeys[row - 1]
            var prevNode = __nodes[prevKey]
            if (prevNode && (allowDisabledEditing || prevNode.enabled)) {
                for (var c2 = columnCount() - 1; c2 >= 0; --c2) {
                    if (isColumnEditable(columnAt(c2))) {
                        beginEditCell(prevKey, c2)
                        return
                    }
                }
            }
        }
    }

    function __clearEditState() {
        var wasEditing = __editingKey.length > 0
        __editingKey = ""
        __editingColumn = -1
        __editingOriginalValue = null
        __editingPendingValue = null
        __editingIsDirty = false
        __editingError = ""
        editingIndex = null
        if (wasEditing)
            editingChanged()
    }

    function __normalizeEditorValue(value, column, keyValue) {
        var t = columnType(keyValue || "", column)
        if (t === "number") {
            var num = Number(value)
            return isNaN(num) ? 0 : num
        }
        if (t === "bool")       return value === true || value === "true"
        if (t === "checkState") return value
        if (t === "string")     return value !== undefined && value !== null ? String(value) : ""
        return value
    }

    function __runColumnValidator(value, column, indexObject) {
        if (!column || column.validator === undefined || column.validator === null)
            return true
        if (typeof column.validator === "function") {
            // Convention: return true for success; non-true is treated as an error
            // message (use empty string to reject silently).
            var r = column.validator(value, indexObject)
            return r === true ? true : (r !== undefined && r !== null ? String(r) : "")
        }
        // QValidator: defer to the editor; treat as accepting.
        return true
    }

    function __valuesEqual(a, b) {
        if (a === b) return true
        if (a === null || a === undefined || b === null || b === undefined) return false
        if (typeof a === "number" && typeof b === "number") return a === b
        return String(a) === String(b)
    }

    function __firstEditableColumn() {
        var n = columnCount()
        for (var i = 0; i < n; ++i) {
            if (isColumnEditable(columnAt(i))) return i
        }
        return 0
    }

    function __columnIndexByName(name) {
        for (var i = 0; i < columnCount(); ++i) {
            var col = columnAt(i)
            if (col && col.name === name) return i
        }
        return -1
    }

    function __autoLeaveEdit() {
        if (!editing) return
        if (editingFocusOutPolicy === "commit") commitEdit(__editingPendingValue)
        else if (editingFocusOutPolicy === "cancel") cancelEdit()
        // "keep" — leave the editor open
    }

    // ═══════════════════════════════════════════════════════════════════════
    //  Node setters
    // ═══════════════════════════════════════════════════════════════════════

    function setNodeText(keyValue, value) {
        var node = __nodes[keyValue]
        if (!node) return
        node.text = String(value)
        writeBackNode(node)
        syncVisibleNode(keyValue)
        if (currentIndex && currentIndex.key === keyValue)
            currentIndex = createIndex(node)
    }

    function setNodeEnabled(keyValue, value) {
        var node = __nodes[keyValue]
        if (!node) return
        node.enabled = value
        writeBackNode(node)
        syncVisibleNode(keyValue)
    }

    // ═══════════════════════════════════════════════════════════════════════
    //  Visible model sync helpers
    // ═══════════════════════════════════════════════════════════════════════

    function syncVisibleNode(keyValue) {
        var row = visibleRowOf(keyValue)
        if (row < 0) return
        var node = __nodes[keyValue]
        if (!node) return
        visibleModel.set(row, __toVisibleObject(node))
    }

    function __syncVisibleNodes(keys) {
        for (var i = 0; i < keys.length; ++i)
            syncVisibleNode(keys[i])
    }

    function syncVisibleAncestors(node) {
        var current = node
        while (current && current.parentKey !== "") {
            current = __nodes[current.parentKey]
            if (current) syncVisibleNode(current.key)
        }
    }

    function __rebuildVisibleRowsFrom(startRow) {
        if (startRow < 0) startRow = 0
        for (var i = startRow; i < __visibleKeys.length; ++i)
            __visibleRowsByKey[__visibleKeys[i]] = i
    }

    function visibleRowOf(keyValue) {
        var row = __visibleRowsByKey[keyValue]
        return row === undefined ? -1 : row
    }

    function nodeForKey(keyValue)  { return __nodes[keyValue] || null }
    function indexForKey(keyValue) { var n = __nodes[keyValue]; return n ? createIndex(n) : null }
    function keyForIndex(indexObject) { return indexObject && indexObject.key ? indexObject.key : "" }
    function isExpanded(keyValue)  { var n = __nodes[keyValue]; return n ? n.expanded : false }
    function isSelected(keyValue)  { var n = __nodes[keyValue]; return n ? n.selected : false }

    // ═══════════════════════════════════════════════════════════════════════
    //  Visibility / scroll
    // ═══════════════════════════════════════════════════════════════════════

    function ensureVisible(keyValue) {
        expandParents(keyValue)
        var row = visibleRowOf(keyValue)
        if (row < 0) return

        listView.positionViewAtIndex(row, ListView.Contain)

        if (root.flickable) {
            // Use coordinate mapping so intermediate containers don't break the math.
            var headerOffset = root.showHeader ? root.headerHeight + 1 : 0
            var localY = headerOffset + row * root.rowHeight
            var target = root.flickable.contentItem ? root.flickable.contentItem : root.flickable
            if (!target)
                return
            var mapped = root.mapToItem(target, 0, localY)
            var itemTop = mapped.y
            var itemBottom = itemTop + root.rowHeight
            var viewTop = root.flickable.contentY
            var viewBottom = viewTop + root.flickable.height

            if (itemBottom > viewBottom)
                root.flickable.contentY = itemBottom - root.flickable.height
            else if (itemTop < viewTop)
                root.flickable.contentY = itemTop
        }
    }

    function expandParents(keyValue) {
        var node = __nodes[keyValue]
        if (!node) return

        var parents = []
        var current = node
        while (current && current.parentKey !== "") {
            current = __nodes[current.parentKey]
            if (current) parents.unshift(current.key)
        }
        for (var i = 0; i < parents.length; ++i) {
            var parent = __nodes[parents[i]]
            if (parent && !parent.expanded)
                expandNode(parent.key)
        }
    }

    function selectAndEnsureVisible(keyValue) {
        ensureVisible(keyValue)
        select(keyValue)
    }

    function scrollToCurrent() {
        if (!currentIndex || !currentIndex.key) return
        ensureVisible(currentIndex.key)
    }

    // ═══════════════════════════════════════════════════════════════════════
    //  Keyboard navigation
    // ═══════════════════════════════════════════════════════════════════════

    function __handleKey(event) {
        if (event.key === Qt.Key_F2) {
            if (currentIndex && currentIndex.key)
                editCurrent(__firstEditableColumn())
            return true
        }
        if (event.key === Qt.Key_Escape && editing) {
            cancelEdit(); return true
        }
        if ((event.key === Qt.Key_Return || event.key === Qt.Key_Enter) && editing) {
            commitEdit(__editingPendingValue); return true
        }
        if (event.key === Qt.Key_Down)  { __navigateDown(); return true }
        if (event.key === Qt.Key_Up)    { __navigateUp();   return true }
        if (event.key === Qt.Key_Right) { __navigateRight(); return true }
        if (event.key === Qt.Key_Left)  { __navigateLeft();  return true }
        if (event.key === Qt.Key_Space) {
            if (currentIndex && currentIndex.key)
                toggleCheckState(currentIndex.key)
            return true
        }
        if (event.key === Qt.Key_Home) {
            if (__visibleKeys.length > 0) selectAndEnsureVisible(__visibleKeys[0])
            return true
        }
        if (event.key === Qt.Key_End) {
            if (__visibleKeys.length > 0) selectAndEnsureVisible(__visibleKeys[__visibleKeys.length - 1])
            return true
        }
        if (event.key === Qt.Key_Asterisk) { expandAll(); return true }
        return false
    }

    function __navigateDown() {
        if (__visibleKeys.length === 0) return
        if (!currentIndex || !currentIndex.key) {
            selectAndEnsureVisible(__visibleKeys[0]); return
        }
        var row = visibleRowOf(currentIndex.key)
        if (row < 0 || row >= __visibleKeys.length - 1) return
        selectAndEnsureVisible(__visibleKeys[row + 1])
    }

    function __navigateUp() {
        if (__visibleKeys.length === 0) return
        if (!currentIndex || !currentIndex.key) {
            selectAndEnsureVisible(__visibleKeys[0]); return
        }
        var row = visibleRowOf(currentIndex.key)
        if (row <= 0) return
        selectAndEnsureVisible(__visibleKeys[row - 1])
    }

    function __navigateRight() {
        if (!currentIndex || !currentIndex.key) return
        var node = __nodes[currentIndex.key]
        if (node && node.childrenKeys.length > 0 && !node.expanded)
            expandNode(node.key)
        else
            __navigateDown()
    }

    function __navigateLeft() {
        if (!currentIndex || !currentIndex.key) return
        var node = __nodes[currentIndex.key]
        if (node && node.expanded && node.childrenKeys.length > 0)
            collapseNode(node.key)
        else if (node && node.parentKey !== "")
            selectAndEnsureVisible(node.parentKey)
    }
}
