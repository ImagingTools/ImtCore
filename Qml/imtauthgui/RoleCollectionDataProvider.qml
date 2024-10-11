import QtQuick 2.12
import imtcolgui 1.0
import imtauthRolesSdl 1.0

CollectionDataProvider {
    id: container;

    fields: [
        RoleItemTypeMetaInfo.s_id,
        RoleItemTypeMetaInfo.s_roleName,
        RoleItemTypeMetaInfo.s_roleDescription,
        RoleItemTypeMetaInfo.s_parentRoles,
        RoleItemTypeMetaInfo.s_roleId,
        RoleItemTypeMetaInfo.s_productId
    ];

    commandId: ImtauthRolesSdlCommandIds.s_rolesList;
    subscriptionCommandId: "OnRolesCollectionChanged"
    sortByField: RoleItemTypeMetaInfo.s_roleName;

    property string productId;

    function getHeaders(){
        if (productId === ""){
            console.error("Unable to get header for request:", commandId, ". Error: Product-ID is empty");
            return null;
        }

        let obj = {}
        obj["ProductId"] = productId;
        return obj;
    }
}


