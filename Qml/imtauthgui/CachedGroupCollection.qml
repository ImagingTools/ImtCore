pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0

CollectionDataProvider {
    id: container;

    fields: ["Id", "Name", "Description", "ParentGroups", "Roles"];

    commandId: "GroupsList";

    sortByField: "Name";
}


