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

    property var payloadModel

    property string subscriptionCommandId;

    property bool ok: subscriptionCommandId != "" && documentId != "";

    Component.onDestruction: {
        Events.sendEvent("UnRegisterSubscription", container.subscriptionClient);
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

    function getAdditionalInputParams(){
        let obj = {}
        return obj;
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

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", container.documentId);
            inputParams.InsertField ("Item", container.documentModel);

            let additionInputParams = container.getAdditionalInputParams();
            if (Object.keys(additionInputParams).length > 0){
                let additionParams = Gql.GqlObject("addition");
                for (let key in additionInputParams){
                    additionParams.InsertField(key, additionInputParams[key]);
                }
                inputParams.InsertFieldObject(additionParams);
            }

            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("updatedNotification");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();
            this.setGqlQuery(gqlData);
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

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", container.documentId);

            let additionInputParams = container.getAdditionalInputParams();
            if (Object.keys(additionInputParams).length > 0){
                let additionParams = Gql.GqlObject("addition");
                for (let key in additionInputParams){
                    additionParams.InsertField(key, additionInputParams[key]);
                }
                inputParams.InsertFieldObject(additionParams);
            }

            query.AddParam(inputParams);

            var gqlData = query.GetQuery();

            this.setGqlQuery(gqlData);
        }

        onStateChanged: {
            if (state === "Error"){
                container.error("Network error", "Critical");
            }
            else if (state === "Ready"){
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

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", container.documentId);
            inputParams.InsertField ("Item", container.documentModel);

            let additionInputParams = container.getAdditionalInputParams();
            if (Object.keys(additionInputParams).length > 0){
                let additionParams = Gql.GqlObject("addition");
                for (let key in additionInputParams){
                    additionParams.InsertField(key, additionInputParams[key]);
                }
                inputParams.InsertFieldObject(additionParams);
            }

            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("addedNotification");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            var gqlData = query.GetQuery();

            this.setGqlQuery(gqlData);
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
}
