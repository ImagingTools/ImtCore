import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0
import imtauthGroupsSdl 1.0

CollectionDataProvider {
    id: container;
    fields: [
        GroupItemTypeMetaInfo.s_id,
        GroupItemTypeMetaInfo.s_name,
        GroupItemTypeMetaInfo.s_description,
        GroupItemTypeMetaInfo.s_parentGroups,
        GroupItemTypeMetaInfo.s_roles
    ];
    commandId: ImtauthGroupsSdlCommandIds.s_groupsList;
    subscriptionCommandId: "OnGroupsCollectionChanged"
    sortByField: GroupItemTypeMetaInfo.s_name;
}


