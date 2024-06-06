import QtQuick 2.15
import Acf 1.0
import imtcolgui 1.0

CollectionFilter {
    id: root;

    property TreeItemModel filterableHeadersModel: TreeItemModel{
    }

    Component.onCompleted: {
        filterableHeadersModel.SetData("Id", "Text")
        let index = filterableHeadersModel.InsertNewItem()
        filterableHeadersModel.SetData("Id", "Timestamp", index)
        index = filterableHeadersModel.InsertNewItem()
        filterableHeadersModel.SetData("Id", "Source", index)
        setFilteringInfoIds(filterableHeadersModel)
    }

    function setMessageStatusFilter(messageKey, status){
        console.log("*DEBUG* setMessageStatusFilter")
        let objectFilter = filterModel.GetData("ObjectFilter");
        if (!objectFilter){
            objectFilter = filterModel.AddTreeModel("ObjectFilter")
        }
        let categoryFilter = objectFilter.GetData("Category");
        if (!categoryFilter){
            categoryFilter = objectFilter.AddTreeModel("Category")
        }
        categoryFilter.SetData(messageKey, status);
        console.log("*DEBUG* filterModel", filterModel.ToJson())

        filterChanged();
    }
}

