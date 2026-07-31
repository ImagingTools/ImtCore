pragma Singleton

import QtQuick 2.12
import Acf 1.0
import com.imtcore.imtqml 1.0
import imtcolgui 1.0
import imtlicFeaturesSdl 1.0

CollectionDataProvider {
    id: container;
    commandId: ImtlicFeaturesSdlCommandIds.s_featuresList;
    subscriptionCommandId: "OnFeaturesCollectionChanged"

    fields: [
        FeatureItemTypeMetaInfo.s_id,
        FeatureItemTypeMetaInfo.s_featureId,
        FeatureItemTypeMetaInfo.s_featureName,
        FeatureItemTypeMetaInfo.s_description,
        FeatureItemTypeMetaInfo.s_optional,
        FeatureItemTypeMetaInfo.s_subFeatures
    ];
    sortByField: FeatureItemTypeMetaInfo.s_featureName;
}


