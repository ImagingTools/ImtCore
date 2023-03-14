import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: container;

    property var collectionView;
    property var commands;

    property TreeItemModel sortModel: TreeItemModel {}

    function headerClicked(headerId){
        console.log("headerClicked", headerId, sortModel.toJSON());

        let currentHeaderId = container.sortModel.GetData("HeaderId");
        let currentSortOrder = container.sortModel.GetData("SortOrder");

        let order = "ASC";

        if (currentHeaderId === headerId){
            order = currentSortOrder === "ASC" ? "DESC" : "ASC";
        }

        container.setHeaderSort(headerId, order);
        container.commands.updateItemsModel();
    }

    function setHeaderSort(headerId, sortOrder){
        container.sortModel.SetData("HeaderId", headerId);
        container.sortModel.SetData("SortOrder", sortOrder);
    }

    function isEmpty(){
        return container.sortModel.GetItemsCount() === 0;
    }
}
