pragma Singleton

import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcolgui 1.0

CollectionDataProvider {
    id: container;

    fields: ["id", "name", "description"];
    commandId: "AccountsList";
    subscriptionCommandId: "OnAccountsCollectionChanged"

    sortByField: "name";
}


