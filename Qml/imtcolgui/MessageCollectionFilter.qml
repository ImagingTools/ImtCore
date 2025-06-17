import QtQuick 2.15
import Acf 1.0
import com.imtcore.imtqml 1.0
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
        filterChanged();
    }
}

