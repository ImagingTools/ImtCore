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
        Events.sendEvent("SendError", {"Message": message, "ErrorType": type})
    }

    onDocumentModelChanged: {
        if (documentModel.ContainsKey("Name")){
            documentName = documentModel.GetData("Name");
        }

        hasRemoteChanges = false;
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

    function getDocumentName(){
        return documentName;
    }

    function updateDocumentModel(){
        gqlGetModel.getData();
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
            inputParams.InsertField ("Item", container.documentModel.ToJson());

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
            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            console.log("onResult", container.gqlUpdateModel.state);
            let state = container.gqlUpdateModel.state;
            if (state === "Error"){
                container.error("Network error", "Critical");
            }
            if (state === "Ready"){
                var dataModelLocal;
                if (container.gqlUpdateModel.ContainsKey("errors")){
                    dataModelLocal = container.gqlUpdateModel.GetData("errors");

                    if (dataModelLocal.ContainsKey(container.gqlUpdateCommandId)){
                        dataModelLocal = dataModelLocal.GetData(container.gqlUpdateCommandId);
                    }

                    let message = ""
                    if (dataModelLocal.ContainsKey("message")){
                        message = dataModelLocal.GetData("message");
                    }

                    let type;
                    if (dataModelLocal.ContainsKey("type")){
                        type = dataModelLocal.GetData("type");
                    }

                    container.error(message, type);
                }

                else if (container.gqlUpdateModel.ContainsKey("data")){
                    dataModelLocal = container.gqlUpdateModel.GetData("data");

                    if (dataModelLocal.ContainsKey(container.gqlUpdateCommandId)){
                        dataModelLocal = dataModelLocal.GetData(container.gqlUpdateCommandId);
                        dataModelLocal = dataModelLocal.GetData("updatedNotification");

                        let documentId = dataModelLocal.GetData("Id");
                        let documentName = dataModelLocal.GetData("Name");

                        container.saved(documentId, documentName);
                    }
                }
            }
        }
    }

    property GqlModel gqlGetModel: GqlModel {
        function getData(){
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

            this.SetGqlQuery(gqlData);
        }

        onStateChanged: {
            let state = container.gqlGetModel.state;
            if (state === "Error"){
                container.error("Network error", "Critical");
            }
            else if (state === "Ready"){
                if (container.gqlGetModel.ContainsKey("errors")){
                    let dataModelLocal = container.gqlGetModel.GetData("errors");

                    if (dataModelLocal.ContainsKey(container.gqlGetCommandId)){
                        dataModelLocal = dataModelLocal.GetData(container.gqlGetCommandId);
                    }

                    let message = ""
                    if (dataModelLocal.ContainsKey("message")){
                        message = dataModelLocal.GetData("message");
                    }

                    let type;
                    if (dataModelLocal.ContainsKey("type")){
                        type = dataModelLocal.GetData("type");
                    }

                    container.error(message, type);
                }

                else if (container.gqlGetModel.ContainsKey("data")){
                    let dataModelLocal = container.gqlGetModel.GetData("data")
                    if (dataModelLocal.ContainsKey(container.gqlGetCommandId)){
                        container.documentModel = dataModelLocal.GetData(container.gqlGetCommandId)
                    }
                }
            }
        }
    }//GqlModel itemModel

    property GqlModel gqlAddModel: GqlModel {
        onStateChanged: {
            console.log("onResult", container.gqlAddModel.state);
            let state = container.gqlAddModel.state;
            if (state === "Error"){
                container.error("Network error", "Critical");
            }
            if (state === "Ready"){
                var dataModelLocal;
                if (container.gqlAddModel.ContainsKey("errors")){
                    dataModelLocal = container.gqlAddModel.GetData("errors");

                    if (dataModelLocal.ContainsKey(container.gqlAddCommandId)){
                        dataModelLocal = dataModelLocal.GetData(container.gqlAddCommandId);
                    }

                    let message = ""
                    if (dataModelLocal.ContainsKey("message")){
                        message = dataModelLocal.GetData("message");
                    }

                    let type;
                    if (dataModelLocal.ContainsKey("type")){
                        type = dataModelLocal.GetData("type");
                    }

                    container.error(message, type);
                }

                else if (container.gqlAddModel.ContainsKey("data")){
                    dataModelLocal = container.gqlAddModel.GetData("data");

                    if (dataModelLocal.ContainsKey(container.gqlAddCommandId)){
                        dataModelLocal = dataModelLocal.GetData(container.gqlAddCommandId);
                        dataModelLocal = dataModelLocal.GetData("addedNotification");

                        let documentId = ""
                        let documentName = ""

                        if (dataModelLocal){
                            documentId = dataModelLocal.GetData("Id");
                            documentName = dataModelLocal.GetData("Name");
                        }

                        container.saved(documentId, documentName);
                    }
                }
            }
        }

        function save(){
            var query = Gql.GqlRequest("mutation", container.gqlAddCommandId);

            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("Id", container.documentId);
            inputParams.InsertField ("Item", container.documentModel.ToJson());

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
            this.SetGqlQuery(gqlData);
        }
    }
}
