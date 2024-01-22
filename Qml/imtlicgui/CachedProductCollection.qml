pragma Singleton

import QtQuick 2.12
import Acf 1.0
import imtcolgui 1.0

CollectionDataProvider {
    id: container;

    commandId: "Products";
    fields: ["Id", "ProductName", "Description", "CategoryId", "Licenses"];
    sortByField: "ProductName";

    property TreeItemModel hardwareProductsModel: TreeItemModel {};
    property TreeItemModel softwareProductsModel: TreeItemModel {};

    onCompletedChanged: {
        if (completed){
            hardwareProductsModel.Clear();
            softwareProductsModel.Clear();

            for (let i = 0; i < container.collectionModel.GetItemsCount(); i++){
                let category = container.collectionModel.GetData("CategoryId", i);
                if (category === "Software"){
                    let index = softwareProductsModel.InsertNewItem();
                    softwareProductsModel.CopyItemDataFromModel(index, container.collectionModel, i)
                }
                else if (category === "Hardware"){
                    let index = hardwareProductsModel.InsertNewItem();
                    hardwareProductsModel.CopyItemDataFromModel(index, container.collectionModel, i)
                }
            }

            hardwareProductsModel.Refresh();
            softwareProductsModel.Refresh();
        }
    }
}


