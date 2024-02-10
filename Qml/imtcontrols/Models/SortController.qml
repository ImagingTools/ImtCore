import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item {
    id: container;

    property TreeItemModel sortModel: TreeItemModel {}

    function setHeaderSort(headerId, sortOrder){
        container.sortModel.SetData("HeaderId", headerId);
        container.sortModel.SetData("SortOrder", sortOrder);
    }

    function getCurrentSort(){
        if (container.sortModel.ContainsKey("SortOrder")){
            return container.sortModel.GetData("SortOrder")
        }

        return "";
    }

    function getCurrentHeaderId(){
        if (container.sortModel.ContainsKey("HeaderId")){
            return container.sortModel.GetData("HeaderId")
        }

        return "";
    }

    function isEmpty(){
        return container.sortModel.GetItemsCount() === 0;
    }
}
