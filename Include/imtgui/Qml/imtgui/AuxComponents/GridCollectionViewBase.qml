import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: collectionViewBaseContainer;

    property alias commands: baseCommands;

    property alias gqlModelObjectView: baseCommands.gqlModelObjectView;
    property alias gqlModelHeadersInfo: baseCommands.gqlModelHeadersInfo;
    property alias gqlModelItemsInfo: baseCommands.gqlModelItemsInfo;

    property bool isWeb: Qt.platform.os == "web";

    property bool hasExtention: true;
    property bool openST: false;

    property alias gridElementsDelegate: gridInternal.delegate;
    property alias gridElementsModel: gridInternal.model;
    property alias gridCellWidth: gridInternal.cellWidth;
    property alias gridCellHeight: gridInternal.cellHeight;
    property alias gridContentHeight: gridInternal.contentHeight;
    property alias gridContentY: gridInternal.contentY;
    property alias gridBottomMargin: backgroundTable.anchors.bottomMargin;

    property alias gridCount: gridInternal.count;
    property int gridCountInLine: Math.trunc(width/gridCellWidth);
    property int gridRowCount : Math.ceil(gridCount/gridCountInLine);
    property int gridSelectedRow: gridInternal.selectedIndex < 0 ? -1 : Math.trunc(gridInternal.selectedIndex/gridCountInLine);
    property bool gridIsLastRow: gridSelectedRow == gridRowCount -1;
    property real gridAddHeight: 110*3;
    property int gridCellHeightMin: 110;//???
    property int gridCellHeightMax: (gridCellHeightMin * gridRowCount + gridAddHeight) / gridRowCount + !isWeb * gridIsLastRow * gridAddHeight;

    property alias extendingInfoComp: extendingInfoLoader.sourceComponent;

//    property alias gridMinWidth: gridInternal.minWidth;
    property alias gridDecoratorPath: loaderTableDecorator.source;
    property alias modelFilter: modelFilterObj;
    property alias pagination: paginationObj;
    property alias paginationCurrentIndex: paginationObj.currentIndex;
    property alias paginationPageSize: paginationObj.pagesSize;
    property string commandsId;
//    property alias elementsList: gridInternal.elementsList;
//    property alias gridItemHeight: gridInternal.itemHeight;
    property string itemId;

    property alias grid: gridInternal;
    property alias indexSelected: gridInternal.selectedIndex;

    property bool hasPagination: true;   

    signal selectedItem(string id, string name);
    signal selectedIndexChanged(int index);
    signal elementsChanged();

    /**
        Если true -> данные будут запрошены с сервера,
        иначе нужно будет подставлять данные вручную
      */
    property bool loadData;

    Component.onCompleted: {
        console.log("CollectionViewBase onCompleted");

        gridInternal.focus = true;
    }



    Rectangle {
        id: backgroundTable;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 50;

//        anchors.top: filterMenu.visible ? filterMenu.bottom: parent.top;
//        anchors.left: parent.left;
//        width: gridInternal.minWidth * gridInternal.columnCount < parent.width ? gridInternal.minWidth * gridInternal.columnCount : parent.width;
//        anchors.bottom: parent.bottom;

        color: Style.baseColor;

        radius: 10;

        TreeItemModel{
            id: gridDecoratorModel;

            property bool compl: false;
            property bool complCompl: gridDecoratorModel.compl && loaderTableDecorator.compl && gridInternal.headersCompl;
            Component.onCompleted: {
                gridDecoratorModel.compl = true;
            }
            onComplComplChanged: {
                if(gridDecoratorModel.complCompl){

                    var tableDecorator = loaderTableDecorator.item;
                    var count = gridInternal.columnCount;

                    if(tableDecorator.GetItemsCount()){

                        var headers = gridDecoratorModel.AddTreeModel("Headers");
                        var cells = gridDecoratorModel.AddTreeModel("Cells");
                        var cellWidth = gridDecoratorModel.AddTreeModel("CellWidth");

                        for(var i = 0; i < count; i++){
                            headers.InsertNewItem();
                            cells.InsertNewItem();
                            cellWidth.InsertNewItem();
                            headers.CopyItemDataFromModel(i,tableDecorator.GetTreeItemModel("Headers"));
                            cells.CopyItemDataFromModel(i,tableDecorator.GetTreeItemModel("Cells"));
                            cellWidth.CopyItemDataFromModel(i,tableDecorator.GetTreeItemModel("CellWidth"));
                        }

                        //gridInternal.tableDecorator = gridDecoratorModel;
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



        GridView {
            id: gridInternal;

            anchors.fill: parent;

            boundsBehavior: Flickable.StopAtBounds;
            clip: true;

            cellHeight: !collectionViewBaseContainer.hasExtention ? collectionViewBaseContainer.gridCellHeightMin : !collectionViewBaseContainer.openST ? collectionViewBaseContainer.gridCellHeightMin : collectionViewBaseContainer.gridCellHeightMax;

            property int selectedIndex: -1;
//            property alias elements: model;
            signal selectItem(string idSelected, string name);

            onSelectItem: {
                console.log("MEGATEST1")
                collectionViewBaseContainer.selectedItem(idSelected, name);
            }

            onSelectedIndexChanged: {
                console.log("CollectionView GridView onSelectedIndexChanged");
                collectionViewBaseContainer.selectedIndexChanged(gridInternal.selectedIndex);
            }

//            onElementsChanged: {
//                collectionViewBaseContainer.elementsChanged();
//            }

//            cellWidth: 160
//            cellHeight: 160

//            anchors.bottom: pagination.visible ? pagination.top : parent.bottom;
//            anchors.margins: thumbnailDecoratorContainer.mainMargin;
//            property real minWidth: 1000000;
        }
    }

    Item{
        id: extendingInfoContainer;

        width: parent.width;
        height: parent.height - collectionViewBaseContainer.gridBottomMargin;
        clip: true;
        Item{
            id: extendingInfoLoaderContainer;

            anchors.top: parent.top;
            anchors.topMargin: collectionViewBaseContainer.gridCellHeightMin * (collectionViewBaseContainer.gridSelectedRow + 1)  - collectionViewBaseContainer.gridContentY;

            width: collectionViewBaseContainer.gridCellWidth * collectionViewBaseContainer.gridCountInLine - 10;
            height: collectionViewBaseContainer.gridAddHeight - 10;

            visible: collectionViewBaseContainer.openST;

            Loader{
                id: extendingInfoLoader;

                anchors.fill: parent;

                sourceComponent: extendingInfoCompDefault;

            }
        }

    }

    Component{
        id: extendingInfoCompDefault;

        Rectangle{
            color: "#ffffff";
            border.color: "gray";
            border.width: 2;

        }

    }

    TreeItemModel {
        id: modelFilterObj;

        Component.onCompleted: {
            modelFilterObj.AddTreeModel("FilterIds");
            modelFilterObj.AddTreeModel("Sort");
        }
    }

    Pagination {
        id: paginationObj;

        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        anchors.horizontalCenter: parent.horizontalCenter;

        pagesSize: 1;

        visible: collectionViewBaseContainer.hasPagination;

        onCurrentValueChanged: {
            console.log("Pagination onCurrentValueChanged", paginationObj.currentValue);
            gridInternal.selectedIndex = -1;
            baseCommands.updateModels();
        }
    }

    CollectionViewBaseGqlModels {
        id: baseCommands;

        itemId: collectionViewBaseContainer.itemId;
        table: collectionViewBaseContainer.table;

        rootItem: collectionViewBaseContainer;
        commandsId: collectionViewBaseContainer.commandsId;
        pagination: paginationObj;

        Component.onCompleted: {
            console.log("CollectionViewBaseGqlModels onCompleted")
        }


        onCommandsIdChanged: {
            console.log("CollectionViewBase onCommandsIdChanged", loadData, baseCommands.commandsId, baseCommands.gqlModelHeadersInfo);

            if (loadData){
                baseCommands.updateModels();
            }
        }

        onHeadersChanged: {
//            gridInternal.headers = baseCommands.headers;
//            gridInternal.headersCompl = true;
        }

        onItemsChanged: {
            console.log("baseCommands.items - ", baseCommands.items);
            console.log("baseCommands.items.toJSON - ", baseCommands.items.toJSON());
            gridInternal.model = baseCommands.items;
        }
    }
}
