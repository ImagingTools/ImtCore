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
            height: filterInput.height + Style.marginM + treeViewImpl.contentHeight;

            SearchTextInput {
                id: filterInput;

                width: parent.width;
                placeHolderText: qsTr("Filter...");

                onSearchChanged: {
                    treeViewImpl.filterText = filterInput.text;
                    treeViewImpl.__rebuildFlatModel();
                }
            }

            FocusScope {
                id: treeViewImpl;

                anchors.top: filterInput.bottom;
                anchors.topMargin: Style.marginM;
                width: parent.width;
                height: contentHeight;

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
                property int contentHeight: treeListView.contentHeight;
                property string filterText: "";

                signal checkedItemsChanged();
                signal rowAdded();
                signal rowRemoved();

                // All node objects (flat list of every node in the tree)
                property var __allNodes: [];
                // Flat model for the ListView (only visible/expanded nodes)
                property var __flatModel: ListModel {};

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

                    treeViewImpl.__rebuildFlatModel();
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
                    treeViewImpl.__rebuildFlatModel();
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
                        __syncFlatModelCheckStates();
                        treeViewImpl.checkedItemsChanged();
                    }
                }

                function uncheckItem(node) {
                    if (node && node.checkState !== Qt.Unchecked) {
                        node.checkState = Qt.Unchecked;
                        __propagateCheckDown(node, Qt.Unchecked);
                        __propagateCheckUp(node);
                        __syncFlatModelCheckStates();
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
                        __syncFlatModelCheckStates();
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
                        __syncFlatModelCheckStates();
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

                function __syncFlatModelCheckStates() {
                    for (var i = 0; i < __flatModel.count; i++) {
                        var flatItem = __flatModel.get(i);
                        var node = __allNodes[flatItem.nodeIndex];
                        if (node && flatItem.checkState !== node.checkState) {
                            __flatModel.setProperty(i, "checkState", node.checkState);
                        }
                    }
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

                // --- Internal: build flat model from hierarchical rowModel ---

                function __buildNodes(sourceModel, level, parentNode) {
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

                        if (item.ChildModel && item.ChildModel.count > 0) {
                            __buildNodes(item.ChildModel, level + 1, node);
                        }
                    }
                }

                function __rebuildFlatModel() {
                    __allNodes = [];
                    __flatModel.clear();

                    if (!rowModel) {
                        return;
                    }

                    __buildNodes(rowModel, 0, null);

                    var filterLower = filterText.toLowerCase();

                    for (var i = 0; i < __allNodes.length; i++) {
                        var node = __allNodes[i];
                        var selfMatch = __matchesFilter(node.sourceItem, filterLower);
                        var childMatch = false;
                        if (node.sourceItem.ChildModel && node.sourceItem.ChildModel.count > 0) {
                            childMatch = __subtreeMatchesFilter(node.sourceItem.ChildModel, filterLower);
                        }
                        node.__visible = selfMatch || childMatch;
                    }

                    __appendVisibleNodes(__allNodes, 0);
                }

                function __appendVisibleNodes(nodes, startIdx) {
                    // Walk top-level nodes only (parentNode === null), then recurse for open children
                    var topNodes = [];
                    for (var i = 0; i < nodes.length; i++) {
                        if (!nodes[i].parentNode) {
                            topNodes.push(nodes[i]);
                        }
                    }
                    __flattenNodes(topNodes);
                }

                function __flattenNodes(nodes) {
                    for (var i = 0; i < nodes.length; i++) {
                        var node = nodes[i];
                        if (!node.__visible) {
                            continue;
                        }

                        var nodeIdx = __allNodes.indexOf(node);

                        // Build display text from column model
                        var displayText = "";
                        for (var c = 0; c < columnModel.count; c++) {
                            var colId = columnModel.get(c).id;
                            var val = node.sourceItem[colId];
                            if (val !== undefined) {
                                if (displayText !== "") {
                                    displayText += " | ";
                                }
                                displayText += val;
                            }
                        }

                        __flatModel.append({
                            "nodeIndex": nodeIdx,
                            "level": node.level,
                            "isOpen": node.isOpen,
                            "checkState": node.checkState,
                            "isCheckable": node.isCheckable,
                            "isActive": node.isActive,
                            "hasChild": node.hasChild,
                            "displayText": displayText
                        });

                        if (node.isOpen && node.childNodes.length > 0) {
                            __flattenNodes(node.childNodes);
                        }
                    }
                }

                function __toggleExpand(flatIndex) {
                    var flatItem = __flatModel.get(flatIndex);
                    var node = __allNodes[flatItem.nodeIndex];
                    node.isOpen = !node.isOpen;
                    __rebuildFlatModel();
                }

                function __toggleCheck(flatIndex) {
                    var flatItem = __flatModel.get(flatIndex);
                    var node = __allNodes[flatItem.nodeIndex];
                    if (node.checkState === Qt.Checked) {
                        uncheckItem(node);
                    } else {
                        checkItem(node);
                    }
                }

                Component {
                    id: childModelComponent;
                    ListModel {}
                }

                onRowModelChanged: {
                    __rebuildFlatModel();
                }

                // --- Standard QML ListView for tree display ---
                ListView {
                    id: treeListView;

                    width: parent.width;
                    height: contentHeight;

                    interactive: false;
                    model: treeViewImpl.__flatModel;

                    delegate: Item {
                        id: rowDelegate;

                        width: treeListView.width;
                        height: treeViewImpl.rowItemHeight;

                        Row {
                            id: nodeRow;

                            anchors.fill: parent;
                            leftPadding: model.level * treeViewImpl.shiftLevel;
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

                                    visible: model.hasChild;

                                    iconSource: model.isOpen
                                        ? "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal)
                                        : "../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal);

                                    onClicked: {
                                        treeViewImpl.__toggleExpand(model.index);
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

                                    checkState: model.checkState;
                                    isActive: model.isActive && !treeViewImpl.readOnly;
                                    visible: treeViewImpl.tristate && model.isCheckable;

                                    function nextCheckState() {
                                        treeViewImpl.__toggleCheck(model.index);
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
                                    color: model.isActive ? Style.textColor : Style.inactiveTextColor;

                                    wrapMode: Text.WordWrap;
                                    elide: Text.ElideRight;

                                    text: model.displayText !== undefined ? model.displayText : "";
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
