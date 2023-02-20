import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: container;

    property Item documentBase: null;

    property bool showInputIdDialog: false;

    property string commandsId;

    property string gqlModelQueryType;
    property string gqlModelQueryTypeNotify;

    property int updateItemTimer;

    signal entered(string value);
    signal saved(string id, string name);
    signal closed();
    signal commandActivated(string commandId);
    signal itemLoaded();

    property var itemModelInputParams: ({});

    onVisibleChanged: {
        if (container.visible){
            Events.sendEvent("CommandsModelChanged", {"Model": documentBase.commandsProvider.commandsModel,
                                 "CommandsId": documentBase.commandsProvider.commandsId});

            Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
        }
        else{
            Events.unSubscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
        }
    }

    function commandHandle(commandId){
        console.log("DocumentCommandsBase commandHandle", container.documentBase.itemId, commandId);

        if (commandId === "Close"){
            if (container.documentBase.isDirty){
                modalDialogManager.openDialog(saveDialog, {"message": qsTr("Save all changes ?")});
            }
            else{
                Events.sendEvent("CloseDocument", {"Id": container.documentBase.itemId});
            }
        }
        else if (commandId === "Save"){
            container.documentBase.updateModel();
            let itemId = container.documentBase.itemId;
            if (itemId === ""){
                if (container.showInputIdDialog){
                    modalDialogManager.openDialog(inputDialog, {"message": qsTr("Please enter the name of the document:")});
                }
                else{
                    container.addObject();
                }
            }
            else{
                container.saveObject();
            }
        }

        container.commandActivated(commandId);
    }

    Component {
        id: saveDialog;
        MessageDialog {
            Component.onCompleted: {
                console.log("saveDialog onCompleted");
                buttons.addButton({"Id":"Cancel", "Name":"Cancel", "Enabled": true});
            }

            onFinished: {
                console.log("saveDialog onFinished", buttonId);
                if (buttonId == "Yes"){
                    container.documentBase.closingFlag = true;
                    container.documentBase.save();
                }
                else if (buttonId == "No"){
                    Events.sendEvent("CloseDocument", {"Id": container.documentBase.itemId});
                }
            }
        }
    }

    Component {
        id: inputDialog;
        InputDialog {
            title: qsTr("Entering a name");
            onFinished: {
                console.log("InputDialog result", buttonId, inputValue);
                if (buttonId == "Ok"){
                    container.entered(inputValue);

                    container.addObject();
                }
            }
        }
    }

    Component {
        id: errorDialog;

        ErrorDialog {
            onFinished: {
                if (container.documentBase.closingFlag){
                    container.documentBase.closingFlag = false;
                }
            }
        }
    }

    function saveObject(){
        container.gqlModelQueryType = "Update";
        container.gqlModelQueryTypeNotify = "updatedNotification";
        saveQuery.updateModel();
    }

    function addObject(){
        container.gqlModelQueryType = "Add";
        container.gqlModelQueryTypeNotify = "addedNotification";
        saveQuery.updateModel();
    }

    GqlModel {
        id: saveQuery;

        function updateModel(){
            console.log( "updateModel saveModel");
            var query = Gql.GqlRequest("query", container.commandsId + container.gqlModelQueryType);
            var queryFields = Gql.GqlObject(container.gqlModelQueryTypeNotify);

            let itemId = container.documentBase.itemId;
            let itemName = container.documentBase.itemName;

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", itemId);

            var jsonString = container.documentBase.documentModel.toJSON();
//            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")

            inputParams.InsertField ("Item", jsonString);

            query.AddParam(inputParams);

            queryFields.InsertField("Id");
            queryFields.InsertField("Successed");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log("ObjectView query ", gqlData);
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("State:", this.state, saveQuery);
            if (this.state === "Ready"){
                var dataModelLocal;
                if (saveQuery.ContainsKey("errors")){
                    dataModelLocal = saveQuery.GetData("errors");
                    dataModelLocal = dataModelLocal.GetData(container.commandsId + container.gqlModelQueryType);
                    if (container.gqlModelQueryType == "Add"){
                        container.documentBase.itemId = "";
                    }

                    if (dataModelLocal.ContainsKey("message")){
                        let message = dataModelLocal.GetData("message");
                        modalDialogManager.openDialog(errorDialog, {"message": message});
                    }

                    return;
                }

                if (saveQuery.ContainsKey("data")){
                    dataModelLocal = saveQuery.GetData("data");

                    dataModelLocal = dataModelLocal.GetData(container.commandsId + container.gqlModelQueryType);

                    if (dataModelLocal.ContainsKey(container.gqlModelQueryTypeNotify)){
                        dataModelLocal = dataModelLocal.GetData(container.gqlModelQueryTypeNotify);

                        let itemId = dataModelLocal.GetData("Id");
                        let itemName = dataModelLocal.GetData("Name");

                        container.documentBase.isDirty = false;
                        Events.sendEvent("DocumentSaved", {"Id": itemId, "Name": itemName});

                        if (container.documentBase.closingFlag){
                            Events.sendEvent("CloseDocument", {"Id": container.documentBase.itemId});
                        }
                    }
                }
            }
        }
    }

    function updateModel(){
        itemModel.updateModel(container.itemModelInputParams);
    }

    GqlModel {
        id: itemModel;

        function updateModel(externInputParams) {
            console.log("itemModel updateModel", externInputParams);
            var query = Gql.GqlRequest("query", container.commandsId + "Item");

            var queryFields = Gql.GqlObject("item");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            var inputParams = Gql.GqlObject("input");
            let keys = Object.keys(externInputParams)
            for (let key of keys){
                inputParams.InsertField(key, externInputParams[key]);
            }
            query.AddParam(inputParams);

            var gqlData = query.GetQuery();
            console.log(container.commandsId + " Item query ", gqlData);
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
                if(dataModelLocal && dataModelLocal.ContainsKey(container.commandsId + "Item")){
                    dataModelLocal = dataModelLocal.GetData(container.commandsId + "Item");

                    container.documentBase.documentModel = dataModelLocal;

                    container.itemLoaded();
                }
            }
        }
    }//GqlModel itemModel
}
