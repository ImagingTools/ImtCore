import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

DocumentWorkspaceCommandsDelegateBase {
    id: container;

    showInputIdDialog: true;

    property int accountCurrentIndex: customerCB.currentIndex;
    property int productCurrentIndex: productCB.currentIndex;

    Component.onCompleted: {

        updateItemTimer = 200;
        itemsModel.updateModel("AccountsList");

        timer.start();
    }

    onEntered: {
        objectModel.SetData("Name", value);
    }

    Timer {
        id: timer;

        interval: 100;

        onTriggered: {
            itemsModel.updateModel("ProductsList");
        }
    }

    onProductCurrentIndexChanged: {
        console.log("InstallationCommands onProductCurrentIndexChanged", productCurrentIndex);

        if (!productCB.model){
            return;
        }

        let productId = productCB.model.GetData("Id", productCurrentIndex);
        if (documentModel.GetData("ActiveLicenses").ContainsKey("ProductId")){
            let currentProductId = documentModel.GetData("ActiveLicenses").GetData("ProductId");

            if (currentProductId == productId){
                return;
            }
        }

        licensesModel.updateModel(productId);
    }

    Component {
        id: inputDialog;
        InputDialog {
            onFinished: {
                if (buttonId == "Ok"){
                    documentBase.itemName = inputValue;
                    saveQuery.updateModel();
                }
            }
        }
    }

    Component {
        id: errorDialog;
        MessageDialog {
            onFinished: {
            }
        }
    }

    GqlModel {
        id: itemsModel;

        function updateModel(modelInfo) {
            console.log( "InstallationInfoEditor load items", modelInfo);
            var query = Gql.GqlRequest("query", modelInfo);

            var inputParams = Gql.GqlObject("input");
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField("Id");
            queryFields.InsertField("Name");
            query.AddField(queryFields)

            var gqlData = query.GetQuery();
            console.log("InstallationInfoEditor items query  ", gqlData);

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, itemsModel);
            if (this.state === "Ready"){
                let dataModelLocal;

                if (itemsModel.ContainsKey("errors")){

                    return;
                }

                if (itemsModel.ContainsKey("data")){
                    dataModelLocal = itemsModel.GetData("data");
                    console.log("itemsModel", itemsModel);
                    if (dataModelLocal.ContainsKey("AccountsList")){
                        dataModelLocal = dataModelLocal.GetData("AccountsList");
                        dataModelLocal = dataModelLocal.GetData("items");

                        customerCB.model = dataModelLocal;

                        console.log("customerCB.model", customerCB.model);
                    }
                    else if (dataModelLocal.ContainsKey("ProductsList")){
                        dataModelLocal = dataModelLocal.GetData("ProductsList");

                        dataModelLocal = dataModelLocal.GetData("items");

                        productCB.model = dataModelLocal;

                        console.log("productCB.model", productCB.model);

                    }
                }
            }
        }
    }//GqlModel itemsModel

    GqlModel {
        id: licensesModel;

        function updateModel(productId) {
            console.log( "InstallationEditor licensesModel", licensesModel);
            var query = Gql.GqlRequest("query", "ProductList");

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", productId);
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("items");
            queryFields.InsertField("Id");
            queryFields.InsertField("Name");
            query.AddField(queryFields)

            var gqlData = query.GetQuery();
            console.log("InstallationInfoEditor items query", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, licensesModel);
            if (this.state === "Ready"){
                let dataModelLocal;

                if (licensesModel.ContainsKey("errors")){

                    return;
                }

                if (licensesModel.ContainsKey("data")){
                    dataModelLocal = licensesModel.GetData("data");
                    if (dataModelLocal.ContainsKey("ProductList")){
                        dataModelLocal = dataModelLocal.GetData("ProductList");
                        if(dataModelLocal.ContainsKey("items")){
                            dataModelLocal = dataModelLocal.GetData("items");

                            for (let i = 0; i < dataModelLocal.GetItemsCount(); i++){
                                dataModelLocal.SetData("LicenseState", 0, i);
                                dataModelLocal.SetData("ExpirationState", 0, i);
                                dataModelLocal.SetData("Expiration", "Unlimited", i);
                            }

                            activeLicenses = dataModelLocal;
                            documentModel.SetExternTreeModel("ActiveLicenses", dataModelLocal);
                        }
                    }
                }
            }
        }
    }//GqlModel licensesModel
}
