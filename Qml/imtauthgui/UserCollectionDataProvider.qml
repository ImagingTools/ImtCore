import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcolgui 1.0
import imtauthUsersSdl 1.0

CollectionDataProvider {
    id: container;
    fields: [UserItemDataTypeMetaInfo.s_id, UserItemDataTypeMetaInfo.s_name];
    commandId: ImtauthUsersSdlCommandIds.s_usersList;
    subscriptionCommandId: "OnUsersCollectionChanged"
    sortByField: UserItemDataTypeMetaInfo.s_name;
}


