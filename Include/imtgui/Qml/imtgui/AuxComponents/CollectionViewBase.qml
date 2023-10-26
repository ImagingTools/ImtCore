import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Item {
    id: collectionViewBaseContainer;

    property alias commands: gqlModels;

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
    property alias isMultiCheckable: tableInternal.isMultiCheckable;

    property alias filterMenu: filterMenuLocal.sourceComponent;
    property alias filterMenuItem: filterMenuLocal.item;
    property alias filterMenuVisible: filterMenuLocal.visible;
    property alias modelFilter: modelFilterObj;
    property alias pagination: paginationObj;

    // Sort by default this index
    property int defaultSortHeaderIndex: 0;

    // only ASC or DESC
    property alias defaultOrderType: sortCont.currentOrder;

    signal selectedItem(string id, string name);
    signal selectedIndexChanged(int index);

    signal elementsChanged();
    signal headersChanged();

    signal selectionChanged(var selection);
    signal filterDecoratorLoaded();

    property bool loadData;

    Component.onCompleted: {
        tableInternal.focus = true;

        Events.subscribeEvent("OnLocalizationChanged", collectionViewBaseContainer.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", collectionViewBaseContainer.onLocalizationChanged);
    }


    onCommandsIdChanged: {
        if (collectionViewBaseContainer.loadData){
            gqlModels.updateModels();

//            collectionViewBaseContainer.registerSubscription();
        }
    }

    Loader {
        id: filterMenuLocal;

        anchors.top: collectionViewBaseContainer.top;
        anchors.topMargin: Style.margin;

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
            }
        }

        onLoaded: {
            filterMenuLocal.item.onTextFilterChanged.connect(collectionViewBaseContainer.onTextFilterChanged);
            filterMenuLocal.item.onClosed.connect(collectionViewBaseContainer.onFilterClosed);
        }
    }

    function onLocalizationChanged(language){
        console.log("CollectionViewBase onLocalizationChanged", language);
        gqlModels.updateModels();
    }

    function onFilterClosed(){
        filterMenuLocal.visible = false;
    }

    function onTextFilterChanged(index, text){
        console.log("onTextFilterChanged", text);
        modelFilterObj.SetData("TextFilter", text);
        gqlModels.updateModels();
    }

    Rectangle {
        id: backgroundTable;

        anchors.top: filterMenuLocal.visible ? filterMenuLocal.bottom: parent.top;
        anchors.topMargin: filterMenuLocal.visible ? Style.margin : 0;
        anchors.left: parent.left;

        width: tableInternal.minWidth * tableInternal.columnCount < parent.width ? tableInternal.minWidth * tableInternal.columnCount : parent.width;

        anchors.bottom: paginationObj.top;

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
            anchors.top: parent.top;

            anchors.bottom: paginationObj.visible ? paginationObj.top : parent.bottom;
            anchors.margins: Style.size_mainMargin !== undefined ? Style.size_mainMargin : 0;
            hasFilter: collectionViewBaseContainer.hasFilter;
            hasSort: collectionViewBaseContainer.hasSort;
            scrollbarVisible: false;

            canSetBorderParams: true;
            canSetBorderParams_deleg: true;
            property real minWidth: 1000000;
            property bool headersCompl: false;

            canMoveColumns: true;

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

            onHeadersChanged: {
                collectionViewBaseContainer.headersChanged();
            }

            onFilterClicked: {
                filterMenuLocal.visible = !filterMenuLocal.visible;
            }
        }

        Item {
            id: tableRightPanel;

            anchors.top: parent.top;
            anchors.right: parent.right;
            anchors.rightMargin: 5;
            anchors.bottom: parent.bottom;

            width: hasFilter ? 35 : 10;

            visible: tableInternal.width > 0;

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

//                    width: collectionViewBaseContainer.hasFilter ? tableRightPanel.width : 0;
                    width: Style.buttonWidthMedium;
                    height: width;

                    iconWidth: Style.iconSizeSmall;
                    iconHeight: iconWidth;

                    iconSource: "../../../" + Style.getIconPath("Icons/Filter", Icon.State.On, Icon.Mode.Normal);

                    onClicked: {
                        filterMenuLocal.visible = !filterMenuLocal.visible;
                    }
                }
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

        commands: gqlModels;

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
            modelFilterObj.SetUpdateEnabled(true)
            modelFilterObj.AddTreeModel("FilterIds");
            sortCont.sortModel = modelFilterObj.AddTreeModel("Sort");
            console.log("modelFilterObj onCompleted", modelFilterObj.toJSON())
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
            gqlModels.updateModels();
        }
    }

    CollectionViewBaseGqlModels {
        id: gqlModels;

        itemId: collectionViewBaseContainer.itemId;
        table: collectionViewBaseContainer.table;

        commandsId: collectionViewBaseContainer.commandsId;
        rootItem: collectionViewBaseContainer;

        pagination: paginationObj;
        loading: ldng;

        onHeadersChanged: {
            let headersCount = gqlModels.headers.GetItemsCount();
            if (headersCount > 0 && sortCont.isEmpty()){
                if (collectionViewBaseContainer.defaultSortHeaderIndex < 0 || headersCount <= collectionViewBaseContainer.defaultSortHeaderIndex){
                    collectionViewBaseContainer.defaultSortHeaderIndex = 0;
                }

                let headerId = gqlModels.headers.GetData("Id", collectionViewBaseContainer.defaultSortHeaderIndex);

                sortCont.currentHeaderId = headerId;

                sortCont.setHeaderSort(headerId, collectionViewBaseContainer.defaultOrderType);
            }

            tableInternal.headers = gqlModels.headers;
            tableInternal.headersCompl = true;
        }

        onItemsChanged: {
            tableInternal.elements = gqlModels.items;
        }

        onItemsInfoGqlStateChanged: {
            if (state === "Loading"){
                ldng.visible = true;
            }
            else{
                ldng.visible = false;
            }
        }
    }

    function registerSubscription(){
        console.log("try registerSubscription OnObjectCollectionChanged");

        var query = Gql.GqlRequest("subscription", "OnObjectCollectionChanged");
        var queryFields = Gql.GqlObject("notification");
        queryFields.InsertField("Id");
        query.AddField(queryFields);
        subscriptionManager.registerSubscription(query, subscriptionClient);
    }

//    SubscriptionClient {
//        id: subscriptionClient;

//        onStateChanged: {
//            console.log("SubscriptionClient onStateChanged", state);
//            if (state == "Registered"){
//                gqlModels.updateItemsModel();
//            }
//        }
//    }
}
