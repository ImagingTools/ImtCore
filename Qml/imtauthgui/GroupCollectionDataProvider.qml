import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcolgui 1.0
import imtauthGroupsSdl 1.0

CollectionDataProvider {
    id: container;
    fields: [
        GroupItemDataTypeMetaInfo.s_id,
        GroupItemDataTypeMetaInfo.s_name,
        GroupItemDataTypeMetaInfo.s_description,
        GroupItemDataTypeMetaInfo.s_parentGroups,
        GroupItemDataTypeMetaInfo.s_roles
    ];
    commandId: ImtauthGroupsSdlCommandIds.s_groupsList;
    subscriptionCommandId: "OnGroupsCollectionChanged"
    sortByField: GroupItemDataTypeMetaInfo.s_name;
}


