import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

TreeView {
    id: root;

    property int shiftLevel: 15;
    property bool tristate: false;
    property bool readOnly: false;
    property var rowModel: ListModel {};
    property var columnModel: ListModel {};
    property int rowItemHeight: Style.controlHeightM;
    property string filterText: "";

    signal checkedItemsChanged();
    signal rowAdded();
    signal rowRemoved();

    // --- Internal state ---
    property var __allNodes: [];

    // --- Public functions ---

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
        if (!node || node.checkState === Qt.Checked) return;
        node.checkState = Qt.Checked;
        if (node.sourceItem) node.sourceItem.CheckState = Qt.Checked;
        __propagateDown(node, Qt.Checked);
        __propagateUp(node);
        __rebuild();
        checkedItemsChanged();
    }

    function uncheckItem(node) {
        if (!node || node.checkState === Qt.Unchecked) return;
        node.checkState = Qt.Unchecked;
        if (node.sourceItem) node.sourceItem.CheckState = Qt.Unchecked;
        __propagateDown(node, Qt.Unchecked);
        __propagateUp(node);
        __rebuild();
        checkedItemsChanged();
    }

    function checkAll() {
        var changed = false;
        for (var i = 0; i < __allNodes.length; i++) {
            if (__allNodes[i].checkState !== Qt.Checked) {
                __allNodes[i].checkState = Qt.Checked;
                if (__allNodes[i].sourceItem) __allNodes[i].sourceItem.CheckState = Qt.Checked;
                changed = true;
            }
        }
        if (changed) {
            __rebuild();
            checkedItemsChanged();
        }
    }

    function uncheckAll() {
        var changed = false;
        for (var i = 0; i < __allNodes.length; i++) {
            if (__allNodes[i].checkState !== Qt.Unchecked) {
                __allNodes[i].checkState = Qt.Unchecked;
                if (__allNodes[i].sourceItem) __allNodes[i].sourceItem.CheckState = Qt.Unchecked;
                changed = true;
            }
        }
        if (changed) {
            __rebuild();
            checkedItemsChanged();
        }
    }

    // --- Legacy API for backward compatibility ---

    Component {
        id: __modelComponent;
        ListModel {}
    }

    function addRow(row) {
        insertRow([rowModel.count], row);
    }

    function insertRow(indexes, row) {
        if (!indexes || indexes.length === 0) {
            console.error("BasicTreeView::insertRow() - invalid indexes", indexes);
            return;
        }

        if (!("ChildModel" in row)) row["ChildModel"] = __modelComponent.createObject(rowModel);
        if (!("CheckState" in row)) row["CheckState"] = Qt.Unchecked;
        if (!("Visible" in row)) row["Visible"] = true;
        if (!("Selected" in row)) row["Selected"] = false;
        if (!("Active" in row)) row["Active"] = true;
        if (!("CheckBoxVisible" in row)) row["CheckBoxVisible"] = true;
        if (!("IsOpen" in row)) row["IsOpen"] = true;

        var localModel = rowModel;
        for (var i = 0; i < indexes.length - 1; i++) {
            var index = indexes[i];
            if (localModel.count <= index) {
                console.error("BasicTreeView::insertRow() - invalid index", index, "from indexes", indexes);
                return;
            }
            localModel = localModel.get(index).ChildModel;
        }

        row["Level"] = indexes.length - 1;
        localModel.insert(indexes[indexes.length - 1], row);
        __rebuild();
        rowAdded();
    }

    function getRow(indexes) {
        if (!indexes || indexes.length === 0) {
            console.error("BasicTreeView::getRow() - invalid indexes", indexes);
            return;
        }

        var localModel = rowModel;
        for (var i = 0; i < indexes.length - 1; i++) {
            var index = indexes[i];
            if (localModel.count <= index) {
                console.error("BasicTreeView::getRow() - invalid index", index, "from indexes", indexes);
                return;
            }
            localModel = localModel.get(index).ChildModel;
        }

        return localModel.get(indexes[indexes.length - 1]);
    }

    function setRow(indexes, row) {
        if (!indexes || indexes.length === 0) return;

        var localModel = rowModel;
        for (var i = 0; i < indexes.length - 1; i++) {
            var index = indexes[i];
            if (localModel.count <= index) return;
            localModel = localModel.get(index).ChildModel;
        }

        localModel.set(indexes[indexes.length - 1], row);
        __rebuild();
    }

    function removeRow(indexes) {
        if (!indexes || indexes.length === 0) {
            console.error("BasicTreeView::removeRow() - invalid indexes", indexes);
            return;
        }

        var localModel = rowModel;
        for (var i = 0; i < indexes.length - 1; i++) {
            var index = indexes[i];
            if (localModel.count <= index) {
                console.error("BasicTreeView::removeRow() - invalid index", index, "from indexes", indexes);
                return;
            }
            localModel = localModel.get(index).ChildModel;
        }

        localModel.remove(indexes[indexes.length - 1]);
        __rebuild();
        rowRemoved();
    }

    // --- Internal: check propagation ---

    function __propagateDown(node, state) {
        for (var i = 0; i < node.childNodes.length; i++) {
            var child = node.childNodes[i];
            if (child.isCheckable) {
                child.checkState = state;
                if (child.sourceItem) child.sourceItem.CheckState = state;
            }
            __propagateDown(child, state);
        }
    }

    function __propagateUp(node) {
        var p = node.parentNode;
        if (!p || !p.isCheckable) return;

        var allChecked = true;
        var allUnchecked = true;
        for (var i = 0; i < p.childNodes.length; i++) {
            var cs = p.childNodes[i].checkState;
            if (cs !== Qt.Checked) allChecked = false;
            if (cs !== Qt.Unchecked) allUnchecked = false;
        }

        var newState = allChecked ? Qt.Checked : (allUnchecked ? Qt.Unchecked : Qt.PartiallyChecked);
        p.checkState = newState;
        if (p.sourceItem) p.sourceItem.CheckState = newState;
        __propagateUp(p);
    }

    // --- Internal: filter ---

    function __matchesFilter(item, filterLower) {
        if (filterLower === "") return true;
        for (var i = 0; i < columnModel.count; i++) {
            var value = item[columnModel.get(i).id];
            if (value && value.toString().toLowerCase().indexOf(filterLower) >= 0) return true;
        }
        return false;
    }

    function __subtreeMatches(sourceModel, filterLower) {
        for (var i = 0; i < sourceModel.count; i++) {
            var item = sourceModel.get(i);
            if (__matchesFilter(item, filterLower)) return true;
            if (item.ChildModel && item.ChildModel.count > 0 && __subtreeMatches(item.ChildModel, filterLower)) return true;
        }
        return false;
    }

    // --- Internal: build tree model ---

    function __buildTreeItems(sourceModel, level, parentNode, filterLower) {
        var items = [];
        for (var i = 0; i < sourceModel.count; i++) {
            var item = sourceModel.get(i);

            var selfMatch = __matchesFilter(item, filterLower);
            var childMatch = (item.ChildModel && item.ChildModel.count > 0) ? __subtreeMatches(item.ChildModel, filterLower) : false;
            if (filterLower !== "" && !selfMatch && !childMatch) continue;

            var node = {
                sourceItem: item,
                level: level,
                checkState: item.CheckState !== undefined ? item.CheckState : Qt.Unchecked,
                isCheckable: item.CheckBoxVisible !== undefined ? item.CheckBoxVisible : true,
                isActive: item.Active !== undefined ? item.Active : true,
                parentNode: parentNode,
                childNodes: []
            };
            node.getItemData = function() { return this.sourceItem; }.bind(node);

            if (parentNode) parentNode.childNodes.push(node);
            var nodeIndex = __allNodes.length;
            __allNodes.push(node);

            var childTreeItems = [];
            if (item.ChildModel && item.ChildModel.count > 0) {
                childTreeItems = __buildTreeItems(item.ChildModel, level + 1, node, filterLower);
            }

            var displayText = "";
            for (var c = 0; c < columnModel.count; c++) {
                var val = item[columnModel.get(c).id];
                if (val !== undefined) {
                    displayText += (displayText !== "" ? " | " : "") + val;
                }
            }

            items.push({
                displayText: displayText,
                nodeIndex: nodeIndex,
                children: childTreeItems
            });
        }
        return items;
    }

    function __rebuild() {
        __allNodes = [];
        root.model = rowModel ? __buildTreeItems(rowModel, 0, null, filterText.toLowerCase()) : [];
        root.expandRecursively();
    }

    function __toggleCheck(nodeIndex) {
        var node = __allNodes[nodeIndex];
        if (node.checkState === Qt.Checked) {
            uncheckItem(node);
        } else {
            checkItem(node);
        }
    }

    function __checkState(delegates, state) {
        var result = true;
        for (var i = 0; i < delegates.length; i++) {
            if (delegates[i].checkState !== state) {
                result = false;
                break;
            }
        }
        return result;
    }

    onRowModelChanged: __rebuild();
    onFilterTextChanged: __rebuild();

    // --- TreeView configuration ---

    height: contentHeight;
    rowHeightProvider: function() { return root.rowItemHeight; };

    delegate: Item {
        id: treeDelegate;

        required property bool isTreeNode;
        required property bool expanded;
        required property bool hasChildren;
        required property int depth;
        required property var treeView;
        required property var model;
        required property int row;

        implicitWidth: root.width;
        implicitHeight: root.rowItemHeight;

        Row {
            id: nodeRow;

            anchors.fill: parent;
            leftPadding: treeDelegate.depth * root.shiftLevel;
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

                    onClicked: treeDelegate.treeView.toggleExpanded(treeDelegate.row);

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

                    property var __node: (treeDelegate.model.nodeIndex !== undefined && treeDelegate.model.nodeIndex >= 0 && treeDelegate.model.nodeIndex < root.__allNodes.length) ? root.__allNodes[treeDelegate.model.nodeIndex] : null;

                    checkState: __node ? __node.checkState : Qt.Unchecked;
                    isActive: __node ? (__node.isActive && !root.readOnly) : !root.readOnly;
                    visible: root.tristate && (__node ? __node.isCheckable : true);

                    function nextCheckState() {
                        if (treeDelegate.model.nodeIndex !== undefined) {
                            root.__toggleCheck(treeDelegate.model.nodeIndex);
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
                    color: checkBox.__node ? (checkBox.__node.isActive ? Style.textColor : Style.inactiveTextColor) : Style.textColor;

                    wrapMode: Text.WordWrap;
                    elide: Text.ElideRight;

                    text: treeDelegate.model.displayText !== undefined ? treeDelegate.model.displayText : "";
                }
            }
        }
    }
}
