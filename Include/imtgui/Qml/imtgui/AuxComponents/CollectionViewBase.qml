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

    property string itemId;
    property var table: tableInternal;
    property bool hasPagination: true;
    property bool hasFilter: true;
    property bool hasSort: true;

    property alias filterMenu: filterMenuLocal.sourceComponent;
    property alias filterMenuVisible: filterMenuLocal.visible;
    property alias modelFilter: modelFilterObj;
    property alias pagination: paginationObj;

    signal selectedItem(string id, string name);
    signal selectedIndexChanged(int index);
    signal elementsChanged();

    signal selectionChanged(var selection);

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
                            cellWidth.CopyItemDataFromModel(i,tableDecorator.GetTreeItemModel("CellWidth"));
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

            anchors.fill: parent;

            anchors.bottom: paginationObj.visible ? paginationObj.top : parent.bottom;
            anchors.margins: Style.size_mainMargin !== undefined ? Style.size_mainMargin : 0;
            //anchors.margins: thumbnailDecoratorContainer.mainMargin;
            hasFilter: collectionViewBaseContainer.hasFilter;
            hasSort: collectionViewBaseContainer.hasSort;

            canSetBorderParams: true;
            canSetBorderParams_deleg: true;
            property real minWidth: 1000000;
            property bool headersCompl: false;

            sortController: sortCont;

            onSelectItem: {
                collectionViewBaseContainer.selectedItem(idSelected, name);
            }

//            onSelectedIndexChanged: {
//                console.log("CollectionView AuxTable onSelectedIndexChanged");
//                collectionViewBaseContainer.selectedIndexChanged(tableInternal.selectedIndex);
//            }

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

        Loading {
            id: ldng;

            anchors.fill: tableInternal;
            anchors.topMargin: tableInternal.headerElementHeight;

            visible: false;
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
    }
}
