import QtQuick 2.12
import Acf 1.0
import imtguigql 1.0
import imtcontrols 1.0
import imtdocgui 1.0

DocumentDataController {
    id: container;

    property string gqlGetCommandId;
    property string gqlAddCommandId;
    property string gqlUpdateCommandId;
    property string gqlImportCommandId;

    property var payloadModel

    property string subscriptionCommandId;

    property bool ok: subscriptionCommandId != "" && documentId != "";

    property var getRequestInputParam: Gql.GqlObject("input");
    property var addRequestInputParam: Gql.GqlObject("input");
    property var updateRequestInputParam: Gql.GqlObject("input");

    Component.onCompleted: {
        let additionInputParams = container.getHeaders();
        if (additionInputParams && Object.keys(additionInputParams).length > 0){
            let additionParams = Gql.GqlObject("addition");
            for (let key in additionInputParams){
                additionParams.InsertField(key, additionInputParams[key]);
            }
            getRequestInputParam.InsertFieldObject(additionParams);
            addRequestInputParam.InsertFieldObject(additionParams);
            updateRequestInputParam.InsertFieldObject(additionParams);
        }
    }

    Component.onDestruction: {
        Events.sendEvent("UnRegisterSubscription", container.subscriptionClient);
    }

    onDocumentModelChanged: {
        updateRequestInputParam.InsertField ("Item", container.documentModel);
        addRequestInputParam.InsertField ("Item", container.documentModel);
    }

    onDocumentIdChanged: {
        getRequestInputParam.InsertField("Id", container.documentId);
        addRequestInputParam.InsertField("Id", container.documentId);
        updateRequestInputParam.InsertField("Id", container.documentId);
    }

    onError: {
        ModalDialogManager.showWarningDialog(message)
    }

    onOkChanged: {
        if (documentId !== ""){
            container.subscriptionClient.register();
        }
    }

    function setupDocumentInfo(){
        if (documentModel && documentModel.m_name){
            container.documentName = documentModel.m_name;
        }
        else{
            console.warn("Unable to set document name. Error: field 'm_name' is not found.");
        }
    }

    function updateDocumentModel(){
        gqlGetModel.getData();
    }

    function insertDocument(){
        if (documentModel && documentModel.m_id !== undefined && documentModel.m_id !== null){
            documentModel.m_id = documentId;
        }

        gqlAddModel.save();
    }

    function saveDocument(){
        if (documentModel && documentModel.m_id !== undefined && documentModel.m_id !== null){
            documentModel.m_id = documentId;
        }

        gqlUpdateModel.save();
    }

    function createDocumentModel(){
        if (container.documentModelComp){
            let objectData = container.documentModelComp.createObject(null);
            objectData.connectProperties();
            documentModel = objectData;
        }
        else{
            console.error("Unable to create document model for DocumentDataController. Error: 'documentModelComp' is invalid")
        }
    }

    // TODO: ???
    function getHeaders(){
        return {};
    }

    property SubscriptionClient subscriptionClient: SubscriptionClient {
        function register(){
            if (container.subscriptionCommandId === ""){
                console.warn("Unable to register subscription for document because command-ID is empty");
                return;
            }

            let subscriptionRequestId = container.subscriptionCommandId;
            var query = Gql.GqlRequest("subscription", subscriptionRequestId);
            var queryFields = Gql.GqlObject("notification");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            Events.sendEvent("RegisterSubscription", {"Query": query, "Client": container.subscriptionClient});
        }

        onStateChanged: {
            if (state === "Ready"){
                container.hasRemoteChanges = true;
            }
        }
    }

    property GqlRequest gqlUpdateModel: GqlRequest {
        function save(){
            var query = Gql.GqlRequest("mutation", container.gqlUpdateCommandId);
            query.AddParam(container.updateRequestInputParam);

            var queryFields = Gql.GqlObject("updatedNotification");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            let headers = container.getHeaders()

            this.setGqlQuery(gqlData, headers);
        }

        onStateChanged: {
            let state = container.gqlUpdateModel.state;
            if (state === "Error"){
                container.error("Network error", "Critical");
            }
            if (state === "Ready"){
                let responseObj = JSON.parse(this.json)
                if (!responseObj){
                    console.error("Unable convert json ", json, " to object")
                    return;
                }

                if ("errors" in responseObj){
                    let errorsObject = responseObj["errors"];
                    if (container.gqlUpdateCommandId in errorsObject){
                        errorsObject = errorsObject[container.gqlUpdateCommandId]
                    }

                    let message = ""
                    if ("message" in errorsObject){
                        message = errorsObject["message"];
                    }

                    let type;
                    if ("type" in errorsObject){
                        type = errorsObject["type"];
                    }

                    container.error(message, type);

                    return;
                }

                if ("data" in responseObj){
                    let dataObject = responseObj["data"];

                    if (container.gqlUpdateCommandId in dataObject){
                        dataObject = dataObject[container.gqlUpdateCommandId];

                        if ("updatedNotification" in dataObject){
                            dataObject = dataObject["updatedNotification"];

                            let documentId = "";
                            let documentName = "";

                            if ("Id" in dataObject){
                                documentId = dataObject["Id"]
                            }

                            if ("Name" in dataObject){
                                documentName = dataObject["Name"]
                            }

                            container.saved(documentId, documentName);
                        }
                    }
                }
            }
        }
    }

    property GqlRequest gqlGetModel: GqlRequest {
        function getData(){
            var query = Gql.GqlRequest("query", container.gqlGetCommandId);

            var queryFields = Gql.GqlObject("item");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            query.AddParam(container.getRequestInputParam);

            var gqlData = query.GetQuery();
            let headers = container.getHeaders()

            this.setGqlQuery(gqlData, headers);
        }

        onStateChanged: {
            if (state === "Error"){
                container.error("Network error", "Critical");
            }
            else if (state === "Ready"){
                console.log("Json", this.json);
                let responseObj = JSON.parse(this.json)
                if (!responseObj){
                    console.error("Unable convert json ", json, " to object")
                    return;
                }

                if ("errors" in responseObj){
                    let errorsObject = responseObj["errors"];
                    if (container.gqlGetCommandId in errorsObject){
                        errorsObject = errorsObject[container.gqlGetCommandId]
                    }

                    let message = ""
                    if ("message" in errorsObject){
                        message = errorsObject["message"];
                    }

                    let type;
                    if ("type" in errorsObject){
                        type = errorsObject["type"];
                    }

                    container.error(message, type);

                    return;
                }

                if ("data" in responseObj){
                    let dataObject = responseObj["data"];
                    let itemObject = dataObject[container.gqlGetCommandId];
                    console.log("*DEBUG* itemObject", itemObject)

                    container.payloadModel.fromObject(itemObject);
                }
            }
        }
    }//GqlRequest itemModel

    property GqlRequest gqlAddModel: GqlRequest {
        function save(){
            var query = Gql.GqlRequest("mutation", container.gqlAddCommandId);
            query.AddParam(container.addRequestInputParam);

            var queryFields = Gql.GqlObject("addedNotification");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            let headers = container.getHeaders()

            this.setGqlQuery(gqlData, headers);
        }

        onStateChanged: {
            if (state === "Error"){
                container.error("Network error", "Critical");
            }
            if (state === "Ready"){
                console.log("Add", this.json)
                let responseObj = JSON.parse(this.json)
                if (!responseObj){
                    console.error("Unable convert json '", json, "' to object")
                    return;
                }

                if ("errors" in responseObj){
                    let errorsObject = responseObj["errors"];
                    if (container.gqlAddCommandId in errorsObject){
                        errorsObject = errorsObject[container.gqlAddCommandId]
                    }

                    let message = ""
                    if ("message" in errorsObject){
                        message = errorsObject["message"];
                    }

                    let type;
                    if ("type" in errorsObject){
                        type = errorsObject["type"];
                    }

                    container.error(message, type);

                    return;
                }

                if ("data" in responseObj){
                    let dataObject = responseObj["data"];

                    if (container.gqlAddCommandId in dataObject){
                        dataObject = dataObject[container.gqlAddCommandId]
                    }

                    if ("addedNotification" in dataObject){
                        dataObject = dataObject["addedNotification"]
                    }

                    let documentId = ""
                    let documentName = ""

                    if ("Id" in dataObject){
                        documentId = dataObject["Id"];
                    }

                    if ("Name" in dataObject){
                        documentName = dataObject["Name"];
                    }

                    container.saved(documentId, documentName);
                }
            }
        }
    }


    FileDialog {
        id: fileDialog

        title: container.importTitle

        fileMode: FileDialog.OpenFile

        property string measurementFilter: "JSON files (*.json)"

        nameFilters: [measurementFilter]

        onAccepted: {
            let filePath;
            if (Qt.platform.os == "web"){
                filePath = fileDialog.file.toString()
            }
            else{
                filePath = fileDialog.selectedFile.toString()
            }

            filePath = filePath.replace('file:///', '')

            if (Qt.platform.os == "web"){
                let reader = new FileReader()

                reader.readAsText(filePath)

                reader.onload = function(){
                    container.newCharacterisationModel.clear()
                    let data = reader.result //String.fromCharCode.apply(null, new Uint8Array(reader.result))
                    console.log("project", data)
                    container.newCharacterisationModel.createFromJson(data)
                    gqlImportModel.import()
                }.bind(this)
            }
            else {
                fileIO.source = filePath
                let data = fileIO.read()
                let jsonResult = JSON.parse(data)
                container.newCharacterisationModel.clear()
                container.newCharacterisationModel.createFromJson(data)
                gqlImportModel.import()
            }

        }

        FileIO {
            id: fileIO
        }
    }

    GqlModel {
        id: gqlImportModel

        function import(){
            let query = Gql.GqlRequest("mutation", container.gqlImportCommandId)

            let inputParams = Gql.GqlObject("input")
            inputParams.InsertField ("FileContent", container.newCharacterisationModel.toJson())

            let additionInputParams = container.getAdditionalInputParams()
            if (Object.keys(additionInputParams).length > 0){
                let additionParams = Gql.GqlObject("addition")
                for (let key in additionInputParams){
                    additionParams.InsertField(key, additionInputParams[key])
                }
                inputParams.InsertFieldObject(additionParams)
            }

            query.AddParam(inputParams)

            let queryFields = Gql.GqlObject("addedNotification")
            queryFields.InsertField("Id")
            query.AddField(queryFields)

            let gqlData = query.GetQuery()
            this.setGqlQuery(gqlData)
        }

        onStateChanged: {
            console.log("onResult", gqlAddModel.state)
            let state = gqlAddModel.state
            if (state === "Error"){
                console.log("Network error")
                ModalDialogManager.showWarningDialog("Network error")
            }
            if (state === "Ready"){
                let dataModelLocal
                if (gqlAddModel.containsKey("errors")){
                    dataModelLocal = gqlAddModel.getData("errors")

                    if (dataModelLocal.containsKey(CharacterizationsSdlCommandIds.s_characterizationImport)){
                        dataModelLocal = dataModelLocal.getData(CharacterizationsSdlCommandIds.s_characterizationImport)
                    }

                    let message = ""
                    if (dataModelLocal.containsKey("message")){
                        message = dataModelLocal.getData("message")
                    }

                    let type
                    if (dataModelLocal.containsKey("type")){
                        type = dataModelLocal.getData("type")
                    }
                    console.log(message)
                    ModalDialogManager.showWarningDialog(message)
                }
                else if (gqlAddModel.containsKey("data")){
                    dataModelLocal = gqlAddModel.getData("data")
                    console.log("onResult data", dataModelLocal.toJson())

                    let documentId = ""

                    if (dataModelLocal.containsKey(CharacterizationsSdlCommandIds.s_characterizationImport)){
                        dataModelLocal = dataModelLocal.getData(CharacterizationsSdlCommandIds.s_characterizationImport)
                        dataModelLocal = dataModelLocal.getData(AddedNotificationPayloadTypeMetaInfo.s_addedNotification)

                        if (dataModelLocal){
                            documentId = dataModelLocal.getData(CharacterizationTypeMetaInfo.s_id)
                        }
                    }
                    console.log("GMG Measurement Project upload successful")

                    let message
                    console.log("Message")
                    message = qsTr("GMG Measurement Project import successful")

                    ModalDialogManager.showInfoDialog(message)
                }
            }
        }

    }

}
