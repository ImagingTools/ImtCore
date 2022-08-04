import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

Item {
    id: container;

    property Item editorItem: null;
    property Item commandsProvider: null;

    property string itemId: editorItem.itemId;
    property string commandsId;

    property string itemsInfoModel;

    property int accountCurrentIndex: customerCB.currentIndex;
    property int productCurrentIndex: productCB.currentIndex;

    property string gqlModelQueryType;
    property string gqlModelQueryTypeNotify;

    signal modelItemsLoaded();

    Component.onCompleted: {
        itemsModel.updateModel("AccountsList");
    }

    onVisibleChanged: {
        if (container.visible){
            Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
        }
        else{
            Events.unSubscribeEvent(container.commandsId + "CommandActivated", container.commandHandle)
        }
    }

    onModelItemsLoaded: {

        /**
            Если создается новая инсталляция, то загружаем лиценции первого продукта,
            иначе делаем Item запрос
        */
        if (itemId === ""){

            productCB.currentIndex = 0;
            customerCB.currentIndex = 0;

            let productId = productCB.model.GetData("Id");
            licensesModel.updateModel(productId);
        }
        else{
            itemModel.updateModel();
        }
    }

    onProductCurrentIndexChanged: {
        console.log("InstallationCommands onProductCurrentIndexChanged", productCurrentIndex);

        if (!productCB.model){
            return;
        }

        let productId = productCB.model.GetData("Id", productCurrentIndex);

        /**
            Если лицензии текущего продукта уже загружены, то return
        */
        if (licensesTable.elements.ContainsKey("ProductId")){
            let currentProductId = licensesTable.elements.GetData("ProductId");

            if (currentProductId == productId){
                return;
            }
        }

        licensesModel.updateModel(productId);
    }

    onCommandsIdChanged: {
        console.log("InstallationCommands onCommandsIdChanged", container.commandsId);
        Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
    }

    Component {
        id: inputDialog;
        InputDialog {
            onFinished: {
                if (buttonId == "Ok"){
                    editorItem.itemName = inputValue;
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

    function commandHandle(commandId){
        console.log("InstallationCommands commandActivated", commandId);

        if (commandId === "Save"){
            if (itemId === ""){
                gqlModelQueryType = "InstallationAdd";
                gqlModelQueryTypeNotify = "addedNotification";

                modalDialogManager.openDialog(inputDialog, {"message": qsTr("Please enter the name of the installation:")});
            }
            else{
                gqlModelQueryType = "InstallationUpdate";
                gqlModelQueryTypeNotify = "updatedNotification";
                saveQuery.updateModel();
            }
        }
        else if (commandId === "Close"){
            multiDocView.closePage("");
        }
    }

    GqlModel {
        id: itemModel;

        function updateModel() {
            console.log( "updateModel InstallationItem");

            var query = Gql.GqlRequest("query", "InstallationItem");

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", installationEditorContainer.itemId);
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("item");

            queryFields.InsertField("Id");
            queryFields.InsertField("Name");
            queryFields.InsertField("AccountId");
            queryFields.InsertField("ProductId");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("InstallationItem query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, itemModel);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (itemModel.ContainsKey("errors")){
//                    modalDialogManager.openDialog(inputDialog, {"message": qsTr("Please enter the name of the installation:")});
                    return;
                }

                dataModelLocal = itemModel.GetData("data");
                if(dataModelLocal.ContainsKey("InstallationItem")){
                    dataModelLocal = dataModelLocal.GetData("InstallationItem");
                    if(dataModelLocal.ContainsKey("item")){
                        dataModelLocal = dataModelLocal.GetData("item");

                        let keys = dataModelLocal.GetKeys();
                        for (let i = 0; i < keys.length; i++){
                            installationModel.SetData(keys[i], dataModelLocal.GetData(keys[i]));
                        }

                        licensesTable.elements = dataModelLocal.GetData("ActiveLicenses");

                        updateGui();
                    }
                }
            }
        }
    }//GqlModel itemModel

    GqlModel {
        id: saveQuery;

        function updateModel() {
            console.log( "InstallationInfoEditor saveQuery updateModel");

            var query = Gql.GqlRequest("query", gqlModelQueryType);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", container.itemId);
            inputParams.InsertField("Name", editorItem.itemName);
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject(gqlModelQueryTypeNotify);

            var jsonString = installationModel.toJSON();
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")
            inputParams.InsertField ("Item", jsonString);

            queryFields.InsertField("Id");
            queryFields.InsertField("Successed");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("InstallationInfoEditot save model query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, saveQuery);
            if (this.state === "Ready"){
                var dataModelLocal;

                if (saveQuery.ContainsKey("errors")){

                    dataModelLocal = saveQuery.GetData("errors");

                    dataModelLocal = dataModelLocal.GetData(container.gqlModelQueryType);

                    if (dataModelLocal){
                        var messageError = dataModelLocal.GetData("message");

                        modalDialogManager.openDialog(inputDialog, {"message": messageError});
                    }

                    return;
                }
                if (saveQuery.ContainsKey("data")){
                    dataModelLocal = saveQuery.GetData("data");
                    if (dataModelLocal.ContainsKey(container.gqlModelQueryType)){
                        dataModelLocal = dataModelLocal.GetData(container.gqlModelQueryType);

                        if (dataModelLocal.ContainsKey(container.gqlModelQueryTypeNotify)){
                            dataModelLocal = dataModelLocal.GetData(container.gqlModelQueryTypeNotify);

                            if (dataModelLocal.ContainsKey("Id")){
                                container.itemId = dataModelLocal.GetData("Id");
                            }

                            if (dataModelLocal.ContainsKey("Name")){
                                let name = dataModelLocal.GetData("Name");
                                multiDocView.updatePageTitle({"ItemId": "", "Title": name});
                            }
                            Events.sendEvent(commandsId + "CollectionUpdateGui");
                        }
                    }
                }
            }
        }
    }//GqlModel saveQuery

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

            itemsInfoModel = modelInfo;
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
                    if (dataModelLocal.ContainsKey(itemsInfoModel)){
                        dataModelLocal = dataModelLocal.GetData(itemsInfoModel);
                        if(dataModelLocal.ContainsKey("items")){
                            dataModelLocal = dataModelLocal.GetData("items");

                            if (itemsInfoModel === "AccountsList"){
                                customerCB.model = dataModelLocal;
                                itemsModel.updateModel("ProductsList");
                            }
                            else if (itemsInfoModel === "ProductsList"){
                                productCB.model = dataModelLocal;

                                modelItemsLoaded();
                            }
                        }
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

                            licensesTable.elements = dataModelLocal;
                            installationModel.SetData("ActiveLicenses", dataModelLocal);
                        }
                    }
                }
            }
        }
    }//GqlModel licensesModel
}
