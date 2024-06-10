pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0

CollectionDataProvider {
    id: container;

    commandId: "ProductsList";
    subscriptionCommandId: "OnProductsCollectionChanged"

    fields: ["Id", "ProductName", "Description", "CategoryId", "Licenses", "Features"];
    sortByField: "ProductName";

    property TreeItemModel hardwareProductsModel: TreeItemModel {};
    property TreeItemModel softwareProductsModel: TreeItemModel {};

    onCompletedChanged: {
        if (completed){
            hardwareProductsModel.clear();
            softwareProductsModel.clear();

            for (let i = 0; i < container.collectionModel.getItemsCount(); i++){
                let category = container.collectionModel.getData("CategoryId", i);
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


