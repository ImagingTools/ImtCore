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

    property string subscriptionCommandId;

    documentModelComp: Component {
        id: treeItemModelComp;

        TreeItemModel {}
    }

    Component.onDestruction: {
        Events.sendEvent("UnRegisterSubscription", container.subscriptionClient);
    }

    onError: {
        ModalDialogManager.showWarningDialog(message)
    }

    onDocumentIdChanged: {
        if (documentId !== ""){
            container.subscriptionClient.register();
        }
    }

    function setupDocumentInfo(){
        if (documentModel && documentModel.containsKey("Name")){
            container.documentName = documentModel.getData("Name");
        }
        else{
            console.error("Unable to set document name. Error: Document model is not contains key 'Name'");
        }
    }

    function updateDocumentModel(){
        gqlGetModel.send();
    }

    function insertDocument(){
        gqlAddModel.send();
    }

    function saveDocument(){
        gqlUpdateModel.send();
    }

    function getAdditionalInputParams(){
        let obj = {}
        return obj;
    }

    function createGetInputParams(){
        return {};
    }

    function createUpdateInputParams(){
        return {};
    }

    function createAddInputParams(){
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

    property GqlRequestSender gqlUpdateModel: GqlRequestSender {
        requestType: 1; // Mutation
        gqlCommandId: container.gqlUpdateCommandId;

        function createQueryParams(query){
            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", container.documentId);
            inputParams.InsertField ("Item", container.documentModel.toJson());

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
        }

        function onError(message, type){
            container.error(message, type);
        }

        function onResult(data){
            let dataModelLocal = data.getData("updatedNotification");

            let documentId = dataModelLocal.getData("Id");
            let documentName = dataModelLocal.getData("Name");

            container.saved(documentId, documentName);
        }
    }

    property GqlRequestSender gqlGetModel: GqlRequestSender {
        requestType: 0; // Query
        gqlCommandId: container.gqlGetCommandId;

        function createQueryParams(query){
            var queryFields = Gql.GqlObject("item");
            queryFields.InsertField("Id");
            query.AddField(queryFields);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", container.documentId);

            let inputObj = container.createGetInputParams();
            for (let key in inputObj){
                inputParams.InsertField(key, inputObj[key]);
            }

            let additionInputParams = container.getAdditionalInputParams();
            if (Object.keys(additionInputParams).length > 0){
                let additionParams = Gql.GqlObject("addition");
                for (let key in additionInputParams){
                    additionParams.InsertField(key, additionInputParams[key]);
                }
                inputParams.InsertFieldObject(additionParams);
            }

            query.AddParam(inputParams);
        }

        function onError(message, type){
            container.error(message, type);
        }

        function onResult(data){
            container.documentModel = data;
        }
    }

    property GqlRequestSender gqlAddModel: GqlRequestSender {
        requestType: 1; // Mutation
        gqlCommandId: container.gqlAddCommandId;

        function createQueryParams(query){
            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", container.documentId);
            inputParams.InsertField ("Item", container.documentModel.toJson());

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
        }

        function onError(message, type){
            container.error(message, type);
        }

        function onResult(data){
            let dataModelLocal = data.getData("addedNotification");

            let documentId = ""
            let documentName = ""

            if (dataModelLocal){
                documentId = dataModelLocal.getData("Id");
                documentName = dataModelLocal.getData("Name");
            }

            container.saved(documentId, documentName);
        }
    }
}


