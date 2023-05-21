import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: collectionViewBaseContainer;

    property alias commands: baseCommands;

    property string commandsId;

    property alias tableElementsDelegate: tableInternal.delegate;
    property alias tableHeadersDelegate: tableInternal.headerDelegate;
    property alias elementsList: tableInternal.elementsList;

    property alias tableMinWidth: tableInternal.minWidth;
    property alias tableHeaderHeight: tableInternal.headerHeight;
    property alias tableItemHeight: tableInternal.itemHeight;
    property alias tableDecoratorPath: loaderTableDecorator.source;
    property alias tableDecoratorComp: loaderTableDecorator.sourceComponent;
    property alias tableCellDecorator: tableInternal.cellDecorator;
    property alias tableWidthDecorator: tableInternal.widthDecorator;
    property alias tableWidth: tableInternal.width;
    property alias tableHeaders: tableInternal.headers;

    property string itemId;
    property var table: tableInternal;
    property bool hasPagination: true;
    property bool hasFilter: true;
    property bool hasSort: true;

    property alias filterMenu: filterMenuLocal.sourceComponent;
    property alias filterMenuItem: filterMenuLocal.item;
    property alias filterMenuVisible: filterMenuLocal.visible;
    property alias modelFilter: modelFilterObj;
    property alias pagination: paginationObj;


    signal selectedItem(string id, string name);
    signal selectedIndexChanged(int index);
    signal elementsChanged();

    signal selectionChanged(var selection);
    signal filterDecoratorLoaded();

    /**
        Если true -> данные будут запрошены с сервера,
        иначе нужно будет подставлять данные вручную
      */
    property bool loadData;

    Component.onCompleted: {
        console.log("CollectionViewBase onCompleted");

        tableInternal.focus = true;

    }

    onCommandsIdChanged: {
        console.log("CollectionViewBase onCommandsIdChanged", collectionViewBaseContainer.loadData);

        if (collectionViewBaseContainer.loadData){
            baseCommands.updateModels();
        }
    }

    Loader {
        id: filterMenuLocal;
        anchors.top: collectionViewBaseContainer.top;
        width: parent.width;
        visible: tableInternal ? false : false;//for web, do not remove!!!

        onVisibleChanged: {
            if (!filterMenuLocal.visible){
                tableInternal.forceActiveFocus();
            }
        }

        sourceComponent: Component {
            FilterMenu {
                decoratorSource: Style.filterPanelDecoratorPath;

                onTextFilterChanged: {
                    modelFilterObj.SetData("TextFilter", text);
                    baseCommands.updateModels();
                }

                onClosed: {
                    filterMenuLocal.visible = false;
                }
            }
        }
    }

    Rectangle {
        id: backgroundTable;

        anchors.top: filterMenuLocal.visible ? filterMenuLocal.bottom: parent.top;
        anchors.left: parent.left;
        width: tableInternal.minWidth * tableInternal.columnCount < parent.width ? tableInternal.minWidth * tableInternal.columnCount : parent.width;

        anchors.bottom: paginationObj.top;

        color: Style.baseColor;

        radius: Style.size_mainCornerRadius !== undefined ? Style.size_mainCornerRadius: 0;
        //radius: thumbnailDecoratorContainer.mainRadius;
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
                    var count = tableInternal.columnCount;

                    if(tableDecorator.GetItemsCount()){

                        var headers = tableDecoratorModel.AddTreeModel("Headers");
                        var cells = tableDecoratorModel.AddTreeModel("Cells");
                        var cellWidth = tableDecoratorModel.AddTreeModel("CellWidth");

                        for(var i = 0; i < count; i++){
                            headers.InsertNewItem();
                            cells.InsertNewItem();
                            cellWidth.InsertNewItem();
                            headers.CopyItemDataFromModel(i,tableDecorator.GetTreeItemModel("Headers"));
                            cells.CopyItemDataFromModel(i,tableDecorator.GetTreeItemModel("Cells"));
                            //cellWidth.CopyItemDataFromModel(i,tableDecorator.GetTreeItemModel("CellWidth"));
                            var widthVal = tableDecorator.GetTreeItemModel("CellWidth").IsValidData("Width",i) ? tableDecorator.GetTreeItemModel("CellWidth").GetData("Width",i) : -1;
                            cellWidth.SetData("Width", widthVal,i);

                        }

                        tableInternal.tableDecorator = tableDecoratorModel;
                    }
                }
            }
        }

        Loader{
            id: loaderTableDecorator;

            property bool compl: false;
            onLoaded: {
                if(loaderTableDecorator.item){
                    loaderTableDecorator.compl = true;
                }
            }
        }

        AuxTable {
            id: tableInternal;

            anchors.left: parent.left;
            anchors.right: tableRightPanel.left;
//            anchors.rightMargin: 5;
            anchors.top: parent.top;

            anchors.bottom: paginationObj.visible ? paginationObj.top : parent.bottom;
            anchors.margins: Style.size_mainMargin !== undefined ? Style.size_mainMargin : 0;
            //anchors.margins: thumbnailDecoratorContainer.mainMargin;
            hasFilter: collectionViewBaseContainer.hasFilter;
            hasSort: collectionViewBaseContainer.hasSort;
            scrollbarVisible: false;

            canSetBorderParams: true;
            canSetBorderParams_deleg: true;
            property real minWidth: 1000000;
            property bool headersCompl: false;

            scrollbarRightMargin: -20;

            sortController: sortCont;

            onSelectItem: {
                collectionViewBaseContainer.selectedItem(idSelected, name);
            }

            onSelectionChanged: {
                collectionViewBaseContainer.selectionChanged(selection);
            }

            onTextFilterChanged: {
                modelFilterObj.SetData("TextFilter", text);
            }

            onElementsChanged: {
                collectionViewBaseContainer.elementsChanged();
            }

            onFilterClicked: {
                console.log("onFilterClicked")
                filterMenuLocal.visible = !filterMenuLocal.visible;
            }
        }


//        Rectangle{
//            id: bottomLine;

//            anchors.left: tableInternal.right;
//            anchors.right: parent.right;
//            anchors.bottom: filterItem.bottom;
//            height: 1;
//            color: "lightgray";

//            visible: tableInternal.emptyDecor;
//        }

        Item {
            id: tableRightPanel;

            anchors.top: parent.top;
            anchors.right: parent.right;
            anchors.rightMargin: 5;
            anchors.bottom: parent.bottom;

            width: 20;

            Item {
                id: filterItem;

                anchors.top: parent.top;

                width: parent.width;
                height: tableInternal.headerHeight;

                AuxButton {
                    id: iconFilter;

                    anchors.centerIn: parent;

                    visible: collectionViewBaseContainer.hasFilter;
                    highlighted: Style.highlightedButtons !==undefined ? Style.highlightedButtons : containsMouse;

                    width: collectionViewBaseContainer.hasFilter ? tableRightPanel.width : 0;
                    height: width;

                    iconSource: "../../../" + "Icons/" + Style.theme + "/Filter_On_Normal.svg";

                    onClicked: {
                        console.log("AuxButton iconFilter onClicked");
                        filterMenuLocal.visible = !filterMenuLocal.visible;
                    }
                }
            }

//            CustomScrollbar {
//                id: scrollbar;

//                z: 100;

//                anchors.bottom: parent.bottom;
//                anchors.bottomMargin: 5;
//                anchors.top: filterItem.bottom;
//                anchors.topMargin: 5;
//                anchors.horizontalCenter: parent.horizontalCenter;

//                secondSize: 10;
//                targetItem: collectionViewBaseContainer.elementsList;

//                onVisibleChanged: {
//                    tableInternal.scrollbarItem.visible = !visible;
//                }

//                Component.onCompleted: {
//                    tableInternal.scrollbarItem.visible = false;
//                }
//            }
        }

        Loading {
            id: ldng;

            anchors.fill: tableInternal;
            anchors.topMargin: tableInternal.headerElementHeight;

            visible: false;

            onVisibleChanged: {
                console.log("Loading onVisibleChanged", ldng.visible);
            }
        }
    }

    SortController {
        id: sortCont;

        commands: baseCommands;

        Component.onCompleted: {
            tableInternal.headerClicked.connect(sortCont.headerClicked);
        }

        Component.onDestruction: {
            tableInternal.headerClicked.disconnect(sortCont.headerClicked);
        }
    }

    TreeItemModel {
        id: modelFilterObj;

        Component.onCompleted: {
            modelFilterObj.AddTreeModel("FilterIds");
            sortCont.sortModel = modelFilterObj.AddTreeModel("Sort");
        }
    }

    Pagination {
        id: paginationObj;

        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        anchors.horizontalCenter: parent.horizontalCenter;

        pagesSize: 1;

        visible: collectionViewBaseContainer.hasPagination && paginationObj.pagesSize != 1;

        onCurrentIndexChanged: {
            tableInternal.selectedIndex = -1;
            baseCommands.updateModels();
        }
    }

    CollectionViewBaseGqlModels {
        id: baseCommands;

        itemId: collectionViewBaseContainer.itemId;
        table: collectionViewBaseContainer.table;

        commandsId: collectionViewBaseContainer.commandsId;
        rootItem: collectionViewBaseContainer;

        pagination: paginationObj;
        loading: ldng;

        onHeadersChanged: {
            console.log("onHeadersChanged", baseCommands.headers)
            if (baseCommands.headers.GetItemsCount() > 0 && sortCont.isEmpty()){
                let headerId = baseCommands.headers.GetData("Id");

                sortCont.currentHeaderId = headerId;

                sortCont.setHeaderSort(headerId, "ASC");
            }

            tableInternal.headers = baseCommands.headers;
            tableInternal.headersCompl = true;
        }

        onItemsChanged: {
            tableInternal.elements = baseCommands.items;
        }

        onItemsInfoGqlStateChanged: {
            console.log("onItemsInfoGqlStateChanged", state);
            if (state === "Loading"){
                ldng.visible = true;
            }
            else{
                ldng.visible = false;
            }
        }
    }
}
