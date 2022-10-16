import QtQuick 2.12
import Acf 1.0
import imtgui 1.0

Item {
    id: container;

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

        setHeaderSort(headerId, order);
        baseCommands.updateModels();
    }
}
