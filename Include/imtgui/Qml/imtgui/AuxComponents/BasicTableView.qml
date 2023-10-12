import QtQuick 2.12
import Acf 1.0

FocusScope {
    id: tableViewRoot;

    clip: true;

    property bool headerVisible: true;
    property bool contentVisible: true;
    property bool readOnly: false;
    property bool withoutSelection: false;

    property bool checkable: false;

    property alias rowDelegate: listView.delegate;
    property alias background: backgroundLoader.sourceComponent;

    property int columnCount:repeater.count;
    property int rowCount: listView.count;

    property int rowItemHeight: 30;
    property int headerHeight: 35;

    property var rowModel: ListModel {};

    onRowModelChanged: {
        //console.log("onRowModelChanged", rowModel.toJSON());
        tableViewRoot.itemsList = []
    }

    property var columnModel: ListModel {};

    property ModelIndex selectedIndex: null;

    property TreeViewSelection tableSelection: TreeViewSelection {
        onSelectionChanged: {
            if (tableViewRoot.tableSelection.items.length > 0){
                tableViewRoot.selectedIndex = tableViewRoot.tableSelection.items[0];
            }
            else{
                tableViewRoot.selectedIndex = null;
            }

            tableViewRoot.selectionChanged();
        }
    };

    property alias tableListView: flick;

    signal selectionChanged();
    signal rowModelDataChanged(var delegate, var prop);
    signal rowAdded();
    signal rowRemoved();
    signal checkedItemsChanged();

    property var itemsList: [];

    function getCheckedItems(){
        let result = []
        let delegates = tableViewRoot.getItemsDataAsList();
        for (let delegate of delegates){
            if (delegate.checkState === Qt.Checked){
                result.push(delegate);
            }
        }
        return result;
    }

    function checkItem(delegate){
//        let delegate = tableViewRoot.getItemByIndex(modelIndex);
        if (delegate){
            if (delegate.checkState !== Qt.Checked){
                delegate.checkState = Qt.Checked;

                if (delegate.parentDelegate){
                    delegate.parentDelegate.childrenCheckStateChanged(delegate);
                }

                for (let childDelegate of delegate.childrenDelegates){
                    childDelegate.parentCheckStateChanged(delegate);
                }

                tableViewRoot.checkedItemsChanged();
            }
        }
    }

    function uncheckItem(delegate){
//        let delegate = tableViewRoot.getItemByIndex(modelIndex);
        if (delegate){
            if (delegate.checkState !== Qt.Unchecked){
                delegate.checkState = Qt.Unchecked;
                if (delegate.parentDelegate){
                    delegate.parentDelegate.childrenCheckStateChanged(delegate);
                }

                for (let childDelegate of delegate.childrenDelegates){
                    childDelegate.parentCheckStateChanged(delegate);
                }

                tableViewRoot.checkedItemsChanged();
            }
        }
    }

    function checkAll(){
        let ok = false;
        let delegates = tableViewRoot.getItemsDataAsList();
        for (let delegate of delegates){
            if (delegate.checkState !== Qt.Checked){
                delegate.checkState = Qt.Checked;
                ok = true;
            }
        }

        if (ok){
            tableViewRoot.checkedItemsChanged();
        }
    }

    function uncheckAll(){
        let ok = false;
        let delegates = tableViewRoot.getItemsDataAsList();
        for (let delegate of delegates){
            if (delegate.checkState !== Qt.Unchecked){
                delegate.checkState = Qt.Unchecked;
                ok = true;
            }
        }

        if (ok){
            tableViewRoot.checkedItemsChanged();
        }
    }

//    function __checkItems(delegates, state){
//        for (let delegate of delegates){
//            delegate.checkState = state;
//            if (delegate.childrenDelegates.length > 0){
//                tableViewRoot.__checkItems(delegate.childrenDelegates, state);
//            }
//        }
//    }

//    function __childrenChanged(state){

//    }

    function _addItem(item){
        tableViewRoot.itemsList.push(item);
    }

    function _removeItem(item){
        var index = tableViewRoot.itemsList.indexOf(item);
        if (index !== -1) {
            tableViewRoot.itemsList.splice(index, 1);
        }
    }

    function getItemsDataAsList(){
        return itemsList;
    }

//    function getCheckedItems(){
//        let result = []

//        for (let i = 0; i < tableViewRoot.itemsList.length; i++){
//            let itemData = tableViewRoot.itemsList[i];
//            if (itemData.CheckState === Qt.Checked){
//                result.push(itemData);
//            }
//        }

//        return result;
//    }

    function resetSelection(){
        tableViewRoot.tableSelection.resetSelection();
    }

    function singleSelect(item){
        tableViewRoot.tableSelection.singleSelect(item);
    }

    function addChildItem(modelIndex, rowObj){
        console.log("addChildItem", modelIndex, rowObj);

        if (!rowObj){
            return false;
        }

        let treeModel;
        let currentIndex = 0;

        if (modelIndex){
            treeModel = modelIndex.getModel();
            currentIndex = modelIndex.index;
        }
        else{
            treeModel = rowModel;
        }

        let childModel = modelIndex.getData("ChildModel");

        if (!childModel){
            childModel = treeModel.AddTreeModel("ChildModel", currentIndex);
        }

        if (!childModel){
            return false;
        }

        console.log("childModel1", childModel.toJSON());

        childModel.InsertNewItemWithParameters(0, rowObj)

        console.log("childModel2", childModel.toJSON());

        return false;
    }

    function removeChildItem(modelIndex){
        if (modelIndex){
           let treeModel = modelIndex.getModel();
            if (!treeModel){
                return false;
            }

            let currentIndex = modelIndex.index;

            treeModel.RemoveItem(currentIndex);

            return true;
        }

        return false;
    }

//    function addChildItem(parentIndex, rowObj){
//        let count = tableViewRoot.rowModel.GetItemsCount();
//        if (parentIndex !== null){
//            let parentModel = parentIndex.getParentModel();
//            let childModel = parentIndex.getData("ChildModel");
//            if (!childModel){
//                if (parentModel){
//                    childModel = parentModel.AddTreeModel("ChildModel", parentIndex.getIndex());
//                }
//                else{
//                    parentModel = tableViewRoot.rowModel;
//                    childModel = tableViewRoot.rowModel.AddTreeModel("ChildModel", parentIndex.getIndex());
//                }
//            }
//            childModel.InsertNewItemWithParameters(count, rowObj)
//        }
//        else{
//            tableViewRoot.rowModel.InsertNewItemWithParameters(count, rowObj)
//        }
//    }

    function removeByIndex(modelIndex){
        for (let i = 0; i < tableViewRoot.itemsList.length; i++){
            let delegateItem = tableViewRoot.itemsList[i];
            if (delegateItem.modelIndex === modelIndex){
                let parentModel = modelIndex.getParentModel();
                if (!parentModel){
                    parentModel = tableViewRoot.rowModel;
                }
                parentModel.RemoveItem(modelIndex.getIndex());
                return true;
            }
        }

        return false;
    }

    function getItemByIndex(index){
        let delegates = tableViewRoot.getItemsDataAsList();
        for (let delegate of delegates){
            if (delegate.modelIndex.equal(index)){
                return delegate;
            }
        }

        return null;
    }

    Loader {
        id: backgroundLoader;

        anchors.fill: parent;

        sourceComponent: Rectangle {
            color: Style.baseColor;
        }
    }

    function getRootItemByIndex(index){
        if (index < 0 || index >= listView.count){
            return null;
        }

        return listView.itemAt(index);
    }

    FocusScope {
        id: headerItem;

        z: 5;

        height: visible ? tableViewRoot.headerHeight : 0;
        width: flick.width;

        visible: tableViewRoot.headerVisible;

        property alias headerBackground: headerBackgroundLoader.sourceComponent;

        MouseArea {
            anchors.fill: parent;

            onClicked: {
                headerItem.focus = true;
            }
        }

        Loader {
            id: headerBackgroundLoader;

            anchors.fill: parent;

            sourceComponent: Rectangle {
                id: headerBackground;

                color: Style.alternateBaseColor;

                border.width: 1;
                border.color: Style.imagingToolsGradient2;
            }
        }

        Row {
            id: headerRow;

            focus: true;

            Repeater {
                id: repeater;

                model: tableViewRoot.columnModel;

                delegate: Item {
                    id: repeaterItem;

                    width: tableViewRoot.width / tableViewRoot.columnCount;
                    height: tableViewRoot.headerHeight;

                    Text {
                        id: headerText;

                        anchors.left: repeaterItem.left;
                        anchors.leftMargin: 10;
                        anchors.verticalCenter: repeaterItem.verticalCenter;

                        Component.onCompleted: {
                            headerText.text =  model.Name;
                        }

                        width: parent.width;

                        font.pixelSize: Style.fontSize_common;
                        font.family: Style.fontFamilyBold;
                        font.bold: true;
                        color: Style.textColor;

                        wrapMode: Text.WordWrap;
                        elide: Text.ElideRight;
                    }
                }
            }
        }
    }

    function flickableToEnd(){
        flick.contentY = flick.contentHeight;
    }

    property alias flickable: flick;

    Flickable {
        id: flick;

        anchors.top: headerItem.bottom;
        anchors.bottom: parent.bottom;
        width: parent.width;

        contentWidth: bodyColumn.width;
        contentHeight: bodyColumn.height;

        boundsBehavior: Flickable.StopAtBounds;

        visible: tableViewRoot.contentVisible;

        Column {
            id: bodyColumn;

            width: parent.width;

            Repeater{
                id: listView;

                anchors.fill: parent;

                model: tableViewRoot.rowModel;

                delegate: TableViewItemDelegateBase {
                    root: tableViewRoot;
                }
            }
        }
    }

    CustomScrollbar {
        id: scrollbar;

        z: 100;

        anchors.right: flick.right;
        anchors.top: flick.top;

        secondSize: 10;
        targetItem: flick;
    }

    function addRow(row){
        return insertRow(rowModel.count, row);
    }

    function addColumn(column) {
        return tableViewRoot.insertColumn(columnModel.count, column);
    }

    function insertColumn(index, column){
        columnModel.insert(index, column);
    }

    function insertRow(index, row){
        row["Level"] = 0;

        let hasKey = "Visible" in row;
        if (!hasKey){
            row["Visible"] = true;
        }

        hasKey = "Active" in row;
        if (!hasKey){
            row["Active"] = true;
        }

        hasKey = "Selected" in row;
        if (!hasKey){
            row["Selected"] = false;
        }

        rowModel.insert(index, row);

        rowAdded();
    }

    function removeColumn(index){
        if (index < 0 || index >= columnCount) {
            console.warn("TableView::removeColumn(): invalid argument")
            return
        }

        columnModel.remove(index);
    }

    function removeRow(index){
        if (index < 0 || index >= rowCount) {
            console.warn("TableView::removeRow(): invalid argument")
            return
        }

        rowModel.remove(index);

        tableViewRoot.rowRemoved();
    }

    function __checkColumn(columnId){
        for (let i = 0; i < columnModel.count; i++){
            let id = columnModel.get(i).Id;
            if (columnId == id){
                return true;
            }
        }

        return false;
    }
}
