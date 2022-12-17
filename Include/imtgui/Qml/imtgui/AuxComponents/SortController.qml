import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: container;

    property var collectionView;
    property var commands;

    TreeItemModel {
        id: sortModel;
    }

    function headerClicked(headerId){
        console.log("headerClicked", headerId);

        let order = "";

        if(sortModel.ContainsKey(headerId)){
            order = sortModel.GetData(headerId);
        }

        if (order == "ASC"){
            sortModel.SetData(headerId, "DESC");
            order = "DESC";
        }
        else{
            sortModel.SetData(headerId, "ASC");
            order = "ASC";
        }

        container.collectionView.setHeaderSort(headerId, order);
        container.commands.updateItemsModel();
    }
}
