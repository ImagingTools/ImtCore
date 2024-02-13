import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtgui 1.0

ViewBase {
    id: collectionViewBaseContainer;

    property alias table: tableInternal;
    property bool hasPagination: true;
    property bool hasFilter: true;

    property bool hasSort: true;
    property alias isMultiCheckable: tableInternal.isMultiCheckable;

    property alias filterMenu: filterMenu_;
    property alias loading: loading_;
    property alias filterMenuVisible: filterMenu_.visible;
    property alias pagination: pagination_;

    property CollectionFilter collectionFilter: CollectionFilter {}

    signal selectedIndexChanged(int index);

    signal elementsChanged();
    signal headersChanged();

    signal filterChanged();

    signal selectionChanged(var selection);
    signal rightButtonMouseClicked(int mouseX, int mouseY);
    signal doubleClicked(string id, int index);

    Component.onCompleted: {
        tableInternal.focus = true;

        Events.subscribeEvent("OnLocalizationChanged", collectionViewBaseContainer.onLocalizationChanged);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent("OnLocalizationChanged", collectionViewBaseContainer.onLocalizationChanged);
    }

    Connections {
        target: collectionViewBaseContainer.collectionFilter;

        function onFilterChanged(){
            console.log("onFilterChanged");

            tableInternal.currentHeaderId = collectionViewBaseContainer.collectionFilter.getSortingInfoId();
            tableInternal.currentSortOrder = collectionViewBaseContainer.collectionFilter.getSortingOrder();

            if (tableInternal.headers.GetItemsCount() > 0){
                collectionViewBaseContainer.doUpdateGui();
            }
        }
    }

    FilterMenu {
        id: filterMenu_;

        anchors.top: collectionViewBaseContainer.top;
        anchors.topMargin: Style.margin;
        anchors.left: parent.left;
        anchors.leftMargin: Style.margin;
        anchors.right: parent.right;
        anchors.rightMargin: Style.margin;

        onClose: {
            filterMenu_.visible = false;
        }

        onFilterChanged: {
            collectionViewBaseContainer.onFilterChanged(filterId, filterValue);
        }
    }

    function onLocalizationChanged(language){
    }

    function onFilterChanged(filterId, filterValue){
        collectionViewBaseContainer.collectionFilter.setTextFilter(filterValue);
    }

    Rectangle {
        id: backgroundTable;

        anchors.top: filterMenu_.visible ? filterMenu_.bottom: parent.top;
        anchors.topMargin: filterMenu_.visible ? Style.margin : 0;
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

            onSelectionChanged: {
                collectionViewBaseContainer.selectionChanged(selection);
            }

            onElementsChanged: {
                collectionViewBaseContainer.elementsChanged();
            }

            onHeadersChanged: {
                collectionViewBaseContainer.headersChanged();

                let filteringInfoIds = []
                for (let i = 0; i < tableInternal.headers.GetItemsCount(); i++){
                    let headerId = tableInternal.headers.GetData("Id", i);

                    filteringInfoIds.push(headerId);
                }

                collectionViewBaseContainer.collectionFilter.setFilteringInfoIds(filteringInfoIds);
            }

            onRightButtonMouseClicked: {
                collectionViewBaseContainer.rightButtonMouseClicked(mouseX, mouseY);
            }

            onDoubleClicked: {
                collectionViewBaseContainer.doubleClicked(id, index);
            }

            onHeaderClicked: {
                console.log("onHeaderClicked");
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
        }

        Item {
            id: tableRightPanel;

            anchors.top: parent.top;
            anchors.right: parent.right;
            anchors.rightMargin: 5;
            anchors.bottom: parent.bottom;

            width: collectionViewBaseContainer.hasFilter ? 35 : 10;

            visible: tableInternal.width > 0;

            Item {
                id: filterItem;

                anchors.top: parent.top;

                width: parent.width;
                height: tableInternal.headerHeight;

                ToolButton {
                    id: iconFilter;

                    anchors.centerIn: parent;

                    width: Style.buttonWidthMedium;
                    height: width;

                    visible: collectionViewBaseContainer.hasFilter;

                    iconSource: "../../../" + Style.getIconPath("Icons/Filter", Icon.State.On, Icon.Mode.Normal);

                    onClicked: {
                        filterMenu_.visible = !filterMenu_.visible;
                    }
                }
            }
        }
    }

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
                console.log("P onCurrentIndexChanged");

                tableInternal.selectedIndex = -1;

                collectionViewBaseContainer.doUpdateGui();
            }

            onCountElementsChanged: {
                console.log("P onCountElementsChanged");

                tableInternal.selectedIndex = -1;

                collectionViewBaseContainer.doUpdateGui();
            }
        }
    }
}
