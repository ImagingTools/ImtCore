pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0

CollectionDataProvider {
    id: container;

    fields: ["Id", "RoleName", "RoleDescription", "ParentRoles", "RoleId", "Name"];
    commandId: "RolesList";
    subscriptionCommandId: "OnRolesCollectionChanged"

    sortByField: "Name";
}


