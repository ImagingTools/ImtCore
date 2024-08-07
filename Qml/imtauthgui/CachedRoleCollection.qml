pragma Singleton

import QtQuick 2.12
import imtcolgui 1.0

CollectionDataProvider {
    id: container;

    fields: ["Id", "RoleName", "RoleDescription", "ParentRoles", "RoleId", "Name"];
    commandId: "RolesList";
    subscriptionCommandId: "OnRolesCollectionChanged"

    sortByField: "Name";

//    property string appName: context.appName ? context.appName : ""

//    function getAdditionalInputParams(){
//        console.log("CachedRoleCollection.qml getAdditionalInputParams",context.appName);

//        let obj = {}
//        obj["ProductId"] = appName;
//        return obj;
//    }
}


