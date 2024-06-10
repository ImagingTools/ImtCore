pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0

CollectionDataProvider {
    id: container;

    fields: ["Id", "Name", "Description"];
    commandId: "RolesList";
    subscriptionCommandId: "OnRolesCollectionChanged"

    sortByField: "Name";

    property TreeItemModel rolesModel: TreeItemModel {}

    onCollectionModelChanged: {
        if (container.collectionModel != null){
            if (container.collectionModel.containsKey("Roles")){
                let rolesModel = container.collectionModel.getData("Roles");
                container.rolesModel = rolesModel;
            }
        }
    }
}


