pragma Singleton

import QtQuick 2.12
import imtcolgui 1.0

CollectionDataProvider {
    id: container;

    fields: ["Id", "RoleName", "RoleDescription", "ParentRoles", "RoleId", "Name"];
    commandId: "RolesList";
    subscriptionCommandId: "OnRolesCollectionChanged"

    sortByField: "Name";

    property string productId;

    function getAdditionalInputParams(){
        let obj = {}
        obj["ProductId"] = productId;
        return obj;
    }
}


