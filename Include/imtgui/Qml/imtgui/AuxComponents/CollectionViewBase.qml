import QtQuick 2.12
import Acf 1.0
import imtqml 1.0


Item {
    id: collectionViewBaseContainer;

    property alias commands: baseCommands;

    property var table: tableInternal;
    property bool hasPagination: true;

    signal selectedItem(string id, string name);
    signal selectedIndexChanged(int index);

    Rectangle {
        id: backgroundTable;

        anchors.fill: parent;
        anchors.topMargin: thubnailDecoratorContainer.mainMargin;
        anchors.leftMargin: thubnailDecoratorContainer.mainMargin;
        anchors.bottomMargin: thubnailDecoratorContainer.mainMargin;
        anchors.rightMargin: thubnailDecoratorContainer.mainMargin;

        color: Style.baseColor;

        radius: thubnailDecoratorContainer.mainRadius;

        AuxTable {
            id: tableInternal;

            anchors.fill: parent;
            anchors.topMargin: thubnailDecoratorContainer.mainMargin;
            anchors.leftMargin: thubnailDecoratorContainer.mainMargin;
            anchors.bottom: pagination.visible ? pagination.top : parent.bottom;

            hasFilter: true;
            hasSort: true;

            onSelectItem: {
                console.log("CollectionView AuxTable onSelectItem", idSelected, name);
//                collectionViewContainer.itemSelect(idSelected, name);

                collectionViewBaseContainer.selectedItem(idSelected, name);
            }

            onRightButtonMouseClicked: {
                console.log("CollectionView AuxTable onRightButtonMouseClicked");
                collectionViewBaseContainer.collectionViewRightButtonMouseClicked(item, mouseX, mouseY);
            }

            onSelectedIndexChanged: {
                console.log("CollectionView AuxTable onSelectedIndexChanged");
                collectionViewBaseContainer.selectedIndexChanged(tableInternal.selectedIndex);
//                console.log(" CollectionView AuxTable onSelectedIndexChanged", collectionViewContainer.selectedIndex, tableInternal.selectedIndex);
//                collectionViewBaseContainer.collectionViewModel.SetData("selectedIndex", tableInternal.selectedIndex);
//                collectionViewContainer.selectedIndex = tableInternal.selectedIndex;

                //tableInternal.elementsList.selectedId = elements.GetData("Id", tableInternal.selectedIndex);
            }

            onSetActiveFocusFromTable: {
                console.log("CollectionView AuxTable onSetActiveFocusFromTable");
                //collectionViewContainer.setActiveFocusFromCollectionView();
//                collectionViewContainer.forceActiveFocus();
            }

            onTextFilterChanged: {
                modelFilter.SetData("TextFilter", text);
//                modelItems.updateModel();
            }


            onHeaderOnClicked: {
                console.log("CollectionView AuxTable onHeaderOnClicked", headerId, sortOrder);
//                collectionViewContainer.headerClicked(headerId, sortOrder);

//                modelItems.updateModel();
            }
        }
    }

    function setHeaderSort(headerId, sortOrder){
        var filterLocal = modelFilter.GetData("Sort");
        filterLocal.SetData("HeaderId", headerId);
        filterLocal.SetData("SortOrder", sortOrder);
    }

    TreeItemModel {
        id: modelFilter;

        Component.onCompleted: {
            modelFilter.AddTreeModel("FilterIds");
            modelFilter.AddTreeModel("Sort");
        }
    }

    Pagination {
        id: pagination;

        anchors.bottom: parent.bottom;
        anchors.bottomMargin: 10;
        anchors.horizontalCenter: parent.horizontalCenter;

        pagesSize: 1;

        visible: collectionViewBaseContainer.hasPagination;

        onCurrentValueChanged: {
            console.log("Pagination onCurrentValueChanged", pagination.currentValue);
            tableInternal.selectedIndex = -1;
            modelItems.updateModel();
        }
    }

    CollectionViewBaseGqlModels {
        id: baseCommands;

        onHeadersChanged: {
            tableInternal.headers = baseCommands.headers;
        }

        onItemsChanged: {
            tableInternal.elements = baseCommands.items;
        }
    }
}
