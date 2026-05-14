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
                property int contentHeight: bodyColumn.height;
                property string filterText: "";

                signal checkedItemsChanged();
                signal rowAdded();
                signal rowRemoved();

                property var __itemsList: [];

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
                    return __itemsList;
                }

                function getCheckedItems() {
                    var result = [];
                    for (var i = 0; i < __itemsList.length; i++) {
                        if (__itemsList[i].checkState === Qt.Checked) {
                            result.push(__itemsList[i]);
                        }
                    }
                    return result;
                }

                function checkItem(delegate) {
                    if (delegate && delegate.checkState !== Qt.Checked) {
                        delegate.checkState = Qt.Checked;
                        if (delegate.parentDelegate) {
                            delegate.parentDelegate.childrenCheckStateChanged(delegate);
                        }
                        for (var i = 0; i < delegate.childrenDelegates.length; i++) {
                            delegate.childrenDelegates[i].parentCheckStateChanged(delegate);
                        }
                        treeViewImpl.checkedItemsChanged();
                    }
                }

                function uncheckItem(delegate) {
                    if (delegate && delegate.checkState !== Qt.Unchecked) {
                        delegate.checkState = Qt.Unchecked;
                        if (delegate.parentDelegate) {
                            delegate.parentDelegate.childrenCheckStateChanged(delegate);
                        }
                        for (var i = 0; i < delegate.childrenDelegates.length; i++) {
                            delegate.childrenDelegates[i].parentCheckStateChanged(delegate);
                        }
                        treeViewImpl.checkedItemsChanged();
                    }
                }

                function checkAll() {
                    var changed = false;
                    for (var i = 0; i < __itemsList.length; i++) {
                        if (__itemsList[i].checkState !== Qt.Checked) {
                            __itemsList[i].checkState = Qt.Checked;
                            changed = true;
                        }
                    }
                    if (changed) {
                        treeViewImpl.checkedItemsChanged();
                    }
                }

                function uncheckAll() {
                    var changed = false;
                    for (var i = 0; i < __itemsList.length; i++) {
                        if (__itemsList[i].checkState !== Qt.Unchecked) {
                            __itemsList[i].checkState = Qt.Unchecked;
                            changed = true;
                        }
                    }
                    if (changed) {
                        treeViewImpl.checkedItemsChanged();
                    }
                }

                function _addItem(item) {
                    __itemsList.push(item);
                }

                function _removeItem(item) {
                    var index = __itemsList.indexOf(item);
                    if (index !== -1) {
                        __itemsList.splice(index, 1);
                    }
                }

                function __checkState(delegates, state) {
                    for (var i = 0; i < delegates.length; i++) {
                        if (delegates[i].checkState !== state) {
                            return false;
                        }
                    }
                    return true;
                }

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

                function __subtreeMatchesFilter(model, filterLower) {
                    if (filterLower === "") {
                        return true;
                    }
                    for (var i = 0; i < model.count; i++) {
                        var item = model.get(i);
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

                Component {
                    id: childModelComponent;
                    ListModel {}
                }

                onRowModelChanged: {
                    treeViewImpl.__itemsList = [];
                }

                // --- Tree body ---
                Column {
                    id: bodyColumn;
                    width: parent.width;

                    Repeater {
                        id: rootRepeater;
                        model: treeViewImpl.rowModel;

                        delegate: treeNodeDelegate;
                    }
                }

                Component {
                    id: treeNodeDelegate;

                    Column {
                        id: nodeColumn;

                        width: treeViewImpl.width;

                        property var parentDelegate: null;
                        property var childrenDelegates: [];
                        property var rootDelegate: nodeColumn.level === 0 ? nodeColumn : (parentDelegate ? parentDelegate.rootDelegate : nodeColumn);

                        property int level: parentDelegate ? parentDelegate.level + 1 : 0;
                        property bool isOpened: model.IsOpen !== undefined ? model.IsOpen : true;
                        property int checkState: model.CheckState !== undefined ? model.CheckState : Qt.Unchecked;
                        property bool isCheckable: model.CheckBoxVisible !== undefined ? model.CheckBoxVisible : true;
                        property bool isActive: model.Active !== undefined ? model.Active : true;
                        property bool hasChild: childRepeater.count > 0;
                        property bool selected: false;
                        property var itemData: model;
                        property string childModelKey: "ChildModel";

                        property bool __matchesFilterSelf: treeViewImpl.__matchesFilter(model, treeViewImpl.filterText.toLowerCase());
                        property bool __subtreeVisible: __matchesFilterSelf ||
                            (model.ChildModel ? treeViewImpl.__subtreeMatchesFilter(model.ChildModel, treeViewImpl.filterText.toLowerCase()) : false);

                        visible: __subtreeVisible;
                        height: __subtreeVisible ? (nodeRow.height + (isOpened ? childColumn.height : 0)) : 0;

                        signal parentCheckStateChanged(var delegate);
                        signal childrenCheckStateChanged(var delegate);

                        onCheckStateChanged: {
                            if (model.CheckState !== undefined) {
                                model.CheckState = nodeColumn.checkState;
                            }
                        }

                        onParentCheckStateChanged: {
                            if (isCheckable) {
                                nodeColumn.checkState = delegate.checkState;
                            }
                            for (var i = 0; i < childrenDelegates.length; i++) {
                                childrenDelegates[i].parentCheckStateChanged(nodeColumn);
                            }
                        }

                        onChildrenCheckStateChanged: {
                            if (isCheckable) {
                                var isAllChecked = treeViewImpl.__checkState(childrenDelegates, Qt.Checked);
                                var isAllUnchecked = treeViewImpl.__checkState(childrenDelegates, Qt.Unchecked);

                                if (isAllChecked) {
                                    if (nodeColumn.checkState !== Qt.Checked) {
                                        nodeColumn.checkState = Qt.Checked;
                                    }
                                } else if (isAllUnchecked) {
                                    if (nodeColumn.checkState !== Qt.Unchecked) {
                                        nodeColumn.checkState = Qt.Unchecked;
                                    }
                                } else {
                                    if (nodeColumn.checkState !== Qt.PartiallyChecked) {
                                        nodeColumn.checkState = Qt.PartiallyChecked;
                                    }
                                }
                            }
                            if (nodeColumn.parentDelegate) {
                                nodeColumn.parentDelegate.childrenCheckStateChanged(nodeColumn);
                            }
                        }

                        function getItemData() {
                            return nodeColumn.itemData;
                        }

                        Component.onCompleted: {
                            treeViewImpl._addItem(nodeColumn);
                        }

                        Component.onDestruction: {
                            treeViewImpl._removeItem(nodeColumn);
                        }

                        // --- Single row ---
                        Row {
                            id: nodeRow;

                            width: parent.width;
                            height: treeViewImpl.rowItemHeight;

                            leftPadding: nodeColumn.level * treeViewImpl.shiftLevel;
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

                                    visible: nodeColumn.hasChild;

                                    iconSource: nodeColumn.isOpened
                                        ? "../../../" + Style.getIconPath("Icons/Down", Icon.State.On, Icon.Mode.Normal)
                                        : "../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal);

                                    onClicked: {
                                        nodeColumn.isOpened = !nodeColumn.isOpened;
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

                                    checkState: nodeColumn.checkState;
                                    isActive: nodeColumn.isActive && !treeViewImpl.readOnly;
                                    visible: treeViewImpl.tristate && nodeColumn.isCheckable;

                                    function nextCheckState() {
                                        if (nodeColumn.checkState === Qt.Checked) {
                                            treeViewImpl.uncheckItem(nodeColumn);
                                        } else {
                                            treeViewImpl.checkItem(nodeColumn);
                                        }
                                    }
                                }
                            }

                            // Text columns
                            Repeater {
                                model: treeViewImpl.columnModel;

                                delegate: Item {
                                    width: treeViewImpl.columnModel.count > 0
                                        ? (treeViewImpl.width - nodeColumn.level * treeViewImpl.shiftLevel - treeViewImpl.__prefixWidth - Style.spacingM * 2) / treeViewImpl.columnModel.count
                                        : 0;
                                    height: nodeRow.height;

                                    Text {
                                        anchors.verticalCenter: parent.verticalCenter;
                                        width: parent.width;

                                        font.family: Style.fontFamily;
                                        font.pixelSize: Style.fontSizeM;
                                        color: nodeColumn.isActive ? Style.textColor : Style.inactiveTextColor;

                                        wrapMode: Text.WordWrap;
                                        elide: Text.ElideRight;

                                        text: {
                                            var colId = model.id;
                                            var val = nodeColumn.itemData[colId];
                                            return val !== undefined ? val : "";
                                        }
                                    }
                                }
                            }
                        }

                        // --- Children ---
                        Column {
                            id: childColumn;

                            visible: nodeColumn.isOpened;
                            width: parent.width;

                            Repeater {
                                id: childRepeater;

                                model: nodeColumn.itemData[nodeColumn.childModelKey]
                                    ? nodeColumn.itemData[nodeColumn.childModelKey]
                                    : 0;

                                delegate: Loader {
                                    width: treeViewImpl.width;
                                    sourceComponent: treeNodeDelegate;

                                    onLoaded: {
                                        item.parentDelegate = nodeColumn;
                                        nodeColumn.childrenDelegates.push(item);
                                    }

                                    Component.onDestruction: {
                                        if (item) {
                                            var idx = nodeColumn.childrenDelegates.indexOf(item);
                                            if (idx > -1) {
                                                nodeColumn.childrenDelegates.splice(idx, 1);
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
