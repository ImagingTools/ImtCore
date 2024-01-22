pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0

CollectionDataProvider {
    id: container;

    commandId: "Licenses";

    fields: ["Id", "LicenseId", "LicenseName", "ProductId"]
    sortByField: "LicenseName";
}


