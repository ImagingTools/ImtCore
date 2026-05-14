import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

ElementView {
    id: root;

    property var treeView;

    bottomComp: treeViewComp;

    Component {
        id: treeViewComp;

        FocusScope {
            id: treeViewContainer;

            width: root.contentWidth;
            height: filterInput.height + Style.marginM + treeViewImpl.height;

            SearchTextInput {
                id: filterInput;

                width: parent.width;
                placeHolderText: qsTr("Filter...");

                onSearchChanged: {
                    treeViewImpl.filterText = filterInput.text;
                    treeViewImpl.__rebuildTreeModel();
                }
            }

            FocusScope {
                id: treeViewImpl;

                anchors.top: filterInput.bottom;
                anchors.topMargin: Style.marginM;
                width: parent.width;
                height: internalTreeView.contentHeight;

                clip: true;

                Component.onCompleted: {
                    root.treeView = treeViewImpl;
                }

                // --- Public API (backward-compatible with BasicTreeView) ---
                property int shiftLevel: 20;
                property int __prefixWidth: 30;
                property bool tristate: false;
                property bool headerVisible: false;
                property bool readOnly: false;
                property var rowModel: ListModel {};
                property var columnModel: ListModel {};
                property int rowItemHeight: Style.controlHeightM;
                property int contentHeight: internalTreeView.contentHeight;
                property string filterText: "";

                signal checkedItemsChanged();
                signal rowAdded();
                signal rowRemoved();

                // All node objects (flat list of every node in the tree)
                property var __allNodes: [];
                // Tree model data for JQML2 TreeView (array of objects with children)
                property var __treeModelData: [];

                // --- Public functions ---
                function addRow(row) {
                    insertRow([rowModel.count], row);
                }

                function insertRow(indexes, row) {
                    if (!indexes || indexes.length === 0) {
                        console.error("TreeView::insertRow() - invalid indexes", indexes);
                        return;
                    }

                    if (!("ChildModel" in row)) {
                        row["ChildModel"] = childModelComponent.createObject(null);
                    }
                    if (!("CheckState" in row)) {
                        row["CheckState"] = Qt.Unchecked;
                    }
                    if (!("Visible" in row)) {
                        row["Visible"] = true;
                    }
                    if (!("Selected" in row)) {
                        row["Selected"] = false;
                    }
                    if (!("Active" in row)) {
                        row["Active"] = true;
                    }
                    if (!("CheckBoxVisible" in row)) {
                        row["CheckBoxVisible"] = true;
                    }
                    if (!("IsOpen" in row)) {
                        row["IsOpen"] = true;
                    }

                    var localModel = rowModel;
                    for (var i = 0; i < indexes.length - 1; i++) {
                        var index = indexes[i];
                        if (localModel.count <= index) {
                            console.error("TreeView::insertRow() - invalid index", index, "from indexes", indexes);
                            return;
                        }
                        localModel = localModel.get(index).ChildModel;
                    }

                    row["Level"] = indexes.length - 1;
                    var lastIndex = indexes[indexes.length - 1];
                    localModel.insert(lastIndex, row);

                    treeViewImpl.__rebuildTreeModel();
                    treeViewImpl.rowAdded();
                }

                function removeRow(indexes) {
                    var localModel = rowModel;
                    for (var i = 0; i < indexes.length - 1; i++) {
                        var index = indexes[i];
                        if (localModel.count <= index) {
                            return;
                        }
                        localModel = localModel.get(index).ChildModel;
                    }
                    var lastIndex = indexes[indexes.length - 1];
                    localModel.remove(lastIndex);
                    treeViewImpl.__rebuildTreeModel();
                    treeViewImpl.rowRemoved();
                }

                function getRow(indexes) {
                    if (!indexes || indexes.length === 0) {
                        return;
                    }
                    var localModel = rowModel;
                    for (var i = 0; i < indexes.length - 1; i++) {
                        var index = indexes[i];
                        if (localModel.count <= index) {
                            return;
                        }
                        localModel = localModel.get(index).ChildModel;
                    }
                    return localModel.get(indexes[indexes.length - 1]);
                }

                function setRow(indexes, row) {
                    if (!indexes || indexes.length === 0) {
                        return;
                    }
                    var localModel = rowModel;
                    for (var i = 0; i < indexes.length - 1; i++) {
                        var index = indexes[i];
                        if (localModel.count <= index) {
                            return;
                        }
                        localModel = localModel.get(index).ChildModel;
                    }
                    localModel.set(indexes[indexes.length - 1], row);
                }

                function getItemsDataAsList() {
                    return __allNodes;
                }

                function getCheckedItems() {
                    var result = [];
                    for (var i = 0; i < __allNodes.length; i++) {
                        if (__allNodes[i].checkState === Qt.Checked) {
                            result.push(__allNodes[i]);
                        }
                    }
                    return result;
                }

                function checkItem(node) {
                    if (node && node.checkState !== Qt.Checked) {
                        node.checkState = Qt.Checked;
                        __propagateCheckDown(node, Qt.Checked);
                        __propagateCheckUp(node);
                        treeViewImpl.checkedItemsChanged();
                    }
                }

                function uncheckItem(node) {
                    if (node && node.checkState !== Qt.Unchecked) {
                        node.checkState = Qt.Unchecked;
                        __propagateCheckDown(node, Qt.Unchecked);
                        __propagateCheckUp(node);
                        treeViewImpl.checkedItemsChanged();
                    }
                }

                function checkAll() {
                    var changed = false;
                    for (var i = 0; i < __allNodes.length; i++) {
                        if (__allNodes[i].checkState !== Qt.Checked) {
                            __allNodes[i].checkState = Qt.Checked;
                            changed = true;
                        }
                    }
                    if (changed) {
                        treeViewImpl.checkedItemsChanged();
                    }
                }

                function uncheckAll() {
                    var changed = false;
                    for (var i = 0; i < __allNodes.length; i++) {
                        if (__allNodes[i].checkState !== Qt.Unchecked) {
                            __allNodes[i].checkState = Qt.Unchecked;
                            changed = true;
                        }
                    }
                    if (changed) {
                        treeViewImpl.checkedItemsChanged();
                    }
                }

                // --- Internal: tree node management ---

                function __propagateCheckDown(node, state) {
                    for (var i = 0; i < node.childNodes.length; i++) {
                        var child = node.childNodes[i];
                        if (child.isCheckable) {
                            child.checkState = state;
                        }
                        __propagateCheckDown(child, state);
                    }
                }

                function __propagateCheckUp(node) {
                    var parent = node.parentNode;
                    if (!parent || !parent.isCheckable) {
                        return;
                    }

                    var allChecked = true;
                    var allUnchecked = true;
                    for (var i = 0; i < parent.childNodes.length; i++) {
                        var cs = parent.childNodes[i].checkState;
                        if (cs !== Qt.Checked) {
                            allChecked = false;
                        }
                        if (cs !== Qt.Unchecked) {
                            allUnchecked = false;
                        }
                    }

                    if (allChecked) {
                        parent.checkState = Qt.Checked;
                    } else if (allUnchecked) {
                        parent.checkState = Qt.Unchecked;
                    } else {
                        parent.checkState = Qt.PartiallyChecked;
                    }

                    __propagateCheckUp(parent);
                }

                // --- Internal: filter matching ---

                function __matchesFilter(itemData, filterLower) {
                    if (filterLower === "") {
                        return true;
                    }
                    for (var i = 0; i < columnModel.count; i++) {
                        var colId = columnModel.get(i).id;
                        var value = itemData[colId];
                        if (value && value.toString().toLowerCase().indexOf(filterLower) >= 0) {
                            return true;
                        }
                    }
                    return false;
                }

                function __subtreeMatchesFilter(sourceModel, filterLower) {
                    if (filterLower === "") {
                        return true;
                    }
                    for (var i = 0; i < sourceModel.count; i++) {
                        var item = sourceModel.get(i);
                        if (__matchesFilter(item, filterLower)) {
                            return true;
                        }
                        if (item.ChildModel && item.ChildModel.count > 0) {
                            if (__subtreeMatchesFilter(item.ChildModel, filterLower)) {
                                return true;
                            }
                        }
                    }
                    return false;
                }

                // --- Internal: build nodes and tree model for JQML2 TreeView ---

                function __buildNodes(sourceModel, level, parentNode) {
                    var result = [];
                    for (var i = 0; i < sourceModel.count; i++) {
                        var item = sourceModel.get(i);
                        var nodeIsOpen = item.IsOpen !== undefined ? item.IsOpen : true;
                        var node = {
                            sourceItem: item,
                            level: level,
                            isOpen: nodeIsOpen,
                            isOpened: nodeIsOpen,
                            checkState: item.CheckState !== undefined ? item.CheckState : Qt.Unchecked,
                            isCheckable: item.CheckBoxVisible !== undefined ? item.CheckBoxVisible : true,
                            isActive: item.Active !== undefined ? item.Active : true,
                            hasChild: false,
                            parentNode: parentNode,
                            childNodes: [],
                            selected: false
                        };

                        node.getItemData = function() { return this.sourceItem; }.bind(node);

                        if (parentNode) {
                            parentNode.childNodes.push(node);
                            parentNode.hasChild = true;
                        }

                        __allNodes.push(node);

                        var childTreeItems = [];
                        if (item.ChildModel && item.ChildModel.count > 0) {
                            childTreeItems = __buildNodes(item.ChildModel, level + 1, node);
                        }

                        // Build display text from column model
                        var displayText = "";
                        for (var c = 0; c < columnModel.count; c++) {
                            var colId = columnModel.get(c).id;
                            var val = item[colId];
                            if (val !== undefined) {
                                if (displayText !== "") {
                                    displayText += " | ";
                                }
                                displayText += val;
                            }
                        }

                        // Create tree item for JQML2 TreeView model (uses 'children' property)
                        var treeItem = {
                            displayText: displayText,
                            nodeIndex: __allNodes.length - 1,
                            checkState: node.checkState,
                            isCheckable: node.isCheckable,
                            isActive: node.isActive,
                            children: childTreeItems
                        };
                        node.__treeItem = treeItem;

                        result.push(treeItem);
                    }
                    return result;
                }

                function __rebuildTreeModel() {
                    __allNodes = [];

                    if (!rowModel) {
                        __treeModelData = [];
                        return;
                    }

                    var filterLower = filterText.toLowerCase();
                    var allItems = __buildNodes(rowModel, 0, null);

                    // Apply filter: mark visibility
                    for (var i = 0; i < __allNodes.length; i++) {
                        var node = __allNodes[i];
                        var selfMatch = __matchesFilter(node.sourceItem, filterLower);
                        var childMatch = false;
                        if (node.sourceItem.ChildModel && node.sourceItem.ChildModel.count > 0) {
                            childMatch = __subtreeMatchesFilter(node.sourceItem.ChildModel, filterLower);
                        }
                        node.__visible = selfMatch || childMatch;
                    }

                    // Build filtered tree model
                    __treeModelData = __filterTreeItems(allItems);

                    // Expand all nodes by default in the TreeView
                    internalTreeView.expandRecursively();
                }

                function __filterTreeItems(items) {
                    var result = [];
                    for (var i = 0; i < items.length; i++) {
                        var item = items[i];
                        var node = __allNodes[item.nodeIndex];
                        if (!node.__visible) {
                            continue;
                        }
                        var filteredChildren = __filterTreeItems(item.children);
                        result.push({
                            displayText: item.displayText,
                            nodeIndex: item.nodeIndex,
                            checkState: node.checkState,
                            isCheckable: node.isCheckable,
                            isActive: node.isActive,
                            children: filteredChildren
                        });
                    }
                    return result;
                }

                function __toggleCheck(nodeIndex) {
                    var node = __allNodes[nodeIndex];
                    if (node.checkState === Qt.Checked) {
                        uncheckItem(node);
                    } else {
                        checkItem(node);
                    }
                    // Rebuild model to update TreeView display
                    __rebuildTreeModel();
                }

                Component {
                    id: childModelComponent;
                    ListModel {}
                }

                onRowModelChanged: {
                    __rebuildTreeModel();
                }

                // --- JQML2 TreeView ---
                TreeView {
                    id: internalTreeView;

                    width: parent.width;
                    height: contentHeight;

                    model: treeViewImpl.__treeModelData;
                    rowHeightProvider: function() { return treeViewImpl.rowItemHeight; };

                    delegate: Item {
                        id: treeDelegate;

                        required property bool isTreeNode;
                        required property bool expanded;
                        required property bool hasChildren;
                        required property int depth;
                        required property var treeView;
                        required property var model;
                        required property int row;

                        implicitWidth: internalTreeView.width;
                        implicitHeight: treeViewImpl.rowItemHeight;

                        Row {
                            id: nodeRow;

                            anchors.fill: parent;
                            leftPadding: treeDelegate.depth * treeViewImpl.shiftLevel;
                            spacing: Style.spacingM;

                            // Expand/collapse arrow
                            Item {
                                width: 15;
                                height: nodeRow.height;

                                Button {
                                    id: arrowButton;

                                    anchors.verticalCenter: parent.verticalCenter;
                                    anchors.horizontalCenter: parent.horizontalCenter;

                                    width: parent.width;
                                    height: width;

                                    visible: treeDelegate.hasChildren;

                                    iconSource: treeDelegate.expanded
                                        ? "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal)
                                        : "../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal);

                                    onClicked: {
                                        treeDelegate.treeView.toggleExpanded(treeDelegate.row);
                                    }

                                    decorator: Component {
                                        ButtonDecorator {
                                            color: parent.hovered ? Style.buttonHoverColor : "transparent";
                                            border.width: 0;
                                        }
                                    }
                                }
                            }

                            // Checkbox
                            Item {
                                width: checkBox.visible ? 15 : 0;
                                height: nodeRow.height;

                                CheckBox {
                                    id: checkBox;

                                    anchors.verticalCenter: parent.verticalCenter;
                                    anchors.horizontalCenter: parent.horizontalCenter;

                                    checkState: (treeDelegate.model.nodeIndex !== undefined && treeDelegate.model.nodeIndex >= 0 && treeDelegate.model.nodeIndex < treeViewImpl.__allNodes.length)
                                        ? treeViewImpl.__allNodes[treeDelegate.model.nodeIndex].checkState
                                        : Qt.Unchecked;
                                    isActive: (treeDelegate.model.nodeIndex !== undefined && treeDelegate.model.nodeIndex >= 0 && treeDelegate.model.nodeIndex < treeViewImpl.__allNodes.length)
                                        ? (treeViewImpl.__allNodes[treeDelegate.model.nodeIndex].isActive && !treeViewImpl.readOnly)
                                        : !treeViewImpl.readOnly;
                                    visible: !treeViewImpl.tristate
                                        ? false
                                        : (treeDelegate.model.nodeIndex !== undefined && treeDelegate.model.nodeIndex >= 0 && treeDelegate.model.nodeIndex < treeViewImpl.__allNodes.length)
                                            ? treeViewImpl.__allNodes[treeDelegate.model.nodeIndex].isCheckable
                                            : true;

                                    function nextCheckState() {
                                        var ni = treeDelegate.model.nodeIndex;
                                        if (ni !== undefined) {
                                            treeViewImpl.__toggleCheck(ni);
                                        }
                                    }
                                }
                            }

                            // Display text
                            Item {
                                width: nodeRow.width - nodeRow.leftPadding - 15 - (checkBox.visible ? 15 : 0) - Style.spacingM * 2;
                                height: nodeRow.height;

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter;
                                    width: parent.width;

                                    font.family: Style.fontFamily;
                                    font.pixelSize: Style.fontSizeM;
                                    color: (treeDelegate.model.nodeIndex !== undefined && treeDelegate.model.nodeIndex >= 0 && treeDelegate.model.nodeIndex < treeViewImpl.__allNodes.length)
                                        ? (treeViewImpl.__allNodes[treeDelegate.model.nodeIndex].isActive ? Style.textColor : Style.inactiveTextColor)
                                        : Style.textColor;

                                    wrapMode: Text.WordWrap;
                                    elide: Text.ElideRight;

                                    text: treeDelegate.model.displayText !== undefined ? treeDelegate.model.displayText : "";
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
