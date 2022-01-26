import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: productsCollectionViewContainer;
    anchors.fill: parent;
    property Item rootItem;
    property Item multiDocViewItem;
    property alias itemId: productCollectionView.itemId;
    property alias itemName: productCollectionView.itemName;

    property alias model: productCollectionView.model;

    function dialogResult(parameters) {
         console.log("ProductView dialogResult", parameters["status"]);

        if (parameters["status"] === "ok") {

            if (parameters["dialog"] === "EditLicense") {
                var dataModelLocal = productCollectionView.model.GetData("data");
                console.log("ProductView onClicked ", dataModelLocal.GetItemsCount())
                dataModelLocal.SetData("Id", parameters["newLicenseId"] , productCollectionView.selectedIndex);//
                dataModelLocal.SetData("Name", parameters["newLicenseName"], productCollectionView.selectedIndex);

                productCollectionView.model.SetData("data", dataModelLocal);
                productCollectionView.refresh();
            }
            else if (parameters["dialog"] === "InputDialog") {
                var value = parameters["value"];
                console.log("LicenseCollectionViewContainer dialogResult", value);
                productsCollectionViewContainer.rootItem.updateTitleTab(value);
                saveModel.updateModel(value);
            }
        }
        else if (parameters["status"] === "yes") {

            if (featureCollectionView.model.ContainsKey("data")) {
                var dataModelLocal = productCollectionView.model.GetData("data");
                dataModelLocal.RemoveItem(productCollectionView.table.selectedIndex);

                productCollectionView.model.SetData("data", dataModelLocal);
                productCollectionView.model.Refresh();
                productCollectionView.refresh();
            }
        }
    }

    function menuActivated(menuId) {

        console.log("ProductView menuActivated", menuId);
        if (menuId  === "New"){
            var countItems = model.GetData("data").GetItemsCount();
            var dataModelLocal = model.GetData("data");
            var index = dataModelLocal.InsertNewItem();

            dataModelLocal.SetData("Name", "Product Name", index);//
            dataModelLocal.SetData("Id", "", index);//

            model.SetData("data", dataModelLocal);
            model.Refresh();
            productCollectionView.refresh();

        }
        else if (menuId  === "Save") {
            if (productCollectionView.itemId === "") {
                var source = "AuxComponents/InputDialog.qml";
                var parameters = {};
                parameters["message"] = "Please enter the name of the document: ";
                parameters["nameDialog"] = "Document Name";
                parameters["resultItem"] = productsCollectionViewContainer;
                thubnailDecoratorContainer.openDialog(source, parameters);
            }
            else {
                saveModel.updateModel()
            }
        }else if (menuId  === "Remove") {
            var source = "AuxComponents/MessageDialog.qml";
            var parameters = {};
            parameters["message"] = "Remove selected file from the database ?";
            parameters["nameDialog"] = "RemoveDialog";
            parameters["resultItem"] = productsCollectionViewContainer;
            thubnailDecoratorContainer.openDialog(source, parameters);
        }else if (menuId  === "Close") {
             productsCollectionViewContainer.rootItem.closeTab();
        } else {
             productCollectionView.menuActivated(menuId)
        }
    }

    function refresh() {
        console.log("PackageView refresh()");
        productCollectionView.refresh();
    }

    function commandsChanged(commandsId){
        if (commandsId !== "ProductEdit") {
            return;
        }

        if (productCollectionView.selectedIndex > -1) {
            productsCollectionViewContainer.rootItem.setModeMenuButton("Remove", "Normal");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Edit", "Normal");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Duplicate", "Normal");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Import", "Normal");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Export", "Normal");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Save", "Normal");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Close", "Normal");
        } else {
            productsCollectionViewContainer.rootItem.setModeMenuButton("Remove", "Disabled");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Edit", "Disabled");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Duplicate", "Disabled");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Export", "Disabled");
            productsCollectionViewContainer.rootItem.setModeMenuButton("Save", "Disabled");
        }
    }

    TreeItemModel {
        id: productModel;
    }

    CollectionView {
        id: productCollectionView;
        anchors.left: parent.left;
        anchors.right: productMetaInfo.left;
        height: parent.height;

        Component.onCompleted: {
            console.log("ProductView CollectionView onCompleted");
            productCollectionView.gqlModelInfo = "ProductInfo"
            productCollectionView.gqlModelItems = "LicenseList"
            productCollectionView.gqlModelRemove = "";
        }


        onItemIdChanged: {
            if (productCollectionView.itemId){
                console.log("ProductView onItemIdChanged")
                productCollectionView.gqlModelInfo = "ProductInfo"
                productCollectionView.gqlModelItems = "LicenseList"
                productCollectionView.gqlModelRemove = "";
            }
        }

        onSelectItem: {
            console.log("ProductView CollectionView onSelectItem", itemId, name);

            var source = "../imtlicgui/EditLicenseDialog.qml";
            var parameters = {};
            parameters["licenseId"] = itemId;
            parameters["licenseName"] = name;
            parameters["resultItem"] = productsCollectionViewContainer;

            thubnailDecoratorContainer.openDialog(source, parameters);
        }

        onSelectedIndexChanged: {
            console.log("PackageView CollectionView onSelectedIndexChanged", productCollectionView.selectedIndex);
            if (productCollectionView.selectedIndex > -1){
                productsCollectionViewContainer.commandsChanged("ProductEdit")
            }
        }

    }

    GqlModel {
        id: saveModel;

        function updateModel(newId) {
            console.log( "updateModel saveModel");

            var query;
            var queryFields;
            var inputParams = Gql.GqlObject("input");

            if(productsCollectionViewContainer.itemId != ""){
                query = Gql.GqlRequest("query", "ProductUpdate");
                inputParams.InsertField("Id");
                inputParams.InsertFieldArgument("Id", productsCollectionViewContainer.itemId);
                queryFields = Gql.GqlObject("updatedNotification");
            }
            else{
                query = Gql.GqlRequest("query", "ProductAdd");
                queryFields = Gql.GqlObject("addedNotification");
            }
            query.AddParam(inputParams);

            if (newId !== undefined && newId !== "") {
                //console.log();
                productsCollectionViewContainer.itemId = newId;
                productsCollectionViewContainer.itemName = newId;
            }

            productModel.SetData("Id", productsCollectionViewContainer.itemId)
            productModel.SetData("Name", productsCollectionViewContainer.itemName)
            productModel.SetExternTreeModel("features", productCollectionView.model.GetData("data"));

            //featureCollectionViewContainer.model.SetIsArray(false);
            var jsonString = productModel.toJSON();
            console.log("jsonString", jsonString)
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")
            console.log("jsonString", jsonString)

            inputParams.InsertField("Item");
            inputParams.InsertFieldArgument ("Item", jsonString);

            queryFields.InsertField("Id");
            queryFields.InsertField("Successed");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("ProductEdit query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, saveModel);
            if (this.state === "Ready"){

                var dataModelLocal = model.GetData("data");
                if(dataModelLocal.ContainsKey("addedNotification")){
                    dataModelLocal = dataModelLocal.GetData("addedNotification");
                                            productCollectionView.refresh();
                    if(dataModelLocal !== null && dataModelLocal.ContainsKey("Id") && productsCollectionViewContainer.itemId === ""){
                        productsCollectionViewContainer.itemId = dataModelLocal.GetData("Id");
                    }
                    else if(saveModel.ContainsKey("errors")){
                        var errorsModel = accountItemModel.GetData("errors");
                        if(errorsModel !== null && errorsModel.ContainsKey(containerContactInfo.gqlModelItems)){
                            console.log("message", errorsModel.GetData(productsCollectionViewContainer.gqlModelItems).GetData("message"));
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: productMetaInfo;
        anchors.right: parent.right;
        height: parent.height;
        width: 150;
        color: "transparent";
    }
}
