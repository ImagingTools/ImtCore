import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: container;

    property string itemId: objectView.itemId;
    property string itemName: objectView.itemName;

    property Item objectView;
    property TreeItemModel objectModel;

    property bool closingFlag: false;
    property bool showInputIdDialog: true;

    property string commandsId;

    property string gqlModelQueryType;
    property string gqlModelQueryTypeNotify;

    signal entered(string value);
    signal saved(string id, string name);
    signal closed();
    signal commandActivated(string commandId);

    onCommandsIdChanged: {
        Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandler);
    }

    onVisibleChanged: {
        console.log("DocumentCommands onVisibleChanged", itemId, container.visible);
        if (container.visible){
            Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandler);
        }
        else{
            Events.unSubscribeEvent(container.commandsId + "CommandActivated", container.commandHandler)
        }
    }

    function commandHandler(commandId){
        console.log("DocumentCommandsBase commandHandler", commandId);
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
            let itemId = container.objectView.itemId;

            if (itemId === ""){
                if (showInputIdDialog){
                    modalDialogManager.openDialog(inputDialog, {"message": qsTr("Please enter the name of the item: ")});
                }
                else{
                    container.gqlModelQueryType = "Add";
                    container.gqlModelQueryTypeNotify = "addedNotification";
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

    onObjectModelChanged: {
        objectModel.modelChanged.connect(modelChanged);
    }

    onItemIdChanged: {
//        multiDocView.updatePageId({"OldId": "", "NewId": itemId});
    }

    Component {
        id: saveDialog;
        MessageDialog {
            onFinished: {
                console.log("saveDialog onFinished", buttonId);
                if (buttonId == "Yes"){
                    closingFlag = true;
                    commandHandler("Save");
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
                    container.gqlModelQueryType = "Add";
                    container.gqlModelQueryTypeNotify = "addedNotification";
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

        objectView.itemId = itemId;
        objectView.itemName = itemName;

        commandsProvider.changeCommandMode("Save", "Disabled");
        multiDocView.setDocumentTitle({"ItemId": objectView.itemId, "Title": itemName});

        objectModel.modelChanged.connect(modelChanged);

        if (closingFlag){
            documentClosed();
        }
    }

    function documentClosed(){
        console.log("documentClosed", objectView.itemId);
        closed();

        multiDocView.closeDocument(objectView.itemId);
    }

    function modelChanged(){
        console.log("DocumentsCommands modelChanged");
        commandsProvider.changeCommandMode("Save", "Normal");

        let suffix = "*";
        multiDocView.setDocumentTitle({"ItemId": objectView.itemId, "Title": objectView.itemName + suffix});
    }

    GqlModel {
        id: saveQuery;

        function updateModel(){
            console.log( "updateModel saveModel");
            var query = Gql.GqlRequest("query", container.commandsId + container.gqlModelQueryType);
            var queryFields = Gql.GqlObject(container.gqlModelQueryTypeNotify);

            let itemId = objectView.itemId;
            let itemName = objectView.itemName;

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", itemId);

            var jsonString = objectModel.toJSON();
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
                        container.objectView.itemId = "";
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
}
