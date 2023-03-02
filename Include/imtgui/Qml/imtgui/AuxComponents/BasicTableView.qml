import QtQuick 2.12
import Acf 1.0

FocusScope {
    id: tableViewRoot;

    clip: true;

    property bool headerVisible: true;
    property bool readOnly: true;
    property bool withoutSelection: false;

    property alias headerDelegate: listView.header;
    property alias rowDelegate: listView.delegate;
    property alias contentHeader: listView.header;
    property alias contentFooter: listView.footer;
    property alias background: backgroundLoader.sourceComponent;
    property alias boundsBehavior: listView.boundsBehavior;

    property int columnCount:repeater.count;
    property int rowCount: listView.count;

    property int rowItemHeight: 30;
    property int headerHeight: 35;

    property var rowModel: ListModel {};
    property var columnModel: ListModel {};

    property ModelIndex selectedIndex: null;

    property TableViewSelection selection: TableViewSelection {};

    property alias tableListView: listView;

    // For subscribing unique events
    property string commandId: "";

    signal rowModelDataChanged(var delegate, var prop);
    signal rowAdded();
    signal rowRemoved();

    Component.onCompleted: {
        tableViewRoot.Keys.onPressed.connect(tableViewRoot.selection.tableKeyPressed);

        tableViewRoot.selection.selectedIndex = tableViewRoot.selectedIndex;
    }

    Component.onDestruction: {
        tableViewRoot.Keys.onPressed.disconnect(tableViewRoot.selection.tableKeyPressed);
    }

    Keys.onPressed: {
        console.log("TableView Keys.onPressed");

        if (tableViewRoot.selectedIndex == null){
            return;
        }

        let itemData = tableViewRoot.selectedIndex.itemData;

        if (event.key == Qt.Key_Left){
            itemData.IsOpen = false;
        }
        else if (event.key == Qt.Key_Right){
            console.log("Qt.Key_Right");
            itemData.IsOpen = true;
        }
    }

    Loader {
        id: backgroundLoader;

        anchors.fill: parent;

        sourceComponent: Rectangle {
            color: Style.baseColor;
        }
    }

    MouseArea {
        anchors.fill: listView;

        onClicked: {
            console.log("BG onClicked");
        }
    }

    FocusScope {
        id: headerItem;

        z: 5;

        height: visible ? tableViewRoot.headerHeight : 0;
        width: listView.width;

        visible: tableViewRoot.headerVisible;

        property alias headerBackground: headerBackgroundLoader.sourceComponent;

        onFocusChanged: {
            console.log("headerItem onFocusChanged", focus);
        }

        MouseArea {
            anchors.fill: parent;

            onClicked: {
                console.log("Headers onClicked");

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
                            console.log("Header debug:", model.Name, model.index);
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

    ListView {
        id: listView;

        anchors.top: headerItem.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;

        model: tableViewRoot.rowModel;

        boundsBehavior: Flickable.StopAtBounds;

        headerPositioning: ListView.OverlayHeader;

        delegate: TableViewItemDelegateBase {
            root: tableViewRoot;

            Component.onCompleted: {

            }
        }

        onHeightChanged: {
            console.log("listView onHeightChanged", listView.height);
        }
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

//    function insertColumn(index, column){

//        /*let index = */columnModel.InsertNewItem();

//        for (let key in column){
//            columnModel.SetData(key, column[key], index);
//        }
//    }

//    function insertRow(index, row){
//        rowModel.InsertNewItem();

//        for (let key in row){
//            rowModel.SetData(key, row[key], index);
//        }
//    }

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

    //    function moveColumn(from, to) {
    //        if (from < 0 || from >= columnCount || to < 0 || to >= columnCount) {
    //            console.warn(__viewTypeName + "::moveColumn(): invalid argument")
    //            return
    //        }
    //        if (__isTreeView && to === 0) {
    //            console.warn(__viewTypeName + "::moveColumn(): Can't move column 0")
    //            return
    //        }
    //        if (sortIndicatorColumn === from)
    //            sortIndicatorColumn = to
    //        columnModel.move(from, to, 1)
    //    }

    //    function getColumn(index) {
    //        if (index < 0 || index >= columnCount){
    //            return null
    //        }

    //        return columnModel.GetModelFromItem(index);
    //    }
}
