import QtQuick 2.12
import Acf 1.0

QtObject {
    id: root;

    property TreeItemModel filterModel: TreeItemModel {}

    signal filterChanged();

    function getTextFilter(){
        if (filterModel.ContainsKey("TextFilter")){
            return filterModel.GetData("TextFilter");
        }

        return "";
    }

    function setTextFilter(filter){
        filterModel.SetData("TextFilter", filter);

        root.filterChanged();
    }

    function getSortingOrder(){
        if (filterModel.ContainsKey("Sort")){
            let sortModel = filterModel.GetData("Sort");
            if (sortModel.ContainsKey("SortOrder")){
                return sortModel.GetData("SortOrder")
            }
        }

        return "";
    }

    function setSortingOrder(sortingOrder){
        let sortModel = filterModel.GetData("Sort");
        if (!sortModel){
            sortModel = filterModel.AddTreeModel("Sort");
        }

        sortModel.SetData("SortOrder", sortingOrder);

        root.filterChanged();
    }

    function getSortingInfoId(){
        if (filterModel.ContainsKey("Sort")){
            let sortModel = filterModel.GetData("Sort");
            if (sortModel.ContainsKey("HeaderId")){
                return sortModel.GetData("HeaderId")
            }
        }

        return "";
    }

    function setSortingInfoId(sortingInfoId){
        let sortModel = filterModel.GetData("Sort");
        if (!sortModel){
            sortModel = filterModel.AddTreeModel("Sort");
        }

        sortModel.SetData("HeaderId", sortingInfoId);

        root.filterChanged();
    }

    function getFilteringInfoIds(){
        if (filterModel.ContainsKey("FilterIds")){
            return filterModel.GetData("FilterIds");
        }

        return "";
    }

    function setFilteringInfoIds(filteringInfoIds){
        if (filteringInfoIds.length > 0){
            let filteringInfoIdsStr = filteringInfoIds.join(';');

            filterModel.SetData("FilterIds", filteringInfoIdsStr);
        }
        else{
            filterModel.SetData("FilterIds", "");
        }

        root.filterChanged();
    }
}
