import QtQuick 2.15
import Acf 1.0
import imtcolgui 1.0

CollectionFilter {
    id: root;

    property TreeItemModel filterableHeadersModel: TreeItemModel{
    }

    Component.onCompleted: {
        filterableHeadersModel.setData("id", "Text")
        let index = filterableHeadersModel.insertNewItem()
        filterableHeadersModel.setData("id", "Timestamp", index)
        index = filterableHeadersModel.insertNewItem()
        filterableHeadersModel.setData("id", "Source", index)
        setFilteringInfoIds(filterableHeadersModel)
    }

    function setMessageStatusFilter(messageKey, status){
        console.log("*DEBUG* setMessageStatusFilter")
        let objectFilter = filterModel.getData("ObjectFilter");
        if (!objectFilter){
            objectFilter = filterModel.addTreeModel("ObjectFilter")
        }
        let categoryFilter = objectFilter.getData("Category");
        if (!categoryFilter){
            categoryFilter = objectFilter.addTreeModel("Category")
        }
        categoryFilter.setData(messageKey, status);
        console.log("*DEBUG* filterModel", filterModel.toJson())

        filterChanged();
    }
}

