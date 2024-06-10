import QtQuick 2.12
import Acf 1.0

QtObject {
    id: root;

    property TreeItemModel filterModel: TreeItemModel {}

    signal filterChanged();

    function getTextFilter(){
        if (filterModel.containsKey("TextFilter")){
            return filterModel.getData("TextFilter");
        }

        return "";
    }

    function setTextFilter(filter){
        filterModel.setData("TextFilter", filter);

        root.filterChanged();
    }

    function getSortingOrder(){
        if (filterModel.containsKey("Sort")){
            let sortModel = filterModel.getData("Sort");
            if (sortModel.containsKey("SortOrder")){
                return sortModel.getData("SortOrder")
            }
        }

        return "";
    }

    function setSortingOrder(sortingOrder){
        let sortModel = filterModel.getData("Sort");
        if (!sortModel){
            sortModel = filterModel.addTreeModel("Sort");
        }

        sortModel.setData("SortOrder", sortingOrder);

        root.filterChanged();
    }

    function getSortingInfoId(){
        if (filterModel.containsKey("Sort")){
            let sortModel = filterModel.getData("Sort");
            if (sortModel.containsKey("HeaderId")){
                return sortModel.getData("HeaderId")
            }
        }

        return "";
    }

    function setSortingInfoId(sortingInfoId){
        let sortModel = filterModel.getData("Sort");
        if (!sortModel){
            sortModel = filterModel.addTreeModel("Sort");
        }

        sortModel.setData("HeaderId", sortingInfoId);

        root.filterChanged();
    }

    function getFilteringInfoIds(){
        if (filterModel.containsKey("FilterIds")){
            return filterModel.getData("FilterIds");
        }

        return "";
    }

    function setFilteringInfoIds(filteringInfoIds){
        filterModel.setExternTreeModel("FilterIds", filteringInfoIds)

        root.filterChanged();
    }
}
