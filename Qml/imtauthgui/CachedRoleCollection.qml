pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0

CollectionDataProvider {
    id: container;

    fields: ["Id", "Name", "Description"];
    commandId: "Roles";

    sortByField: "Name";

    property TreeItemModel rolesModel: TreeItemModel {}

    onCollectionModelChanged: {
        if (container.collectionModel != null){
            if (container.collectionModel.ContainsKey("Roles")){
                let rolesModel = container.collectionModel.GetData("Roles");
                container.rolesModel = rolesModel;
            }
        }
    }
}


