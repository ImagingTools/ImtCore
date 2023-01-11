import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: container;

    property TreeItemModel objectModel: documentBase ? container.documentBase.documentModel : null;

    property Item documentBase;

    property bool closingFlag: false;
    property bool showInputIdDialog: false;

    property string commandsId;

    property string gqlModelQueryType;
    property string gqlModelQueryTypeNotify;

    /**
        Таймер чтобы запрос на Item не уходил одновременно с другими, иначе запрос не пройдет
    */
    property alias updateItemTimer: timer.interval;

    property bool autoUpdate: true;

    signal entered(string value);
    signal saved(string id, string name);
    signal closed();
    signal commandActivated(string commandId);
    signal itemLoaded();

    property var itemModelInputParams: ({});

    Timer {
        id: timer;

        interval: 0;

        onTriggered: {
            itemModel.updateModel(container.itemModelInputParams)
        }
    }

//    onDocumentBaseChanged: {
//        objectModel = documentBase.documentModel
//    }

    Component.onCompleted: {
        console.log("DocumentCommands onCompleted");
        let itemId = documentsData.GetData("Id", model.index);
        itemModelInputParams["Id"] = itemId;
        console.log("itemId", itemId);
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(container.commandsId + "CommandActivated", container.commandHandle)
    }

    onCommandsIdChanged: {
        console.log("DocumentCommands onCommandsIdChanged", container.commandsId);

        if (documentBase && documentBase.itemLoad){
            Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);

            if (autoUpdate){
                timer.start();
            }
        }
    }

    onVisibleChanged: {
        console.log("DocumentCommands onVisibleChanged");
        if (visible){
            Events.subscribeEvent(commandsId + "CommandActivated", commandHandle);
        }
        else{
            Events.unSubscribeEvent(commandsId + "CommandActivated", commandHandle)
        }
    }

    onObjectModelChanged: {
        if(objectModel){
            objectModel.modelChanged.connect(modelChanged);
        }
    }

    function disconnectModelChanged(){
        if(objectModel){
            objectModel.modelChanged.disconnect(modelChanged);
        }
    }

    function connectModelChanged(){
        if(objectModel){
            objectModel.modelChanged.connect(modelChanged);
        }
    }

    function removeChanges(){
        commandsProvider.changeCommandMode("Save", "Disabled");
        documentManager.setDocumentTitle({"Id": documentBase.itemId, "Title": documentBase.itemName});
    }

    function commandHandle(commandId){
        console.log("DocumentCommandsBase commandHandle", documentBase.itemId, commandId);

        if (commandId == "Close"){
            let saveCommandIsEnabled = commandsProvider.commandIsEnabled("Save");
            if (saveCommandIsEnabled){
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
                saveObject();
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
            title: qsTr("Error");
            onFinished: {
                if (closingFlag){
                    closingFlag = false;
                }
            }
        }
    }

//    Shortcut {
//        sequence: "Ctrl+S";
//        onActivated: {
//            commandActivated("Save");
//        }
//    }

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
        documentManager.setDocumentTitle({"Id": documentBase.itemId, "Title": documentBase.itemName + suffix});
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

                    if (dataModelLocal.ContainsKey(gqlModelQueryTypeNotify)){
                        dataModelLocal = dataModelLocal.GetData(gqlModelQueryTypeNotify);

                        let itemId = dataModelLocal.GetData("Id");
                        let itemName = dataModelLocal.GetData("Name");

                        if (container.documentBase.itemLoad){
                            documentSaved(itemId, itemName);
                        }
                    }
                }
            }
        }
    }

    function updateModel(){
        itemModel.updateModel(itemModelInputParams);
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
