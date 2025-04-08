import QtQuick 2.12
import Acf 1.0
import imtcontrols 1.0
import imtguigql 1.0


/**
    Usage example:

    GqlRequestSender {
        id: requestSender;
        requestType: 0; // Query
        gqlCommandId: "GetDocumentData";

        function createQueryParams(query, params){
            var inputParams = Gql.GqlObject("input");
            inputParams.InsertField("id", params["id"]);
            query.AddParam(inputParams);

            var queryFields = Gql.GqlObject("item");
            queryFields.InsertField("id");
            queryFields.InsertField("name");
            query.AddField(queryFields);
        }

        function onError(message, type){
            // Error handler
        }

        // Response format: {"data": {"GetDocumentData": { <Internal information> }}}

        // parameter data is <Internal information>
        function onResult(data){
            // Result handler
        }
    }
*/

GqlModel {
    id: root;

    property string gqlCommandId;
    property int requestType: 0; // 0 - Query, 1 - Mutation, 2 - Subscription

    function onResult(data){}

    function onError(message, type){
        if (type == "Critical"){
            ModalDialogManager.showCriticalDialog(message);
        }
        else if (type == "Warning"){
            ModalDialogManager.showWarningDialog(message);
        }
        else if (type == "Info"){
            ModalDialogManager.showInfoDialog(message);
        }
        else if (type == "Error"){
            ModalDialogManager.showErrorDialog(message);
        }
    }

    function getHeaders(){
        let obj = {}
        return obj;
    }

    function send(params){
        if (gqlCommandId == "" || gqlCommandId === undefined || gqlCommandId == null ){
            console.error("Unable to send request. Error: GraphQL command-ID is invalid");
            return;
        }

        let type = "";
        if (root.requestType == 0){
            type = "query";
        }
        else if (root.requestType == 1){
            type = "mutation";
        }
        else if (root.requestType == 2){
            type = "subscription";
        }
        else{
            console.error("Unable to send request", root.gqlCommandId ,". Error: Request type is unsupported.");
            return;
        }

        var query = Gql.GqlRequest(type, root.gqlCommandId);
        createQueryParams(query, params);

        root.setGqlQuery(query.GetQuery(), root.getHeaders());
    }

    function createQueryParams(query, params){}

    onStateChanged: {
        if (state === "Error"){
            root.onError("Network error", "Critical");
        }
        else if (state === "Ready"){
            if (root.containsKey("errors")){
                let dataModelLocal = root.getData("errors");

                if (dataModelLocal.containsKey(root.gqlCommandId)){
                    dataModelLocal = dataModelLocal.getData(root.gqlCommandId);
                }

                let message = ""
                if (dataModelLocal.containsKey("message")){
                    message = dataModelLocal.getData("message");
                }

                let type = "";
                if (dataModelLocal.containsKey("type")){
                    type = dataModelLocal.getData("type");
                }

                root.onError(message, type);
            }

            else if (root.containsKey("data")){
                let dataModelLocal = root.getData("data")
                if (dataModelLocal.containsKey(root.gqlCommandId)){
                    let data = dataModelLocal.getData(root.gqlCommandId)

                    root.onResult(data);
                }
            }
        }
    }
}
