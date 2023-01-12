import QtQuick 2.12
import Acf 1.0
import imtgui 1.0
import imtqml 1.0

DocumentWorkspaceCommandsDelegateBase {
    id: container;

    showInputIdDialog: true;

//    autoUpdate: false;

    property bool instanceIdAcceptable: helperInput.acceptableInput;

    Component.onCompleted: {
        updateItemTimer = 400;

        itemsModel.updateModel("AccountsList");
        itemsModel.updateModel("ProductsList");


//        timer.start();
    }

    onEntered: {
        objectModel.SetData("Name", value);
    }

    onInstanceIdAcceptableChanged: {
        console.log("InstallationCommands onInstanceIdAcceptableChanged", instanceIdAcceptable);

        if (!instanceIdAcceptable){
            commandsProvider.setCommandIsEnabled("Save", false);
        }
    }

    Timer {
        id: timer;

        interval: 200;

        onTriggered: {
            itemsModel.updateModel("ProductsList");
        }
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

    function modelChanged(){
        console.log("InstallationCommands modelChanged", instanceIdAcceptable);

        if (instanceIdAcceptable){
            commandsProvider.setCommandIsEnabled("Save", true);

            let suffix = "*";
            documentManager.setDocumentTitle({"Id": documentBase.itemId, "Title": documentBase.itemName + suffix});
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

                console.log("itemsModel Ready");
                let dataModelLocal;

                if (itemsModel.ContainsKey("errors")){

                    return;
                }

                if (itemsModel.ContainsKey("data")){
                    dataModelLocal = itemsModel.GetData("data");

                    if (dataModelLocal.ContainsKey("AccountsList")){
                        dataModelLocal = dataModelLocal.GetData("AccountsList");
                        dataModelLocal = dataModelLocal.GetData("items");

//                        customerCB.model = dataModelLocal;

                        installationEditorContainer.accountsModel = dataModelLocal;

//                        itemsModel.updateModel("ProductsList");

                        console.log("customerCB.model", customerCB.model);
                    }
                    else if (dataModelLocal.ContainsKey("ProductsList")){

                        console.log("ProductsList");
                        dataModelLocal = dataModelLocal.GetData("ProductsList");

                        dataModelLocal = dataModelLocal.GetData("items");
                        console.log("items");

                        installationEditorContainer.productsModel = dataModelLocal;

//                        container.updateModel();
                    }
                }
            }
        }
    }//GqlModel itemsModel
}
