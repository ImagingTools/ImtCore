import QtQuick
import Acf 1.0
import imtcontrols 1.0

Item {
    id: root

    /*
        Fixed model format:

        [
            {
                key: "unique-key",
                text: "Display text",
                checked: Qt.Unchecked,
                checkable: false,
                enabled: true,
                expanded: false,
                children: [],
                data: null
            }
        ]
    */
    property var model: []

    property var columns: []

    property int rowHeight: 28
    property int indentation: 18
    property int cacheBuffer: 4000
    property int headerHeight: 28

    property bool showHeader: true
    property bool multiSelect: false
    property bool allowDisabledSelection: false
    property bool skipDisabledOrNonCheckableOnCheck: true

    property bool editable: false
    property bool editOnDoubleClick: true
    property bool allowDisabledEditing: false

    property string filterText: ""
    property string filterRole: "text"
    property int filterDebounceInterval: 120

    property bool showToolbar: false

    property int __hoveredRow: -1

    property string selectedBackgroundColor: Style.selectedColor
    property string hoveredBackgroundColor: Style.hover
    property string normalTextColor: Style.textColor
    property string selectedTextColor: Style.textSelectedColor
    property string disabledTextColor: Style.inactiveTextColor
    property string headerBackgroundColor: Style.backgroundColor
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

    property alias listView: listView

    property var __nodes: ({})
    property var __rootKeys: []
    property var __expandedState: ({})
    property var __selectedKeys: ({})
    property var __visibleKeys: []
    property var __visibleRowsByKey: ({})
    property int __selectedCount: 0

    property string __editingKey: ""
    property int __editingColumn: -1
    property var __editingOriginalValue: null

    implicitHeight: (root.showToolbar ? toolbarRow.height : 0) + (root.showHeader ? root.headerHeight : 0) + visibleModel.count * root.rowHeight

    Timer {
        id: filterDebounceTimer
        interval: root.filterDebounceInterval
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

    Column {
        width: parent.width
        spacing: 0

        Row {
            id: toolbarRow

            width: parent.width
            height: root.showToolbar ? 28 : 0
            visible: root.showToolbar
            spacing: Style.spacingM
            leftPadding: Style.spacingS

            Text {
                text: qsTr("Expand All")
                color: Style.linkColor
                font.underline: toolbarExpandAllMa.containsMouse
                verticalAlignment: Text.AlignVCenter
                height: parent.height

                MouseArea {
                    id: toolbarExpandAllMa
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.expandAll()
                }
            }

            Text {
                text: qsTr("Collapse All")
                color: Style.linkColor
                font.underline: toolbarCollapseAllMa.containsMouse
                verticalAlignment: Text.AlignVCenter
                height: parent.height

                MouseArea {
                    id: toolbarCollapseAllMa
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.collapseAll()
                }
            }

            Text {
                text: qsTr("Check All")
                color: Style.linkColor
                font.underline: toolbarCheckAllMa.containsMouse
                visible: root.tristate
                verticalAlignment: Text.AlignVCenter
                height: parent.height

                MouseArea {
                    id: toolbarCheckAllMa
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.checkAll()
                }
            }

            Text {
                text: qsTr("Uncheck All")
                color: Style.linkColor
                font.underline: toolbarUncheckAllMa.containsMouse
                visible: root.tristate
                verticalAlignment: Text.AlignVCenter
                height: parent.height

                MouseArea {
                    id: toolbarUncheckAllMa
                    anchors.fill: parent
                    hoverEnabled: true
                    cursorShape: Qt.PointingHandCursor
                    onClicked: root.uncheckAll()
                }
            }
        }

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
                        anchors.leftMargin: 8
                        anchors.rightMargin: 8

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
            height: visibleModel.count * root.rowHeight

            clip: true
            reuseItems: true
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
                    root.commitEdit()
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
                        root.nodeClicked(indexObject)
                    }
                }

                onDoubleClicked: {
                    listView.forceActiveFocus()

                    var node = root.__nodes[delegateRoot.nodeKey]
                    if (!node)
                        return

                    var indexObject = root.createIndex(node)

                    if (root.editable && root.editOnDoubleClick) {
                        root.beginEditCell(delegateRoot.nodeKey, 0)
                    } else if (delegateRoot.nodeHasChildren) {
                        root.toggleExpanded(delegateRoot.nodeKey)
                    }

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
                                anchors.verticalCenter: parent.verticalCenter
                                width: Math.max(0, parent.width - x)
                                text: cellRoot.displayText
                                color: !delegateRoot.nodeIsEnabled ? root.disabledTextColor : delegateRoot.nodeSelected ? root.selectedTextColor : root.normalTextColor
                                horizontalAlignment: cellRoot.column && cellRoot.column.horizontalAlignment !== undefined ? cellRoot.column.horizontalAlignment : Text.AlignLeft
                                elide: Text.ElideRight
                                verticalAlignment: Text.AlignVCenter
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
        visibleModel.clear()
        __visibleKeys = []
        __visibleRowsByKey = ({})

        var ft = filterText.trim().toLowerCase()

        if (ft.length > 0) {
            buildFilteredVisible(ft)
        } else {
            for (var i = 0; i < __rootKeys.length; ++i) {
                var rootNode = __nodes[__rootKeys[i]]
                if (rootNode)
                    appendVisibleBranch(rootNode)
            }
        }
    }

    function nodeMatchesFilter(node, ft) {
        var value = valueByPath(node, filterRole)
        return value !== undefined && value !== null && String(value).toLowerCase().indexOf(ft) >= 0
    }

    function buildFilteredVisible(ft) {
        // Single DFS pass: post-order traversal marks subtrees that match,
        // then pre-order append of marked nodes
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

        let appendMarked = function(nodeKey) {
            if (!matchMap[nodeKey])
                return
            var node = __nodes[nodeKey]
            if (!node)
                return
            appendVisibleNode(node)
            for (var i = 0; i < node.childrenKeys.length; ++i)
                appendMarked(node.childrenKeys[i])
        }

        for (var j = 0; j < __rootKeys.length; ++j)
            appendMarked(__rootKeys[j])
    }

    function appendVisibleBranch(node) {
        appendVisibleNode(node)

        if (!node.expanded)
            return

        for (var i = 0; i < node.childrenKeys.length; ++i) {
            var child = __nodes[node.childrenKeys[i]]
            if (child)
                appendVisibleBranch(child)
        }
    }

    function appendVisibleNode(node) {
        var row = __visibleKeys.length
        __visibleKeys.push(node.key)
        __visibleRowsByKey[node.key] = row
        visibleModel.append(toVisibleObject(node))
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
            return listView.width
        return column && column.width !== undefined && column.width !== null ? column.width : 120
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

        var row = visibleRowOf(keyValue)
        if (row < 0)
            return

        node.expanded = true
        __expandedState[keyValue] = true
        writeBackNode(node)
        visibleModel.setProperty(row, "expanded", true)

        var inserted = []
        flattenExpanded(node, inserted)
        var insertIndex = row + 1

        for (var i = 0; i < inserted.length; ++i) {
            var child = inserted[i]
            __visibleKeys.splice(insertIndex + i, 0, child.key)
            visibleModel.insert(insertIndex + i, toVisibleObject(child))
        }

        rebuildVisibleRowsFrom(insertIndex)
    }

    function collapseNode(keyValue) {
        var node = __nodes[keyValue]
        if (!node || !node.expanded)
            return

        var row = visibleRowOf(keyValue)
        if (row < 0)
            return

        node.expanded = false
        __expandedState[keyValue] = false
        writeBackNode(node)
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
            if (node && node.childrenKeys.length > 0) {
                node.expanded = true
                __expandedState[nodeKey] = true
            }
        }
        buildVisibleTree()
    }

    function collapseAll() {
        for (var nodeKey in __nodes) {
            var node = __nodes[nodeKey]
            if (node && node.childrenKeys.length > 0) {
                node.expanded = false
                __expandedState[nodeKey] = false
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
            commitEdit()

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
            value = cellValue(keyValue, column)

        value = normalizeEditorValue(value, column)
        clearEditState()

        if (value === oldValue)
            return

        setCellValue(keyValue, column, value)
        node = __nodes[keyValue]
        if (!node)
            return

        var indexObject = createIndex(node)
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
    }

    function normalizeEditorValue(value, column) {
        var typeName = columnType("", column)
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
        if (row >= 0)
            listView.positionViewAtIndex(row, ListView.Contain)
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
        syncAllVisibleChecked()
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
        syncAllVisibleChecked()
        checkedItemsChanged()
    }

    function syncAllVisibleChecked() {
        for (var i = 0; i < __visibleKeys.length; ++i) {
            var node = __nodes[__visibleKeys[i]]
            if (node)
                visibleModel.setProperty(i, "checked", node.checked)
        }
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
