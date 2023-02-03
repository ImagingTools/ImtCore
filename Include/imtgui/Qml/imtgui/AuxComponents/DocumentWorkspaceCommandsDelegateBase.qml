import QtQuick 2.12
import Acf 1.0
import imtqml 1.0
import imtgui 1.0

Item {
    id: container;

    property TreeItemModel objectModel: documentBase ? container.documentBase.documentModel : null;
    property TreeItemModel documentsData: null;

    property Item documentBase: null;

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
            itemModel.updateModel(container.itemModelInputParams);
        }
    }

//    onDocumentBaseChanged: {
//        console.log("DocumentCommands onDocumentBaseChanged", container.documentBase.documentsData);

//        let itemId = container.documentBase.documentsData.GetData("Id", model.index);
//        container.itemModelInputParams["Id"] = itemId;
//        console.log("itemId", itemId);
//    }

//    Component.onCompleted: {
//        console.log("DocumentCommands onCompleted");
//        let itemId = documentsData.GetData("Id", model.index);
//        container.itemModelInputParams["Id"] = itemId;
//        console.log("itemId", itemId);
//    }

    onDocumentsDataChanged: {
        if (container.documentsData != null){
            console.log("onDocumentsDataChanged", documentsData.toJSON());
            let itemId = documentsData.GetData("Id", model.index);
            container.itemModelInputParams["Id"] = itemId;
            console.log("itemId", itemId);
        }
    }

    Component.onDestruction: {
        Events.unSubscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
    }

    onCommandsIdChanged: {
        console.log("DocumentCommands onCommandsIdChanged", container.commandsId);

        if (container.documentBase && container.documentBase.itemLoad){
            Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);

            if (container.autoUpdate){
                timer.start();
            }
        }
    }

    onVisibleChanged: {
        console.log("DocumentCommands onVisibleChanged");
        if (container.visible){
            Events.subscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
        }
        else{
            Events.unSubscribeEvent(container.commandsId + "CommandActivated", container.commandHandle);
        }
    }

    onObjectModelChanged: {
        if(container.objectModel){
            container.objectModel.modelChanged.connect(container.modelChanged);
        }
    }

    function removeChanges(){
        container.documentBase.commandsProvider.setCommandIsEnabled("Save", false);
        container.documentBase.documentManager.setDocumentTitle({"Id": container.documentBase.itemId, "Title": container.documentBase.itemName});
    }

    function commandHandle(commandId){
        console.log("DocumentCommandsBase commandHandle", container.documentBase.itemId, commandId);

        if (commandId == "Close"){
            let saveCommandIsEnabled = container.documentBase.commandsProvider.commandIsEnabled("Save");
            if (saveCommandIsEnabled){
                modalDialogManager.openDialog(saveDialog, {"message": qsTr("Save all changes ?")});
            }
            else{
                container.documentClosed();
            }
        }
        else if (commandId == "Save"){
            documentBase.updateModel();
            let itemId = documentBase.itemId;
            if (itemId === ""){
                container.gqlModelQueryType = "Add";
                container.gqlModelQueryTypeNotify = "addedNotification";

                if (container.showInputIdDialog){
                    modalDialogManager.openDialog(inputDialog, {"message": qsTr("Please enter the name of the item: ")});
                }
                else{
                    saveQuery.updateModel();
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
                    container.closingFlag = true;
                    container.commandHandle("Save");
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
                    container.entered(inputValue);

                    saveQuery.updateModel();
                }
            }
        }
    }

    Component {
        id: errorDialog;

        ErrorDialog {
            onFinished: {
                if (container.closingFlag){
                    container.closingFlag = false;
                }
            }
        }
    }

    /**
        Обновляем данные только после получения результата с сервера, удостоверясь
        что нет никаких ошибок
    */
    function documentSaved(itemId, itemName){
        console.log("DocumentsCommands documentSaved", itemId, itemName);

        Events.sendEvent(container.commandsId + "CollectionUpdateGui");

        objectModel.modelChanged.disconnect(container.modelChanged);

        container.saved(itemId, itemName);

        documentBase.itemId = itemId;
        documentBase.itemName = itemName;

        container.removeChanges();

        objectModel.modelChanged.connect(container.modelChanged);

        if (container.closingFlag){
            container.documentClosed();
        }
    }

    function documentClosed(){
        console.log("documentClosed", documentBase.itemId);
        container.closed();

        container.documentBase.documentManager.closeDocument(documentBase.itemId);
    }

    function modelChanged(){
        console.log("DocumentsCommands modelChanged");
        container.documentBase.commandsProvider.setCommandIsEnabled("Save", "Normal");

        let suffix = "*";
        container.documentBase.documentManager.setDocumentTitle({"Id": documentBase.itemId, "Title": documentBase.itemName + suffix});
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

                        if (container.documentBase.itemLoad){
                            container.documentSaved(itemId, itemName);
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
