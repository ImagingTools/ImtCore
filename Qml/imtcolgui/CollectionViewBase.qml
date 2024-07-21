import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0

ViewBase {
    id: collectionViewBaseContainer;

    clip: true;

    property alias table: tableInternal;
    property bool hasPagination: true;
    property bool hasFilter: true;

    property bool hasSort: true;
    property alias isMultiCheckable: tableInternal.isMultiCheckable;

    property alias filterMenu: filterMenu_;
    property alias loading: loading_;
    property alias filterMenuVisible: filterMenu_.visible;
    property alias pagination: pagination_;
    property alias elementsCount: tableInternal.elementsCount;

    property CollectionFilter collectionFilter: CollectionFilter {}

    signal selectedIndexChanged(int index);
    signal tableViewParamsAccepted();

    signal elementsChanged();
    signal headersChanged();

    signal filterChanged(string filterId, var filterValue);

    signal selectionChanged(var selection);
    signal checkedItemsChanged();
    signal rightButtonMouseClicked(int mouseX, int mouseY);
    signal doubleClicked(string id, int index);

    Component.onCompleted: {
        tableInternal.focus = true;
    }

    Connections {
        target: collectionViewBaseContainer.collectionFilter;

        function onFilterChanged(){
            tableInternal.currentHeaderId = collectionViewBaseContainer.collectionFilter.getSortingInfoId();
            tableInternal.currentSortOrder = collectionViewBaseContainer.collectionFilter.getSortingOrder();
        }
    }

    function onModelChanged(){}

    FilterMenu {
        id: filterMenu_;

        anchors.top: parent.top;
        anchors.topMargin: Style.margin;
        anchors.left: parent.left;
        anchors.leftMargin: Style.margin;
        anchors.right: parent.right;
        anchors.rightMargin: Style.margin;

        onClose: {
            filterMenu_.visible = false;
        }

        onFilterChanged: {
            if (filterId == "TextFilter"){
                collectionViewBaseContainer.collectionFilter.setTextFilter(filterValue);
                collectionViewBaseContainer.doUpdateGui();
            }
            else if (filterId == "TimeFilter"){
                collectionViewBaseContainer.collectionFilter.setTimeFilter(filterValue);
                collectionViewBaseContainer.doUpdateGui();
            }
            else{
                collectionViewBaseContainer.filterChanged(filterId, filterValue);
            }
        }
    }

    onHeadersChanged: {
        table.tableViewParams.clear();

        for (let i = 0; i < table.headers.getItemsCount(); i++){
            let headerId = table.headers.getData("Id", i);
            if (!table.tableViewParams.headerIsExists(headerId)){
                table.tableViewParams.setHeaderSize(headerId, -1);
                table.tableViewParams.setHeaderVisible(headerId, true);
            }
        }
    }

    Rectangle {
        id: backgroundTable;

        anchors.top: filterMenu_.visible ? filterMenu_.bottom: parent.top;
        anchors.left: parent.left;
        anchors.topMargin: filterMenu_.visible ? Style.size_mainMargin : 0;
        anchors.bottom: paginationObj.top;

        width: tableInternal.minWidth * tableInternal.columnCount < parent.width ? tableInternal.minWidth * tableInternal.columnCount : parent.width;

        color: Style.baseColor;

        radius: Style.size_mainCornerRadius !== undefined ? Style.size_mainCornerRadius: 0;
        TreeItemModel{
            id: tableDecoratorModel;

            property bool compl: false;
            property bool complCompl: tableDecoratorModel.compl && loaderTableDecorator.compl && tableInternal.headersCompl;
            Component.onCompleted: {
                tableDecoratorModel.compl = true;
            }
            onComplComplChanged: {
                if(tableDecoratorModel.complCompl){

                    var tableDecorator = loaderTableDecorator.item;
                    var count = tableInternal.headers.getItemsCount();

                    if(tableDecorator.getItemsCount()){

                        var headers = tableDecoratorModel.addTreeModel("Headers");
                        var cells = tableDecoratorModel.addTreeModel("Cells");
                        var cellWidth = tableDecoratorModel.addTreeModel("CellWidth");

                        var general;
                        if(tableDecorator.isValidData("General")){
                            general = tableDecorator.getTreeItemModel("General")
                            let keys = general.getKeys();
                            for(let i = 0; i < keys.length; i++){
                                if(tableInternal[keys[i]] !== undefined){
                                    tableInternal[keys[i]] = general.getData(keys[i]);
                                }
                            }
                        }

                        for(let i = 0; i < count; i++){
                            headers.insertNewItem();
                            cells.insertNewItem();
                            cellWidth.insertNewItem();
                            headers.copyItemDataFromModel(i,tableDecorator.getTreeItemModel("Headers"));
                            cells.copyItemDataFromModel(i,tableDecorator.getTreeItemModel("Cells"));
                            //cellWidth.copyItemDataFromModel(i,tableDecorator.getTreeItemModel("CellWidth"));
                            var widthVal = tableDecorator.getTreeItemModel("CellWidth").isValidData("Width",i) ? tableDecorator.getTreeItemModel("CellWidth").getData("Width",i) : -1;
                            cellWidth.setData("Width", widthVal,i);

                        }

                        tableInternal.tableDecorator = tableDecoratorModel;
                    }
                }
            }
        }

        Loader{
            id: loaderTableDecorator;

            property bool compl: false;
            sourceComponent: Style.collectionTableDecorator;
            onLoaded: {
                if(loaderTableDecorator.item){
                    loaderTableDecorator.compl = true;
                }
            }
        }

        Table {
            id: tableInternal;

            anchors.left: parent.left;
            anchors.right: tableRightPanel.left;
            anchors.top: parent.top;
            anchors.bottom: parent.bottom;

            hasFilter: collectionViewBaseContainer.hasFilter;
            hasSort: collectionViewBaseContainer.hasSort;
            scrollbarVisible: false;

            canSetBorderParams: true;
            canSetBorderParams_deleg: true;
            property real minWidth: 1000000;
            property bool headersCompl: false;

            canMoveColumns: true;

            scrollbarRightMargin: -20;
            verticalBorderHeight: headerHeight / 2;
            verticalBorderSize: 1;
            visibleLeftBorderFirst: false;
            borderColorVertical: Style.borderColor;

            onSelectionChanged: {
                collectionViewBaseContainer.selectionChanged(selection);
            }

            onCheckedItemsChanged: {
                collectionViewBaseContainer.checkedItemsChanged();
            }

            onElementsChanged: {
                collectionViewBaseContainer.elementsChanged();
            }

            onHeadersChanged: {
                collectionViewBaseContainer.headersChanged();
                if(tableInternal.headers.getItemsCount()){
                    tableInternal.headersCompl = true;
                }
            }

            onRightButtonMouseClicked: {
                collectionViewBaseContainer.rightButtonMouseClicked(mouseX, mouseY);
            }

            onDoubleClicked: {
                collectionViewBaseContainer.doubleClicked(id, index);
            }

            onHeaderClicked: {
                if (collectionViewBaseContainer.collectionFilter.getSortingInfoId() !== headerId){
                    collectionViewBaseContainer.collectionFilter.setSortingInfoId(headerId);
                    collectionViewBaseContainer.collectionFilter.setSortingOrder("ASC");
                }
                else{
                    collectionViewBaseContainer.collectionFilter.setSortingInfoId(headerId);

                    let currentSortingOrder = collectionViewBaseContainer.collectionFilter.getSortingOrder();
                    collectionViewBaseContainer.collectionFilter.setSortingOrder(currentSortingOrder == "ASC" ? "DESC" : "ASC");
                }

                collectionViewBaseContainer.doUpdateGui();
            }

            function updateWidthFromViewParams(){
                for (let i = 0; i < tableInternal.widthDecorator.getItemsCount(); i++){
                    let headerId = tableInternal.headers.getData("Id", i);
                    for (let j = 0; j < tableInternal.tableViewParams.getItemsCount(); j++){
                        let id = tableInternal.tableViewParams.getData("HeaderId", j);
                        if (headerId === id){
                            let visible = tableInternal.tableViewParams.getData("Visible", j);
                            let size = tableInternal.tableViewParams.getData("Size", j);
                            if (visible){
                                tableInternal.widthDecorator.setData("Width", size, i)
                            }
                            else{
                                tableInternal.widthDecorator.setData("Width", 0, i)
                            }

                            break;
                        }

                    }
                }

                tableInternal.setWidth();
            }

            function updateViewParamsFromWidthModel(){

            }
        }

        Item {
            id: tableRightPanel;

            anchors.top: parent.top;
            anchors.right: parent.right;
            anchors.bottom: parent.bottom;

            width: collectionViewBaseContainer.hasFilter ? 35 : 0;

            visible: tableInternal.width > 0;

            Item {
                id: filterItem;

                anchors.top: parent.top;

                width: parent.width;
                height: tableInternal.headerHeight;
                //                ToolButton {
                //                    id: iconFilter;

                //                    anchors.centerIn: parent;

                //                    width: Style.buttonWidthMedium;
                //                    height: width;

                //                    visible: collectionViewBaseContainer.hasFilter;

                //                    iconSource: "../../../" + Style.getIconPath("Icons/Filter", Icon.State.On, Icon.Mode.Normal);

                //                    onClicked: {
                //                        filterMenu_.visible = !filterMenu_.visible;
                //                    }
                //                }

                ToolButton {
                    id: tableHeaderSettings;

                    anchors.centerIn: parent;

                    width: Style.buttonWidthMedium;
                    height: width;

                    iconSource: "../../../" + Style.getIconPath("Icons/Settings", Icon.State.On, Icon.Mode.Normal);

                    onClicked: {
                        ModalDialogManager.openDialog(tableHeaderParamComp, {});
                    }
                }
            }
        }
    }

    Component {
        id: tableHeaderParamComp;

        Dialog {
            id: dialog;
            width: 300;
            title: qsTr("Table params");
            property TableViewParams tableViewParamsCopied: TableViewParams {};
            contentComp: Component {
                Item {
                    id: item;

                    width: 300;
                    height: leftTable.height;

                    property var checkedIndexes: [];

                    property bool block: true;
                    function updateGui(){
                        item.block = true;
                        for (let i = 0; i < leftTable.elements.getItemsCount(); i++){
                            let id = leftTable.elements.getData("Id", i)

                            if (dialog.tableViewParamsCopied.headerIsVisible(id)){
                                leftTable.checkItem(i);
                            }
                        }
                        item.block = false;
                    }

                    function updateModel(){
                        dialog.tableViewParamsCopied.copyFrom(tableInternal.tableViewParams)
                        leftTable.headers = availableHeadersModel;
                        leftTable.elements = tableInternal.headers;
                    }

                    Table {
                        id: leftTable;
                        anchors.left: parent.left;
                        width: 300;
                        height: itemHeight * elementsCount + headerHeight;
                        checkable: true;
                        onCheckedItemsChanged: {
                            item.checkedIndexes = getCheckedItems();
                            if (item.block){
                                return;
                            }

                            for (let i = 0; i < leftTable.elements.getItemsCount(); i++){
                                let id = leftTable.elements.getData("Id", i)

                                dialog.tableViewParamsCopied.setHeaderVisible(id, item.checkedIndexes.includes(i));
                            }
                        }

                        onElementsChanged: {
                            item.updateGui();
                        }
                    }

                    TreeItemModel {
                        id: availableHeadersModel;

                        Component.onCompleted: {
                            let index = availableHeadersModel.insertNewItem();
                            availableHeadersModel.setData("Id", "Name", index)
                            availableHeadersModel.setData("Name", qsTr("Available Columns"), index)

                            item.updateModel()
                        }
                    }
                }
            }

            Component.onCompleted: {
                buttonsModel.append({Id: Enums.apply, Name:qsTr("Apply"), Enabled: true})
                buttonsModel.append({Id: Enums.cancel, Name:qsTr("Cancel"), Enabled: true})
            }

            onFinished: {
                if (buttonId === Enums.apply){
                    if (contentItem){
                        tableInternal.tableViewParams.copyFrom(dialog.tableViewParamsCopied);

                        tableInternal.updateWidthFromViewParams();
                        collectionViewBaseContainer.tableViewParamsAccepted();
                    }
                }
            }
        }
    }

    //                    TreeItemModel {
    //                        id: headersModel;

    //                        Component.onCompleted: {
    //                            let index = headersModel.insertNewItem();
    //                            headersModel.setData("Id", "Name", index)
    //                            headersModel.setData("Name", "Selected Columns", index)

    //                            rightTable.headers = headersModel;
    //                        }
    //                    }

    //                    Item {
    //                        id: movingItem;
    //                        anchors.left: leftTable.right;
    //                        width: 50;
    //                        height: parent.height;

    //                        Column {
    //                            anchors.centerIn: parent;
    //                            width: parent.width;
    //                            ToolButton {
    //                                id: rightButton;
    //                                anchors.horizontalCenter: parent.horizontalCenter;
    //                                width: 18;
    //                                height: 25;
    //                                enabled: false;
    //                                iconSource: enabled ? "../../../" + Style.getIconPath("Icons/Right", Icon.State.On, Icon.Mode.Normal):
    //                                                      "../../../" + Style.getIconPath("Icons/Right", Icon.State.Off, Icon.Mode.Disabled)
    //                                onClicked: {
    //                                    let checkedItems = leftTable.getCheckedItems();
    //                                    if (checkedItems.length > 0){
    //                                        for (let i = 0; i < checkedItems.length; i++){
    //                                            let index = checkedItems[i];

    //                                        }
    //                                    }
    //                                }
    //                            }

    //                            ToolButton {
    //                                id: leftButton;
    //                                anchors.horizontalCenter: parent.horizontalCenter;
    //                                width: 18;
    //                                height: 25;
    //                                enabled: false;
    //                                iconSource: enabled ? "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal):
    //                                                      "../../../" + Style.getIconPath("Icons/Left", Icon.State.Off, Icon.Mode.Disabled)
    //                                onClicked: {
    //                                }
    //                            }
    //                        }
    //                    }

    //                    Table {
    //                        id: rightTable;
    //                        anchors.left: movingItem.right;
    //                        width: 200;
    //                        height: 300;
    //                        checkable: true;
    //                        onCheckedItemsChanged: {
    //                            let checkedItems = getCheckedItems();
    //                            leftButton.enabled = checkedItems.length > 0;
    //                        }
    //                    }

    //                    Item {
    //                        id: orderedItem;
    //                        anchors.left: rightTable.right;
    //                        width: 50;
    //                        height: parent.height;

    //                        Column {
    //                            anchors.centerIn: parent;
    //                            width: parent.width;
    //                            ToolButton {
    //                                anchors.horizontalCenter: parent.horizontalCenter;
    //                                width: 18;
    //                                height: 25;
    //                                rotation: -90;
    //                                iconSource: enabled ? "../../../" + Style.getIconPath("Icons/Top", Icon.State.On, Icon.Mode.Normal):
    //                                                      "../../../" + Style.getIconPath("Icons/Right", Icon.State.Off, Icon.Mode.Disabled)
    //                                onClicked: {
    //                                }
    //                            }

    //                            ToolButton {
    //                                anchors.horizontalCenter: parent.horizontalCenter;
    //                                width: 18;
    //                                height: 25;
    //                                rotation: -90;
    //                                iconSource: enabled ? "../../../" + Style.getIconPath("Icons/Left", Icon.State.On, Icon.Mode.Normal):
    //                                                      "../../../" + Style.getIconPath("Icons/Left", Icon.State.Off, Icon.Mode.Disabled)
    //                                onClicked: {
    //                                }
    //                            }
    //                        }
    //                    }

    Loading {
        id: loading_;

        anchors.fill: backgroundTable;

        color: Style.baseColor;

        visible: false;
    }

    Rectangle {
        id: paginationObj;

        anchors.bottom: parent.bottom;
        anchors.horizontalCenter: parent.horizontalCenter;

        width: parent.width;
        height: collectionViewBaseContainer.hasPagination ? 30: 0;

        color: Style.baseColor;

        Pagination {
            id: pagination_;

            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.verticalCenter: parent.verticalCenter;

            pagesSize: 1;

            visible: collectionViewBaseContainer.hasPagination /*&& pagesSize > 1*/;

            onCurrentIndexChanged: {
                tableInternal.resetSelection();

                collectionViewBaseContainer.doUpdateGui();
            }

            onCountElementsChanged: {
                tableInternal.selectedIndex = -1;

                collectionViewBaseContainer.doUpdateGui();
            }
        }
    }
}
