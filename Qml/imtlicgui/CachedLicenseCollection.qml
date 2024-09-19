pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0
import imtlicLicensesSdl 1.0

CollectionDataProvider {
    id: container;
    commandId: ImtlicLicensesSdlCommandIds.s_licensesList;
    subscriptionCommandId: "OnLicensesCollectionChanged"
    fields: [
        LicenseItemTypeMetaInfo.s_id,
        LicenseItemTypeMetaInfo.s_licenseId,
        LicenseItemTypeMetaInfo.s_licenseName,
        LicenseItemTypeMetaInfo.s_productId,
        LicenseItemTypeMetaInfo.s_parentLicenses,
        LicenseItemTypeMetaInfo.s_features,
        LicenseItemTypeMetaInfo.s_productUuid
    ]
    sortByField: LicenseItemTypeMetaInfo.s_licenseName;
}


