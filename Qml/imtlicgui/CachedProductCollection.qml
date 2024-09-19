pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0
import imtlicProductsSdl 1.0

CollectionDataProvider {
    id: container;

    commandId: ImtlicProductsSdlCommandIds.s_productsList;
    subscriptionCommandId: "OnProductsCollectionChanged"

    // fields: [
    //     ProductItemTypeMetaInfo.s_id,
    //     ProductItemTypeMetaInfo.s_productName,
    //     ProductItemTypeMetaInfo.s_description,
    //     ProductItemTypeMetaInfo.s_categoryId,
    //     ProductItemTypeMetaInfo.s_licenses,
    //     ProductItemTypeMetaInfo.s_features
    // ];
    fields: [
        "Id",
        "ProductName",
        "Description",
        "CategoryId",
        "Licenses",
        "Features"
    ];
    sortByField: ProductItemTypeMetaInfo.s_productName;

    property TreeItemModel hardwareProductsModel: TreeItemModel {};
    property TreeItemModel softwareProductsModel: TreeItemModel {};

    onCompletedChanged: {
        if (completed){
            hardwareProductsModel.clear();
            softwareProductsModel.clear();

            for (let i = 0; i < container.collectionModel.getItemsCount(); i++){
                let category = container.collectionModel.getData(ProductItemTypeMetaInfo.s_categoryId, i);
                if (category === "Software"){
                    let index = softwareProductsModel.insertNewItem();
                    softwareProductsModel.copyItemDataFromModel(index, container.collectionModel, i)
                }
                else if (category === "Hardware"){
                    let index = hardwareProductsModel.insertNewItem();
                    hardwareProductsModel.copyItemDataFromModel(index, container.collectionModel, i)
                }
            }

            hardwareProductsModel.refresh();
            softwareProductsModel.refresh();
        }
    }
}


