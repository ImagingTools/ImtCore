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

    Component.onDestruction: {
        Events.sendEvent("UnRegisterSubscription", container.subscriptionClient);
    }

    onError: {
        ModalDialogManager.showWarningDialog(message)
    }

    onDocumentModelChanged: {
        if (documentModel.containsKey("Name")){
            documentName = documentModel.getData("Name");
        }

        hasRemoteChanges = false;

        modelChanged();
    }

    onSaved: {
        documentName = name;
        documentId = id;
    }

    onDocumentIdChanged: {
        if (documentId !== ""){
            container.subscriptionClient.register();
        }
    }

    function getDocumentId(){
        let id = "";
        if (documentModel && documentModel.containsKey("Id")){
            id = documentModel.getData("Id");
        }

        return id;
    }

    function getDocumentName(){
        let name = "";
        if (documentModel && documentModel.containsKey("Name")){
            name = documentModel.getData("Name");
        }

        return name;
    }

    function updateDocumentModel(){
        gqlGetModel.getModel();
    }

    function insertDocument(){
        gqlAddModel.save();
    }

    function saveDocument(){
        gqlUpdateModel.save();
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

    property GqlModel gqlUpdateModel: GqlModel {
        function save(){
            var query = Gql.GqlRequest("mutation", container.gqlUpdateCommandId);

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

            var gqlData = query.GetQuery();
            this.setGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("onResult", container.gqlUpdateModel.state);
            let state = container.gqlUpdateModel.state;
            if (state === "Error"){
                container.error("Network error", "Critical");
            }
            if (state === "Ready"){
                var dataModelLocal;
                if (container.gqlUpdateModel.containsKey("errors")){
                    dataModelLocal = container.gqlUpdateModel.getData("errors");

                    if (dataModelLocal.containsKey(container.gqlUpdateCommandId)){
                        dataModelLocal = dataModelLocal.getData(container.gqlUpdateCommandId);
                    }

                    let message = ""
                    if (dataModelLocal.containsKey("message")){
                        message = dataModelLocal.getData("message");
                    }

                    let type;
                    if (dataModelLocal.containsKey("type")){
                        type = dataModelLocal.getData("type");
                    }

                    container.error(message, type);
                }

                else if (container.gqlUpdateModel.containsKey("data")){
                    dataModelLocal = container.gqlUpdateModel.getData("data");

                    if (dataModelLocal.containsKey(container.gqlUpdateCommandId)){
                        dataModelLocal = dataModelLocal.getData(container.gqlUpdateCommandId);
                        dataModelLocal = dataModelLocal.getData("updatedNotification");

                        let documentId = dataModelLocal.getData("Id");
                        let documentName = dataModelLocal.getData("Name");

                        container.saved(documentId, documentName);
                    }
                }
            }
        }
    }

    property GqlModel gqlGetModel: GqlModel {
        function getModel(){
            console.log("getData", container.gqlGetCommandId)
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
            let state = container.gqlGetModel.state;
            if (state === "Error"){
                container.error("Network error", "Critical");
            }
            else if (state === "Ready"){
                if (container.gqlGetModel.containsKey("errors")){
                    let dataModelLocal = container.gqlGetModel.getData("errors");

                    if (dataModelLocal.containsKey(container.gqlGetCommandId)){
                        dataModelLocal = dataModelLocal.getData(container.gqlGetCommandId);
                    }

                    let message = ""
                    if (dataModelLocal.containsKey("message")){
                        message = dataModelLocal.getData("message");
                    }

                    let type;
                    if (dataModelLocal.containsKey("type")){
                        type = dataModelLocal.getData("type");
                    }

                    container.error(message, type);
                }

                else if (container.gqlGetModel.containsKey("data")){
                    let dataModelLocal = container.gqlGetModel.getData("data")
                    if (dataModelLocal.containsKey(container.gqlGetCommandId)){
                        container.documentModel = dataModelLocal.getData(container.gqlGetCommandId)
                    }
                }
            }
        }
    }//GqlModel itemModel

    property GqlModel gqlAddModel: GqlModel {
        function save(){
            var query = Gql.GqlRequest("mutation", container.gqlAddCommandId);

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

            var gqlData = query.GetQuery();

            this.setGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("onResult", state);

            if (state === "Error"){
                container.error("Network error", "Critical");
            }
            if (state === "Ready"){
                var dataModelLocal;
                if (container.gqlAddModel.containsKey("errors")){
                    dataModelLocal = container.gqlAddModel.getData("errors");

                    if (dataModelLocal.containsKey(container.gqlAddCommandId)){
                        dataModelLocal = dataModelLocal.getData(container.gqlAddCommandId);
                    }

                    let message = ""
                    if (dataModelLocal.containsKey("message")){
                        message = dataModelLocal.getData("message");
                    }

                    let type;
                    if (dataModelLocal.containsKey("type")){
                        type = dataModelLocal.getData("type");
                    }

                    container.error(message, type);
                }

                else if (container.gqlAddModel.containsKey("data")){
                    dataModelLocal = container.gqlAddModel.getData("data");

                    if (dataModelLocal.containsKey(container.gqlAddCommandId)){
                        dataModelLocal = dataModelLocal.getData(container.gqlAddCommandId);
                        dataModelLocal = dataModelLocal.getData("addedNotification");

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
        }
    }
}
