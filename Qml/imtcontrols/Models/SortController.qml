import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0

Item {
    id: container;

    property TreeItemModel sortModel: TreeItemModel {}

    function setHeaderSort(headerId, sortOrder){
        container.sortModel.setData("HeaderId", headerId);
        container.sortModel.setData("SortOrder", sortOrder);
    }

    function getCurrentSort(){
        if (container.sortModel.containsKey("SortOrder")){
            return container.sortModel.getData("SortOrder")
        }

        return "";
    }

    function getCurrentHeaderId(){
        if (container.sortModel.containsKey("HeaderId")){
            return container.sortModel.getData("HeaderId")
        }

        return "";
    }

    function isEmpty(){
        return container.sortModel.getItemsCount() === 0;
    }
}
