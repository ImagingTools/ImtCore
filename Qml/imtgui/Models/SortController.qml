import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: container;

    property var commands;

    property TreeItemModel sortModel: TreeItemModel {}

    property string currentHeaderId: "";
    property string currentOrder: "ASC";

    function headerClicked(headerId){
        console.log("headerClicked", headerId, sortModel.toJSON());

        container.setHeaderSort(currentHeaderId, currentOrder);
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
