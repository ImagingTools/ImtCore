import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtgui 1.0
import imtcontrols 1.0

ElementView {
    id: root

    /*
        Exposes a backward-compatible treeView object for callers
        (RoleView.qml, TenantEditor.qml) that use the old API:
            treeView.tristate = true
            treeView.columnModel = headerModel
            treeView.rowModel = dataModel
            treeView.uncheckAll()
            treeView.checkItem(delegateItem)
            treeView.getCheckedItems()
            treeView.getItemsDataAsList()
            onCheckedItemsChanged
    */
    property alias treeView: bridge

    bottomComp: treeViewComp

    QtObject {
        id: bridge

        property bool tristate: false
        property var columnModel: null
        property var rowModel: null

        signal checkedItemsChanged()

        function uncheckAll() {
            checkableTree.uncheckAll()
        }

        function checkItem(legacyItem) {
            if (!legacyItem)
                return
            var key = legacyItem.key !== undefined ? legacyItem.key : ""
            if (key.length > 0)
                checkableTree.checkItem(key)
        }

        function getCheckedItems() {
            var nodes = checkableTree.getCheckedNodes()
            var result = []
            for (var i = 0; i < nodes.length; ++i) {
                result.push(__wrapIndex(nodes[i]))
            }
            return result
        }

        function getItemsDataAsList() {
            var nodes = checkableTree.allNodes()
            var result = []
            for (var i = 0; i < nodes.length; ++i) {
                result.push(__wrapIndex(nodes[i]))
            }
            return result
        }

        function __wrapIndex(idx) {
            return {
                key: idx.key,
                hasChild: idx.item && idx.item.children && idx.item.children.length > 0,
                getItemData: function() { return idx.data || {} }
            }
        }

        onColumnModelChanged: bridge.__rebuildModel()
        onRowModelChanged: bridge.__rebuildModel()

        function __rebuildModel() {
            if (!bridge.rowModel || !bridge.columnModel)
                return

            var displayColumn = __findDisplayColumn()
            var childModelKey = __findChildModelKey()

            var nodes = TreeModelBuilder.fromTreeItemModel(
                bridge.rowModel,
                function(wrapper, index) {
                    var text = displayColumn.length > 0 ? wrapper.data(displayColumn, "") : ""
                    var nodeData = {}

                    // Collect all data fields from columnModel
                    var colCount = TreeModelBuilder.treeItemCount(bridge.columnModel)
                    for (var c = 0; c < colCount; ++c) {
                        var colId = __getColumnData("id", c)
                        if (colId && colId.length > 0) {
                            nodeData[colId] = wrapper.data(colId, "")
                        }
                    }

                    return {
                        key: nodeData["FeatureId"] || text || "",
                        text: text,
                        checkable: true,
                        expanded: true,
                        data: nodeData
                    }
                },
                function(wrapper, index) {
                    return childModelKey.length > 0 ? wrapper.childModel(childModelKey) : null
                }
            )

            checkableTree.model = nodes
        }

        function __findDisplayColumn() {
            var colCount = TreeModelBuilder.treeItemCount(bridge.columnModel)
            if (colCount > 0) {
                var colId = __getColumnData("id", 0)
                return colId || ""
            }
            return ""
        }

        function __findChildModelKey() {
            return "ChildModel"
        }

        function __getColumnData(role, row) {
            if (!bridge.columnModel)
                return ""
            var fn = bridge.columnModel.GetData || bridge.columnModel.getData
            if (!fn)
                return ""
            var val = fn.call(bridge.columnModel, role, row)
            return val !== undefined && val !== null ? String(val) : ""
        }
    }

    Component {
        id: treeViewComp

        FocusScope {
            width: root.contentWidth
            height: filterInput.height + Style.marginM + checkableTree.height

            SearchTextInput {
                id: filterInput

                width: parent.width
                placeHolderText: qsTr("Filter...")

                onSearchChanged: {
                    checkableTree.filterText = filterInput.text
                }
            }

            BasicTreeView {
                id: checkableTree

                anchors.top: filterInput.bottom
                anchors.topMargin: Style.marginM
                width: parent.width

                showHeader: false
                tristate: bridge.tristate

                onCheckedItemsChanged: bridge.checkedItemsChanged()

                Component.onCompleted: root.treeView = bridge
            }
        }
    }
}
