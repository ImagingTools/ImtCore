import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0
import imtauthUsersSdl 1.0

CollectionDataProvider {
    id: container;
    fields: [UserItemTypeMetaInfo.s_id, UserItemTypeMetaInfo.s_name];
    commandId: ImtauthUsersSdlCommandIds.s_usersList;
    subscriptionCommandId: "OnUsersCollectionChanged"
    sortByField: UserItemTypeMetaInfo.s_name;
}


