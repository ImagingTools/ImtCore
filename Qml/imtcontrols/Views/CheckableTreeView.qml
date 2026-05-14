import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcontrols 1.0

FocusScope {
    id: root;

    width: parent ? parent.width : 0;
    height: treeView.contentHeight;
    clip: true;

    property bool tristate: false;
    property bool readOnly: false;
    property var rowModel: ListModel {};
    property var columnModel: ListModel {};
    property int rowItemHeight: Style.controlHeightM;
    property string filterText: "";

    signal checkedItemsChanged();

    property var allNodes: [];
    property var treeModel: [];

    function getItemsDataAsList() { return allNodes; }

    function getCheckedItems() {
        var result = [];
        for (var i = 0; i < allNodes.length; i++)
            if (allNodes[i].checkState === Qt.Checked) result.push(allNodes[i]);
        return result;
    }

    function checkItem(node) {
        if (!node || node.checkState === Qt.Checked) return;
        node.checkState = Qt.Checked;
        if (tristate) {
            setChildState(node, Qt.Checked);
            updateParentState(node);
        }
        checkedItemsChanged();
    }

    function uncheckItem(node) {
        if (!node || node.checkState === Qt.Unchecked) return;
        node.checkState = Qt.Unchecked;
        if (tristate) {
            setChildState(node, Qt.Unchecked);
            updateParentState(node);
        }
        checkedItemsChanged();
    }

    function checkAll() {
        var changed = false;
        for (var i = 0; i < allNodes.length; i++) {
            if (allNodes[i].checkState !== Qt.Checked) { allNodes[i].checkState = Qt.Checked; changed = true; }
        }
        if (changed) checkedItemsChanged();
    }

    function uncheckAll() {
        var changed = false;
        for (var i = 0; i < allNodes.length; i++) {
            if (allNodes[i].checkState !== Qt.Unchecked) { allNodes[i].checkState = Qt.Unchecked; changed = true; }
        }
        if (changed) checkedItemsChanged();
    }

    function setChildState(node, state) {
        for (var i = 0; i < node.children.length; i++) {
            if (node.children[i].isCheckable) node.children[i].checkState = state;
            setChildState(node.children[i], state);
        }
    }

    function updateParentState(node) {
        var p = node.parentNode;
        if (!p || !p.isCheckable) return;
        var allChecked = true, allUnchecked = true;
        for (var i = 0; i < p.children.length; i++) {
            if (p.children[i].checkState !== Qt.Checked) allChecked = false;
            if (p.children[i].checkState !== Qt.Unchecked) allUnchecked = false;
        }
        p.checkState = allChecked ? Qt.Checked : (allUnchecked ? Qt.Unchecked : Qt.PartiallyChecked);
        updateParentState(p);
    }

    function matchesFilter(item, filter) {
        if (!filter) return true;
        for (var c = 0; c < columnModel.count; c++) {
            var v = item[columnModel.get(c).id];
            if (v && v.toString().toLowerCase().indexOf(filter) >= 0) return true;
        }
        return false;
    }

    function subtreeHasMatch(sourceModel, filter) {
        for (var i = 0; i < sourceModel.count; i++) {
            var item = sourceModel.get(i);
            if (matchesFilter(item, filter)) return true;
            if (item.ChildModel && item.ChildModel.count > 0 && subtreeHasMatch(item.ChildModel, filter)) return true;
        }
        return false;
    }

    function buildTree(sourceModel, parentNode, filter) {
        var nodes = [];
        for (var i = 0; i < sourceModel.count; i++) {
            var item = sourceModel.get(i);
            var selfMatch = matchesFilter(item, filter);
            var childMatch = item.ChildModel && item.ChildModel.count > 0 && subtreeHasMatch(item.ChildModel, filter);
            if (filter && !selfMatch && !childMatch) continue;

            var text = "";
            for (var c = 0; c < columnModel.count; c++) {
                var v = item[columnModel.get(c).id];
                if (v !== undefined) text += (text ? " | " : "") + v;
            }

            var node = {
                displayText: text,
                nodeIndex: allNodes.length,
                sourceItem: item,
                checkState: item.CheckState !== undefined ? item.CheckState : Qt.Unchecked,
                isCheckable: item.CheckBoxVisible !== undefined ? item.CheckBoxVisible : true,
                isActive: item.Active !== undefined ? item.Active : true,
                isVisible: true,
                isOpened: item.IsOpen !== undefined ? item.IsOpen : true,
                parentNode: parentNode,
                hasChild: false,
                children: [],
                getItemData: function() { return this.sourceItem; }
            };
            node.getItemData = node.getItemData.bind(node);
            allNodes.push(node);

            if (item.ChildModel && item.ChildModel.count > 0) {
                node.children = buildTree(item.ChildModel, node, filter);
                node.hasChild = node.children.length > 0;
            }

            nodes.push(node);
        }
        return nodes;
    }

    function rebuild() {
        allNodes = [];
        treeModel = rowModel ? buildTree(rowModel, null, filterText.toLowerCase()) : [];
        treeView.expandRecursively();
    }

    function toggleCheck(nodeIndex) {
        var node = allNodes[nodeIndex];
        node.checkState === Qt.Checked ? uncheckItem(node) : checkItem(node);
        treeModel = treeModel.slice();
        treeView.expandRecursively();
    }

    onRowModelChanged: rebuild();

    TreeView {
        id: treeView;

        width: parent.width;
        height: contentHeight;

        model: root.treeModel;
        rowHeightProvider: function() { return root.rowItemHeight; };

        delegate: Item {
            id: nodeDelegate;

            required property bool isTreeNode;
            required property bool expanded;
            required property bool hasChildren;
            required property int depth;
            required property var treeView;
            required property var model;
            required property int row;

            implicitWidth: treeView.width;
            implicitHeight: root.rowItemHeight;

            property var node: (model.nodeIndex !== undefined && model.nodeIndex >= 0 && model.nodeIndex < root.allNodes.length) ? root.allNodes[model.nodeIndex] : null;

            Row {
                id: nodeRow;

                anchors.fill: parent;
                leftPadding: nodeDelegate.depth * 20;
                spacing: Style.spacingM;

                Item {
                    width: 15;
                    height: nodeRow.height;

                    Button {
                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.horizontalCenter: parent.horizontalCenter;
                        width: parent.width;
                        height: width;

                        visible: nodeDelegate.hasChildren;

                        iconSource: nodeDelegate.expanded
                            ? "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal)
                            : "../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal);

                        onClicked: nodeDelegate.treeView.toggleExpanded(nodeDelegate.row);

                        decorator: Component {
                            ButtonDecorator {
                                color: parent.hovered ? Style.buttonHoverColor : "transparent";
                                border.width: 0;
                            }
                        }
                    }
                }

                Item {
                    width: checkBox.visible ? 15 : 0;
                    height: nodeRow.height;

                    CheckBox {
                        id: checkBox;

                        anchors.verticalCenter: parent.verticalCenter;
                        anchors.horizontalCenter: parent.horizontalCenter;

                        checkState: nodeDelegate.node ? nodeDelegate.node.checkState : Qt.Unchecked;
                        isActive: nodeDelegate.node ? (nodeDelegate.node.isActive && !root.readOnly) : !root.readOnly;
                        visible: root.tristate && (nodeDelegate.node ? nodeDelegate.node.isCheckable : true);

                        function nextCheckState() {
                            if (nodeDelegate.node) root.toggleCheck(nodeDelegate.node.nodeIndex);
                        }
                    }
                }

                Item {
                    width: nodeRow.width - nodeRow.leftPadding - 15 - (checkBox.visible ? 15 : 0) - Style.spacingM * 2;
                    height: nodeRow.height;

                    Text {
                        anchors.verticalCenter: parent.verticalCenter;
                        width: parent.width;

                        font.family: Style.fontFamily;
                        font.pixelSize: Style.fontSizeM;
                        color: nodeDelegate.node ? (nodeDelegate.node.isActive ? Style.textColor : Style.inactiveTextColor) : Style.textColor;

                        wrapMode: Text.WordWrap;
                        elide: Text.ElideRight;

                        text: nodeDelegate.model.displayText !== undefined ? nodeDelegate.model.displayText : "";
                    }
                }
            }
        }
    }
}
