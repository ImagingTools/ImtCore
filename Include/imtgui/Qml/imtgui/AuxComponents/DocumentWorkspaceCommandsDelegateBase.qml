import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: container;

    property TreeItemModel objectModel: documentModel;

    property bool closingFlag: false;
    property bool showInputIdDialog: false;

    property string commandsId;

    property string gqlModelQueryType;
    property string gqlModelQueryTypeNotify;

    property alias updateItemTimer: timer.interval;

    signal entered(string value);
    signal saved(string id, string name);
    signal closed();
    signal commandActivated(string commandId);
    signal itemLoaded();

    Timer {
        id: timer;

        interval: 0;

        onTriggered: {
            let itemId = documentsData.GetData("ItemId", model.index);
            console.log("timer itemId", itemId);
            itemModel.updateModel(itemId)
        }
    }

    Component.onCompleted: {
        loadingPage.visible = true;
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(container.commandsId + "CommandActivated", container.commandHandle)
    }

    onCommandsIdChanged: {
        console.log("DocumentCommands onCommandsIdChanged", container.commandsId);
        Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);

        timer.start();
    }

    onVisibleChanged: {
        console.log("DocumentCommands onVisibleChanged", itemId, container.visible);
        if (container.visible){
            Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
        }
        else{
            Events.unSubscribeEvent(container.commandsId + "CommandActivated", container.commandHandle)
        }
    }

    onItemLoaded: {
        loadingPage.visible = false;
    }

    onObjectModelChanged: {
        objectModel.modelChanged.connect(modelChanged);
    }

    function removeChanges(){
        commandsProvider.changeCommandMode("Save", "Disabled");
        documentManager.setDocumentTitle({"ItemId": documentBase.itemId, "Title": documentBase.itemName});
    }

    function commandHandle(commandId){
        console.log("DocumentCommandsBase commandHandle", commandId);
        if (commandId == "Close"){
            let saveMode = commandsProvider.getCommandMode("Save");
            if (saveMode && saveMode == "Normal"){
                modalDialogManager.openDialog(saveDialog, {"message": qsTr("Save all changes ?")});
            }
            else{
                documentClosed()
            }
        }
        else if (commandId == "Save"){
            let itemId = documentBase.itemId;

            if (itemId === ""){
                container.gqlModelQueryType = "Add";
                container.gqlModelQueryTypeNotify = "addedNotification";

                if (showInputIdDialog){
                    modalDialogManager.openDialog(inputDialog, {"message": qsTr("Please enter the name of the item: ")});
                }
                else{
                    saveQuery.updateModel();
                }
            }
            else{
                container.gqlModelQueryType = "Update";
                container.gqlModelQueryTypeNotify = "updatedNotification";
                saveQuery.updateModel();
            }
        }

        commandActivated(commandId);
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
                    closingFlag = true;
                    commandHandle("Save");
                }
                else if (buttonId == "No"){
                    container.documentClosed();
                }
            }
        }
    }

    Component {
        id: inputDialog;
        InputDialog {
            onFinished: {
                console.log("InputDialog result", buttonId, inputValue);
                if (buttonId == "Ok"){
                    entered(inputValue);

                    saveQuery.updateModel();
                }
            }
        }
    }

    Component {
        id: errorDialog;
        MessageDialog {

            onFinished: {
                if (closingFlag){
                    closingFlag = false;
                }
            }
        }
    }

    Shortcut {
        sequence: "Ctrl+S";
        onActivated: {
            commandActivated("Save");
        }
    }

    /**
        Обновляем данные только после получения результата с сервера, удостоверясь
        что нет никаких ошибок
    */
    function documentSaved(itemId, itemName){
        console.log("DocumentsCommands documentSaved", itemId, itemName);

        Events.sendEvent(commandsId + "CollectionUpdateGui");

        objectModel.modelChanged.disconnect(modelChanged);

        saved(itemId, itemName);

        documentBase.itemId = itemId;
        documentBase.itemName = itemName;

        removeChanges();

        objectModel.modelChanged.connect(modelChanged);

        if (closingFlag){
            documentClosed();
        }
    }

    function documentClosed(){
        console.log("documentClosed", documentBase.itemId);
        closed();

        documentManager.closeDocument(documentBase.itemId);
    }

    function modelChanged(){
        console.log("DocumentsCommands modelChanged");
        commandsProvider.changeCommandMode("Save", "Normal");

        let suffix = "*";
        documentManager.setDocumentTitle({"ItemId": documentBase.itemId, "Title": documentBase.itemName + suffix});
    }

    GqlModel {
        id: saveQuery;

        function updateModel(){
            console.log( "updateModel saveModel");
            var query = Gql.GqlRequest("query", container.commandsId + container.gqlModelQueryType);
            var queryFields = Gql.GqlObject(container.gqlModelQueryTypeNotify);

            let itemId = documentBase.itemId;
            let itemName = documentBase.itemName;

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", itemId);

            var jsonString = documentModel.toJSON();
            jsonString = jsonString.replace(/\"/g,"\\\\\\\"")

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
                    if (gqlModelQueryType == "Add"){
                        documentBase.itemId = "";
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

                    if (dataModelLocal.ContainsKey(gqlModelQueryTypeNotify)){
                        dataModelLocal = dataModelLocal.GetData(gqlModelQueryTypeNotify);

                        let itemId = dataModelLocal.GetData("Id");
                        let itemName = dataModelLocal.GetData("Name");

                        documentSaved(itemId, itemName);
                    }
                }
            }
        }
    }

    GqlModel {
        id: itemModel;

        function updateModel(itemId) {
            var query = Gql.GqlRequest("query", commandsId + "Item");

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", itemId);
            query.AddParam(inputParams);
            console.log("itemId", itemId);
            var queryFields = Gql.GqlObject("item");

            queryFields.InsertField("Id");
            queryFields.InsertField("Name");

            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            console.log(commandsId + " Item query ", gqlData);
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
                if(dataModelLocal.ContainsKey(commandsId + "Item")){
                    dataModelLocal = dataModelLocal.GetData(commandsId + "Item");
                    documentModel = dataModelLocal;
                    itemLoaded();
                }
            }
        }
    }//GqlModel itemModel
}
