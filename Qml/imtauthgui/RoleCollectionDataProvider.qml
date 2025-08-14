import QtQuick 2.12
import imtcolgui 1.0
import imtauthRolesSdl 1.0

CollectionDataProvider {
    id: container;

    fields: [
        RoleItemDataTypeMetaInfo.s_id,
        RoleItemDataTypeMetaInfo.s_roleName,
        RoleItemDataTypeMetaInfo.s_roleDescription,
        RoleItemDataTypeMetaInfo.s_parentRoles,
        RoleItemDataTypeMetaInfo.s_roleId,
        RoleItemDataTypeMetaInfo.s_productId
    ];

    commandId: ImtauthRolesSdlCommandIds.s_rolesList;
    subscriptionCommandId: "OnRolesCollectionChanged"
    sortByField: RoleItemDataTypeMetaInfo.s_roleName;

    property string productId;

    function getHeaders(){
        if (productId === ""){
            console.error("Unable to get header for request:", commandId, ". Error: Product-ID is empty");
            return null;
        }

        let obj = {}
        obj["productId"] = productId;
        return obj;
    }
}


